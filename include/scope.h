#ifndef SCOPE_H
#define SCOPE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <dlfcn.h>
#include <errno.h>

typedef struct plugin {
    char name[64];
    void (*analyze_syscall)(int num, long args[], long retval, pid_t pid);
    struct plugin *next;
} plugin_t;

extern plugin_t *plugin_list;

#endif
