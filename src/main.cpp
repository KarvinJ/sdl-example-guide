#include "sdl_starter.h"
#include <time.h>
#include <string>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GameController *controller = nullptr;
SDL_GameController *controller2 = nullptr;

const char *controllerName = "";
const char *controller2Name = "";

Mix_Chunk *actionSound = nullptr;
// Mix_Music *music = nullptr;

Sprite playerSprite;

const int PLAYER_SPEED = 600;

bool isGameRunning = true;
bool isGamePaused = false;
bool isAutoPlayMode = false;
bool shouldClearScreen = true;

SDL_Texture *controllerNameTexture = nullptr;
SDL_Rect controllerNameBounds;

SDL_Texture *controller2NameTexture = nullptr;
SDL_Rect controller2NameBounds;

SDL_Texture *playerPositionTexture = nullptr;
SDL_Rect playerPositionBounds;

SDL_Texture *pauseTexture = nullptr;
SDL_Rect pauseBounds;

SDL_Texture *scoreTexture = nullptr;
SDL_Rect scoreBounds;

SDL_Texture *scoreTexture2 = nullptr;
SDL_Rect scoreBounds2;

int player1Score = 0;
int player2Score = 0;

int gameStatus = -1;

TTF_Font *fontSquare = nullptr;
TTF_Font *fontStart = nullptr;

SDL_Rect player2 = {SCREEN_WIDTH - 32, SCREEN_HEIGHT / 2, 16, 96};
SDL_Rect ball = {SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2, 32, 32};

int ballVelocityX = 400;
int ballVelocityY = 400;

int colorIndex = 0;

SDL_Color colors[] = {
    {128, 128, 128, 0}, // gray
    {255, 255, 255, 0}, // white
    {255, 0, 0, 0},     // red
    {0, 255, 0, 0},     // green
    {0, 0, 255, 0},     // blue
    {255, 255, 0, 0},   // brown
    {0, 255, 255, 0},   // cyan
    {255, 0, 255, 0},   // purple
};

void handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
        {
            isGameRunning = false;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)
        {
            isAutoPlayMode = !isAutoPlayMode;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
        {
            shouldClearScreen = !shouldClearScreen;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
        {
            isGamePaused = !isGamePaused;
            Mix_PlayChannel(-1, actionSound, 0);
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_KP_PLUS && gameStatus < 5)
        {
            gameStatus++;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_KP_MINUS && gameStatus > -9)
        {
            gameStatus--;
        }

        // All this functionalities are shared between controllers. In a future I may want to have specific button for just one control in specific.
        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK)
        {
            isAutoPlayMode = !isAutoPlayMode;
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK)
        {
            shouldClearScreen = !shouldClearScreen;
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_START)
        {
            isGamePaused = !isGamePaused;
            Mix_PlayChannel(-1, actionSound, 0);
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER && gameStatus < 5)
        {
            gameStatus++;
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER && gameStatus > -7)
        {
            gameStatus--;
        }
    }
}

void resetValues()
{
    if (gameStatus == 1)
    {
        if (playerSprite.bounds.y < SCREEN_HEIGHT / 2)
        {
            playerSprite.bounds.x = 0;
            playerSprite.bounds.y = 0;
        }
        else
        {
            playerSprite.bounds.x = 0;
            playerSprite.bounds.y = SCREEN_HEIGHT - playerSprite.bounds.h;
        }
    }

    if (gameStatus > -3)
    {
        playerSprite.bounds.w = 38;
        playerSprite.bounds.h = 34;
    }

    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;

    player1Score = 0;
    updateTextureText(scoreTexture, std::to_string(player1Score).c_str(), fontSquare, renderer);
}

int getRandomNumberBetweenRange(int min, int max)
{
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void update(float deltaTime)
{
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_R])
    {
        resetValues();
    }

    if (currentKeyStates[SDL_SCANCODE_LEFT])
    {
        playerSprite.bounds.w--;
    }

    else if (currentKeyStates[SDL_SCANCODE_RIGHT])
    {
        playerSprite.bounds.w++;
    }

    if (currentKeyStates[SDL_SCANCODE_UP])
    {
        playerSprite.bounds.h--;
    }

    else if (currentKeyStates[SDL_SCANCODE_DOWN])
    {
        playerSprite.bounds.h++;
    }

    if (currentKeyStates[SDL_SCANCODE_W] && playerSprite.bounds.y > 0)
    {
        playerSprite.bounds.y -= PLAYER_SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_S] && playerSprite.bounds.y < SCREEN_HEIGHT - playerSprite.bounds.h)
    {
        playerSprite.bounds.y += PLAYER_SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_A] && playerSprite.bounds.x > 0)
    {
        playerSprite.bounds.x -= PLAYER_SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_D] && playerSprite.bounds.x < SCREEN_WIDTH - playerSprite.bounds.w)
    {
        playerSprite.bounds.x += PLAYER_SPEED * deltaTime;
    }

    if (currentKeyStates[SDL_SCANCODE_I] && player2.y > 0)
    {
        player2.y -= PLAYER_SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_K] && player2.y < SCREEN_HEIGHT - player2.h)
    {
        player2.y += PLAYER_SPEED * deltaTime;
    }

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK))
    {
        resetValues();
    }

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X))
    {
        playerSprite.bounds.w--;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B))
    {
        playerSprite.bounds.w++;
    }

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y))
    {
        playerSprite.bounds.h--;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))
    {
        playerSprite.bounds.h++;
    }

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP) && playerSprite.bounds.y > 0)
    {
        playerSprite.bounds.y -= PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) && playerSprite.bounds.y < SCREEN_HEIGHT - playerSprite.bounds.h)
    {
        playerSprite.bounds.y += PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) && playerSprite.bounds.x > 0)
    {
        playerSprite.bounds.x -= PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && playerSprite.bounds.x < SCREEN_WIDTH - playerSprite.bounds.w)
    {
        playerSprite.bounds.x += PLAYER_SPEED * deltaTime;
    }

    if (SDL_GameControllerGetButton(controller2, SDL_CONTROLLER_BUTTON_DPAD_UP) && player2.y > 0)
    {
        player2.y -= PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller2, SDL_CONTROLLER_BUTTON_DPAD_DOWN) && player2.y < SCREEN_HEIGHT - player2.h)
    {
        player2.y += PLAYER_SPEED * deltaTime;
    }

    if (isAutoPlayMode && ball.y < SCREEN_HEIGHT - player2.h)
    {
        player2.y = ball.y;
    }

    if (gameStatus < 0 && ball.x < 0)
    {
        ball.x = SCREEN_WIDTH / 2 - ball.w;
        ball.y = SCREEN_HEIGHT / 2 - ball.h;

        ballVelocityX *= -1;
        ballVelocityY *= -1;

        player2Score++;
        updateTextureText(scoreTexture2, std::to_string(player2Score).c_str(), fontSquare, renderer);
    }

    else if (gameStatus < 0 && ball.x > SCREEN_WIDTH - ball.w)
    {
        ball.x = SCREEN_WIDTH / 2 - ball.w;
        ball.y = SCREEN_HEIGHT / 2 - ball.h;

        ballVelocityX *= -1;
        ballVelocityY *= -1;

        player1Score++;
        updateTextureText(scoreTexture, std::to_string(player1Score).c_str(), fontSquare, renderer);
    }

    if (gameStatus > 0 && (ball.x < 0 || ball.x > SCREEN_WIDTH - ball.w))
    {
        ballVelocityX *= -1;
        colorIndex = getRandomNumberBetweenRange(0, 5);
    }

    else if (ball.y < 0 || ball.y > SCREEN_HEIGHT - ball.h)
    {
        ballVelocityY *= -1;
        colorIndex = getRandomNumberBetweenRange(0, 5);
    }

    else if (SDL_HasIntersection(&playerSprite.bounds, &ball) || (gameStatus < -3 && SDL_HasIntersection(&player2, &ball)))
    {
        ballVelocityX *= -1;
        ballVelocityY *= -1;

        colorIndex = getRandomNumberBetweenRange(0, 5);

        if (gameStatus < -3 || gameStatus > 2)
        {
            Mix_PlayChannel(-1, actionSound, 0);
        }

        if (gameStatus > 3)
        {
            player1Score++;
            updateTextureText(scoreTexture, std::to_string(player1Score).c_str(), fontSquare, renderer);
        }
    }

    if (gameStatus < -4 || gameStatus > 1)
    {
        ball.x += ballVelocityX * deltaTime;
        ball.y += ballVelocityY * deltaTime;
    }
}

void drawCoordinateSystemLines()
{
    int newPosition = 40;

    for (int i = 0; i < 18; i++)
    {
        SDL_RenderDrawLine(renderer, 0, newPosition, 20, newPosition);
        SDL_RenderDrawLine(renderer, 0, newPosition + 1, 20, newPosition + 1);
        SDL_RenderDrawLine(renderer, 0, newPosition + 2, 20, newPosition + 2);
        newPosition += 40;
    }

    newPosition = 40;

    for (int i = 0; i < 35; i++)
    {
        SDL_RenderDrawLine(renderer, newPosition, SCREEN_HEIGHT, newPosition, SCREEN_HEIGHT - 20);
        SDL_RenderDrawLine(renderer, newPosition + 1, SCREEN_HEIGHT, newPosition + 1, SCREEN_HEIGHT - 20);
        SDL_RenderDrawLine(renderer, newPosition + 2, SCREEN_HEIGHT, newPosition + 2, SCREEN_HEIGHT - 20);
        newPosition += 40;
    }
}

void displayConnectedControllersName()
{
    updateTextureText(controllerNameTexture, controllerName, fontStart, renderer);

    SDL_QueryTexture(controllerNameTexture, NULL, NULL, &controllerNameBounds.w, &controllerNameBounds.h);
    controllerNameBounds.x = 50;
    controllerNameBounds.y = controllerNameBounds.h / 2;
    SDL_RenderCopy(renderer, controllerNameTexture, NULL, &controllerNameBounds);

    updateTextureText(controller2NameTexture, controller2Name, fontStart, renderer);

    SDL_QueryTexture(controller2NameTexture, NULL, NULL, &controller2NameBounds.w, &controller2NameBounds.h);
    controller2NameBounds.x = SCREEN_WIDTH - controller2NameBounds.w - 50;
    controller2NameBounds.y = controller2NameBounds.h / 2;
    SDL_RenderCopy(renderer, controller2NameTexture, NULL, &controller2NameBounds);
}

void render()
{
    if (shouldClearScreen)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        if (gameStatus < -7)
        {
            SDL_Color darkGreen = {20, 160, 133, 255};
            SDL_SetRenderDrawColor(renderer, darkGreen.r, darkGreen.g, darkGreen.b, darkGreen.a);
        }

        SDL_RenderClear(renderer);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    if (gameStatus != -1 && gameStatus != 5)
    {
        SDL_RenderFillRect(renderer, &playerSprite.bounds);
    }

    if (gameStatus < -2)
    {
        SDL_RenderFillRect(renderer, &player2);
    }

    if (gameStatus == 1)
    {
        drawCoordinateSystemLines();

        std::string playerPosition = "(" + std::to_string(playerSprite.bounds.x) + ", " + std::to_string(playerSprite.bounds.y) + ")";
        updateTextureText(playerPositionTexture, playerPosition.c_str(), fontSquare, renderer);

        SDL_QueryTexture(playerPositionTexture, NULL, NULL, &playerPositionBounds.w, &playerPositionBounds.h);
        playerPositionBounds.x = 500;
        playerPositionBounds.y = playerPositionBounds.h / 2 - 10;
        SDL_RenderCopy(renderer, playerPositionTexture, NULL, &playerPositionBounds);
    }

    if (gameStatus > 4)
    {
        renderSprite(renderer, playerSprite);
    }

    if (gameStatus < -5 || gameStatus > 3)
    {
        SDL_QueryTexture(scoreTexture, NULL, NULL, &scoreBounds.w, &scoreBounds.h);
        scoreBounds.x = 400;
        scoreBounds.y = scoreBounds.h / 2 - 10;
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreBounds);
    }

    if (gameStatus < -5)
    {
        SDL_QueryTexture(scoreTexture2, NULL, NULL, &scoreBounds2.w, &scoreBounds2.h);
        scoreBounds2.x = 800;
        scoreBounds2.y = scoreBounds.h / 2 - 10;
        SDL_RenderCopy(renderer, scoreTexture2, NULL, &scoreBounds2);
    }

    if (gameStatus < -7)
    {
        SDL_Color lightGreen = {129, 204, 184, 255};
        SDL_SetRenderDrawColor(renderer, lightGreen.r, lightGreen.g, lightGreen.b, lightGreen.a);
        SDL_RenderFillCircle(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 150);
    }

    if (gameStatus == -1)
    {
        displayConnectedControllersName();
    }

    if (gameStatus < -6)
    {
        displayConnectedControllersName();

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT);
    }

    if (gameStatus > 1)
    {
        SDL_SetRenderDrawColor(renderer, colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b, 255);
    }

    if (gameStatus > 1 || gameStatus < -3)
    {
        SDL_RenderFillRect(renderer, &ball);
    }

    if (isGamePaused)
    {
        SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseBounds);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char *args[])
{
    window = SDL_CreateWindow("My Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (startSDLSystems(window, renderer) > 0)
    {
        return 1;
    }

    fontSquare = TTF_OpenFont("res/fonts/square_sans_serif_7.ttf", 90);
    fontStart = TTF_OpenFont("res/fonts/PressStart2P.ttf", 20);

    updateTextureText(controllerNameTexture, "No Connected", fontStart, renderer);
    updateTextureText(controller2NameTexture, "No Connected", fontStart, renderer);

    updateTextureText(scoreTexture, "0", fontSquare, renderer);
    updateTextureText(scoreTexture2, "0", fontSquare, renderer);

    updateTextureText(pauseTexture, "Game Paused", fontSquare, renderer);
    SDL_QueryTexture(pauseTexture, NULL, NULL, &pauseBounds.w, &pauseBounds.h);
    pauseBounds.x = SCREEN_WIDTH / 2 - pauseBounds.w / 2;
    pauseBounds.y = 100;

    playerSprite = loadSprite(renderer, "res/sprites/alien_1.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    actionSound = loadSound("res/sounds/magic.wav");
    Mix_VolumeChunk(actionSound, MIX_MAX_VOLUME / 10);

    // music = loadMusic("res/music/music.wav");
    // Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    // Mix_PlayMusic(music, -1);

    Uint32 previousFrameTime = SDL_GetTicks();
    Uint32 currentFrameTime = previousFrameTime;
    float deltaTime = 0.0f;

    while (isGameRunning)
    {
        if (SDL_NumJoysticks() > 0 && SDL_IsGameController(0))
        {
            controller = SDL_GameControllerOpen(0);
            controllerName = SDL_GameControllerName(controller);
        }
        else
        {
            controllerName = "No Connected";
        }

        if (SDL_NumJoysticks() > 1 && SDL_IsGameController(1))
        {
            controller2 = SDL_GameControllerOpen(1);
            controller2Name = SDL_GameControllerName(controller2);
        }
        else
        {
            controller2Name = "No Connected";
        }

        currentFrameTime = SDL_GetTicks();
        deltaTime = (currentFrameTime - previousFrameTime) / 1000.0f;
        previousFrameTime = currentFrameTime;

        handleEvents();

        if (!isGamePaused)
        {
            update(deltaTime);
        }

        render();
    }

    // Mix_FreeMusic(music);
    Mix_FreeChunk(actionSound);
    SDL_DestroyTexture(playerSprite.texture);
    SDL_DestroyTexture(pauseTexture);
    SDL_DestroyTexture(playerPositionTexture);
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(scoreTexture2);
    SDL_DestroyTexture(controllerNameTexture);
    SDL_DestroyTexture(controller2NameTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    stopSDLSystems();

    return 0;
}