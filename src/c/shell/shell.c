#include "shell/shell.h"
#include "drivers/vga/vga.h"
#include "filesystem/filesystem.h"
#include "command_editor/command_editor.h"
#include "screensaver/screensaver.h"

static shell_state_t shell_state;
static shell_command_t commands[16];
static u8 command_count = 0;

// Helper function to add line to history buffer
static void add_to_history(const char* line) {
    if (shell_state.history_count < SHELL_HISTORY_LINES) {
        u16 len = 0;
        while (line[len] && len < VGA_WIDTH - 1) {
            shell_state.history_buffer[shell_state.history_count][len] = line[len];
            len++;
        }
        // Pad with spaces
        while (len < VGA_WIDTH) {
            shell_state.history_buffer[shell_state.history_count][len] = ' ';
            len++;
        }
        shell_state.history_buffer[shell_state.history_count][VGA_WIDTH - 1] = '\0';
        shell_state.history_count++;
    } else {
        // Shift buffer up
        for (u8 i = 0; i < SHELL_HISTORY_LINES - 1; i++) {
            for (u16 j = 0; j < VGA_WIDTH; j++) {
                shell_state.history_buffer[i][j] = shell_state.history_buffer[i + 1][j];
            }
        }
        // Add new line at the end
        u16 len = 0;
        while (line[len] && len < VGA_WIDTH - 1) {
            shell_state.history_buffer[SHELL_HISTORY_LINES - 1][len] = line[len];
            len++;
        }
        while (len < VGA_WIDTH) {
            shell_state.history_buffer[SHELL_HISTORY_LINES - 1][len] = ' ';
            len++;
        }
        shell_state.history_buffer[SHELL_HISTORY_LINES - 1][VGA_WIDTH - 1] = '\0';
    }
}

// Helper function to display history with scroll
static void display_history() {
    vga_clear();
    
    u8 start_line = shell_state.scroll_offset;
    u8 end_line = start_line + SHELL_SCREEN_LINES - 1; // Leave space for prompt
    if (end_line > shell_state.history_count) {
        end_line = shell_state.history_count;
    }
    
    for (u8 i = start_line; i < end_line; i++) {
        vga_print(shell_state.history_buffer[i]);
        vga_newline();
    }
}

// Helper function to compare strings
static bool str_equals(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) return false;
        str1++;
        str2++;
    }
    return *str1 == *str2;
}

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

void shell_init() {
    // Initialize shell state
    shell_state.input_length = 0;
    shell_state.cursor_position = 0;
    shell_state.is_running = true;
    
    // Initialize scroll buffer
    shell_state.history_count = 0;
    shell_state.scroll_offset = 0;
    shell_state.scroll_mode = false;
    
    // Clear history buffer
    for (u8 i = 0; i < SHELL_HISTORY_LINES; i++) {
        for (u16 j = 0; j < VGA_WIDTH; j++) {
            shell_state.history_buffer[i][j] = ' ';
        }
    }
    
    // Initialize subsystems
    vga_init();
    fs_init();
    editor_init();
    cmd_editor_init();
    screensaver_init();
    
    // Print welcome message
    vga_print_color("OS Shell v1.0\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("Type 'help' for commands. F1 to scroll history.\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print_color("F2=Ball, F3=Matrix, F4=Stars screensavers.\n\n", VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    
    // Print initial prompt
    shell_print_prompt();
}

void shell_run() {
    // Shell runs in interrupt-driven mode
    // Main loop just keeps the system alive
    while (shell_state.is_running) {
        halt(); // Wait for interrupts
    }
}

void shell_handle_keyboard(struct keyboard_event event) {
    // Check if screensaver is active
    if (screensaver_is_active()) {
        screensaver_handle_keyboard(event);
        return;
    }
    
    // Check if command editor is active
    if (cmd_editor_is_active()) {
        cmd_editor_handle_keyboard(event);
        return;
    }
    
    // Check if editor is active
    if (editor_is_active()) {
        editor_handle_keyboard(event);
        return;
    }
    
    if (event.type != EVENT_KEY_PRESSED) {
        return;
    }
    
    char c = event.key_character;
    
    // Handle screensaver activation
    if (event.key == KEY_F2) {
        screensaver_start(SCREENSAVER_BOUNCING_BALL);
        return;
    }
    
    if (event.key == KEY_F3) {
        screensaver_start(SCREENSAVER_MATRIX_RAIN);
        return;
    }
    
    if (event.key == KEY_F4) {
        screensaver_start(SCREENSAVER_FALLING_STARS);
        return;
    }
    
    // Handle scroll mode
    if (shell_state.scroll_mode) {
        switch (event.key) {
            case KEY_UP:
                if (shell_state.scroll_offset > 0) {
                    shell_state.scroll_offset--;
                    display_history();
                }
                return;
                
            case KEY_DOWN:
                if (shell_state.scroll_offset + SHELL_SCREEN_LINES - 1 < shell_state.history_count) {
                    shell_state.scroll_offset++;
                    display_history();
                }
                return;
                
            case KEY_ESC:
                shell_state.scroll_mode = false;
                vga_clear();
                shell_print_prompt();
                return;
                
            default:
                return;
        }
    }
    
    // Handle scroll toggle
    if (event.key == KEY_F1) {
        shell_state.scroll_mode = true;
        display_history();
        return;
    }
    
    if (c == '\n') { // Enter key
        vga_newline();
        shell_process_command();
        shell_print_prompt();
        return;
    }
    
    if (c == '\b') { // Backspace
        if (shell_state.cursor_position > 0) {
            shell_state.cursor_position--;
            shell_state.input_length--;
            
            // Shift buffer left
            for (u16 i = shell_state.cursor_position; i < shell_state.input_length; i++) {
                shell_state.input_buffer[i] = shell_state.input_buffer[i + 1];
            }
            shell_state.input_buffer[shell_state.input_length] = '\0';
            
            vga_backspace();
        }
        return;
    }
    
    // Handle printable characters
    if (c >= 32 && c <= 126 && shell_state.input_length < SHELL_MAX_INPUT_LENGTH - 1) {
        // Shift buffer right to make space
        for (u16 i = shell_state.input_length; i > shell_state.cursor_position; i--) {
            shell_state.input_buffer[i] = shell_state.input_buffer[i - 1];
        }
        
        // Insert character
        shell_state.input_buffer[shell_state.cursor_position] = c;
        shell_state.cursor_position++;
        shell_state.input_length++;
        shell_state.input_buffer[shell_state.input_length] = '\0';
        
        vga_putchar(c);
    }
}

void shell_process_command() {
    if (shell_state.input_length == 0) {
        return;
    }
    
    // Add command to history
    add_to_history(shell_state.input_buffer);
    
    // Find command name (first word)
    char command_name[SHELL_MAX_COMMAND_LENGTH];
    u16 i = 0;
    u16 j = 0;
    
    // Skip leading spaces
    while (i < shell_state.input_length && shell_state.input_buffer[i] == ' ') {
        i++;
    }
    
    // Extract command name
    while (i < shell_state.input_length && shell_state.input_buffer[i] != ' ' && j < SHELL_MAX_COMMAND_LENGTH - 1) {
        command_name[j] = shell_state.input_buffer[i];
        i++;
        j++;
    }
    command_name[j] = '\0';
    
    // Extract arguments (rest of the line)
    char args[SHELL_MAX_INPUT_LENGTH];
    u16 args_start = i;
    u16 args_len = 0;
    
    // Skip spaces after command
    while (args_start < shell_state.input_length && shell_state.input_buffer[args_start] == ' ') {
        args_start++;
    }
    
    // Copy arguments
    while (args_start < shell_state.input_length && args_len < SHELL_MAX_INPUT_LENGTH - 1) {
        args[args_len] = shell_state.input_buffer[args_start];
        args_start++;
        args_len++;
    }
    args[args_len] = '\0';
    
    // Find and execute command
    bool command_found = false;
    for (u8 k = 0; k < command_count; k++) {
        if (str_equals(command_name, commands[k].name)) {
            commands[k].handler(args);
            command_found = true;
            break;
        }
    }
    
    if (!command_found) {
        shell_print_error("Command not found: ");
        vga_print(command_name);
        vga_newline();
    }
    
    // Clear input buffer
    shell_state.input_length = 0;
    shell_state.cursor_position = 0;
    for (i = 0; i < SHELL_MAX_INPUT_LENGTH; i++) {
        shell_state.input_buffer[i] = '\0';
    }
}

void shell_register_command(const char* name, void (*handler)(const char* args), const char* description) {
    if (command_count >= 16) {
        return; // Max commands reached
    }
    
    str_copy(commands[command_count].name, name);
    commands[command_count].handler = handler;
    str_copy(commands[command_count].description, description);
    command_count++;
}

void shell_print_prompt() {
    vga_print_color("shell> ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
}

void shell_print_error(const char* message) {
    vga_print_color(message, VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
}

void shell_print_info(const char* message) {
    vga_print_color(message, VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
}

shell_state_t* shell_get_state() {
    return &shell_state;
}
