#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>




#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <stdlib.h>

#include "renderwindow.h"
#include "entity.h"
#include "player.h"
#include "ground.h"

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 480;

const int ALIVE = 0;
const int CURSOR_DEATH = 1;
const int HOLE_DEATH = 2;

const Uint8 *keyState;

std::vector<SDL_Texture*> playerTex;
SDL_Texture* groundTex[4];
SDL_Texture* arrow;
SDL_Texture* highscoreBox;
SDL_Texture* deathOverlay;
SDL_Texture* logo;

TTF_Font* font32;
TTF_Font* font32_outline;
TTF_Font* font24;
TTF_Font* font16;

SDL_Color white = { 255, 255, 255 };
SDL_Color black = { 0, 0, 0 };

Mix_Chunk* jumpSfx;
Mix_Chunk* fallSfx;
Mix_Chunk* hitSfx;
Mix_Chunk* clickSfx;

bool gameRunning = true;
bool playedDeathSFX = false;
bool mainMenu = true;

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* p_filePath)
{
    SDL_Texture* texture = NULL;
    texture = IMG_LoadTexture(renderer, p_filePath);


    //if (texture == NULL)
    //std::cout << "Failed to load texture. Error: " << SDL_GetError() << std::endl;

    return texture;
}

void clear(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
    SDL_RenderClear(renderer);
}

void render(Entity& p_entity, SDL_Renderer* renderer)
{

for (int i = p_entity.getSize() - 1; i >= 0; i--)
{
SDL_Rect src;
src.x = p_entity.getCurrentFrame().x;
src.y = p_entity.getCurrentFrame().y;
src.w = p_entity.getCurrentFrame().w;
src.h = p_entity.getCurrentFrame().h;

SDL_Rect dst;
dst.x = p_entity.getX() + p_entity.getAnimOffsetX(i);
dst.y = p_entity.getY() + p_entity.getAnimOffsetY(i);
dst.w = p_entity.getCurrentFrame().w;
dst.h = p_entity.getCurrentFrame().h;

SDL_RenderCopy(renderer, p_entity.getTex(i), &src, &dst);
}
}

void render(float p_x, float p_y, SDL_Texture* p_tex, SDL_Renderer* renderer)
{
    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    SDL_QueryTexture(p_tex, NULL, NULL, &src.w, &src.h);

    SDL_Rect dst;
    dst.x = p_x;
    dst.y = p_y;
    dst.w = src.w;
    dst.h = src.h;

    SDL_RenderCopy(renderer, p_tex, &src, &dst);
}



void render(float p_x, float p_y, const char* p_text, TTF_Font* font, SDL_Color textColor, SDL_Renderer* renderer)
{
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended( font, p_text, textColor);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = surfaceMessage->w;
    src.h = surfaceMessage->h;

    SDL_Rect dst;
    dst.x = p_x;
    dst.y = p_y;
    dst.w = src.w;
    dst.h = src.h;

    SDL_RenderCopy(renderer, message, &src, &dst);
    SDL_FreeSurface(surfaceMessage);
}

void renderCenter(float p_x, float p_y, const char* p_text, TTF_Font* font, SDL_Color textColor, SDL_Renderer* renderer)
{
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended( font, p_text, textColor);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = surfaceMessage->w;
    src.h = surfaceMessage->h;

    SDL_Rect dst;
    dst.x = SCREEN_WIDTH/2 - src.w/2 + p_x;
    dst.y = SCREEN_HEIGHT/2 - src.h/2 + p_y;
    dst.w = src.w;
    dst.h = src.h;

    SDL_RenderCopy(renderer, message, &src, &dst);
    SDL_FreeSurface(surfaceMessage);
}
void render(SDL_Texture* p_tex, SDL_Renderer* renderer)
{
    render(0, 0, p_tex, renderer);
}
void display(SDL_Renderer* renderer)
{
    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[])
{

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;


    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    //SCREEN_WIDTH = DM.w;
    //SCREEN_HEIGHT = DM.h;


    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &pWindow, &pRenderer);
    SDL_RenderSetScale(pRenderer, 2, 2);

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    srand((unsigned)time(0));

    playerTex.push_back(loadTexture(pRenderer, "player_0.png"));
    playerTex.push_back(loadTexture(pRenderer,"player_1.png"));
    playerTex.push_back(loadTexture(pRenderer,"player_2.png"));
    playerTex.push_back(loadTexture(pRenderer,"player_3.png"));
    playerTex.push_back(loadTexture(pRenderer,"player_4.png"));

    groundTex[0] = loadTexture(pRenderer,"left.png");
    groundTex[1] = loadTexture(pRenderer,"center.png");
    groundTex[2] = loadTexture(pRenderer,"right.png");
    groundTex[3] = loadTexture(pRenderer,"hole.png");

    arrow = loadTexture(pRenderer,"arrow.png");
    highscoreBox = loadTexture(pRenderer,"highscore_box.png");
    deathOverlay = loadTexture(pRenderer,"death_overlay.png");
    logo = loadTexture(pRenderer,"logo.png");

    font32 = TTF_OpenFont("cocogoose.ttf", 32);
    font32_outline = TTF_OpenFont("cocogoose.ttf", 32);
    font24 = TTF_OpenFont("cocogoose.ttf", 24);
    font16 = TTF_OpenFont("cocogoose.ttf", 16);
    TTF_SetFontOutline(font32_outline, 3);

    jumpSfx = Mix_LoadWAV("jump.wav");
    fallSfx = Mix_LoadWAV("fall.wav");
    hitSfx = Mix_LoadWAV("hit.wav");
    clickSfx = Mix_LoadWAV("click.wav");
    Mix_PlayChannel(-1, jumpSfx, 0);

    Player player(0, 0, playerTex);
    Ground ground(groundTex[0], groundTex[1], groundTex[2], groundTex[3]);

    //


    bool running = true;
    int width = 320;
    int height = 200;
    int lastTickCount = SDL_GetTicks();
    int curTickCount = SDL_GetTicks();
    int k = 0;
    while (running)
    {
        __android_log_print(ANDROID_LOG_DEBUG, "SCREEN RESOLUTION", "Aa");
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_QUIT:
                {
                    gameRunning = false;
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    if (mainMenu)
                    {
                        if (event.button.button == SDL_BUTTON_LEFT && SDL_GetTicks() > 2500)
                        {
                            mainMenu = false;
                            Mix_PlayChannel(-1, clickSfx, 0);
                        }
                    }
                    else
                    {
                        if (event.button.button == SDL_BUTTON_LEFT && player.isDead() == ALIVE)
                        {
                            if (player.jump())
                            {
                                Mix_PlayChannel(-1, jumpSfx, 0);
                            }
                        }
                        else if (player.isDead() != ALIVE)
                        {
                            Mix_PlayChannel(-1, clickSfx, 0);
                            player.reset();
                            ground.reset();
                            playedDeathSFX = false;
                        }
                    }
                    break;
                }
            }
        }
        if (mainMenu)
        {
            if (SDL_GetTicks() < 2500)
            {
                clear(pRenderer);
                renderCenter(0, sin(SDL_GetTicks()/100) * 2 - 4, "POLYMARS", font24, white, pRenderer);
                display(pRenderer);
            }
            else
            {
                clear(pRenderer);

                render(SCREEN_WIDTH/2 - 234, SCREEN_HEIGHT/2 - 94 - 30, logo, pRenderer);
                renderCenter(0, 90 + sin(SDL_GetTicks()/100) * 2, "Click to start", font24, white, pRenderer);

                for (int i = 0; i < ground.getLength(); i++)
                {
                    render(ground.getTile(i), pRenderer);
                }
                display(pRenderer);
            }
        }
        else
        {
            if (player.isDead() != CURSOR_DEATH)
            {
                ground.update(player.getScoreInt());
            }

            if (player.isDead() == ALIVE)
            {
                player.update(ground);
            }
            else if (!playedDeathSFX) {
                if (player.isDead() == CURSOR_DEATH)
                {
                    Mix_PlayChannel(-1, hitSfx, 0);
                }
                else if (player.isDead() == HOLE_DEATH)
                {
                    Mix_PlayChannel(-1, fallSfx, 0);
                }
                playedDeathSFX = true;
            }

            clear(pRenderer);

            render(player, pRenderer);
            for (int i = 0; i < ground.getLength(); i++)
            {
                render(ground.getTile(i), pRenderer);
            }
            render(25, 30, arrow, pRenderer);
            render(62, 20, player.getScore(), font32_outline, black, pRenderer);
            render(65, 23, player.getScore(), font32, white, pRenderer);
            render(0, 65, highscoreBox, pRenderer);
            render(65, 64, player.getHighscore(), font16, white, pRenderer);

            if (player.isDead() != ALIVE)
            {
                render(deathOverlay, pRenderer);
                if (player.isDead() == CURSOR_DEATH)
                {
                    renderCenter(0, -24, "The cursor is deadly to the player...", font24, white, pRenderer);
                }
                else if (player.isDead() == HOLE_DEATH)
                {
                    renderCenter(0, -24, "The hole had no bottom...", font24, white, pRenderer);
                }
                renderCenter(0, 12, "Click to retry.", font16, white, pRenderer);
            }
            display(pRenderer);
        }

    }

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();

	return 0;
}