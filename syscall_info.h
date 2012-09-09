#ifndef SYSCALL_INFO_H
#define SYSCALL_INFO_H

#define MAX_SYSCALL_ARGS 6
#define REG_ALIGN __attribute__ ((aligned (sizeof(void*))));

struct write_args {
    int    fd    REG_ALIGN;
    void   *buf  REG_ALIGN;
    size_t count REG_ALIGN;
};

struct connect_args {
    int                    sockfd  REG_ALIGN;
    const struct sockaddr *addr    REG_ALIGN;
    socklen_t              addrlen REG_ALIGN;
};

struct exit_args {
    int status REG_ALIGN;
};

struct syscall_info {
    int syscall_no;

    union {
        struct write_args   write;
        struct connect_args connect;
        struct exit_args    exit;
        struct exit_args    exit_group;

        struct {
            void *registers[MAX_SYSCALL_ARGS];
        } __registers;
    } args;
};

int get_syscall_info(pid_t pid, struct syscall_info *info);
int get_syscall_return(pid_t pid);

#endif
