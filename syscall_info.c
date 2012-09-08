#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>

#include "syscall_info.h"

#if !__x86_64__
#  error "This only works on 64-bit Intel machines for now"
#endif

/****************************************************************************/
/* Begin platform-specific code (put this in a different file) */
/****************************************************************************/
int
get_syscall_number(struct user_regs_struct *regs)
{
    return regs->orig_rax;
}

int
populate_registers(struct syscall_info *info, struct user_regs_struct *regs)
{
    info->args.__registers.registers[0] = (void *) regs->rdi; 
    info->args.__registers.registers[1] = (void *) regs->rsi; 
    info->args.__registers.registers[2] = (void *) regs->rdx; 
    info->args.__registers.registers[3] = (void *) regs->r10; 
    info->args.__registers.registers[4] = (void *) regs->r8; 
    info->args.__registers.registers[5] = (void *) regs->r9; 
}

/****************************************************************************/
/* End platform-specific code (put this in a different file) */
/****************************************************************************/

int
get_syscall_info(pid_t pid, struct syscall_info *info)
{
    struct user_regs_struct registers;
    int status;
 
#if __sparc__
    status = ptrace(PTRACE_GETREGS, pid, &registers, NULL);
#else
    status = ptrace(PTRACE_GETREGS, pid, NULL, &registers);
#endif

    info->syscall_no = get_syscall_number(&registers);
    populate_registers(info, &registers);
}
