/* command_editor header stub - module removed */

#ifndef COMMAND_EDITOR_H
#define COMMAND_EDITOR_H

#include "kernel/kernel.h"

// Forward declaration used by stub functions
struct keyboard_event;

typedef struct command_editor_state_t command_editor_state_t;

void cmd_editor_init();
void cmd_editor_start();
void cmd_editor_handle_keyboard(struct keyboard_event event);
void cmd_editor_draw();
void cmd_editor_execute_command();
void cmd_editor_exit();
bool cmd_editor_is_active();
command_editor_state_t* cmd_editor_get_state();

#endif
