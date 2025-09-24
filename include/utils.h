#ifndef UTILS_H
#define UTILS_H

#include "scope.h"

// Colors
#define RED     "\033[1;31m"
#define GRN     "\033[1;32m"
#define YEL     "\033[1;33m"
#define BLU     "\033[1;34m"
#define MAG     "\033[1;35m"
#define CYN     "\033[1;36m"
#define WHT     "\033[1;37m"
#define RESET   "\033[0m"

// Logger
void log_msg(const char *level, const char *fmt, ...);

// UI
void print_banner(void);
void print_help(const char *progname);
void loading_animation(void);

// Helpers
void print_open_flags(long flags);
void print_retval(long retval);

#endif
