#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include "graphics.h"


#ifndef _EFFECTS__H
#define _EFFECTS__H

void changeScene(Graphics &graphics){
    //Mix_Chunk *cS = graphics.loadSound("Sound\\sfx\\gamein.wav");
    //cS = graphics.loadSound("Sound\\sfx\\ChangeScene.wav");
    //graphics.play(cS);

    SDL_Texture *changeScene = graphics.loadTexture("img\\effects\\1.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\2.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\3.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\4.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\5.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\6.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\7.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\8.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\9.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\10.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\11.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\12.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\13.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\14.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\15.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\16.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\17.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("Image\\ChangeScene\\18.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\19.png");
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(20);

    SDL_DestroyTexture( changeScene );
    SDL_RenderClear(graphics.renderer);
    graphics.presentScene();

    SDL_Delay(1000);
}

void dechangeScene(Graphics &graphics, SDL_Texture *mn){
    SDL_RenderClear(graphics.renderer);

    //Mix_Chunk *cS;
    //cS = graphics.loadSound("Sound\\sfx\\reChangeScene.wav");
    //graphics.play(cS);

    SDL_Texture *changeScene = graphics.loadTexture("img\\effects\\19.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\18.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\17.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\16.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\15.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\14.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\13.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\12.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\11.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\10.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\9.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\8.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\7.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\6.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\5.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("img\\effects\\4.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("Image\\ChangeScene\\3.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("Image\\ChangeScene\\2.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    SDL_Delay(30);

    SDL_DestroyTexture( changeScene );
    changeScene = graphics.loadTexture("Image\\ChangeScene\\1.png");
    graphics.prepareImg(mn);
    graphics.prepareImg(changeScene);
    graphics.presentScene();

    graphics.prepareImg(mn);
    graphics.presentScene();

    SDL_DestroyTexture( changeScene );
    SDL_DestroyTexture( mn );

    SDL_Delay(500);
}

#endif
