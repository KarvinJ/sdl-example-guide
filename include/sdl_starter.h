#pragma once

#include <SDL2/SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 768;
const int FRAME_RATE = 60;

int startSDL(SDL_Window *window, SDL_Renderer *renderer);

void capFrameRate(Uint32 frameStartTime);