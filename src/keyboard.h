#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <switch.h>
#include "debug.h"

SwkbdTextCheckResult validate_text(char* tmp_string, size_t tmp_string_size);

void finishinit_cb(void);

void decidedcancel_cb(void);

void strchange_cb(const char* str, SwkbdChangedStringArg* arg);

void movedcursor_cb(const char* str, SwkbdMovedCursorArg* arg);

void decidedenter_cb(const char* str, SwkbdDecidedEnterArg* arg);

char* getKeyboardInput(const char* header_text, const char* initial_text);

#endif /* KEYBOARD_INPUT_H */
