#include <stdio.h>
#include <SDL.h>

int main(int argc, char** argv)
{
    int screen_w;
    int screen_h;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;

    int width = 320;
    int height = 200;

    SDL_CreateWindowAndRenderer(width, height, 0, &pWindow, &pRenderer);


    SDL_GetWindowSize(pWindow, &screen_w, &screen_h);


    bool running = true;

    int lastTickCount = SDL_GetTicks();
    int curTickCount = SDL_GetTicks();
    int k = 0;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
            }
            else if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }


        SDL_Rect srcrect, dstrect;

        srcrect.x = (int)0;
        srcrect.y = (int)0;
        srcrect.w = (int)100;
        srcrect.h = (int)100;

        dstrect.x = (int)0;
        dstrect.y = (int)0;
        dstrect.w = (int)width;
        dstrect.h = (int)height;

        SDL_RenderPresent(pRenderer);

    }

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();

    return 0;
}