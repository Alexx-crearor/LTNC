#ifndef STAGE1_H
#define STAGE1_H

#include "graphics.h"
#include "defs.h"
#include "Player.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

const char* MAP_STAGE1_FILE = "stages\\stage1.png";
const float STAGE1_PLAYER_SCALE = 0.4f;

const int STAGE1_RENDER_WIDTH = static_cast<int>(BOBOIBOY_FRAME_WIDTH * STAGE1_PLAYER_SCALE);
const int STAGE1_RENDER_HEIGHT = static_cast<int>(BOBOIBOY_FRAME_HEIGHT * STAGE1_PLAYER_SCALE);

const vector<SDL_Rect> stage1_platforms = { { 0, 540, 800, 10 }, { 0, 122, 703, 10 }, { 158, 290, 643, 10 } };
const vector<SDL_Rect> stage1_obstacles = { { 214, 86, 103, 66 }, { 400, 252, 68, 88 }, { 333, 481, 82, 58 } };
const SDL_Rect STAGE1_GOAL_RECT = { 698, 455, 66, 84 };

const int PLAYER_START_X_S1 = stage1_platforms[1].x + 20;
const int PLAYER_START_Y_S1 = stage1_platforms[1].y - STAGE1_RENDER_HEIGHT;



inline StageOutcome stage1(Graphics& graphics, bool& soundEnabled, int playerLives, SDL_Texture* texHeart, Mix_Chunk* clickSfx, Mix_Chunk* notiSfx, Mix_Music* bgm) {
    SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Texture* mapTexture = graphics.loadTexture(MAP_STAGE1_FILE);
    SDL_Texture* settingButtonTexture = graphics.loadTexture(SETTING_BUTTON_FILE);
    SDL_Texture* setting2Texture = graphics.loadTexture("menu\\5.png");
    SDL_Texture* setting1Texture = graphics.loadTexture("menu\\6.png");

    auto cleanupStage1Textures = [&]() {
         SDL_DestroyTexture(mapTexture);
         SDL_DestroyTexture(settingButtonTexture);
         SDL_DestroyTexture(setting1Texture);
         SDL_DestroyTexture(setting2Texture);
         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
         SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_NONE);};

    Player boboiboy(graphics, PLAYER_START_X_S1, PLAYER_START_Y_S1);
    StageState currentStageState = StageState::PLAYING;
    bool quitRequested = false;
    SDL_Event event;
    Uint32 pauseStartTime = 0;
    while (!quitRequested) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quitRequested = true;
                break;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                SDL_Point mousePoint = {mouseX, mouseY};

                if (currentStageState == StageState::PLAYING) {
                    if (SDL_PointInRect(&mousePoint, &SETTING_BUTTON_RECT)) {
                        if (soundEnabled) graphics.play(clickSfx);
                        if (soundEnabled) graphics.play(notiSfx);
                        currentStageState = StageState::PAUSED_SETTINGS;
                        Mix_PauseMusic();
                    } else {
                        boboiboy.handleEvent(event);
                    }
                } else {
                    SDL_Rect musicTickRect = {503, 351, 585 - 503, 426 - 351};
                    SDL_Rect closeSettingRect = {527, 233, 585 - 527, 288 - 233};

                    if (SDL_PointInRect(&mousePoint, &musicTickRect)) {
                        if (soundEnabled) graphics.play(clickSfx);
                        soundEnabled = !soundEnabled;
                        if (!soundEnabled) {
                            Mix_PauseMusic();
                            Mix_HaltChannel(-1);
                        } else {
                            Mix_ResumeMusic();
                        }
                    } else if (SDL_PointInRect(&mousePoint, &closeSettingRect)) {
                        if (soundEnabled) graphics.play(clickSfx);
                        currentStageState = StageState::PLAYING;
                        if(soundEnabled) Mix_ResumeMusic();
                    }
                }
            } else if (event.type == SDL_KEYDOWN) {
                 if (event.key.keysym.sym == SDLK_ESCAPE && currentStageState == StageState::PAUSED_SETTINGS) {
                     if (clickSfx && soundEnabled) graphics.play(clickSfx);
                     currentStageState = StageState::PLAYING;
                      if(soundEnabled) Mix_ResumeMusic();
                 } else if (currentStageState == StageState::PLAYING) {
                      boboiboy.handleEvent(event);
                 }
            } else if (currentStageState == StageState::PLAYING) {
                 boboiboy.handleEvent(event);
            }
        }
        if (quitRequested) break;

        if (currentStageState == StageState::PLAYING) {
            const Uint8* keyStates = SDL_GetKeyboardState(NULL);
            boboiboy.handleInput(keyStates);
            boboiboy.update(stage1_platforms, stage1_obstacles);

            SDL_Rect playerCurrentRect = boboiboy.getRect();
            for(const SDL_Rect& obs : stage1_obstacles) {
                if (SDL_HasIntersection(&playerCurrentRect, &obs)) {
                     Mix_HaltMusic();
                     Mix_HaltChannel(-1);
                     SDL_Delay(500);
                     cleanupStage1Textures();
                     return StageOutcome::PLAYER_DIED;
                }
            }
            if (SDL_HasIntersection(&playerCurrentRect, &STAGE1_GOAL_RECT)) {
                Mix_HaltMusic();
                Mix_HaltChannel(-1);
                SDL_Delay(500);
                cleanupStage1Textures();
                return StageOutcome::COMPLETED;
            }
        }

        graphics.prepareScene(mapTexture);
        boboiboy.render(graphics);
        if (settingButtonTexture) SDL_RenderCopy(graphics.renderer, settingButtonTexture, NULL, &SETTING_BUTTON_RECT);
        if (texHeart != nullptr) {
            int currentHeartX = 10;
            const int heartY = 10;
            const int heartSpacing = 5;


            for (int i = 0; i < playerLives; ++i) {
                SDL_Rect heartDestRect = { currentHeartX, heartY, HEART_ICON_WIDTH, HEART_ICON_HEIGHT };
                SDL_RenderCopy(graphics.renderer, texHeart, NULL, &heartDestRect);
                currentHeartX += HEART_ICON_WIDTH + heartSpacing;
            }
        }

        if (currentStageState == StageState::PAUSED_SETTINGS) {
            SDL_Texture* settingTexture = soundEnabled ? setting1Texture : setting2Texture;
            int settingW, settingH; SDL_QueryTexture(settingTexture, NULL, NULL, &settingW, &settingH);
            SDL_Rect settingDest = {SCREEN_WIDTH / 2 - settingW / 2, SCREEN_HEIGHT/ 2 - settingH/ 2, settingW, settingH};
            SDL_RenderCopy(graphics.renderer, settingTexture, NULL, &settingDest);
        }
        graphics.presentScene();
        SDL_Delay(16);

    }

    cleanupStage1Textures();
    Mix_HaltMusic();
    Mix_HaltChannel(-1);
    return StageOutcome::QUIT_APPLICATION;
}

#endif /* STAGE1_H */
