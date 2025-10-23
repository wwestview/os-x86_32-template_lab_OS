#include "editor/editor.h"
#include "drivers/keyboard/keyboard.h"
#include "shell/shell.h"

static editor_state_t editor_state;

void editor_init() {
    editor_state.current_file = 0;
    editor_state.cursor_position = 0;
    editor_state.start_position = 0;
    editor_state.scroll_offset = 0;
    editor_state.max_visible_lines = VGA_HEIGHT - 4; // Leave space for header, separator, and footer
    editor_state.total_lines = 0;
    editor_state.is_active = false;
    editor_state.is_modified = false;
}

bool editor_open_file(const char* filename) {
    file_t* file = fs_get_file(filename);
    if (!file) {
        // Create new file if it doesn't exist
        if (!fs_create_file(filename)) {
            return false;
        }
        file = fs_get_file(filename);
        if (!file) {
            return false;
        }
    }
    
    editor_state.current_file = file;
    editor_state.cursor_position = 0;
    editor_state.start_position = 0;
    editor_state.is_active = true;
    editor_state.is_modified = false;
    
    vga_disable_cursor(); // Disable hardware cursor
    editor_draw();
    return true;
}

bool editor_create_new_file(const char* filename) {
    if (!fs_create_file(filename)) {
        return false;
    }
    
    file_t* file = fs_get_file(filename);
    if (!file) {
        return false;
    }
    
    editor_state.current_file = file;
    editor_state.cursor_position = 0;
    editor_state.start_position = 0;
    editor_state.is_active = true;
    editor_state.is_modified = false;
    
    vga_disable_cursor(); // Disable hardware cursor
    editor_draw();
    return true;
}

void editor_handle_keyboard(struct keyboard_event event) {
    if (!editor_state.is_active || event.type != EVENT_KEY_PRESSED) {
        return;
    }
    
    // Check if file is read-only
    if (editor_state.current_file && editor_state.current_file->is_read_only) {
        // Only allow ESC to exit
        if (event.key == KEY_ESC) {
            editor_exit();
        }
        return;
    }
    
    char c = event.key_character;
    
    switch (event.key) {
        case KEY_ESC:
            editor_exit();
            break;
            
        case KEY_LEFT:
            if (editor_state.cursor_position > 0) {
                editor_state.cursor_position--;
                editor_update_view();
                editor_draw();
            }
            break;
            
        case KEY_RIGHT:
            if (editor_state.cursor_position < editor_state.current_file->content_length) {
                editor_state.cursor_position++;
                editor_update_view();
                editor_draw();
            }
            break;
            
        case KEY_UP:
            editor_move_cursor_up();
            break;
            
        case KEY_DOWN:
            editor_move_cursor_down();
            break;
            
        case KEY_PAGE_UP:
            editor_scroll_up();
            break;
            
        case KEY_PAGE_DOWN:
            editor_scroll_down();
            break;
            
        case KEY_ENTER:
            editor_insert_char('\n');
            break;
            
        case KEY_BACKSPACE:
            editor_delete_char();
            break;
            
        case KEY_DELETE:
            editor_delete_char_forward();
            break;
            
        case KEY_CTRL_S:
            if (editor_save()) {
                vga_print_color("\nFile saved successfully!\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            } else {
                vga_print_color("\nFailed to save file!\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            }
            break;
            
        default:
            if (c >= 32 && c <= 126) {
                editor_insert_char(c);
            }
            break;
    }
}

void editor_draw() {
    if (!editor_state.is_active || !editor_state.current_file) {
        return;
    }
    
    // Update total lines count
    editor_state.total_lines = editor_count_lines();
    
    vga_clear();
    vga_set_cursor(0, 0);
    
    // Draw header
    vga_print_color("Editor: ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print(editor_state.current_file->name);
    if (editor_state.is_modified) {
        vga_print_color(" *", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
    if (editor_state.current_file->is_read_only) {
        vga_print_color(" [READ-ONLY]", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
    vga_print_color(" (ESC=exit, Ctrl+S=save, Arrows=move, PgUp/PgDn=scroll)", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_newline();
    
    // Draw separator
    for (u16 i = 0; i < EDITOR_MAX_COLS; i++) {
        vga_putchar_color('-', VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
    vga_newline();
    
    // Draw scrollable content
    u16 content_len = editor_state.current_file->content_length;
    u16 visible_lines = editor_state.max_visible_lines;
    u16 current_line = 0;
    u16 current_col = 0;
    
    // Find starting position based on scroll offset
    u16 line_start = 0;
    u16 line_count = 0;
    u16 pos = 0;
    
    // Skip to scroll offset line
    while (pos < content_len && line_count < editor_state.scroll_offset) {
        if (editor_state.current_file->content[pos] == '\n') {
            line_count++;
            line_start = pos + 1;
        }
        pos++;
    }
    
    // Display visible lines
    pos = line_start;
    while (current_line < visible_lines && pos < content_len) {
        current_col = 0;
        
        // Display current line
        while (pos < content_len && current_col < EDITOR_MAX_COLS - 2) {
            char ch = editor_state.current_file->content[pos];
            
            if (pos == editor_state.cursor_position) {
                vga_putchar_color(ch == '\n' ? ' ' : ch, VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
            } else {
                vga_putchar_color(ch == '\n' ? ' ' : ch, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            }
            
            if (ch == '\n') {
                pos++;
                break;
            }
            
            pos++;
            current_col++;
        }
        
        // Fill remaining columns in this line
        while (current_col < EDITOR_MAX_COLS - 2) {
            if (pos == editor_state.cursor_position && pos >= content_len) {
                vga_putchar_color(' ', VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
            } else {
                vga_putchar_color(' ', VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            }
            current_col++;
        }
        
        vga_newline();
        current_line++;
    }
    
    // Fill remaining lines
    while (current_line < visible_lines) {
        for (u16 i = 0; i < EDITOR_MAX_COLS - 2; i++) {
            if (editor_state.cursor_position == content_len && current_line == visible_lines - 1 && i == 0) {
                vga_putchar_color(' ', VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
            } else {
                vga_putchar_color(' ', VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            }
        }
        vga_newline();
        current_line++;
    }
    
    // Draw footer with line and column info
    vga_print_color("Line: ", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    
    // Calculate current line number
    u16 current_line_num = 1;
    for (u16 i = 0; i < editor_state.cursor_position && i < content_len; i++) {
        if (editor_state.current_file->content[i] == '\n') {
            current_line_num++;
        }
    }
    
    char line_str[8];
    u16 line_len = 0;
    u16 temp_line = current_line_num;
    if (temp_line == 0) {
        line_str[line_len++] = '0';
    } else {
        while (temp_line > 0) {
            line_str[line_len++] = '0' + (temp_line % 10);
            temp_line /= 10;
        }
    }
    line_str[line_len] = '\0';
    
    // Reverse the string
    for (u16 i = 0; i < line_len / 2; i++) {
        char temp = line_str[i];
        line_str[i] = line_str[line_len - 1 - i];
        line_str[line_len - 1 - i] = temp;
    }
    
    vga_print(line_str);
    vga_print_color(" Col: ", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    
    // Calculate current column number
    u16 current_col_num = 1;
    u16 line_start_pos = editor_state.cursor_position;
    while (line_start_pos > 0 && editor_state.current_file->content[line_start_pos - 1] != '\n') {
        line_start_pos--;
        current_col_num++;
    }
    
    char col_str[8];
    u16 col_len = 0;
    u16 temp_col = current_col_num;
    if (temp_col == 0) {
        col_str[col_len++] = '0';
    } else {
        while (temp_col > 0) {
            col_str[col_len++] = '0' + (temp_col % 10);
            temp_col /= 10;
        }
    }
    col_str[col_len] = '\0';
    
    // Reverse the string
    for (u16 i = 0; i < col_len / 2; i++) {
        char temp = col_str[i];
        col_str[i] = col_str[col_len - 1 - i];
        col_str[col_len - 1 - i] = temp;
    }
    
    vga_print(col_str);
    vga_print_color(" Size: ", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    
    char size_str[8];
    u16 size_num = editor_state.current_file->content_length;
    u16 size_len = 0;
    if (size_num == 0) {
        size_str[size_len++] = '0';
    } else {
        u16 temp = size_num;
        while (temp > 0) {
            size_str[size_len++] = '0' + (temp % 10);
            temp /= 10;
        }
    }
    size_str[size_len] = '\0';
    
    // Reverse the string
    for (u16 i = 0; i < size_len / 2; i++) {
        char temp = size_str[i];
        size_str[i] = size_str[size_len - 1 - i];
        size_str[size_len - 1 - i] = temp;
    }
    
    vga_print(size_str);
}

void editor_move_cursor_up() {
    if (!editor_state.current_file) return;
    
    // Find the start of current line
    u16 line_start = editor_state.cursor_position;
    while (line_start > 0 && editor_state.current_file->content[line_start - 1] != '\n') {
        line_start--;
    }
    
    // If we're not at the first line, move to previous line
    if (line_start > 0) {
        u16 prev_line_start = line_start - 1;
        while (prev_line_start > 0 && editor_state.current_file->content[prev_line_start - 1] != '\n') {
            prev_line_start--;
        }
        
        // Find the end of previous line
        u16 prev_line_end = line_start - 1;
        
        // Calculate cursor position on previous line
        u16 current_pos_in_line = editor_state.cursor_position - line_start;
        u16 prev_line_length = prev_line_end - prev_line_start;
        
        if (current_pos_in_line <= prev_line_length) {
            editor_state.cursor_position = prev_line_start + current_pos_in_line;
        } else {
            editor_state.cursor_position = prev_line_end;
        }
        
        editor_update_scroll();
        editor_draw();
    }
}

void editor_move_cursor_down() {
    if (!editor_state.current_file) return;
    
    // Find the end of current line
    u16 line_end = editor_state.cursor_position;
    while (line_end < editor_state.current_file->content_length && 
           editor_state.current_file->content[line_end] != '\n') {
        line_end++;
    }
    
    // If we're not at the last line, move to next line
    if (line_end < editor_state.current_file->content_length) {
        u16 next_line_start = line_end + 1;
        
        // Find the end of next line
        u16 next_line_end = next_line_start;
        while (next_line_end < editor_state.current_file->content_length && 
               editor_state.current_file->content[next_line_end] != '\n') {
            next_line_end++;
        }
        
        // Calculate cursor position on next line
        u16 current_line_start = editor_state.cursor_position;
        while (current_line_start > 0 && editor_state.current_file->content[current_line_start - 1] != '\n') {
            current_line_start--;
        }
        
        u16 current_pos_in_line = editor_state.cursor_position - current_line_start;
        u16 next_line_length = next_line_end - next_line_start;
        
        if (current_pos_in_line <= next_line_length) {
            editor_state.cursor_position = next_line_start + current_pos_in_line;
        } else {
            editor_state.cursor_position = next_line_end;
        }
        
        editor_update_scroll();
        editor_draw();
    }
}

void editor_update_view() {
    if (!editor_state.current_file) return;
    
    u16 visible_chars = (EDITOR_MAX_LINES - 3) * EDITOR_MAX_COLS - 6;
    
    // If cursor is before visible area, scroll up
    if (editor_state.cursor_position < editor_state.start_position) {
        editor_state.start_position = editor_state.cursor_position;
    }
    // If cursor is after visible area, scroll down
    else if (editor_state.cursor_position >= editor_state.start_position + visible_chars) {
        editor_state.start_position = editor_state.cursor_position - visible_chars + 1;
    }
}

void editor_insert_char(char c) {
    if (!editor_state.current_file) return;
    
    if (editor_state.current_file->content_length >= MAX_FILE_SIZE - 1) {
        return; // No space
    }
    
    // Shift content right
    for (u16 i = editor_state.current_file->content_length; i > editor_state.cursor_position; i--) {
        editor_state.current_file->content[i] = editor_state.current_file->content[i - 1];
    }
    
    // Insert character
    editor_state.current_file->content[editor_state.cursor_position] = c;
    editor_state.cursor_position++;
    editor_state.current_file->content_length++;
    editor_state.current_file->content[editor_state.current_file->content_length] = '\0';
    editor_state.is_modified = true;
    
    editor_draw();
}

void editor_delete_char() {
    if (!editor_state.current_file) return;
    
    if (editor_state.cursor_position > 0) {
        // Shift content left
        for (u16 i = editor_state.cursor_position - 1; i < editor_state.current_file->content_length; i++) {
            editor_state.current_file->content[i] = editor_state.current_file->content[i + 1];
        }
        
        editor_state.cursor_position--;
        editor_state.current_file->content_length--;
        editor_state.current_file->content[editor_state.current_file->content_length] = '\0';
        editor_state.is_modified = true;
        editor_draw();
    }
}

void editor_delete_char_forward() {
    if (!editor_state.current_file) return;
    
    if (editor_state.cursor_position < editor_state.current_file->content_length) {
        // Shift content left
        for (u16 i = editor_state.cursor_position; i < editor_state.current_file->content_length; i++) {
            editor_state.current_file->content[i] = editor_state.current_file->content[i + 1];
        }
        
        editor_state.current_file->content_length--;
        editor_state.current_file->content[editor_state.current_file->content_length] = '\0';
        editor_state.is_modified = true;
        editor_draw();
    }
}

void editor_insert_newline() {
    editor_insert_char('\n');
}

bool editor_save() {
    if (!editor_state.current_file) {
        return false;
    }
    
    editor_state.is_modified = false;
    editor_state.current_file->is_read_only = true; // Make file read-only after saving
    return true;
}

void editor_exit() {
    editor_state.is_active = false;
    editor_state.current_file = 0;
    vga_clear();
    vga_set_cursor(0, 0); // Move cursor to top-left corner
    vga_enable_cursor(14, 15); // Re-enable hardware cursor for shell
    vga_print_color("Editor closed.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("shell> ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    // Set flag to indicate we just exited interactive mode
    shell_get_state()->just_exited_interactive = true;
}

// Scroll editor content up
void editor_scroll_up() {
    if (editor_state.scroll_offset > 0) {
        editor_state.scroll_offset--;
        editor_draw();
    }
}

// Scroll editor content down
void editor_scroll_down() {
    if (editor_state.scroll_offset + editor_state.max_visible_lines < editor_state.total_lines) {
        editor_state.scroll_offset++;
        editor_draw();
    }
}

// Update scroll position based on cursor
void editor_update_scroll() {
    if (!editor_state.current_file) return;
    
    // Calculate current line from cursor position
    u16 current_line = 0;
    u16 pos = 0;
    while (pos < editor_state.cursor_position && pos < editor_state.current_file->content_length) {
        if (editor_state.current_file->content[pos] == '\n') {
            current_line++;
        }
        pos++;
    }
    
    // Adjust scroll if cursor is outside visible area
    if (current_line < editor_state.scroll_offset) {
        editor_state.scroll_offset = current_line;
    } else if (current_line >= editor_state.scroll_offset + editor_state.max_visible_lines) {
        editor_state.scroll_offset = current_line - editor_state.max_visible_lines + 1;
    }
}

// Count total lines in file
u16 editor_count_lines() {
    if (!editor_state.current_file) return 0;
    
    u16 lines = 1; // At least one line
    for (u16 i = 0; i < editor_state.current_file->content_length; i++) {
        if (editor_state.current_file->content[i] == '\n') {
            lines++;
        }
    }
    return lines;
}

bool editor_is_active() {
    return editor_state.is_active;
}

editor_state_t* editor_get_state() {
    return &editor_state;
}