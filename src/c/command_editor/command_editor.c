/* command_editor module removed. This file kept as a harmless stub to avoid
   accidental build/link errors while the rest of the system is refactored.
   No operational code remains here. */

#include "kernel/kernel.h"

void cmd_editor_init() {}
void cmd_editor_start() {}
void cmd_editor_handle_keyboard(struct keyboard_event event) { (void)event; }
void cmd_editor_draw() {}
void cmd_editor_execute_command() {}
void cmd_editor_exit() {}
bool cmd_editor_is_active() { return false; }
command_editor_state_t* cmd_editor_get_state() { return (command_editor_state_t*)0; }
