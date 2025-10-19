#include "editor/editor.h"
#include "drivers/keyboard/keyboard.h"

static editor_state_t editor_state;

// Helper function to get string length
static u16 str_length(const char* str) {
    u16 len = 0;
    while (*str) {
        len++;
        str++;
    }
    return len;
}

// Helper function to copy string
static void str_copy(char* dest, const char* src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

void editor_init() {
    editor_state.current_file = 0;
    editor_state.cursor_position = 0;
    editor_state.start_position = 0;
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
    
    editor_draw();
    return true;
}

void editor_handle_keyboard(struct keyboard_event event) {
    if (!editor_state.is_active || event.type != EVENT_KEY_PRESSED) {
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
                if (editor_state.cursor_position < editor_state.start_position) {
                    editor_state.start_position = editor_state.cursor_position;
                }
                editor_draw();
            }
            break;
            
        case KEY_RIGHT:
            if (editor_state.cursor_position < editor_state.current_file->content_length) {
                editor_state.cursor_position++;
                u16 visible_chars = (EDITOR_MAX_LINES - 3) * EDITOR_MAX_COLS - 6;
                if (editor_state.cursor_position >= editor_state.start_position + visible_chars) {
                    editor_state.start_position = editor_state.cursor_position - visible_chars + 1;
                }
                editor_draw();
            }
            break;
            
        case KEY_ENTER:
            editor_insert_char('\n');
            break;
            
        case KEY_BACKSPACE:
            editor_delete_char();
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
    
    vga_clear();
    
    // Draw header
    vga_print_color("Editor: ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print(editor_state.current_file->name);
    vga_print_color(" (ESC to exit)", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_newline();
    
    // Draw separator
    for (u16 i = 0; i < EDITOR_MAX_COLS; i++) {
        vga_putchar_color('-', VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
    vga_newline();
    
    // Draw file content
    u16 content_len = editor_state.current_file->content_length;
    u16 visible_chars = (EDITOR_MAX_LINES - 3) * EDITOR_MAX_COLS - 6;
    u16 end_position = editor_state.start_position + visible_chars;
    if (end_position > content_len) {
        end_position = content_len;
    }
    
    u16 line_pos = 0;
    u16 col_pos = 0;
    
    for (u16 i = editor_state.start_position; i < end_position; i++) {
        char ch = editor_state.current_file->content[i];
        
        if (i == editor_state.cursor_position) {
            vga_putchar_color(ch == '\0' ? ' ' : ch, VGA_COLOR_BLACK, VGA_COLOR_WHITE);
        } else {
            vga_putchar_color(ch == '\0' ? ' ' : ch, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        }
        
        col_pos++;
        if (ch == '\n' || col_pos >= EDITOR_MAX_COLS - 6) {
            vga_newline();
            col_pos = 0;
            line_pos++;
            if (line_pos >= EDITOR_MAX_LINES - 3) {
                break;
            }
        }
    }
    
    // Fill remaining space
    while (line_pos < EDITOR_MAX_LINES - 3) {
        for (u16 i = col_pos; i < EDITOR_MAX_COLS - 6; i++) {
            if (editor_state.cursor_position == end_position && i == col_pos) {
                vga_putchar_color(' ', VGA_COLOR_BLACK, VGA_COLOR_WHITE);
            } else {
                vga_putchar_color(' ', VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            }
        }
        vga_newline();
        line_pos++;
        col_pos = 0;
    }
    
    // Draw footer
    vga_print_color("Position: ", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    char pos_str[8];
    u16 pos_num = editor_state.cursor_position;
    u16 pos_len = 0;
    if (pos_num == 0) {
        pos_str[pos_len++] = '0';
    } else {
        u16 temp = pos_num;
        while (temp > 0) {
            pos_str[pos_len++] = '0' + (temp % 10);
            temp /= 10;
        }
    }
    pos_str[pos_len] = '\0';
    
    // Reverse the string
    for (u16 i = 0; i < pos_len / 2; i++) {
        char temp = pos_str[i];
        pos_str[i] = pos_str[pos_len - 1 - i];
        pos_str[pos_len - 1 - i] = temp;
    }
    
    vga_print(pos_str);
    vga_print_color(" / ", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    
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

void editor_move_cursor(u16 position) {
    if (!editor_state.current_file) return;
    
    if (position <= editor_state.current_file->content_length) {
        editor_state.cursor_position = position;
    }
    
    editor_draw();
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

void editor_insert_newline() {
    editor_insert_char('\n');
}

bool editor_save() {
    if (!editor_state.current_file) {
        return false;
    }
    
    editor_state.is_modified = false;
    return true;
}

void editor_exit() {
    editor_state.is_active = false;
    editor_state.current_file = 0;
    vga_clear();
    vga_print_color("Editor closed.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
}

bool editor_is_active() {
    return editor_state.is_active;
}

editor_state_t* editor_get_state() {
    return &editor_state;
}