#include "shell/commands.h"
#include "shell/shell.h"
#include "drivers/vga/vga.h"
#include "filesystem/filesystem.h"
#include "editor/editor.h"
#include "command_editor/command_editor.h"


void cmd_help(const char* args) {
    // Print header and commands with minimal spacing so they appear closer together
    vga_print_color("Available commands:\n", VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_print_color("help - Show this help message\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("clear - Clear the screen\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("screensaver - Start the screensaver\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("create <name> - Create a new file\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("edit <name> - Edit an existing file\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("list - List all files in system\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("read <name> - Read a file's content\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("delete <name> - Delete a file\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_newline();
}

void cmd_clear(const char* args) {
    vga_clear();
}

void cmd_screensaver(const char* args) {
    // Start the default screensaver type
    screensaver_start(SCREENSAVER_SPACE_BATTLE);
}


void cmd_info(const char* args) {
    vga_newline();
    vga_print_color("System Information\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("==================\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("OS: Custom x86-32 Operating System\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("Architecture: x86-32\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("Shell: OS Shell v1.0\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("Display: VGA Text Mode (80x25)\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("Input: PS/2 Keyboard\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_newline();
}

void cmd_version(const char* args) {
    vga_newline();
    vga_print_color("OS Version: 1.0.0\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("Build Date: 2024\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print_color("Kernel: Custom x86-32 Kernel\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_newline();
}

// File system commands
void cmd_list(const char* args) {
    fs_list_files();
}

void cmd_create(const char* args) {
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: create <filename>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    if (fs_create_file(args)) {
        vga_print_color("File '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("' created successfully.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    } else {
        vga_print_color("Failed to create file '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("'. File may already exist or no space available.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}

void cmd_delete(const char* args) {
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: delete <filename>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    if (fs_delete_file(args)) {
        vga_print_color("File '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("' deleted successfully.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    } else {
        vga_print_color("Failed to delete file '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("'. File not found.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}

void cmd_new(const char* args) {
    vga_newline();
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: new <filename>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    if (editor_create_new_file(args)) {
        // Editor will handle its own display, so we don't need to print anything
    } else {
        vga_print_color("Failed to create file '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("' for editing.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}

void cmd_edit(const char* args) {
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: edit <filename>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    if (editor_open_file(args)) {
        // Editor will handle its own display, so we don't need to print anything
    } else {
        vga_print_color("Failed to open file '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("' for editing.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}

void cmd_save(const char* args) {
    vga_newline();
    if (editor_is_active()) {
        if (editor_save()) {
            vga_print_color("File saved successfully!\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        } else {
            vga_print_color("Failed to save file!\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        }
    } else {
        vga_print_color("No file is currently being edited.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print_color("Use 'new <filename>' or 'edit <filename>' to open a file.\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    }
}

void cmd_read(const char* args) {
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: read <filename>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char buffer[MAX_FILE_SIZE];
    if (fs_read_file(args, buffer, MAX_FILE_SIZE)) {
        vga_print_color("Content of '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("':\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print_color("================\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(buffer);
        vga_newline();
    } else {
        vga_print_color("File '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("' not found.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}

void cmd_view(const char* args) {
    vga_newline();
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: view <filename>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char buffer[MAX_FILE_SIZE];
    if (fs_read_file(args, buffer, MAX_FILE_SIZE)) {
        vga_print_color("Content of '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("':\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print_color("================\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(buffer);
        vga_newline();
    } else {
        vga_print_color("File '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("' not found.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}






void cmd_size(const char* args) {
    vga_newline();
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: size <filename>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    u16 size = fs_get_file_size(args);
    vga_print_color("File '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print(args);
    vga_print_color("' size: ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    
    char size_str[8];
    u16 pos = 0;
    if (size == 0) {
        size_str[pos++] = '0';
    } else {
        u16 temp = size;
        while (temp > 0) {
            size_str[pos++] = '0' + (temp % 10);
            temp /= 10;
        }
    }
    size_str[pos] = '\0';
    
    // Reverse the string
    for (u16 i = 0; i < pos / 2; i++) {
        char temp = size_str[i];
        size_str[i] = size_str[pos - 1 - i];
        size_str[pos - 1 - i] = temp;
    }
    
    vga_print(size_str);
    vga_print_color(" characters\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
}

void cmd_clear_content(const char* args) {
    vga_newline();
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: clear <filename>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    if (fs_clear_file(args)) {
        vga_print_color("Content of '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("' cleared successfully.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    } else {
        vga_print_color("Failed to clear '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(args);
        vga_print_color("'. File not found.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}




void commands_init() {
    // Register only the commands requested by the user
    shell_register_command("help", cmd_help, "Show help message");
    shell_register_command("clear", cmd_clear, "Clear the screen");
    shell_register_command("screensaver", cmd_screensaver, "Start the screensaver");

    // File management commands
    shell_register_command("list", cmd_list, "List files");
    shell_register_command("create", cmd_create, "Create a new file");
    shell_register_command("delete", cmd_delete, "Delete a file");
    shell_register_command("read", cmd_read, "Read file content");
    shell_register_command("edit", cmd_edit, "Edit an existing file");
    
}
