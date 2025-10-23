#include "screensaver/screensaver.h"
#include "drivers/keyboard/keyboard.h"
#include "shell/shell.h"

static screensaver_state_t screensaver_state;
static u32 inactivity_timer = 0;
static const u32 INACTIVITY_TIMEOUT = 350; // 7 seconds (350 timer ticks at ~20ms each)

void screensaver_init() {
    screensaver_state.is_active = false;
    screensaver_state.type = SCREENSAVER_SPACE_BATTLE;
    screensaver_state.animation_frame = 0;
    screensaver_state.last_timer_tick = 0;
    
    // Initialize space battle
    screensaver_state.spaceship_x = 40;
    screensaver_state.spaceship_y = 20;
    screensaver_state.spaceship_direction = 0;
    
    // Initialize asteroids
    for (u8 i = 0; i < 15; i++) {
        screensaver_state.asteroid_x[i] = 0;
        screensaver_state.asteroid_y[i] = 0;
        screensaver_state.asteroid_type[i] = 0;
        screensaver_state.asteroid_speed[i] = 0;
        screensaver_state.asteroid_active[i] = false;
    }
    
    // Initialize lasers
    for (u8 i = 0; i < 10; i++) {
        screensaver_state.laser_x[i] = 0;
        screensaver_state.laser_y[i] = 0;
        screensaver_state.laser_active[i] = false;
        screensaver_state.laser_frame[i] = 0;
    }
    
    // Initialize explosions
    for (u8 i = 0; i < 8; i++) {
        screensaver_state.explosion_x[i] = 0;
        screensaver_state.explosion_y[i] = 0;
        screensaver_state.explosion_frame[i] = 0;
        screensaver_state.explosion_active[i] = false;
    }
    
    // Initialize stars
    for (u8 i = 0; i < 50; i++) {
        screensaver_state.star_x[i] = (i * 7) % 80;
        screensaver_state.star_y[i] = (i * 3) % 25;
        screensaver_state.star_brightness[i] = 1 + (i % 3);
    }
    
    // Initialize game state
    screensaver_state.score = 0;
    screensaver_state.lives = 3;
    screensaver_state.game_over = false;
}

void screensaver_start(screensaver_type_t type) {
    screensaver_state.is_active = true;
    screensaver_state.type = type;
    screensaver_state.animation_frame = 0;
    screensaver_state.last_timer_tick = 0;
    
    // Reset space battle
    screensaver_state.spaceship_x = 40;
    screensaver_state.spaceship_y = 20;
    screensaver_state.spaceship_direction = 0;
    
    // Reset asteroids
    for (u8 i = 0; i < 15; i++) {
        screensaver_state.asteroid_active[i] = false;
    }
    
    // Reset lasers
    for (u8 i = 0; i < 10; i++) {
        screensaver_state.laser_active[i] = false;
    }
    
    // Reset explosions
    for (u8 i = 0; i < 8; i++) {
        screensaver_state.explosion_active[i] = false;
    }
    
    // Reset game state
    screensaver_state.score = 0;
    screensaver_state.lives = 3;
    screensaver_state.game_over = false;
    
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
    
    // Update animation every 3 timer ticks (faster animation)
    if (screensaver_state.animation_frame % 3 != 0) {
        return;
    }
    
    switch (screensaver_state.type) {
        case SCREENSAVER_SPACE_BATTLE:
            // Don't update animation if game is over
            if (screensaver_state.game_over) {
                break;
            }
            
            // Update spaceship movement
            if (screensaver_state.spaceship_direction == 0) {
                screensaver_state.spaceship_x++;
                if (screensaver_state.spaceship_x >= 75) {
                    screensaver_state.spaceship_direction = 1;
                }
            } else {
                screensaver_state.spaceship_x--;
                if (screensaver_state.spaceship_x <= 5) {
                    screensaver_state.spaceship_direction = 0;
                }
            }
            
            // Spawn asteroids
            if (screensaver_state.animation_frame % 15 == 0) {
                for (u8 i = 0; i < 15; i++) {
                    if (!screensaver_state.asteroid_active[i]) {
                        screensaver_state.asteroid_x[i] = 5 + (screensaver_state.animation_frame % 70);
                        screensaver_state.asteroid_y[i] = 2;
                        screensaver_state.asteroid_type[i] = (screensaver_state.animation_frame + i) % 3;
                        screensaver_state.asteroid_speed[i] = 1 + (screensaver_state.animation_frame % 3);
                        screensaver_state.asteroid_active[i] = true;
                        break;
                    }
                }
            }
            
            // Update asteroids
            for (u8 i = 0; i < 15; i++) {
                if (screensaver_state.asteroid_active[i]) {
                    screensaver_state.asteroid_y[i] += screensaver_state.asteroid_speed[i];
                    
                    // Check collision with spaceship
                    if (screensaver_state.asteroid_y[i] >= screensaver_state.spaceship_y - 1 &&
                        screensaver_state.asteroid_y[i] <= screensaver_state.spaceship_y + 1 &&
                        screensaver_state.asteroid_x[i] >= screensaver_state.spaceship_x - 2 &&
                        screensaver_state.asteroid_x[i] <= screensaver_state.spaceship_x + 2) {
                        
                        // Create explosion
                        for (u8 j = 0; j < 8; j++) {
                            if (!screensaver_state.explosion_active[j]) {
                                screensaver_state.explosion_x[j] = screensaver_state.asteroid_x[i];
                                screensaver_state.explosion_y[j] = screensaver_state.asteroid_y[i];
                                screensaver_state.explosion_frame[j] = 0;
                                screensaver_state.explosion_active[j] = true;
                                break;
                            }
                        }
                        
                        screensaver_state.asteroid_active[i] = false;
                        screensaver_state.lives--;
                        if (screensaver_state.lives <= 0) {
                            screensaver_state.game_over = true;
                        } else {
                            // Respawn spaceship after collision
                            screensaver_state.spaceship_x = 40;
                            screensaver_state.spaceship_y = 20;
                            screensaver_state.spaceship_direction = 0;
                        }
                    }
                    
                    // Remove asteroids that went off screen
                    if (screensaver_state.asteroid_y[i] >= 25) {
                        screensaver_state.asteroid_active[i] = false;
                        screensaver_state.score += 1;
                    }
                }
            }
            
            // Auto-shoot lasers
            if (screensaver_state.animation_frame % 8 == 0) {
                for (u8 i = 0; i < 10; i++) {
                    if (!screensaver_state.laser_active[i]) {
                        screensaver_state.laser_x[i] = screensaver_state.spaceship_x;
                        screensaver_state.laser_y[i] = screensaver_state.spaceship_y - 1;
                        screensaver_state.laser_frame[i] = 0;
                        screensaver_state.laser_active[i] = true;
                        break;
                    }
                }
            }
            
            // Update lasers
            for (u8 i = 0; i < 10; i++) {
                if (screensaver_state.laser_active[i]) {
                    screensaver_state.laser_y[i]--;
                    screensaver_state.laser_frame[i]++;
                    
                    // Check collision with asteroids
                    for (u8 j = 0; j < 15; j++) {
                        if (screensaver_state.asteroid_active[j] &&
                            screensaver_state.laser_y[i] == screensaver_state.asteroid_y[j] &&
                            screensaver_state.laser_x[i] == screensaver_state.asteroid_x[j]) {
                            
                            // Create explosion
                            for (u8 k = 0; k < 8; k++) {
                                if (!screensaver_state.explosion_active[k]) {
                                    screensaver_state.explosion_x[k] = screensaver_state.asteroid_x[j];
                                    screensaver_state.explosion_y[k] = screensaver_state.asteroid_y[j];
                                    screensaver_state.explosion_frame[k] = 0;
                                    screensaver_state.explosion_active[k] = true;
                                    break;
                                }
                            }
                            
                            screensaver_state.asteroid_active[j] = false;
                            screensaver_state.laser_active[i] = false;
                            screensaver_state.score += 10;
            break;
                        }
                    }
                    
                    // Remove lasers that went off screen
                    if (screensaver_state.laser_y[i] <= 0) {
                        screensaver_state.laser_active[i] = false;
                    }
                }
            }
            
            // Update explosions
            for (u8 i = 0; i < 8; i++) {
                if (screensaver_state.explosion_active[i]) {
                    screensaver_state.explosion_frame[i]++;
                    if (screensaver_state.explosion_frame[i] >= 8) {
                        screensaver_state.explosion_active[i] = false;
                    }
                }
            }
            
            // Update stars (twinkling effect)
            for (u8 i = 0; i < 50; i++) {
                if (screensaver_state.animation_frame % (10 + i) == 0) {
                    screensaver_state.star_brightness[i] = 1 + (screensaver_state.animation_frame % 3);
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
        // Print shell prompt after exiting screensaver
        vga_print_color("shell> ", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        // Set flag to indicate we just exited interactive mode
        shell_get_state()->just_exited_interactive = true;
        // Don't process the key that exited screensaver
        return;
    }
}

void screensaver_draw() {
    if (!screensaver_state.is_active) {
        return;
    }
    
    vga_clear();
    
    switch (screensaver_state.type) {
        case SCREENSAVER_SPACE_BATTLE:
            // Draw stars background
            for (u8 i = 0; i < 50; i++) {
                u8 star_color;
                switch (screensaver_state.star_brightness[i]) {
                    case 1: star_color = VGA_COLOR_DARK_GREY; break;
                    case 2: star_color = VGA_COLOR_LIGHT_GREY; break;
                    case 3: star_color = VGA_COLOR_WHITE; break;
                    default: star_color = VGA_COLOR_DARK_GREY; break;
                }
                vga_set_cursor(screensaver_state.star_x[i], screensaver_state.star_y[i]);
                vga_putchar_color('.', star_color, VGA_COLOR_BLACK);
            }
            
            // Draw spaceship
            if (!screensaver_state.game_over) {
                vga_set_cursor(screensaver_state.spaceship_x, screensaver_state.spaceship_y);
                vga_putchar_color('^', VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
                vga_set_cursor(screensaver_state.spaceship_x - 1, screensaver_state.spaceship_y);
                vga_putchar_color('<', VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
                vga_set_cursor(screensaver_state.spaceship_x + 1, screensaver_state.spaceship_y);
                vga_putchar_color('>', VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
                vga_set_cursor(screensaver_state.spaceship_x, screensaver_state.spaceship_y + 1);
                vga_putchar_color('|', VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
            }
            
            // Draw asteroids
            for (u8 i = 0; i < 15; i++) {
                if (screensaver_state.asteroid_active[i]) {
                    char asteroid_char;
                    u8 asteroid_color;
                    
                    switch (screensaver_state.asteroid_type[i]) {
                        case 0: // Small
                            asteroid_char = '*';
                            asteroid_color = VGA_COLOR_LIGHT_BROWN;
                            break;
                        case 1: // Medium
                            asteroid_char = 'O';
                            asteroid_color = VGA_COLOR_BROWN;
                            break;
                        case 2: // Large
                            asteroid_char = '@';
                            asteroid_color = VGA_COLOR_DARK_GREY;
                            break;
                        default:
                            asteroid_char = '*';
                            asteroid_color = VGA_COLOR_LIGHT_BROWN;
                            break;
                    }
                    
                    vga_set_cursor(screensaver_state.asteroid_x[i], screensaver_state.asteroid_y[i]);
                    vga_putchar_color(asteroid_char, asteroid_color, VGA_COLOR_BLACK);
                }
            }
            
            // Draw lasers
            for (u8 i = 0; i < 10; i++) {
                if (screensaver_state.laser_active[i]) {
                    vga_set_cursor(screensaver_state.laser_x[i], screensaver_state.laser_y[i]);
                    vga_putchar_color('|', VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
                }
            }
            
            // Draw explosions
            for (u8 i = 0; i < 8; i++) {
                if (screensaver_state.explosion_active[i]) {
                    u8 explosion_size = screensaver_state.explosion_frame[i] / 2;
                    u8 explosion_color = VGA_COLOR_LIGHT_RED;
                    
                    if (screensaver_state.explosion_frame[i] > 4) {
                        explosion_color = VGA_COLOR_LIGHT_BROWN;
                    }
                    
                    // Draw explosion pattern
                    for (u8 j = 0; j < explosion_size && j < 3; j++) {
                        vga_set_cursor(screensaver_state.explosion_x[i] - j, screensaver_state.explosion_y[i]);
                        vga_putchar_color('X', explosion_color, VGA_COLOR_BLACK);
                        vga_set_cursor(screensaver_state.explosion_x[i] + j, screensaver_state.explosion_y[i]);
                        vga_putchar_color('X', explosion_color, VGA_COLOR_BLACK);
                        vga_set_cursor(screensaver_state.explosion_x[i], screensaver_state.explosion_y[i] - j);
                        vga_putchar_color('X', explosion_color, VGA_COLOR_BLACK);
                        vga_set_cursor(screensaver_state.explosion_x[i], screensaver_state.explosion_y[i] + j);
                        vga_putchar_color('X', explosion_color, VGA_COLOR_BLACK);
                    }
                }
            }
            
            // Draw UI
            vga_set_cursor(0, 0);
            vga_print_color("SPACE BATTLE", VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            
            vga_set_cursor(0, 1);
            vga_print_color("Score: ", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            
            // Simple score display - just show the number directly
            if (screensaver_state.score == 0) {
                vga_print_color("0", VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
            } else {
                // Convert score to string manually
                u32 temp_score = screensaver_state.score;
                char score_digits[10];
                u8 digit_count = 0;
                
                while (temp_score > 0 && digit_count < 9) {
                    score_digits[digit_count] = '0' + (temp_score % 10);
                    temp_score /= 10;
                    digit_count++;
                }
                
                // Print digits in reverse order
                for (u8 i = digit_count; i > 0; i--) {
                    vga_print_color(&score_digits[i-1], VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
                }
            }
            
            vga_set_cursor(0, 2);
            vga_print_color("Lives: ", VGA_COLOR_WHITE, VGA_COLOR_BLACK);
            for (u8 i = 0; i < screensaver_state.lives; i++) {
                vga_print_color("^", VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
            }
            
            if (screensaver_state.game_over) {
                vga_set_cursor(30, 12);
                vga_print_color("GAME OVER!", VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
                vga_set_cursor(25, 13);
                vga_print_color("Press any key to exit", VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
            } else {
                vga_set_cursor(25, 24);
                vga_print_color("Press any key to exit screensaver", VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
            }
            break;
            
        default:
            break;
    }
}

bool screensaver_is_active() {
    return screensaver_state.is_active;
}

screensaver_state_t* screensaver_get_state() {
    return &screensaver_state;
}

void screensaver_check_inactivity() {
    if (screensaver_state.is_active) {
        return; // Screensaver already active
    }
    
    inactivity_timer++;
    
    if (inactivity_timer >= INACTIVITY_TIMEOUT) {
        screensaver_start(SCREENSAVER_SPACE_BATTLE); // Auto-start space battle screensaver
        inactivity_timer = 0;
    }
}

void screensaver_reset_timer() {
    if (!screensaver_state.is_active) {
        inactivity_timer = 0;
    }
}
