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
    u16 scroll_offset;    // Vertical scroll offset (in lines)
    u16 max_visible_lines; // Maximum lines visible on screen
    u16 total_lines;      // Total lines in file
    bool is_active;
    bool is_modified;
} editor_state_t;

// Initialize editor
void editor_init();

// Create new file
bool editor_create_new_file(const char* filename);

// Start editing a file
bool editor_open_file(const char* filename);

// Handle keyboard input in editor
void editor_handle_keyboard(struct keyboard_event event);

// Draw editor interface
void editor_draw();

// Move cursor up one line
void editor_move_cursor_up();

// Move cursor down one line
void editor_move_cursor_down();

// Scroll editor content
void editor_scroll_up();
void editor_scroll_down();
void editor_update_scroll();

// Count total lines in file
u16 editor_count_lines();

// Update view position based on cursor
void editor_update_view();

// Insert character at cursor
void editor_insert_char(char c);

// Delete character at cursor
void editor_delete_char();

// Delete character after cursor (Delete key)
void editor_delete_char_forward();

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
