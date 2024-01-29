#include "ui.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <errno.h>

#include <switch.h>

#include "audio.h"
#include "game.h"
#include "helper.h"
#include "map.h"
#include "net.h"
#include "render.h"
#include "res.h"
#include "storage.h"
#include "text.h"
#include "types.h"
#include "keyboard.h"

extern LinkList animationsList[];
extern bool hasMap[MAP_SIZE][MAP_SIZE];
extern Text texts[TEXTSET_SIZE];
extern SDL_Renderer* renderer;
extern int renderFrames;
extern SDL_Color WHITE;
extern Texture textures[];
extern Effect effects[];
bool changed_name = false;

extern LinkList animationsList[ANIMATION_LINK_LIST_NUM];
int cursorPos;
bool moveCursor(int optsNum) {
  SDL_Event e;
  bool quit = false;
  while (SDL_PollEvent(&e)) {
    // Use switch controls.
    switch(e.type) {
      case SDL_JOYBUTTONDOWN:
        switch (e.jbutton.button) {
            case JOY_UP:
              cursorPos--;
              playAudio(AUDIO_INTER1);
              break;
            case JOY_DOWN:
              cursorPos++;
              playAudio(AUDIO_INTER1);
              break;
            case JOY_A:
              quit = true;
              break;
            case JOY_MINUS:
              quit = true;
              cursorPos = optsNum;
              playAudio(AUDIO_BUTTON1);
              return quit;
              break;
            // We'll also let the B button take us back a screen. 
            case JOY_B:
              quit = true;
              cursorPos = optsNum;
              playAudio(AUDIO_BUTTON1);
              return quit;
              break;
          }
        }
    }
  cursorPos += optsNum;
  cursorPos %= optsNum;
  return quit;
}
int chooseOptions(int optionsNum, Text** options) {
  cursorPos = 0;
  Snake* player = createSnake(2, 0, LOCAL);
  appendSpriteToSnake(player, SPRITE_KNIGHT, SCREEN_WIDTH / 2,
                      SCREEN_HEIGHT / 2, UP);
  int lineGap = FONT_SIZE + FONT_SIZE / 2,
      totalHeight = lineGap * (optionsNum - 1);
  int startY = (SCREEN_HEIGHT - totalHeight) / 2;
  while (!moveCursor(optionsNum)) {
    Sprite* sprite = player->sprites->head->element;
    sprite->ani->at = AT_CENTER;
    sprite->x = SCREEN_WIDTH / 2 - options[cursorPos]->width / 2 - UNIT / 2;
    sprite->y = startY + cursorPos * lineGap;
    updateAnimationOfSprite(sprite);
    renderUi();
    for (int i = 0; i < optionsNum; i++) {
      renderCenteredText(options[i], SCREEN_WIDTH / 2, startY + i * lineGap, 1);
    }
    // Update Screen
    SDL_RenderPresent(renderer);
    renderFrames++;
  }
  playAudio(AUDIO_BUTTON1);
  destroySnake(player);
  destroyAnimationsByLinkList(&animationsList[RENDER_LIST_SPRITE_ID]);
  return cursorPos;
}
void baseUi(int w, int h) {
  initRenderer();
  initBlankMap(w, h);
  pushMapToRender();
}
bool chooseLevelUi() {
  baseUi(30, 12);
  int optsNum = 3;
  Text** opts = malloc(sizeof(Text*) * optsNum);
  for (int i = 0; i < optsNum; i++) opts[i] = texts + i + 10;
  int opt = chooseOptions(optsNum, opts);
  if (opt != optsNum) setLevel(opt);
  clearRenderer();
  return opt != optsNum;
}

void launchLocalGame(int localPlayerNum) {
  Score** scores = startGame(localPlayerNum, 0, true);
  rankListUi(localPlayerNum, scores);
  for (int i = 0; i < localPlayerNum; i++) updateLocalRanklist(scores[i]);
  destroyRanklist(localPlayerNum, scores);
}
int rangeOptions(int start, int end) {
  int optsNum = end - start + 1;
  Text** opts = malloc(sizeof(Text*) * optsNum);
  for (int i = 0; i < optsNum; i++) opts[i] = texts + i + start;
  int opt = chooseOptions(optsNum, opts);
  free(opts);
  return opt;
}

void launchLanGame() {
  baseUi(10, 10);
  int opt = rangeOptions(LAN_HOSTGAME, LAN_JOINGAME);
  blackout();
  clearRenderer();
  if (opt == 0) {
    hostGame();
  } else {
    // Use Switch keyboard for inputting IP address
    char* ip = getKeyboardInput("Enter IP Address", "[IP Address Here]");
    if (ip == NULL) return;
    #ifdef DEBUG
      TRACE("Joining game...");
    #endif
    joinGame(ip, LAN_LISTEN_PORT);
    free(ip);
  }
}

int chooseOnLanUi() {
  baseUi(10, 10);
  int opt = rangeOptions(MULTIPLAYER_LOCAL, MULTIPLAYER_LAN);
  clearRenderer();
  return opt;
}

void mainUi() {
  baseUi(30, 12);
  playBgm(0);
  int startY = SCREEN_HEIGHT / 2 - 70;
  int startX = SCREEN_WIDTH / 5 + 32;
  // Fix title being being too far down.
  createAndPushAnimation(&animationsList[RENDER_LIST_UI_ID],
                         &textures[RES_TITLE], NULL, LOOP_INFI, 80,
                         SCREEN_WIDTH / 2, 150, SDL_FLIP_NONE, 0, AT_CENTER);
  createAndPushAnimation(&animationsList[RENDER_LIST_SPRITE_ID],
                         &textures[RES_KNIGHT_M], NULL, LOOP_INFI,
                         SPRITE_ANIMATION_DURATION, startX, startY,
                         SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER);
  createAndPushAnimation(
      &animationsList[RENDER_LIST_EFFECT_ID], &textures[RES_SwordFx], NULL,
      LOOP_INFI, SPRITE_ANIMATION_DURATION, startX + UI_MAIN_GAP_ALT * 2,
      startY - 32, SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER)
      ->scaled = false;
  createAndPushAnimation(
      &animationsList[RENDER_LIST_SPRITE_ID], &textures[RES_CHORT], NULL,
      LOOP_INFI, SPRITE_ANIMATION_DURATION, startX + UI_MAIN_GAP_ALT * 2,
      startY - 32, SDL_FLIP_HORIZONTAL, 0, AT_BOTTOM_CENTER);

  startX += UI_MAIN_GAP_ALT * (6 + 2 * randDouble());
  startY += UI_MAIN_GAP * (1 + randDouble());
  createAndPushAnimation(&animationsList[RENDER_LIST_SPRITE_ID],
                         &textures[RES_ELF_M], NULL, LOOP_INFI,
                         SPRITE_ANIMATION_DURATION, startX, startY,
                         SDL_FLIP_HORIZONTAL, 0, AT_BOTTOM_CENTER);
  createAndPushAnimation(&animationsList[RENDER_LIST_EFFECT_ID],
                         &textures[RES_HALO_EXPLOSION2], NULL, LOOP_INFI,
                         SPRITE_ANIMATION_DURATION, startX - UI_MAIN_GAP * 1.5,
                         startY, SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER);
  createAndPushAnimation(&animationsList[RENDER_LIST_SPRITE_ID],
                         &textures[RES_ZOMBIE], NULL, LOOP_INFI,
                         SPRITE_ANIMATION_DURATION, startX - UI_MAIN_GAP * 1.5,
                         startY, SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER);

  startX -= UI_MAIN_GAP_ALT * (1 + 2 * randDouble());
  startY += UI_MAIN_GAP * (2 + randDouble());
  createAndPushAnimation(&animationsList[RENDER_LIST_SPRITE_ID],
                         &textures[RES_WIZZARD_M], NULL, LOOP_INFI,
                         SPRITE_ANIMATION_DURATION, startX, startY,
                         SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER);
  createAndPushAnimation(&animationsList[RENDER_LIST_EFFECT_ID],
                         &textures[RES_FIREBALL], NULL, LOOP_INFI,
                         SPRITE_ANIMATION_DURATION, startX + UI_MAIN_GAP,
                         startY, SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER);

  startX += UI_MAIN_GAP_ALT * (18 + 4 * randDouble());
  startY -= UI_MAIN_GAP * (1 + 3 * randDouble());
  createAndPushAnimation(&animationsList[RENDER_LIST_SPRITE_ID],
                         &textures[RES_LIZARD_M], NULL, LOOP_INFI,
                         SPRITE_ANIMATION_DURATION, startX, startY,
                         SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER);
  createAndPushAnimation(
      &animationsList[RENDER_LIST_EFFECT_ID], &textures[RES_CLAWFX2], NULL,
      LOOP_INFI, SPRITE_ANIMATION_DURATION, startX, startY - UI_MAIN_GAP + 16,
      SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER);
  createAndPushAnimation(
      &animationsList[RENDER_LIST_SPRITE_ID], &textures[RES_MUDDY], NULL,
      LOOP_INFI, SPRITE_ANIMATION_DURATION, startX, startY - UI_MAIN_GAP,
      SDL_FLIP_HORIZONTAL, 0, AT_BOTTOM_CENTER);

  createAndPushAnimation(
      &animationsList[RENDER_LIST_EFFECT_ID], &textures[RES_CLAWFX2], NULL,
      LOOP_INFI, SPRITE_ANIMATION_DURATION, startX + UI_MAIN_GAP,
      startY - UI_MAIN_GAP + 16, SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER);
  createAndPushAnimation(
      &animationsList[RENDER_LIST_SPRITE_ID], &textures[RES_SWAMPY], NULL,
      LOOP_INFI, SPRITE_ANIMATION_DURATION, startX + UI_MAIN_GAP,
      startY - UI_MAIN_GAP, SDL_FLIP_HORIZONTAL, 0, AT_BOTTOM_CENTER);

  createAndPushAnimation(&animationsList[RENDER_LIST_EFFECT_ID],
                         &textures[RES_CLAWFX2], NULL, LOOP_INFI,
                         SPRITE_ANIMATION_DURATION, startX + UI_MAIN_GAP,
                         startY + 16, SDL_FLIP_NONE, 0, AT_BOTTOM_CENTER);
  createAndPushAnimation(&animationsList[RENDER_LIST_SPRITE_ID],
                         &textures[RES_SWAMPY], NULL, LOOP_INFI,
                         SPRITE_ANIMATION_DURATION, startX + UI_MAIN_GAP,
                         startY, SDL_FLIP_HORIZONTAL, 0, AT_BOTTOM_CENTER);
  /*
   startX = SCREEN_WIDTH/3*2;
   startY = SCREEN_HEIGHT/3 + 10;
   int colNum = 8;
   for (int i = RES_TINY_ZOMBIE; i <= RES_CHORT; i+=2) {
     int col = (i - RES_TINY_ZOMBIE)%colNum;
     int row = (i - RES_TINY_ZOMBIE)/colNum;
     createAndPushAnimation(&animationsList[RENDER_LIST_SPRITE_ID],
                           &textures[i], NULL, LOOP_INFI,
                           SPRITE_ANIMATION_DURATION, startX +
   col*UI_MAIN_GAP_ALT, startY + row*UI_MAIN_GAP, SDL_FLIP_HORIZONTAL, 0,
   AT_BOTTOM_CENTER);
   }
   for (int i = RES_BIG_ZOMBIE; i <= RES_BIG_DEMON; i+=2) {
     createAndPushAnimation(&animationsList[RENDER_LIST_SPRITE_ID],
                           &textures[i], NULL, LOOP_INFI,
                           SPRITE_ANIMATION_DURATION, startX +
   (i-RES_BIG_ZOMBIE)*UNIT, startY + 200, SDL_FLIP_HORIZONTAL, 0,
   AT_BOTTOM_CENTER);
   }
   */
  // Modified to set the player name from the menu. Due to the structure, I don't want to insert into the texts file
  // so it's just at the end. Modify this code to reflect that. 
  int optsNum = 5;
  Text** opts = malloc(sizeof(Text*) * optsNum);
  for (int i = 0; i < optsNum; i++) {
    if (i == 3) {
      // Update the text if the player name was changed.
      if (changed_name) {
        opts[i] = texts + 18;
      } else {
        opts[i] = texts + 17;
      }
    } else if ( i == 4) {
      opts[i] = texts + i + 5;
    } else {
      opts[i] = texts + i + 6;
    }
  }

  int opt = chooseOptions(optsNum, opts);
  free(opts);

  blackout();
  clearRenderer();
  int lan;
  switch (opt) {
    case 0:
      if (!chooseLevelUi()) break;
      launchLocalGame(1);
      break;
    case 1:
      lan = chooseOnLanUi();
      if (lan == 0) {
        if (!chooseLevelUi()) break;
        launchLocalGame(2);
      } else if (lan == 1) {
        launchLanGame();
      }
      break;
    case 2:
      localRankListUi();
      break;
    case 3:
    // Get the players name and reload the texts.
      char* name = getKeyboardInput("Enter Name", "[NAME]");
      modify_fifth_line(configFile, name);
      loadTextset(configFile);
      free(name);
      changed_name = true;
      break;
    case 4:
      break;
  }
  if (opt == optsNum) return;
  if (opt != 4) {
    mainUi();
  }
}

// Modify the config file to change the player name.
void modify_fifth_line(const char *filename, char* name) {
    FILE *fp, *fp_temp;
    char buffer[1024];
    int line_num = 1;

    fp = fopen(filename, "r");
    fp_temp = fopen("sdmc:/config/DungeonRush/config.ini_temp", "w");

    if (!fp || !fp_temp) {
        TRACE("Error opening file.\n");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (line_num == 5) {
            fputs(name, fp_temp);
            fputs("\n", fp_temp);
        } else {
            fputs(buffer, fp_temp);
        }
        line_num++;
    }

    fclose(fp);
    fclose(fp_temp);
    remove(filename);

    if (rename("sdmc:/config/DungeonRush/config.ini_temp", filename) != 0) {
      #ifdef DEBUG
        TRACE("Error renaming file");
        TRACE("Error code: %d\n", errno);
      #endif
    }
}

void rankListUi(int count, Score** scores) {
  baseUi(30, 12 + MAX(0, count - 4));
  playBgm(0);
  Text** opts = malloc(sizeof(Text*) * count);
  char buf[1 << 8];
  for (int i = 0; i < count; i++) {
    sprintf(buf, "Score: %-6.0lf Got: %-6d Kill: %-6d Damage: %-6d Stand: %-6d",
            scores[i]->rank, scores[i]->got, scores[i]->killed,
            scores[i]->damage, scores[i]->stand);
    opts[i] = createText(buf, WHITE);
  }

  chooseOptions(count, opts);

  for (int i = 0; i < count; i++) destroyText(opts[i]);
  free(opts);
  blackout();
  clearRenderer();
}
void localRankListUi() {
  int count;
  Score** scores = readRanklist(STORAGE_PATH, &count);
  rankListUi(count, scores);
  destroyRanklist(count, scores);
}
