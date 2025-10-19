#ifndef COMMAND_EDITOR_H
#define COMMAND_EDITOR_H

#include "kernel/kernel.h"
#include "drivers/vga/vga.h"

// Forward declaration
struct keyboard_event;

#define MAX_COMMANDS 20
#define MAX_COMMAND_LENGTH 64
#define MAX_COMMAND_DESC 128

// Command structure for editor
typedef struct {
    char name[MAX_COMMAND_LENGTH];
    char description[MAX_COMMAND_DESC];
    char example[MAX_COMMAND_LENGTH];
    bool is_file_command;
} command_info_t;

// Command editor state
typedef struct {
    command_info_t commands[MAX_COMMANDS];
    u8 command_count;
    u8 selected_command;
    u8 start_display;
    u8 max_visible_commands;
    bool is_active;
    bool show_examples;
} command_editor_state_t;

// Initialize command editor
void cmd_editor_init();

// Start command editor
void cmd_editor_start();

// Handle keyboard input in command editor
void cmd_editor_handle_keyboard(struct keyboard_event event);

// Draw command editor interface
void cmd_editor_draw();

// Execute selected command
void cmd_editor_execute_command();

// Exit command editor
void cmd_editor_exit();

// Check if command editor is active
bool cmd_editor_is_active();

// Get command editor state
command_editor_state_t* cmd_editor_get_state();

#endif
