#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <switch.h>
#include "debug.h"
#include "keyboard.h"

SwkbdTextCheckResult validate_text(char* tmp_string, size_t tmp_string_size) {
    if (strcmp(tmp_string, "bad")==0) {
        strncpy(tmp_string, "Bad string.", tmp_string_size);
        return SwkbdTextCheckResult_Bad;
    }

    return SwkbdTextCheckResult_OK;
}

// The rest of these callbacks are for swkbd-inline.
void finishinit_cb(void) {
    #ifdef DEBUG
        TRACE("reply: FinishedInitialize\n");
    #endif
}

void decidedcancel_cb(void) {
    #ifdef DEBUG
        TRACE("reply: DecidedCancel\n");
    #endif
}

// String changed callback.
void strchange_cb(const char* str, SwkbdChangedStringArg* arg) {
    #ifdef DEBUG
        TRACE("reply: ChangedString. str = %s, arg->stringLen = 0x%x, arg->dicStartCursorPos = 0x%x, arg->dicEndCursorPos = 0x%x, arg->arg->cursorPos = 0x%x\n", str, arg->stringLen, arg->dicStartCursorPos, arg->dicEndCursorPos, arg->cursorPos);
    #endif
}

// Moved cursor callback.
void movedcursor_cb(const char* str, SwkbdMovedCursorArg* arg) {
    #ifdef DEBUG
        TRACE("reply: MovedCursor. str = %s, arg->stringLen = 0x%x, arg->cursorPos = 0x%x\n", str, arg->stringLen, arg->cursorPos);
    #endif
}

// Text submitted callback, this is used to get the output text from submit.
void decidedenter_cb(const char* str, SwkbdDecidedEnterArg* arg) {
    #ifdef DEBUG
        TRACE("reply: DecidedEnter. str = %s, arg->stringLen = 0x%x\n", str, arg->stringLen);
    #endif
}

char* getKeyboardInput(const char* header_text, const char* initial_text) {
    Result rc=0;
    #ifdef DEBUG
        TRACE("Setting up for keyboard...");
    #endif
    consoleUpdate(NULL);

    SwkbdConfig kbd;
    //char tmpoutstr[16] = {0};
    rc = swkbdCreate(&kbd, 0);
    #ifdef DEBUG
        TRACE("swkbdCreate(): 0x%x\n", rc);
    #endif
    char* tmpoutstr = (char*)malloc(50); // Allocate memory dynamically
    if (tmpoutstr == NULL) {
        #ifdef DEBUG
            TRACE("Malloc failed.");
        #endif
        return NULL;
    }

    if (R_SUCCEEDED(rc)) {

        // Select a Preset to use, if any.
        swkbdConfigMakePresetDefault(&kbd);
        //swkbdConfigMakePresetPassword(&kbd);
        //swkbdConfigMakePresetUserName(&kbd);
        //swkbdConfigMakePresetDownloadCode(&kbd);

        // Optional, set any text if you want (see swkbd.h).
        //swkbdConfigSetOkButtonText(&kbd, "Submit");
        //swkbdConfigSetLeftOptionalSymbolKey(&kbd, "a");
        //swkbdConfigSetRightOptionalSymbolKey(&kbd, "b");
        if (header_text != NULL) {
            swkbdConfigSetHeaderText(&kbd, header_text);
        }
        //swkbdConfigSetSubText(&kbd, "Sub");
        //swkbdConfigSetGuideText(&kbd, "Guide");

        //swkbdConfigSetTextCheckCallback(&kbd, validate_text);//Optional, can be removed if not using TextCheck.

        // Set the initial string if you want.
        if (initial_text != NULL) {
            swkbdConfigSetInitialText(&kbd, initial_text);
        }

        // You can also use swkbdConfigSet*() funcs if you want.
        #ifdef DEBUG
            TRACE("Running swkbdShow...\n");
        #endif
        rc = swkbdShow(&kbd, tmpoutstr, 50);
        #ifdef DEBUG
            TRACE("swkbdShow(): 0x%x\n", rc);
        #endif

        if (R_SUCCEEDED(rc)) {
            #ifdef DEBUG
                TRACE("out str: %s\n", tmpoutstr);
            #endif
            return tmpoutstr;
        }
        free(tmpoutstr); // Free the memory in case of failure
        swkbdClose(&kbd);
    }
    return tmpoutstr;
}