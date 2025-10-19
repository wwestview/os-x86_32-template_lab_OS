#include "command_editor/command_editor.h"
#include "drivers/keyboard/keyboard.h"
#include "shell/shell.h"

static command_editor_state_t cmd_editor_state;

// Helper function to copy string
static void str_copy(char* dest, const char* src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
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

void cmd_editor_init() {
    cmd_editor_state.command_count = 0;
    cmd_editor_state.selected_command = 0;
    cmd_editor_state.start_display = 0;
    cmd_editor_state.max_visible_commands = 15; // Show 15 commands at once
    cmd_editor_state.is_active = false;
    cmd_editor_state.show_examples = false;
    
    // Initialize all commands
    str_copy(cmd_editor_state.commands[0].name, "help");
    str_copy(cmd_editor_state.commands[0].description, "Show help message");
    str_copy(cmd_editor_state.commands[0].example, "help");
    cmd_editor_state.commands[0].is_file_command = false;
    
    str_copy(cmd_editor_state.commands[1].name, "clear");
    str_copy(cmd_editor_state.commands[1].description, "Clear the screen");
    str_copy(cmd_editor_state.commands[1].example, "clear");
    cmd_editor_state.commands[1].is_file_command = false;
    
    str_copy(cmd_editor_state.commands[2].name, "echo");
    str_copy(cmd_editor_state.commands[2].description, "Print text to screen");
    str_copy(cmd_editor_state.commands[2].example, "echo Hello World");
    cmd_editor_state.commands[2].is_file_command = false;
    
    str_copy(cmd_editor_state.commands[3].name, "list");
    str_copy(cmd_editor_state.commands[3].description, "List files in memory");
    str_copy(cmd_editor_state.commands[3].example, "list");
    cmd_editor_state.commands[3].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[4].name, "create");
    str_copy(cmd_editor_state.commands[4].description, "Create a new file");
    str_copy(cmd_editor_state.commands[4].example, "create myfile.txt");
    cmd_editor_state.commands[4].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[5].name, "delete");
    str_copy(cmd_editor_state.commands[5].description, "Delete a file");
    str_copy(cmd_editor_state.commands[5].example, "delete myfile.txt");
    cmd_editor_state.commands[5].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[6].name, "view");
    str_copy(cmd_editor_state.commands[6].description, "View file content");
    str_copy(cmd_editor_state.commands[6].example, "view myfile.txt");
    cmd_editor_state.commands[6].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[7].name, "write");
    str_copy(cmd_editor_state.commands[7].description, "Write content to file");
    str_copy(cmd_editor_state.commands[7].example, "write myfile.txt Hello World");
    cmd_editor_state.commands[7].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[8].name, "append");
    str_copy(cmd_editor_state.commands[8].description, "Append content to file");
    str_copy(cmd_editor_state.commands[8].example, "append myfile.txt More text");
    cmd_editor_state.commands[8].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[9].name, "insert");
    str_copy(cmd_editor_state.commands[9].description, "Insert content at position");
    str_copy(cmd_editor_state.commands[9].example, "insert myfile.txt 5 new");
    cmd_editor_state.commands[9].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[10].name, "del");
    str_copy(cmd_editor_state.commands[10].description, "Delete content from position");
    str_copy(cmd_editor_state.commands[10].example, "del myfile.txt 0 5");
    cmd_editor_state.commands[10].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[11].name, "replace");
    str_copy(cmd_editor_state.commands[11].description, "Replace text in file");
    str_copy(cmd_editor_state.commands[11].example, "replace myfile.txt old new");
    cmd_editor_state.commands[11].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[12].name, "size");
    str_copy(cmd_editor_state.commands[12].description, "Show file size");
    str_copy(cmd_editor_state.commands[12].example, "size myfile.txt");
    cmd_editor_state.commands[12].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[13].name, "clearfile");
    str_copy(cmd_editor_state.commands[13].description, "Clear file content");
    str_copy(cmd_editor_state.commands[13].example, "clearfile myfile.txt");
    cmd_editor_state.commands[13].is_file_command = true;
    
    str_copy(cmd_editor_state.commands[14].name, "info");
    str_copy(cmd_editor_state.commands[14].description, "Show system information");
    str_copy(cmd_editor_state.commands[14].example, "info");
    cmd_editor_state.commands[14].is_file_command = false;
    
    str_copy(cmd_editor_state.commands[15].name, "version");
    str_copy(cmd_editor_state.commands[15].description, "Show OS version");
    str_copy(cmd_editor_state.commands[15].example, "version");
    cmd_editor_state.commands[15].is_file_command = false;
    
    cmd_editor_state.command_count = 16;
}

void cmd_editor_start() {
    cmd_editor_state.is_active = true;
    cmd_editor_state.selected_command = 0;
    cmd_editor_state.start_display = 0;
    cmd_editor_draw();
}

void cmd_editor_handle_keyboard(struct keyboard_event event) {
    if (!cmd_editor_state.is_active || event.type != EVENT_KEY_PRESSED) {
        return;
    }
    
    switch (event.key) {
        case KEY_ESC:
            cmd_editor_exit();
            break;
            
        case KEY_UP:
            if (cmd_editor_state.selected_command > 0) {
                cmd_editor_state.selected_command--;
            } else {
                // Wrap to last command
                cmd_editor_state.selected_command = cmd_editor_state.command_count - 1;
                cmd_editor_state.start_display = cmd_editor_state.selected_command - cmd_editor_state.max_visible_commands + 1;
                if (cmd_editor_state.start_display > cmd_editor_state.selected_command) {
                    cmd_editor_state.start_display = 0;
                }
            }
            // Scroll if needed
            if (cmd_editor_state.selected_command < cmd_editor_state.start_display) {
                cmd_editor_state.start_display = cmd_editor_state.selected_command;
            }
            cmd_editor_draw();
            break;
            
        case KEY_DOWN:
            if (cmd_editor_state.selected_command < cmd_editor_state.command_count - 1) {
                cmd_editor_state.selected_command++;
            } else {
                // Wrap to first command
                cmd_editor_state.selected_command = 0;
                cmd_editor_state.start_display = 0;
            }
            // Scroll if needed
            if (cmd_editor_state.selected_command >= cmd_editor_state.start_display + cmd_editor_state.max_visible_commands) {
                cmd_editor_state.start_display = cmd_editor_state.selected_command - cmd_editor_state.max_visible_commands + 1;
            }
            cmd_editor_draw();
            break;
            
        case KEY_ENTER:
            cmd_editor_execute_command();
            break;
            
        case KEY_TAB:
            cmd_editor_state.show_examples = !cmd_editor_state.show_examples;
            cmd_editor_draw();
            break;
            
        case KEY_W:
            // Alternative navigation with W key
            if (cmd_editor_state.selected_command > 0) {
                cmd_editor_state.selected_command--;
            } else {
                // Wrap to last command
                cmd_editor_state.selected_command = cmd_editor_state.command_count - 1;
                cmd_editor_state.start_display = cmd_editor_state.selected_command - cmd_editor_state.max_visible_commands + 1;
                if (cmd_editor_state.start_display > cmd_editor_state.selected_command) {
                    cmd_editor_state.start_display = 0;
                }
            }
            // Scroll if needed
            if (cmd_editor_state.selected_command < cmd_editor_state.start_display) {
                cmd_editor_state.start_display = cmd_editor_state.selected_command;
            }
            cmd_editor_draw();
            break;
            
        case KEY_S:
            // Alternative navigation with S key
            if (cmd_editor_state.selected_command < cmd_editor_state.command_count - 1) {
                cmd_editor_state.selected_command++;
            } else {
                // Wrap to first command
                cmd_editor_state.selected_command = 0;
                cmd_editor_state.start_display = 0;
            }
            // Scroll if needed
            if (cmd_editor_state.selected_command >= cmd_editor_state.start_display + cmd_editor_state.max_visible_commands) {
                cmd_editor_state.start_display = cmd_editor_state.selected_command - cmd_editor_state.max_visible_commands + 1;
            }
            cmd_editor_draw();
            break;
            
        default:
            break;
    }
}

void cmd_editor_draw() {
    if (!cmd_editor_state.is_active) {
        return;
    }
    
    vga_clear();
    
    // Draw header
    vga_print_color("Command Editor", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color(" (ESC=exit, UP/DOWN or W/S=navigate, ENTER=execute, TAB=toggle examples)", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_newline();
    
    // Draw separator
    for (u16 i = 0; i < 80; i++) {
        vga_putchar_color('-', VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
    vga_newline();
    
    // Draw commands with scrolling
    u8 max_display = cmd_editor_state.start_display + cmd_editor_state.max_visible_commands;
    if (max_display > cmd_editor_state.command_count) {
        max_display = cmd_editor_state.command_count;
    }
    
    for (u8 i = cmd_editor_state.start_display; i < max_display; i++) {
        // Highlight selected command
        if (i == cmd_editor_state.selected_command) {
            vga_print_color("> ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        } else {
            vga_print_color("  ", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        }
        
        // Command name
        vga_print_color(cmd_editor_state.commands[i].name, VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        
        // Command type indicator
        if (cmd_editor_state.commands[i].is_file_command) {
            vga_print_color(" [FILE]", VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
        } else {
            vga_print_color(" [SYS]", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
        
        // Description
        vga_print_color(" - ", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_print_color(cmd_editor_state.commands[i].description, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        
        vga_newline();
        
        // Show example if enabled
        if (cmd_editor_state.show_examples) {
            vga_print_color("    Example: ", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
            vga_print_color(cmd_editor_state.commands[i].example, VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
            vga_newline();
        }
    }
    
    // Draw footer
    vga_newline();
    vga_print_color("Selected: ", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print_color(cmd_editor_state.commands[cmd_editor_state.selected_command].name, VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color(" (", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    
    char pos_str[8];
    u16 pos_num = cmd_editor_state.selected_command + 1;
    u16 pos_len = 0;
    if (pos_num < 10) {
        pos_str[pos_len++] = ' ';
    }
    u16 temp = pos_num;
    while (temp > 0) {
        pos_str[pos_len++] = '0' + (temp % 10);
        temp /= 10;
    }
    pos_str[pos_len] = '\0';
    
    // Reverse the string
    for (u16 i = 0; i < pos_len / 2; i++) {
        char temp = pos_str[i];
        pos_str[i] = pos_str[pos_len - 1 - i];
        pos_str[pos_len - 1 - i] = temp;
    }
    
    vga_print(pos_str);
    vga_print_color("/", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    
    char total_str[8];
    u16 total_num = cmd_editor_state.command_count;
    u16 total_len = 0;
    if (total_num < 10) {
        total_str[total_len++] = ' ';
    }
    temp = total_num;
    while (temp > 0) {
        total_str[total_len++] = '0' + (temp % 10);
        temp /= 10;
    }
    total_str[total_len] = '\0';
    
    // Reverse the string
    for (u16 i = 0; i < total_len / 2; i++) {
        char temp = total_str[i];
        total_str[i] = total_str[total_len - 1 - i];
        total_str[total_len - 1 - i] = temp;
    }
    
    vga_print(total_str);
    vga_print_color(")", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    
    // Show scroll indicator
    if (cmd_editor_state.command_count > cmd_editor_state.max_visible_commands) {
        vga_print_color(" | Scroll: ", VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
        if (cmd_editor_state.start_display > 0) {
            vga_print_color("↑", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        } else {
            vga_print_color(" ", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
        vga_print_color(" ", VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
        if (cmd_editor_state.start_display + cmd_editor_state.max_visible_commands < cmd_editor_state.command_count) {
            vga_print_color("↓", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        } else {
            vga_print_color(" ", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        }
    }
}

void cmd_editor_execute_command() {
    if (!cmd_editor_state.is_active) {
        return;
    }
    
    // Get selected command
    command_info_t* selected = &cmd_editor_state.commands[cmd_editor_state.selected_command];
    
    // Exit editor first
    cmd_editor_exit();
    
    // Print command execution message
    vga_print_color("Executing: ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color(selected->name, VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_newline();
    
    // Execute the command (simulate by printing the command)
    vga_print_color("Command: ", VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_print(selected->example);
    vga_newline();
    
    // Note: In a real implementation, you would call the actual command function here
    // For now, we just show what would be executed
    vga_print_color("(Command executed - in real implementation this would run the actual command)", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_newline();
}

void cmd_editor_exit() {
    cmd_editor_state.is_active = false;
    vga_clear();
    vga_print_color("Command editor closed.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
}

bool cmd_editor_is_active() {
    return cmd_editor_state.is_active;
}

command_editor_state_t* cmd_editor_get_state() {
    return &cmd_editor_state;
}
