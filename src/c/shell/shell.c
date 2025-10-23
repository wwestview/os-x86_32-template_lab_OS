#include "shell/shell.h"
#include "shell/commands.h"
#include "drivers/vga/vga.h"
#include "filesystem/filesystem.h"
#include "command_editor/command_editor.h"
#include "screensaver/screensaver.h"

static shell_state_t shell_state;
static shell_command_t commands[16];
static u8 command_count = 0;

static bool str_equals(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return false;
        a++; b++;
    }
    return *a == *b;
}

/* History buffer and display removed. */

static void str_copy(char* dest, const char* src) {
    while (*src) *dest++ = *src++;
    *dest = '\0';
}

void shell_init() {
    shell_state.input_length = 0;
    shell_state.cursor_position = 0;
    shell_state.is_running = true;
    shell_state.just_exited_interactive = false;
    vga_init();
    fs_init();
    editor_init();
    cmd_editor_init();
    screensaver_init();
    vga_clear();
    vga_print_color("OS Shell v2.0\n", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_print_color("Type 'help' for commands.\n\n", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    shell_print_prompt();
}

void shell_run() { while (shell_state.is_running) halt(); }

void shell_handle_keyboard(struct keyboard_event event) {
    screensaver_reset_timer();
    if (shell_state.just_exited_interactive) { shell_state.just_exited_interactive = false; return; }
    if (screensaver_is_active()) { screensaver_handle_keyboard(event); return; }
    if (cmd_editor_is_active()) { cmd_editor_handle_keyboard(event); return; }
    if (editor_is_active()) { editor_handle_keyboard(event); return; }
    if (event.type != EVENT_KEY_PRESSED) return;
    char c = event.key_character;
    // History display removed; direct input handling continues.
    if (c == '\n') {
        vga_newline(); shell_process_command();
        if (!editor_is_active() && !cmd_editor_is_active() && !screensaver_is_active()) shell_print_prompt();
        return;
    }
    if (c == '\b') {
        if (shell_state.cursor_position > 0) {
            shell_state.cursor_position--; shell_state.input_length--;
            for (u16 i = shell_state.cursor_position; i < shell_state.input_length; i++) shell_state.input_buffer[i] = shell_state.input_buffer[i + 1];
            shell_state.input_buffer[shell_state.input_length] = '\0'; vga_backspace();
        }
        return;
    }
    if (c >= 32 && c <= 126 && shell_state.input_length < SHELL_MAX_INPUT_LENGTH - 1) {
        for (u16 i = shell_state.input_length; i > shell_state.cursor_position; i--) shell_state.input_buffer[i] = shell_state.input_buffer[i - 1];
        shell_state.input_buffer[shell_state.cursor_position] = c; shell_state.cursor_position++; shell_state.input_length++;
        shell_state.input_buffer[shell_state.input_length] = '\0'; vga_putchar(c);
    }
}

void shell_process_command() {
    if (shell_state.input_length == 0) return;
    (void)0; // history removed
    char command_name[SHELL_MAX_COMMAND_LENGTH]; u16 i = 0, j = 0;
    while (i < shell_state.input_length && shell_state.input_buffer[i] == ' ') i++;
    while (i < shell_state.input_length && shell_state.input_buffer[i] != ' ' && j < SHELL_MAX_COMMAND_LENGTH - 1) command_name[j++] = shell_state.input_buffer[i++];
    command_name[j] = '\0';
    char args[SHELL_MAX_INPUT_LENGTH]; u16 args_start = i, args_len = 0;
    while (args_start < shell_state.input_length && shell_state.input_buffer[args_start] == ' ') args_start++;
    while (args_start < shell_state.input_length && args_len < SHELL_MAX_INPUT_LENGTH - 1) args[args_len++] = shell_state.input_buffer[args_start++];
    args[args_len] = '\0';
    bool command_found = false;
    for (u8 k = 0; k < command_count; k++) {
        if (str_equals(command_name, commands[k].name)) { commands[k].handler(args); command_found = true; break; }
    }
    if (!command_found) { shell_print_error("Command not found: "); vga_print(command_name); vga_newline(); }
    shell_state.input_length = 0; shell_state.cursor_position = 0; for (i = 0; i < SHELL_MAX_INPUT_LENGTH; i++) shell_state.input_buffer[i] = '\0';
}

void shell_register_command(const char* name, void (*handler)(const char* args), const char* description) {
    if (command_count >= 16) return;
    str_copy(commands[command_count].name, name);
    commands[command_count].handler = handler;
    str_copy(commands[command_count].description, description);
    command_count++;
}

void shell_print_prompt() { vga_print_color("shell> ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK); }

void shell_check_and_scroll() {
    u16 x, y; vga_get_cursor(&x, &y);
    if (y >= VGA_HEIGHT - 1) shell_scroll_up();
}

void shell_scroll_up() {
    u16* framebuffer = (u16*)VGA_FRAMEBUFFER_ADDR;
    for (u8 y = 0; y < VGA_HEIGHT - 1; y++) for (u16 x = 0; x < VGA_WIDTH; x++) framebuffer[y * VGA_WIDTH + x] = framebuffer[(y + 1) * VGA_WIDTH + x];
    for (u16 x = 0; x < VGA_WIDTH; x++) { vga_set_cursor(x, VGA_HEIGHT - 1); vga_putchar_color(' ', VGA_COLOR_BLACK, VGA_COLOR_BLACK); }
    vga_set_cursor(0, VGA_HEIGHT - 1);
}

void shell_print_error(const char* message) { vga_print_color(message, VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK); }
void shell_print_info(const char* message) { vga_print_color(message, VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK); }
shell_state_t* shell_get_state() { return &shell_state; }

