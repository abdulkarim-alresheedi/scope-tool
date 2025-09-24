#include "../include/tracer.h"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <stdlib.h>

void tracer_loop(pid_t child) {
    int status;
    struct user_regs_struct regs;
    int in_syscall = 0;
    int syscall_num = -1;
    long args[6];

    ptrace(PTRACE_SYSCALL, child, 0, 0);

    while (1) {
        waitpid(child, &status, 0);
        if (WIFEXITED(status)) break;

        ptrace(PTRACE_GETREGS, child, 0, &regs);

        if (!in_syscall) {
            syscall_num = regs.orig_rax;
            args[0] = regs.rdi;
            args[1] = regs.rsi;
            args[2] = regs.rdx;
            args[3] = regs.r10;
            args[4] = regs.r8;
            args[5] = regs.r9;
            in_syscall = 1;
        } else {
            long retval = regs.rax;
            for (plugin_t *p = plugin_list; p != NULL; p = p->next) {
                if (p->analyze_syscall)
                    p->analyze_syscall(syscall_num, args, retval, child);
            }
            in_syscall = 0;
        }

        ptrace(PTRACE_SYSCALL, child, 0, 0);
    }
}
