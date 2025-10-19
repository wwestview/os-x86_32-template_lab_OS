#ifndef EDITOR_H
#define EDITOR_H

#include "kernel/kernel.h"
#include "filesystem/filesystem.h"
#include "drivers/vga/vga.h"

// Forward declaration
struct keyboard_event;

#define EDITOR_MAX_LINES 25
#define EDITOR_MAX_COLS 80
#define EDITOR_MAX_CONTENT 2000

// Editor state
typedef struct {
    file_t* current_file;
    u16 cursor_position;
    u16 start_position;  // First visible position
    bool is_active;
    bool is_modified;
} editor_state_t;

// Initialize editor
void editor_init();

// Start editing a file
bool editor_open_file(const char* filename);

// Handle keyboard input in editor
void editor_handle_keyboard(struct keyboard_event event);

// Draw editor interface
void editor_draw();

// Move cursor
void editor_move_cursor(u16 position);

// Insert character at cursor
void editor_insert_char(char c);

// Delete character at cursor
void editor_delete_char();

// Insert newline
void editor_insert_newline();

// Save current file
bool editor_save();

// Exit editor
void editor_exit();

// Check if editor is active
bool editor_is_active();

// Get editor state
editor_state_t* editor_get_state();

#endif
