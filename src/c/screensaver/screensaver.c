#include "screensaver/screensaver.h"
#include "drivers/keyboard/keyboard.h"

static screensaver_state_t screensaver_state;

void screensaver_init() {
    screensaver_state.is_active = false;
    screensaver_state.type = SCREENSAVER_BOUNCING_BALL;
    screensaver_state.animation_frame = 0;
    screensaver_state.last_timer_tick = 0;
    
    // Initialize ball animation
    screensaver_state.ball_x = 40;
    screensaver_state.ball_y = 12;
    screensaver_state.ball_dx = 1;
    screensaver_state.ball_dy = 1;
    
    // Initialize matrix rain
    for (u8 i = 0; i < 80; i++) {
        screensaver_state.matrix_chars[i] = 'A' + (i % 26);
        screensaver_state.matrix_positions[i] = 0;
        screensaver_state.matrix_speeds[i] = 1 + (i % 3);
    }
    
    // Initialize stars
    for (u8 i = 0; i < 20; i++) {
        screensaver_state.star_x[i] = i * 4;
        screensaver_state.star_y[i] = i * 2;
        screensaver_state.star_speed[i] = 1 + (i % 2);
    }
}

void screensaver_start(screensaver_type_t type) {
    screensaver_state.is_active = true;
    screensaver_state.type = type;
    screensaver_state.animation_frame = 0;
    screensaver_state.last_timer_tick = 0;
    
    // Reset animation data based on type
    switch (type) {
        case SCREENSAVER_BOUNCING_BALL:
            screensaver_state.ball_x = 40;
            screensaver_state.ball_y = 12;
            screensaver_state.ball_dx = 1;
            screensaver_state.ball_dy = 1;
            break;
            
        case SCREENSAVER_MATRIX_RAIN:
            for (u8 i = 0; i < 80; i++) {
                screensaver_state.matrix_positions[i] = 0;
                screensaver_state.matrix_speeds[i] = 1 + (i % 3);
            }
            break;
            
        case SCREENSAVER_FALLING_STARS:
            for (u8 i = 0; i < 20; i++) {
                screensaver_state.star_x[i] = i * 4;
                screensaver_state.star_y[i] = i * 2;
                screensaver_state.star_speed[i] = 1 + (i % 2);
            }
            break;
            
        default:
            break;
    }
    
    vga_clear();
}

void screensaver_stop() {
    screensaver_state.is_active = false;
    vga_clear();
}

void screensaver_timer_tick() {
    if (!screensaver_state.is_active) {
        return;
    }
    
    screensaver_state.animation_frame++;
    
    // Update animation every 5 timer ticks (slower animation)
    if (screensaver_state.animation_frame % 5 != 0) {
        return;
    }
    
    switch (screensaver_state.type) {
        case SCREENSAVER_BOUNCING_BALL:
            // Update ball position
            screensaver_state.ball_x += screensaver_state.ball_dx;
            screensaver_state.ball_y += screensaver_state.ball_dy;
            
            // Bounce off walls
            if (screensaver_state.ball_x <= 0 || screensaver_state.ball_x >= 79) {
                screensaver_state.ball_dx = -screensaver_state.ball_dx;
            }
            if (screensaver_state.ball_y <= 0 || screensaver_state.ball_y >= 24) {
                screensaver_state.ball_dy = -screensaver_state.ball_dy;
            }
            
            // Keep ball in bounds
            if (screensaver_state.ball_x < 0) screensaver_state.ball_x = 0;
            if (screensaver_state.ball_x > 79) screensaver_state.ball_x = 79;
            if (screensaver_state.ball_y < 0) screensaver_state.ball_y = 0;
            if (screensaver_state.ball_y > 24) screensaver_state.ball_y = 24;
            break;
            
        case SCREENSAVER_MATRIX_RAIN:
            // Update matrix rain
            for (u8 i = 0; i < 80; i++) {
                screensaver_state.matrix_positions[i] += screensaver_state.matrix_speeds[i];
                if (screensaver_state.matrix_positions[i] >= 25) {
                    screensaver_state.matrix_positions[i] = 0;
                    screensaver_state.matrix_chars[i] = 'A' + (screensaver_state.animation_frame + i) % 26;
                }
            }
            break;
            
        case SCREENSAVER_FALLING_STARS:
            // Update falling stars
            for (u8 i = 0; i < 20; i++) {
                screensaver_state.star_y[i] += screensaver_state.star_speed[i];
                if (screensaver_state.star_y[i] >= 25) {
                    screensaver_state.star_y[i] = 0;
                    screensaver_state.star_x[i] = (screensaver_state.animation_frame + i * 3) % 80;
                }
            }
            break;
            
        default:
            break;
    }
    
    screensaver_draw();
}

void screensaver_handle_keyboard(struct keyboard_event event) {
    if (!screensaver_state.is_active) {
        return;
    }
    
    // Any key press exits screensaver
    if (event.type == EVENT_KEY_PRESSED) {
        screensaver_stop();
    }
}

void screensaver_draw() {
    if (!screensaver_state.is_active) {
        return;
    }
    
    vga_clear();
    
    switch (screensaver_state.type) {
        case SCREENSAVER_BOUNCING_BALL:
            // Draw bouncing ball
            vga_set_cursor(screensaver_state.ball_x, screensaver_state.ball_y);
            vga_putchar_color('O', VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
            
            // Draw title
            vga_set_cursor(30, 0);
            vga_print_color("Bouncing Ball Screensaver", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            break;
            
        case SCREENSAVER_MATRIX_RAIN:
            // Draw matrix rain
            for (u8 i = 0; i < 80; i++) {
                if (screensaver_state.matrix_positions[i] < 25) {
                    vga_set_cursor(i, screensaver_state.matrix_positions[i]);
                    vga_putchar_color(screensaver_state.matrix_chars[i], VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
                }
            }
            
            // Draw title
            vga_set_cursor(32, 0);
            vga_print_color("Matrix Rain Screensaver", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            break;
            
        case SCREENSAVER_FALLING_STARS:
            // Draw falling stars
            for (u8 i = 0; i < 20; i++) {
                if (screensaver_state.star_y[i] < 25) {
                    vga_set_cursor(screensaver_state.star_x[i], screensaver_state.star_y[i]);
                    vga_putchar_color('*', VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
                }
            }
            
            // Draw title
            vga_set_cursor(32, 0);
            vga_print_color("Falling Stars Screensaver", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            break;
            
        default:
            break;
    }
    
    // Draw instructions
    vga_set_cursor(25, 24);
    vga_print_color("Press any key to exit screensaver", VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
}

bool screensaver_is_active() {
    return screensaver_state.is_active;
}

screensaver_state_t* screensaver_get_state() {
    return &screensaver_state;
}
