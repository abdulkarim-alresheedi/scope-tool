#include "../include/ext_mem.h"
#include "../include/utils.h"
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <stdio.h>

static void print_prot_flags(long prot) {
    printf("            Prot          = %#lx (", prot);
    if (prot & 0x1) printf("PROT_READ|");
    if (prot & 0x2) printf("PROT_WRITE|");
    if (prot & 0x4) printf("PROT_EXEC|");
    printf(")\n");
}

static void print_map_flags(long flags) {
    printf("            Flags         = %#lx (", flags);
    if (flags & 0x01) printf("MAP_SHARED|");
    if (flags & 0x02) printf("MAP_PRIVATE|");
    if (flags & 0x20) printf("MAP_ANONYMOUS|");
    if (flags & 0x1000) printf("MAP_STACK|");
    if (flags & 0x40) printf("MAP_FIXED|");
    printf(")\n");
}

static void analyze_mem(int syscall_number, long args[], long retval, pid_t pid) {
    if (syscall_number == SYS_mmap) {
        printf(CYN "[MEM] " RESET "mmap\n");
        printf("            Addr          = %p\n", (void*)args[0]);
        printf("            Length        = %ld\n", args[1]);     
        print_prot_flags(args[2]);                        
        print_map_flags(args[3]);                         
        printf("            FD            = %d\n", (int)args[4]); 
        printf("            Offset        = %#lx\n", args[5]);   
        print_retval(retval);

        printf(CYN "    [Hint] " CYN "Memory mapping → watch for shellcode injection or unpacking.\n");
    }
    else if (syscall_number == SYS_mprotect) {
        printf(CYN "[MEM] " RESET "mprotect\n");
        printf("            Addr          = %p\n", (void*)args[0]);
        printf("            Length        = %ld\n", args[1]);     
        print_prot_flags(args[2]);                        
        print_retval(retval);

        printf(CYN "    [Hint] " CYN "Memory protection change → check if RX added (possible shellcode).\n");
    }
    else if (syscall_number == SYS_brk) {
        printf(CYN "[MEM] " RESET "brk\n");
        printf("            Addr          = %p\n", (void*)args[0]); 
        print_retval(retval);

        printf(CYN "    [Hint] " CYN "Heap manipulation → track heap expansion for suspicious allocs.\n");
    }
}

plugin_t* init_plugin(void) {
    plugin_t *p = malloc(sizeof(plugin_t));
    if (!p) return NULL;
    strcpy(p->name, "MemSyscalls");
    p->analyze_syscall = analyze_mem;
    p->next = NULL;
    return p;
}
