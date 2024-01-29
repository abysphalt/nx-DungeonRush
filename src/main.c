#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "res.h"
#include "game.h"
#include "ui.h"
#include "prng.h"
#include "debug.h"

#ifdef DEBUG
#include <assert.h>
#endif

int main(int argc, char** args) {
  #ifdef DEBUG
    initNxLink();
    TRACE("Starting init...");
  #endif
  prngSrand(time(NULL));
  // Start up SDL and create window
  if (!init()) {
    #ifdef DEBUG
      TRACE("Failed to initialize!\n");
    #endif
  } else {
    // Load media
    if (!loadMedia()) {
      #ifdef DEBUG
        TRACE("Failed to load media!\n");
      #endif
    } else {
      mainUi();
    }
  }
  cleanup();
  return 0;
}
