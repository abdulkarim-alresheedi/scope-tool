#include "../include/ext_file.h"
#include "../include/utils.h"
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static char* read_string_from_pid(pid_t pid, unsigned long addr) {
    static char buffer[256];
    int i = 0;
    long word;

    while (i < 255) {
        errno = 0;
        word = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
        if (errno != 0) break;

        memcpy(buffer + i, &word, sizeof(word));
        if (memchr(&word, 0, sizeof(word)) != NULL) break;

        i += sizeof(word);
    }

    buffer[255] = '\0';
    return buffer;
}

static void read_data_from_pid(pid_t pid, unsigned long addr, unsigned char *buf, size_t len) {
    size_t i = 0;
    long word;
    while (i < len) {
        errno = 0;
        word = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
        if (errno != 0) break;

        size_t copy_size = sizeof(word);
        if (i + copy_size > len)
            copy_size = len - i;

        memcpy(buf + i, &word, copy_size);
        i += copy_size;
    }
}

static void print_hex_ascii_line(const unsigned char *buf, size_t len, unsigned long offset) {
    printf("            %08lx  ", offset);
    for (size_t i = 0; i < 16; i++) {
        if (i < len) printf("%02x ", buf[i]);
        else printf("   ");
    }
    printf(" |");
    for (size_t i = 0; i < len; i++) {
        if (buf[i] >= 32 && buf[i] <= 126) printf("%c", buf[i]);
        else printf(".");
    }
    printf("|\n");
}

static void print_hex_ascii(pid_t pid, unsigned long addr, size_t len) {
    unsigned char buf[16];
    size_t offset = 0;
    while (offset < len) {
        size_t chunk_len = (len - offset > 16) ? 16 : (len - offset);
        read_data_from_pid(pid, addr + offset, buf, chunk_len);
        print_hex_ascii_line(buf, chunk_len, offset);
        offset += chunk_len;
    }
}

static void analyze_file(int syscall_number, long args[], long retval, pid_t pid) {
    if (syscall_number == SYS_openat) {
        char *path_str = read_string_from_pid(pid, args[1]);
        printf(GRN "[FILE] " RESET "openat\n");
        printf("            DirFD        = %d\n", (int)args[0]);
        printf("            Path         = \"%s\"\n", path_str ? path_str : "<invalid>");
        print_open_flags(args[2]);
        printf("            Mode         = %#lx (%ld)\n", args[3], args[3]);
        print_retval(retval);
        printf(CYN "    [Hint] File opened → track file access for sensitive paths.\n" RESET);
    } 
    else if (syscall_number == SYS_read) {
        printf(GRN "[FILE] " RESET "read\n");
        printf("            FD           = %d\n", (int)args[0]);
        printf("            Buffer Addr  = %p\n", (void*)args[1]);
        printf("            Requested    = %ld\n", args[2]);
        print_retval(retval);

        if (retval > 0) {
            size_t preview_len = (retval < 64) ? retval : 64;
            printf("            Buffer Preview (Hex+ASCII):\n");
            print_hex_ascii(pid, args[1], preview_len);
        }
        printf(CYN "    [Hint] Reading file → may reveal config or key material.\n" RESET);
    } 
    else if (syscall_number == SYS_write) {
        printf(GRN "[FILE] " RESET "write\n");
        printf("            FD           = %d\n", (int)args[0]);
        printf("            Buffer Addr  = %p\n", (void*)args[1]);
        printf("            Requested    = %ld\n", args[2]);
        print_retval(retval);

        if (retval > 0) {
            size_t preview_len = (retval < 64) ? retval : 64;
            printf("            Buffer Preview (Hex+ASCII):\n");
            print_hex_ascii(pid, args[1], preview_len);
        }
        printf(CYN "    [Hint] Writing file → could indicate log tampering or data exfil.\n" RESET);
    } 
    else if (syscall_number == SYS_close) {
        printf(GRN "[FILE] " RESET "close\n");
        printf("            FD           = %d\n", (int)args[0]);
        print_retval(retval);
        printf(CYN "    [Hint] Closing FD → track if handles are repeatedly opened/closed.\n" RESET);
    }
}

plugin_t* init_plugin(void) {
    plugin_t *p = malloc(sizeof(plugin_t));
    if (!p) return NULL;
    strcpy(p->name, "FileSyscalls");
    p->analyze_syscall = analyze_file;
    p->next = NULL;
    return p;
}
