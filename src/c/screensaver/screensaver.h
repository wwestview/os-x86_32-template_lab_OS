#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include "kernel/kernel.h"
#include "drivers/vga/vga.h"

// Forward declaration
struct keyboard_event;

// Screensaver types
typedef enum {
    SCREENSAVER_BOUNCING_BALL,
    SCREENSAVER_FALLING_STARS,
    SCREENSAVER_MATRIX_RAIN,
    SCREENSAVER_COUNT
} screensaver_type_t;

// Screensaver state
typedef struct {
    bool is_active;
    screensaver_type_t type;
    u32 animation_frame;
    u32 last_timer_tick;
    
    // Animation data
    u16 ball_x, ball_y;
    u16 ball_dx, ball_dy;
    
    // Matrix rain data
    char matrix_chars[80];
    u16 matrix_positions[80];
    u8 matrix_speeds[80];
    
    // Stars data
    u16 star_x[20];
    u16 star_y[20];
    u8 star_speed[20];
} screensaver_state_t;

// Initialize screensaver
void screensaver_init();

// Start screensaver
void screensaver_start(screensaver_type_t type);

// Stop screensaver
void screensaver_stop();

// Handle timer tick for animation
void screensaver_timer_tick();

// Handle keyboard input
void screensaver_handle_keyboard(struct keyboard_event event);

// Draw screensaver
void screensaver_draw();

// Check if screensaver is active
bool screensaver_is_active();

// Get screensaver state
screensaver_state_t* screensaver_get_state();

#endif
