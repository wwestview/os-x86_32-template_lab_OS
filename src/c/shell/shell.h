#ifndef SHELL_H
#define SHELL_H

#include "kernel/kernel.h"
#include "drivers/vga/vga.h"
#include "drivers/keyboard/keyboard.h"
#include "editor/editor.h"
#include "command_editor/command_editor.h"
#include "screensaver/screensaver.h"

#define SHELL_MAX_INPUT_LENGTH 256
#define SHELL_MAX_COMMAND_LENGTH 64
#define SHELL_MAX_ARGS 16
#define SHELL_HISTORY_LINES 100
#define SHELL_SCREEN_LINES 25

// Shell state
typedef struct {
    char input_buffer[SHELL_MAX_INPUT_LENGTH];
    u16 input_length;
    u16 cursor_position;
    bool is_running;
    
    // Scroll buffer for history
    char history_buffer[SHELL_HISTORY_LINES][VGA_WIDTH];
    u8 history_count;
    u8 scroll_offset;
    bool scroll_mode;
} shell_state_t;

// Command structure
typedef struct {
    char name[SHELL_MAX_COMMAND_LENGTH];
    void (*handler)(const char* args);
    char description[128];
} shell_command_t;

// Initialize shell
void shell_init();

// Main shell loop
void shell_run();

// Handle keyboard input
void shell_handle_keyboard(struct keyboard_event event);

// Process command input
void shell_process_command();

// Add a command to the shell
void shell_register_command(const char* name, void (*handler)(const char* args), const char* description);

// Print shell prompt
void shell_print_prompt();

// Print error message
void shell_print_error(const char* message);

// Print info message
void shell_print_info(const char* message);

// Get current shell state
shell_state_t* shell_get_state();

#endif
