#include "shell/commands.h"
#include "drivers/vga/vga.h"
#include "filesystem/filesystem.h"
#include "editor/editor.h"
#include "command_editor/command_editor.h"

// Helper function to print all available commands
static void print_command_list() {
    vga_print_color("Available commands:\n", VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_print_color("  help     - Show this help message\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  clear    - Clear the screen\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  echo     - Print text to screen\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  info     - Show system information\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  version  - Show OS version\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("\nFile operations:\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("  list     - List files in memory\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  create   - Create a new file\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  delete   - Delete a file\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  view     - View file content\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  size     - Show file size\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("\nContent manipulation:\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print_color("  write    - Write content to file\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  append   - Append content to file\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  insert   - Insert content at position\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  del      - Delete content from position\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  replace  - Replace text in file\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("  clearfile - Clear file content\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print_color("\nInteractive:\n", VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_print_color("  edit     - Open interactive command editor\n", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void cmd_help(const char* args) {
    vga_newline();
    vga_print_color("OS Shell Help\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("=============\n\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    
    print_command_list();
    
    vga_newline();
    vga_print_color("Usage: command [arguments]\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print_color("Use backspace to edit your input before pressing Enter.\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
}

void cmd_clear(const char* args) {
    vga_clear();
}

void cmd_echo(const char* args) {
    vga_newline();
    if (args && args[0] != '\0') {
        vga_print(args);
    }
    vga_newline();
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
    vga_newline();
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
    vga_newline();
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

void cmd_edit(const char* args) {
    vga_newline();
    vga_print_color("Opening command editor...\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("Use arrow keys to navigate, ENTER to execute, ESC to exit.\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    cmd_editor_start();
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

// File content manipulation commands
void cmd_write(const char* args) {
    vga_newline();
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: write <filename> <content>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    // Find filename and content
    u16 i = 0;
    while (args[i] && args[i] != ' ') i++;
    if (!args[i]) {
        vga_print_color("Usage: write <filename> <content>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char filename[32];
    u16 j = 0;
    while (j < i && j < 31) {
        filename[j] = args[j];
        j++;
    }
    filename[j] = '\0';
    
    // Skip spaces
    i++;
    while (args[i] && args[i] == ' ') i++;
    
    if (fs_write_file(filename, &args[i])) {
        vga_print_color("Content written to '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("' successfully.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    } else {
        vga_print_color("Failed to write to '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("'. File not found or content too large.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}

void cmd_append(const char* args) {
    vga_newline();
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: append <filename> <content>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    // Find filename and content
    u16 i = 0;
    while (args[i] && args[i] != ' ') i++;
    if (!args[i]) {
        vga_print_color("Usage: append <filename> <content>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char filename[32];
    u16 j = 0;
    while (j < i && j < 31) {
        filename[j] = args[j];
        j++;
    }
    filename[j] = '\0';
    
    // Skip spaces
    i++;
    while (args[i] && args[i] == ' ') i++;
    
    if (fs_append_file(filename, &args[i])) {
        vga_print_color("Content appended to '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("' successfully.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    } else {
        vga_print_color("Failed to append to '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("'. File not found or content too large.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}

void cmd_insert(const char* args) {
    vga_newline();
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: insert <filename> <position> <content>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    // Find filename
    u16 i = 0;
    while (args[i] && args[i] != ' ') i++;
    if (!args[i]) {
        vga_print_color("Usage: insert <filename> <position> <content>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char filename[32];
    u16 j = 0;
    while (j < i && j < 31) {
        filename[j] = args[j];
        j++;
    }
    filename[j] = '\0';
    
    // Skip spaces and find position
    i++;
    while (args[i] && args[i] == ' ') i++;
    u16 pos_start = i;
    while (args[i] && args[i] != ' ') i++;
    if (!args[i]) {
        vga_print_color("Usage: insert <filename> <position> <content>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char pos_str[8];
    u16 k = 0;
    while (pos_start < i && k < 7) {
        pos_str[k] = args[pos_start];
        pos_start++;
        k++;
    }
    pos_str[k] = '\0';
    u16 position = 0;
    for (u16 l = 0; l < k; l++) {
        position = position * 10 + (pos_str[l] - '0');
    }
    
    // Skip spaces and get content
    i++;
    while (args[i] && args[i] == ' ') i++;
    
    if (fs_insert_at_position(filename, &args[i], position)) {
        vga_print_color("Content inserted at position ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(pos_str);
        vga_print_color(" in '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("' successfully.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    } else {
        vga_print_color("Failed to insert content in '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("'.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}

void cmd_delete_content(const char* args) {
    vga_newline();
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: del <filename> <position> <length>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    // Find filename
    u16 i = 0;
    while (args[i] && args[i] != ' ') i++;
    if (!args[i]) {
        vga_print_color("Usage: del <filename> <position> <length>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char filename[32];
    u16 j = 0;
    while (j < i && j < 31) {
        filename[j] = args[j];
        j++;
    }
    filename[j] = '\0';
    
    // Skip spaces and find position
    i++;
    while (args[i] && args[i] == ' ') i++;
    u16 pos_start = i;
    while (args[i] && args[i] != ' ') i++;
    if (!args[i]) {
        vga_print_color("Usage: del <filename> <position> <length>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char pos_str[8];
    u16 k = 0;
    while (pos_start < i && k < 7) {
        pos_str[k] = args[pos_start];
        pos_start++;
        k++;
    }
    pos_str[k] = '\0';
    u16 position = 0;
    for (u16 l = 0; l < k; l++) {
        position = position * 10 + (pos_str[l] - '0');
    }
    
    // Skip spaces and find length
    i++;
    while (args[i] && args[i] == ' ') i++;
    u16 len_start = i;
    while (args[i] && args[i] != ' ') i++;
    
    char len_str[8];
    k = 0;
    while (len_start < i && k < 7) {
        len_str[k] = args[len_start];
        len_start++;
        k++;
    }
    len_str[k] = '\0';
    u16 length = 0;
    for (u16 l = 0; l < k; l++) {
        length = length * 10 + (len_str[l] - '0');
    }
    
    if (fs_delete_from_position(filename, position, length)) {
        vga_print_color("Deleted ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(len_str);
        vga_print_color(" characters from position ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(pos_str);
        vga_print_color(" in '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("' successfully.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    } else {
        vga_print_color("Failed to delete content from '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("'.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    }
}

void cmd_replace(const char* args) {
    vga_newline();
    if (!args || args[0] == '\0') {
        vga_print_color("Usage: replace <filename> <old_text> <new_text>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    // Find filename
    u16 i = 0;
    while (args[i] && args[i] != ' ') i++;
    if (!args[i]) {
        vga_print_color("Usage: replace <filename> <old_text> <new_text>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char filename[32];
    u16 j = 0;
    while (j < i && j < 31) {
        filename[j] = args[j];
        j++;
    }
    filename[j] = '\0';
    
    // Skip spaces and find old_text
    i++;
    while (args[i] && args[i] == ' ') i++;
    u16 old_start = i;
    while (args[i] && args[i] != ' ') i++;
    if (!args[i]) {
        vga_print_color("Usage: replace <filename> <old_text> <new_text>\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        return;
    }
    
    char old_text[256];
    u16 k = 0;
    while (old_start < i && k < 255) {
        old_text[k] = args[old_start];
        old_start++;
        k++;
    }
    old_text[k] = '\0';
    
    // Skip spaces and get new_text
    i++;
    while (args[i] && args[i] == ' ') i++;
    
    char new_text[256];
    k = 0;
    while (args[i] && k < 255) {
        new_text[k] = args[i];
        i++;
        k++;
    }
    new_text[k] = '\0';
    
    if (fs_replace_content(filename, old_text, new_text)) {
        vga_print_color("Replaced '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(old_text);
        vga_print_color("' with '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(new_text);
        vga_print_color("' in '", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("' successfully.\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    } else {
        vga_print_color("Failed to replace text in '", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print_color("'. Text not found or operation failed.\n", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
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
    shell_register_command("help", cmd_help, "Show help message");
    shell_register_command("clear", cmd_clear, "Clear the screen");
    shell_register_command("echo", cmd_echo, "Print text to screen");
    shell_register_command("info", cmd_info, "Show system information");
    shell_register_command("version", cmd_version, "Show OS version");
    shell_register_command("list", cmd_list, "List files in memory");
    shell_register_command("create", cmd_create, "Create a new file");
    shell_register_command("delete", cmd_delete, "Delete a file");
    shell_register_command("edit", cmd_edit, "Edit a file");
    shell_register_command("view", cmd_view, "View file content");
    shell_register_command("write", cmd_write, "Write content to file");
    shell_register_command("append", cmd_append, "Append content to file");
    shell_register_command("insert", cmd_insert, "Insert content at position");
    shell_register_command("del", cmd_delete_content, "Delete content from position");
    shell_register_command("replace", cmd_replace, "Replace text in file");
    shell_register_command("size", cmd_size, "Show file size");
    shell_register_command("clearfile", cmd_clear_content, "Clear file content");
}
