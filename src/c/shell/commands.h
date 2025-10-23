#ifndef COMMANDS_H
#define COMMANDS_H

#include "kernel/kernel.h"
#include "shell/shell.h"

// Command implementations
void cmd_help(const char* args);
void cmd_clear(const char* args);
void cmd_info(const char* args);
void cmd_version(const char* args);

// File system commands
void cmd_list(const char* args);
void cmd_create(const char* args);
void cmd_delete(const char* args);
void cmd_edit(const char* args);
void cmd_view(const char* args);

void cmd_read(const char* args);
void cmd_screensaver(const char* args);

void cmd_size(const char* args);
void cmd_clear_content(const char* args);

// Register all built-in commands
void commands_init();

#endif
