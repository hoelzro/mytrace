#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <asm/unistd.h>
#include <unistd.h>

#include "die.h"
#include "mysql_protocol.h"
#include "syscall_info.h"

#define WORD_SIZE (sizeof(void*))

fd_set mysql_candidates;
fd_set pending_auth;
fd_set mysql_connections;

static void
init_tracing(pid_t pid)
{
    int status;
    int ok;

    ok = waitpid(pid, &status, 0);
    /* XXX check ok */
    /* XXX check signal == SIGINT */

    ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_TRACESYSGOOD);
}

static int
is_syscall(int status)
{
    return WSTOPSIG(status) == (SIGTRAP | 0x80);
}

static int
is_signal(int status)
{
    return !is_syscall(status); /* XXX dubious */
}

static int
next_trace(pid_t pid)
{
    int ok;
    int status;

    ptrace(PTRACE_SYSCALL, pid, NULL, 0);
    ok = waitpid(pid, &status, 0);

    if(ok == -1) {
        /* XXX elaborate */
        return 0;
    }

    return status;
}

static int
pmemcpy(char *dst, const char *src, pid_t pid, size_t count)
{
    size_t offset = 0;

    while(offset < count) {
        void *word;

        word = (void *) ptrace(PTRACE_PEEKDATA, pid, src + offset, 0);
        memcpy(dst + offset, &word, WORD_SIZE);

        offset += WORD_SIZE;
    }
}

static int
handle_syscall(pid_t pid)
{
    struct syscall_info info;

    get_syscall_info(pid, &info);

    switch(info.syscall_no) {
        case __NR_connect: {
            int status;
            int retval;

            status = next_trace(pid);
            retval = get_syscall_return(pid);

            /* XXX filter based on port number/socket name? */
            if(retval >= 0 || retval == -EINPROGRESS) {
                FD_SET(info.args.connect.sockfd, &mysql_candidates);
            }

            return status;
        }
        /* XXX what about recv? */
        case __NR_read: {
            int status = next_trace(pid);

            /* XXX bail if it's SSL */
            /* XXX compression? */
            if(FD_ISSET(info.args.read.fd, &mysql_candidates)) {
                int retval;

                FD_CLR(info.args.read.fd, &mysql_candidates);
                retval = get_syscall_return(pid);

                if(retval > 0) {
                    char *buffer; /* XXX can we do without an intermediary buffer? */
                    struct mysql_handshake_packet packet;
                    int ok;

                    buffer = malloc(retval);
                    memset(buffer, 0, retval);
                    pmemcpy(buffer, info.args.read.buf, pid, retval);
                    ok = read_mysql_handshake(buffer, retval, &packet);

                    if(ok) {
                        FD_SET(info.args.read.fd, &pending_auth);
                        FD_SET(info.args.read.fd, &mysql_connections);
                    }
                    free(buffer);
                }
            }

            return status;
        }
        default:
            return next_trace(pid); /* we don't care about other syscalls */
    }
}

static void
perform_trace(pid_t pid)
{
    int status;

    init_tracing(pid);

    while(status = next_trace(pid)) {
        if(is_signal(status)) {
            printf("Got signal: %d\n", WSTOPSIG(status));
        } else if(is_syscall(status)) {
            if(! handle_syscall(pid)) {
                break;
            }
        }
    }
}

int
main(void)
{
    pid_t child;

    FD_ZERO(&mysql_candidates);
    FD_ZERO(&pending_auth);
    FD_ZERO(&mysql_connections);

    child = fork();
    if(child == -1) {
        die("Unable to fork: %s", strerror(errno));
    }

    if(child) {
        perform_trace(child);
    } else {
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        raise(SIGINT);

        execlp("perl", "perl", "test.pl", NULL);
    }

    return 0;
}
