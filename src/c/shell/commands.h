#ifndef COMMANDS_H
#define COMMANDS_H

#include "kernel/kernel.h"
#include "shell/shell.h"

// Command implementations
void cmd_help(const char* args);
void cmd_clear(const char* args);
void cmd_echo(const char* args);
void cmd_info(const char* args);
void cmd_version(const char* args);

// File system commands
void cmd_list(const char* args);
void cmd_create(const char* args);
void cmd_delete(const char* args);
void cmd_edit(const char* args);
void cmd_view(const char* args);

// File content manipulation commands
void cmd_write(const char* args);
void cmd_append(const char* args);
void cmd_insert(const char* args);
void cmd_delete_content(const char* args);
void cmd_replace(const char* args);
void cmd_size(const char* args);
void cmd_clear_content(const char* args);

// Register all built-in commands
void commands_init();

#endif
