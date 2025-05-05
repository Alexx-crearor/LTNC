#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include "graphics.h"

using namespace std;

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_PollEvent(&e) != 0 && (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}


void ScrollingBG(char *s, Graphics& graphics) {

    ScrollingBackground background;
    background.setTexture(graphics.loadTexture(s));

    bool quit = false;
    SDL_Event e;
    while( !quit ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) quit = true;
        }

        background.scroll(5);

        graphics.render(background);

        graphics.presentScene();
        SDL_Delay(70);
    }

    SDL_DestroyTexture( background.texture );




}
