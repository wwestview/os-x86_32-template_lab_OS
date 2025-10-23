#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include "kernel/kernel.h"
#include "drivers/vga/vga.h"

// Forward declaration
struct keyboard_event;

// Screensaver types
typedef enum {
    SCREENSAVER_SPACE_BATTLE,
    SCREENSAVER_COUNT
} screensaver_type_t;

// Screensaver state
typedef struct {
    bool is_active;
    screensaver_type_t type;
    u32 animation_frame;
    u32 last_timer_tick;
    
    // Space battle data
    u16 spaceship_x, spaceship_y;
    u16 spaceship_direction; // 0=right, 1=left
    
    // Asteroids
    u16 asteroid_x[15];
    u16 asteroid_y[15];
    u8 asteroid_type[15]; // 0=small, 1=medium, 2=large
    u8 asteroid_speed[15];
    bool asteroid_active[15];
    
    // Lasers
    u16 laser_x[10];
    u16 laser_y[10];
    bool laser_active[10];
    u8 laser_frame[10];
    
    // Explosions
    u16 explosion_x[8];
    u16 explosion_y[8];
    u8 explosion_frame[8];
    bool explosion_active[8];
    
    // Stars background
    u16 star_x[50];
    u16 star_y[50];
    u8 star_brightness[50];
    
    // Score and game state
    u32 score;
    u16 lives;
    bool game_over;
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

// Check for inactivity and auto-start screensaver
void screensaver_check_inactivity();

// Reset inactivity timer
void screensaver_reset_timer();

// Get screensaver state
screensaver_state_t* screensaver_get_state();

#endif
