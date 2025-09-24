#include "../include/ext_net.h"
#include "../include/utils.h"
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static void read_sockaddr_from_pid(pid_t pid, unsigned long addr, struct sockaddr_in *out) {
    size_t len = sizeof(struct sockaddr_in);
    size_t offset = 0;
    long word;

    unsigned char *buf = (unsigned char*)out;

    while (offset < len) {
        errno = 0;
        word = ptrace(PTRACE_PEEKDATA, pid, addr + offset, NULL);
        if (errno != 0) break;

        size_t copy_size = sizeof(word);
        if (offset + copy_size > len)
            copy_size = len - offset;

        memcpy(buf + offset, &word, copy_size);
        offset += copy_size;
    }
}

static void analyze_net(int syscall_number, long args[], long retval, pid_t pid) {
    if (syscall_number == SYS_socket) {
        printf(CYN "[NET] " RESET "socket\n");
        printf("            Domain        = %ld\n", args[0]);
        printf("            Type          = %ld\n", args[1]);
        printf("            Protocol      = %ld\n", args[2]);
        print_retval(retval);

        printf("    " CYN "[Hint] " CYN "Socket creation → may indicate network comms or IPC.\n");
    }
    else if (syscall_number == SYS_connect) {
        printf(CYN "[NET] " RESET "connect\n");
        printf("            FD            = %ld\n", args[0]);
        printf("            Addr Ptr      = %p\n", (void*)args[1]);
        printf("            Addr Len      = %ld\n", args[2]);
        print_retval(retval);

        if (args[1] && args[2] >= sizeof(struct sockaddr_in)) {
            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            read_sockaddr_from_pid(pid, args[1], &addr);
            printf("            IPv4 Addr     = %s\n", inet_ntoa(addr.sin_addr));
            printf("            Port          = %d\n", ntohs(addr.sin_port));
        }

        printf("    " CYN "[Hint] " CYN "Connection attempt → monitor for C2 servers or data exfiltration.\n");
    }
}

plugin_t* init_plugin(void) {
    plugin_t *p = malloc(sizeof(plugin_t));
    if (!p) return NULL;
    strcpy(p->name, "NetSyscalls");
    p->analyze_syscall = analyze_net;
    p->next = NULL;
    return p;
}
