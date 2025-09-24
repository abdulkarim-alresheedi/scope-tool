#include "../include/ext_exec.h"
#include "../include/utils.h"
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <linux/sched.h>

static unsigned long peek_ptr(pid_t pid, unsigned long addr) {
    errno = 0;
    unsigned long val = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
    return (errno == 0) ? val : 0;
}

static char* read_string_from_pid(pid_t pid, unsigned long addr) {
    static char buf[256];
    int i = 0;
    long word;

    if (!addr) return "<nil>";

    while (i < 255) {
        errno = 0;
        word = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
        if (errno != 0) break;

        memcpy(buf + i, &word, sizeof(word));
        if (memchr(&word, 0, sizeof(word)) != NULL) break;

        i += sizeof(word);
    }
    buf[255] = '\0';
    return buf;
}

static void print_clone_flags(long flags) {
    printf("            Flags         = %#lx (%ld)", flags, flags);
    printf(" (");
    if (flags & CLONE_VM)        printf("CLONE_VM ");
    if (flags & CLONE_FS)        printf("CLONE_FS ");
    if (flags & CLONE_FILES)     printf("CLONE_FILES ");
    if (flags & CLONE_SIGHAND)   printf("CLONE_SIGHAND ");
    if (flags & CLONE_THREAD)    printf("CLONE_THREAD ");
    if (flags & CLONE_PARENT)    printf("CLONE_PARENT ");
    if (flags & CLONE_VFORK)     printf("CLONE_VFORK ");
    if (flags & CLONE_PARENT_SETTID) printf("CLONE_PARENT_SETTID ");
    if (flags & CLONE_CHILD_CLEARTID) printf("CLONE_CHILD_CLEARTID ");
    printf(")\n");
}

static void analyze_exec(int syscall_number, long args[], long retval, pid_t pid) {
    if (syscall_number == SYS_execve) {
        char *path_str = read_string_from_pid(pid, args[0]);

        char *argv0 = "<nil>";
        if (args[1]) {
            unsigned long argv0_ptr = peek_ptr(pid, args[1]);
            if (argv0_ptr)
                argv0 = read_string_from_pid(pid, argv0_ptr);
        }

        printf(CYN "[EXEC] " RESET "execve\n");
        printf("            Path          = \"%s\"\n", path_str ? path_str : "<invalid>");
        printf("            argv[0]       = \"%s\"\n", argv0);
        printf("            envp Ptr      = %p\n", (void*)args[2]);
        print_retval(retval);
        printf(CYN "    [Hint] Program execution → check for process injection or suspicious binaries.\n" RESET);
    }
    else if (syscall_number == SYS_fork) {
        printf(CYN "[EXEC] " RESET "fork\n");
        print_retval(retval);
        printf(CYN "    [Hint] Fork called → may indicate process spawning or privilege escalation.\n" RESET);
    }
    else if (syscall_number == SYS_clone) {
        printf(CYN "[EXEC] " RESET "clone\n");
        print_clone_flags(args[0]);
        printf("            Child Stack   = %p\n", (void*)args[1]);
        printf("            PTID          = %#lx\n", args[2]);
        printf("            CTID          = %#lx\n", args[3]);
        print_retval(retval);
        printf(CYN "    [Hint] Clone detected → often used for threads or sandbox evasion.\n" RESET);
    }
}

plugin_t* init_plugin(void) {
    plugin_t *p = malloc(sizeof(plugin_t));
    if (!p) return NULL;
    strcpy(p->name, "ExecSyscalls");
    p->analyze_syscall = analyze_exec;
    p->next = NULL;
    return p;
}
