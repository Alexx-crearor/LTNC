#ifndef MENU_H
#define MENU_H

#include "graphics.h"
#include "defs.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std;

inline MenuResult menu(Graphics& graphics, bool& soundEnabled, Mix_Chunk* clickSfx, Mix_Chunk* notiSfx, Mix_Music* menuMusicBgm) {
    ScrollingBackground background;
    SDL_Texture* bgTexture = graphics.loadTexture("bg\\menu.png");

    SDL_Texture* menu1 = graphics.loadTexture("menu\\1.png"); //intro
    SDL_Texture* menu2 = graphics.loadTexture("menu\\2.png"); //chọn
    SDL_Texture* menu3 = graphics.loadTexture("menu\\3.png"); // hover start
    SDL_Texture* menu4 = graphics.loadTexture("menu\\4.png"); // hover menu

    SDL_Texture* setting2Texture = graphics.loadTexture("menu\\5.png"); // Ảnh không tick (Sound Off)
    SDL_Texture* setting1Texture = graphics.loadTexture("menu\\6.png"); // Ảnh có tick (Sound On)
    SDL_Texture* setting3Texture = graphics.loadTexture("menu\\7.png"); // Ảnh trống

    background.setTexture(bgTexture);

    SDL_Texture* menuSetting = setting3Texture;
    SDL_Texture* menuTexture = menu1;

    int x, y;
    MenuResult finalOutcome = MenuResult::QUIT_APPLICATION;

    int menuState = 0;
    SDL_Event e;

    while (true) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                finalOutcome = MenuResult::QUIT_APPLICATION;
                goto cleanup_and_return;
            }
            else if ((e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) && menuState == 0) {
                 graphics.play(clickSfx);
                 menuState = 1;
                 menuTexture = menu2;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                SDL_Point mousePoint = {x, y};

                if (menuState == 1) {
                    SDL_Rect startRect = {310, 288, 486-310, 359-288};
                    SDL_Rect settingRect = {310, 403, 486-310, 479-403};
                    if (SDL_PointInRect(&mousePoint, &startRect)) {
                        if (soundEnabled) graphics.play(clickSfx);
                        finalOutcome = MenuResult::START_GAME;
                        goto cleanup_and_return;
                    } else if (SDL_PointInRect(&mousePoint, &settingRect)) {
                        if (soundEnabled) graphics.play(clickSfx);
                        if (soundEnabled) graphics.play(notiSfx);
                        menuState = 3;
                        menuTexture = menu2;
                        menuSetting = soundEnabled ? setting1Texture : setting2Texture;
                    }
                } else if (menuState == 3) {
                    SDL_Rect musicTickRect = {503, 351, 585 - 503, 426 - 351};
                    SDL_Rect closeSettingRect = {527, 233, 585 - 527, 288 - 233};
                    if (SDL_PointInRect(&mousePoint, &musicTickRect)) {
                         if (soundEnabled) graphics.play(clickSfx);
                         //phủ định lại sounEnabled
                         soundEnabled = !soundEnabled;
                         //kiểm tra âm thanh
                         if (!soundEnabled) {
                             Mix_HaltMusic(); Mix_HaltChannel(-1);
                         } else {
                             if(menuMusicBgm) graphics.play(menuMusicBgm);
                         }
                         menuSetting = soundEnabled ? setting1Texture : setting2Texture;

                    } else if (SDL_PointInRect(&mousePoint, &closeSettingRect)) {
                         if (soundEnabled) graphics.play(clickSfx);
                         menuState = 1;
                         menuTexture = menu2;
                         menuSetting = setting3Texture;
                    }
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE && menuState == 3) {
                 if (soundEnabled) graphics.play(clickSfx);
                 menuState = 1;
                 menuTexture = menu2;
                 menuSetting = setting3Texture;
            }
        }

        // Hover
        if (menuState == 1) {
            SDL_GetMouseState(&x, &y);
            SDL_Rect startRect = {310, 288, 486-310, 359-288};
            SDL_Rect settingRect = {310, 403, 486-310, 479-403};
            SDL_Point mousePoint = {x, y};
            if (SDL_PointInRect(&mousePoint, &startRect)) {
                    menuTexture = menu3;
            } else if (SDL_PointInRect(&mousePoint, &settingRect)) {
                 menuTexture = menu4;
            } else {
                menuTexture = menu2;
            }
        }

        graphics.prepareScene();
        background.scroll(1);
        graphics.render(background);
        graphics.renderTexture(menuTexture, 0, 0);

        if (menuState == 3) {
             menuSetting = soundEnabled ? setting1Texture : setting2Texture;
             if (menuSetting) {
                 int settingW, settingH;
                 SDL_QueryTexture(menuSetting, NULL, NULL, &settingW, &settingH);
                 SDL_Rect settingDest = {SCREEN_WIDTH / 2 - settingW / 2, SCREEN_HEIGHT/ 2 - settingH/ 2, settingW, settingH};
                 SDL_RenderCopy(graphics.renderer, menuSetting, NULL, &settingDest);
             }
        }
        graphics.presentScene();

        SDL_Delay(16);
    }

cleanup_and_return:
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(menu1);
    SDL_DestroyTexture(menu2);
    SDL_DestroyTexture(menu3);
    SDL_DestroyTexture(menu4);
    SDL_DestroyTexture(setting1Texture);
    SDL_DestroyTexture(setting2Texture);
    SDL_DestroyTexture(setting3Texture);

    return finalOutcome;
}

#endif /* MENU_H */
