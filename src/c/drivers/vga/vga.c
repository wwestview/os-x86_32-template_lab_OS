#include "vga.h"

static cursor_pos_t cursor = {0, 0};
static u8 current_color = ((VGA_DEFAULT_FG) | ((VGA_DEFAULT_BG) << 4));

// Helper macro to create color byte
#define VGA_COLOR_MAKE(fg, bg) ((fg) | ((bg) << 4))

// Helper function to get VGA entry
static inline vga_entry_t vga_make_entry(char c, u8 color) {
    vga_entry_t entry;
    entry.character = (u8)c;
    entry.color = color;
    return entry;
}

// Helper function to get VGA entry index
static inline u16 vga_entry_index(u8 x, u8 y) {
    return y * VGA_WIDTH + x;
}

void vga_init() {
    vga_clear();
    vga_set_cursor(0, 0);
}

void vga_clear() {
    vga_entry_t* framebuffer = (vga_entry_t*)VGA_FRAMEBUFFER_ADDR;
    vga_entry_t blank = vga_make_entry(' ', current_color);
    
    for (u16 i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        framebuffer[i] = blank;
    }
    
    cursor.x = 0;
    cursor.y = 0;
}

void vga_set_cursor(u8 x, u8 y) {
    if (x >= VGA_WIDTH) x = VGA_WIDTH - 1;
    if (y >= VGA_HEIGHT) y = VGA_HEIGHT - 1;
    
    cursor.x = x;
    cursor.y = y;
    
    u16 pos = vga_entry_index(x, y);
    out(0x3D4, 14);
    out(0x3D5, ((pos >> 8) & 0x00FF));
    out(0x3D4, 15);
    out(0x3D5, pos & 0x00FF);
}

cursor_pos_t vga_get_cursor() {
    return cursor;
}

void vga_putchar(char c) {
    vga_putchar_color(c, VGA_DEFAULT_FG, VGA_DEFAULT_BG);
}

void vga_putchar_color(char c, u8 fg_color, u8 bg_color) {
    u8 color = VGA_COLOR_MAKE(fg_color, bg_color);
    
    if (c == '\n') {
        vga_newline();
        return;
    }
    
    if (c == '\r') {
        vga_carriage_return();
        return;
    }
    
    if (c == '\b') {
        vga_backspace();
        return;
    }
    
    // Handle printable characters
    if (c >= 32 && c <= 126) {
        vga_entry_t* framebuffer = (vga_entry_t*)VGA_FRAMEBUFFER_ADDR;
        u16 index = vga_entry_index(cursor.x, cursor.y);
        framebuffer[index] = vga_make_entry(c, color);
        
        cursor.x++;
        if (cursor.x >= VGA_WIDTH) {
            vga_newline();
        }
    }
    
    vga_set_cursor(cursor.x, cursor.y);
}

void vga_print(const char* str) {
    vga_print_color(str, VGA_DEFAULT_FG, VGA_DEFAULT_BG);
}

void vga_print_color(const char* str, u8 fg_color, u8 bg_color) {
    while (*str) {
        vga_putchar_color(*str, fg_color, bg_color);
        str++;
    }
}

void vga_newline() {
    cursor.x = 0;
    cursor.y++;
    
    if (cursor.y >= VGA_HEIGHT) {
        vga_scroll();
        cursor.y = VGA_HEIGHT - 1;
    }
    
    vga_set_cursor(cursor.x, cursor.y);
}

void vga_scroll() {
    vga_entry_t* framebuffer = (vga_entry_t*)VGA_FRAMEBUFFER_ADDR;
    
    // Move all lines up by one
    for (u8 y = 0; y < VGA_HEIGHT - 1; y++) {
        for (u8 x = 0; x < VGA_WIDTH; x++) {
            u16 src_index = vga_entry_index(x, y + 1);
            u16 dst_index = vga_entry_index(x, y);
            framebuffer[dst_index] = framebuffer[src_index];
        }
    }
    
    // Clear the last line
    vga_entry_t blank = vga_make_entry(' ', current_color);
    for (u8 x = 0; x < VGA_WIDTH; x++) {
        u16 index = vga_entry_index(x, VGA_HEIGHT - 1);
        framebuffer[index] = blank;
    }
}

void vga_carriage_return() {
    cursor.x = 0;
    vga_set_cursor(cursor.x, cursor.y);
}

void vga_backspace() {
    if (cursor.x > 0) {
        cursor.x--;
        vga_entry_t* framebuffer = (vga_entry_t*)VGA_FRAMEBUFFER_ADDR;
        u16 index = vga_entry_index(cursor.x, cursor.y);
        framebuffer[index] = vga_make_entry(' ', current_color);
        vga_set_cursor(cursor.x, cursor.y);
    }
}

void vga_set_color(u8 fg_color, u8 bg_color) {
    current_color = VGA_COLOR_MAKE(fg_color, bg_color);
}

u8 vga_get_color() {
    return current_color;
}
