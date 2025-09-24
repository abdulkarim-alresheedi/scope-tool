#include "../include/utils.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>

void log_msg(const char *level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    if (strcmp(level, "ALERT") == 0) printf(RED "[ALERT] ");
    else if (strcmp(level, "WARN") == 0) printf(YEL "[WARN] ");
    else printf(GRN "[INFO] ");

    vprintf(fmt, args);
    printf("%s\n", RESET);

    va_end(args);
}

void print_banner() {
    printf(
"                                                                                                     \n"
"                                                                                                     \n"
"   SSSSSSSSSSSSSSS         CCCCCCCCCCCCC     OOOOOOOOO     PPPPPPPPPPPPPPPPP   EEEEEEEEEEEEEEEEEEEEEE\n"
" SS:::::::::::::::S     CCC::::::::::::C   OO:::::::::OO   P::::::::::::::::P  E::::::::::::::::::::E\n"
"S:::::SSSSSS::::::S   CC:::::::::::::::C OO:::::::::::::OO P::::::PPPPPP:::::P E::::::::::::::::::::E\n"
"S:::::S     SSSSSSS  C:::::CCCCCCCC::::CO:::::::OOO:::::::OPP:::::P     P:::::PEE::::::EEEEEEEEE::::E\n"
"S:::::S             C:::::C       CCCCCCO::::::O   O::::::O  P::::P     P:::::P  E:::::E       EEEEEE\n"
"S:::::S            C:::::C              O:::::O     O:::::O  P::::P     P:::::P  E:::::E             \n"
" S::::SSSS         C:::::C              O:::::O     O:::::O  P::::PPPPPP:::::P   E::::::EEEEEEEEEE   \n"
"  SS::::::SSSSS    C:::::C              O:::::O     O:::::O  P:::::::::::::PP    E:::::::::::::::E   \n"
"    SSS::::::::SS  C:::::C              O:::::O     O:::::O  P::::PPPPPPPPP      E:::::::::::::::E   \n"
"       SSSSSS::::S C:::::C              O:::::O     O:::::O  P::::P              E::::::EEEEEEEEEE   \n"
"            S:::::SC:::::C              O:::::O     O:::::O  P::::P              E:::::E             \n"
"            S:::::S C:::::C       CCCCCCO::::::O   O::::::O  P::::P              E:::::E       EEEEEE\n"
"SSSSSSS     S:::::S  C:::::CCCCCCCC::::CO:::::::OOO:::::::OPP::::::PP          EE::::::EEEEEEEE:::::E\n"
"S::::::SSSSSS:::::S   CC:::::::::::::::C OO:::::::::::::OO P::::::::P          E::::::::::::::::::::E\n"
"S:::::::::::::::SS      CCC::::::::::::C   OO:::::::::OO   P::::::::P          E::::::::::::::::::::E\n"
" SSSSSSSSSSSSSSS           CCCCCCCCCCCCC     OOOOOOOOO     PPPPPPPPPP          EEEEEEEEEEEEEEEEEEEEEE\n"
"                                                                                                     \n"
RESET);

    printf("\n    System Call Observation &  Plugin Explorer \n\n");
}

void print_help(const char *progname) {
    printf(GRN "Usage: %s <program> [options]\n\n" RESET, progname);
    printf("Options:\n");
    printf("  " YEL "-file " RESET "   Monitor file syscalls (open, read, write, close)\n");
    printf("  " YEL "-net  " RESET "   Monitor network syscalls (socket, connect)\n");
    printf("  " YEL "-exec " RESET "   Monitor process execution (fork, execve)\n");
    printf("  " YEL "-mem  " RESET "   Monitor memory syscalls (mmap, mprotect)\n");
    printf("  " YEL "--help" RESET "   Show this help message\n");
}

void loading_animation() {
    const char *msg = "Loading SCOPE";
    printf(GRN "%s" RESET, msg);
    fflush(stdout);

    for (int i = 0; i < 5; i++) {
        usleep(300000);
        printf(".");
        fflush(stdout);
    }
    printf(" " GRN "[OK]\n\n" RESET);
}

void print_open_flags(long flags) {
    printf("            Flags        = %ld (", flags);
    int first = 1;
    if ((flags & O_RDONLY) == O_RDONLY) { printf("O_RDONLY"); first = 0; }
    if (flags & O_WRONLY) { if (!first) printf("|"); printf("O_WRONLY"); first = 0; }
    if (flags & O_RDWR)   { if (!first) printf("|"); printf("O_RDWR"); first = 0; }
    if (flags & O_CREAT)  { if (!first) printf("|"); printf("O_CREAT"); first = 0; }
    if (flags & O_EXCL)   { if (!first) printf("|"); printf("O_EXCL"); first = 0; }
    if (flags & O_TRUNC)  { if (!first) printf("|"); printf("O_TRUNC"); first = 0; }
    if (flags & O_APPEND) { if (!first) printf("|"); printf("O_APPEND"); first = 0; }
    #ifdef O_DIRECTORY
    if (flags & O_DIRECTORY) { if (!first) printf("|"); printf("O_DIRECTORY"); first = 0; }
#endif

#ifdef O_CLOEXEC
    if (flags & O_CLOEXEC) { if (!first) printf("|"); printf("O_CLOEXEC"); first = 0; }
#endif
    printf(")\n");
}

void print_retval(long retval) {
    if (retval < 0) {
        printf("            Return Value = %ld (%s)\n", retval, strerror(-retval));
    } else {
        printf("            Return Value = %ld\n", retval);
    }
}