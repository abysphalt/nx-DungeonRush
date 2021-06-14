#ifndef SNAKE_UI_H_
#define SNAKE_UI_H_
#include <SDL2/SDL.h>
#include "types.h"
#define UI_MAIN_GAP 50
#define UI_MAIN_GAP_ALT 33

// some switch buttons for joycon
#define JOY_A     0
#define JOY_B     1
#define JOY_X     2
#define JOY_Y     3
#define JOY_PLUS  10
#define JOY_MINUS 11
#define JOY_LEFT  12
#define JOY_UP    13
#define JOY_RIGHT 14
#define JOY_DOWN  15

int chooseOptions(int optsNum, Text** options);
void baseUi(int,int);
void mainUi();
void rankListUi(int,Score**);
void localRankListUi();
#endif
