#include "../include/utils.h"
#include "../include/loader.h"
#include "../include/tracer.h"
#include <sys/ptrace.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    print_banner();

    if (argc < 2) {
        print_help(argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "--help") == 0) {
        print_help(argv[0]);
        exit(0);
    }

    loading_animation();

    char *target = argv[1];

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-file") == 0)
            load_plugin("./plugins/ext_file.so");
        else if (strcmp(argv[i], "-net") == 0)
            load_plugin("./plugins/ext_net.so");
        else if (strcmp(argv[i], "-exec") == 0)
            load_plugin("./plugins/ext_exec.so");
        else if (strcmp(argv[i], "-mem") == 0)
            load_plugin("./plugins/ext_mem.so");
        else
            log_msg("WARN", "Unknown option: %s", argv[i]);
    }

    pid_t child = fork();
    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execlp(target, target, NULL);
        perror("execve failed");
        exit(1);
    } else {
        ptrace(PTRACE_SYSCALL, child, 0, 0);
        tracer_loop(child);
    }

    return 0;
}
