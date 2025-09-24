# SCOPE: System Call Observation & Plugin Explorer

## Overview
SCOPE is a modular system call tracing and analysis tool written in C.  
It is designed to monitor program execution at the syscall level, using a plugin-based architecture that makes it easy to extend with new functionality.  

The tool allows researchers and security professionals to trace how programs interact with the operating system, with a focus on file operations, networking, memory management, and process execution.  

## Features
- **Runtime plugin loader**: Extensions are built as shared libraries (`.so`) and loaded at runtime.
- **Linked list plugin manager**: Multiple plugins can be added dynamically without recompiling the core.
- **Syscall tracing**: Uses `ptrace` to monitor system calls in real time.
- **Plugin support**:
  - **FileSyscalls**: Tracks `open`, `read`, `write`, `close`, etc.
  - **NetSyscalls**: Tracks socket operations (`socket`, `connect`).
  - **ExecSyscalls**: Tracks process creation (`execve`, `fork`, `clone`).
  - **MemSyscalls**: Tracks memory syscalls (`mmap`, `mprotect`, `brk`).
- **Formatted output**: Colorized terminal output with both raw syscall data and helpful analysis hints.
- **Extensibility**: New plugins can be easily added to support other syscall families.

## Architecture
The project is divided into clearly separated modules:

- **Core**
  - `main.c` → Entry point and argument parser.
  - `tracer.c/h` → Implements the syscall tracer loop.
  - `loader.c/h` → Dynamically loads plugins at runtime.
- **Plugins**
  - `ext_file.c` → File syscall analysis.
  - `ext_net.c` → Network syscall analysis.
  - `ext_exec.c` → Process execution analysis.
  - `ext_mem.c` → Memory syscall analysis.
- **Include**
  - `scope.h` → Core type definitions (e.g., plugin_t).
  - `utils.c/h` → Utility functions for logging, colors, and formatting.

## Usage
To run SCOPE against a target binary:

```bash
./scope <program> [options]
