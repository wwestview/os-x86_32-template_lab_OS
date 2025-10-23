#ifndef VGA_H
#define VGA_H

#include "kernel/kernel.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_FRAMEBUFFER_ADDR 0xb8000

// VGA color codes
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN 14
#define VGA_COLOR_WHITE 15

// Default colors
#define VGA_DEFAULT_FG VGA_COLOR_LIGHT_GREY
#define VGA_DEFAULT_BG VGA_COLOR_BLACK

// VGA entry structure
typedef struct {
    u8 character;
    u8 color;
} vga_entry_t;

// Cursor position
typedef struct {
    u8 x;
    u8 y;
} cursor_pos_t;

// Initialize VGA driver
void vga_init();

// Clear screen
void vga_clear();

// Set cursor position
void vga_set_cursor(u8 x, u8 y);

// Get cursor position
cursor_pos_t vga_get_cursor();

// Disable hardware cursor
void vga_disable_cursor();

// Enable hardware cursor
void vga_enable_cursor(u8 start_line, u8 end_line);

// Print character at current cursor position
void vga_putchar(char c);

// Print character with specific color
void vga_putchar_color(char c, u8 fg_color, u8 bg_color);

// Print string
void vga_print(const char* str);

// Print string with color
void vga_print_color(const char* str, u8 fg_color, u8 bg_color);

// Print newline
void vga_newline();

// Scroll screen up by one line
void vga_scroll();

// Move cursor to beginning of next line
void vga_carriage_return();

// Backspace - move cursor back and clear character
void vga_backspace();

// Set text color
void vga_set_color(u8 fg_color, u8 bg_color);

// Get current text color
u8 vga_get_color();

#endif
