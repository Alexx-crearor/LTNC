#ifndef STAGE2_H
#define STAGE2_H

#include "graphics.h"
#include "defs.h"
#include "Player2.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

const char* MAP_STAGE2_FILE = "stages\\stage2.png";
const float STAGE2_PLAYER_SCALE_FACTOR = 0.4f;
const int STAGE2_RENDER_WIDTH = static_cast<int>(SWIM_FRAME_WIDTH * STAGE2_PLAYER_SCALE_FACTOR);
const int STAGE2_RENDER_HEIGHT = static_cast<int>(SWIM_FRAME_HEIGHT * STAGE2_PLAYER_SCALE_FACTOR);

const vector<SDL_Rect> stage2_platforms = { { 0, 159, 645, 60 }, { 154, 368, 645, 60 } };
const vector<SDL_Rect> stage2_obstacles = { { 433, 107, 63, 52 }, { 465, 70, 28, 37 }, { 281, 220, 17, 79 }, { 378, 429, 123, 45 }, { 423, 475, 69, 29 }, { 443, 504, 21, 16 } };
const SDL_Rect STAGE2_GOAL_RECT = { 0, 75, 66, 84 };

const int STAGE2_START_X = SCREEN_WIDTH - STAGE2_RENDER_WIDTH - 30;
const int STAGE2_START_Y = SCREEN_HEIGHT - STAGE2_RENDER_HEIGHT - 20;

const char* FISH_SPRITE_FILE = "bg/fish.png";
const int FISH_WIDTH = 45;
const int FISH_HEIGHT = 30;
const int FISH_SPEED = 2;
const int NUMBER_OF_FISH  = 5;
struct FishState { float x, y; int dx; bool facingRight; };

inline StageOutcome stage2(Graphics& graphics, bool& soundEnabled, int playerLives, SDL_Texture* texHeart, Mix_Chunk* clickSfx, Mix_Chunk* notiSfx, Mix_Music* bgm) {
    SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    srand(time(0));

    SDL_Texture* mapTexture = graphics.loadTexture(MAP_STAGE2_FILE);
    SDL_Texture* fishTexture = graphics.loadTexture(FISH_SPRITE_FILE);
    SDL_Texture* settingButtonTexture = graphics.loadTexture(SETTING_BUTTON_FILE);
    SDL_Texture* setting1Texture = graphics.loadTexture("menu\\5.png");
    SDL_Texture* setting2Texture = graphics.loadTexture("menu\\6.png");

    auto cleanupStage2Textures = [&]() {
         SDL_DestroyTexture(mapTexture);
         SDL_DestroyTexture(fishTexture);
         SDL_DestroyTexture(settingButtonTexture);
         SDL_DestroyTexture(setting1Texture);
         SDL_DestroyTexture(setting2Texture);
         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
         SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_NONE);
    };

    Player2 playerSwim(graphics, STAGE2_START_X, STAGE2_START_Y);
    vector<FishState> fishList;
    for (int i = 0; i < NUMBER_OF_FISH; ++i) {
        FishState fish;
        fish.x = rand() % (SCREEN_WIDTH - FISH_WIDTH);
        fish.y = 50 + rand() % (SCREEN_HEIGHT - FISH_HEIGHT - 100);
        fish.dx = (rand() % 2 == 0) ? FISH_SPEED : -FISH_SPEED;
        fish.facingRight = (fish.dx > 0);
        fishList.push_back(fish);
    }

    StageState currentStageState = StageState::PLAYING;
    bool quitRequested = false;
    SDL_Event event;
    if (soundEnabled) {
        graphics.play(bgm);
    }
    while (!quitRequested) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quitRequested = true;
                break;
            }
             if (currentStageState == StageState::PLAYING) {
                 if (event.type == SDL_MOUSEBUTTONDOWN) {
                     int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY);
                     SDL_Point mousePoint = {mouseX, mouseY};
                     if (SDL_PointInRect(&mousePoint, &SETTING_BUTTON_RECT)) {
                        if (soundEnabled) graphics.play(clickSfx);
                        if (soundEnabled) graphics.play(notiSfx);
                        currentStageState = StageState::PAUSED_SETTINGS;
                        Mix_PauseMusic();;
                     }
                 }
             } else {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY);
                    SDL_Point mousePoint = {mouseX, mouseY};
                    SDL_Rect musicTickRect = {503, 351, 585 - 503, 426 - 351};
                    SDL_Rect closeSettingRect = {527, 233, 585 - 527, 288 - 233};
                    if (SDL_PointInRect(&mousePoint, &musicTickRect)) {
                        if (soundEnabled) graphics.play(clickSfx);
                        soundEnabled = !soundEnabled;
                        if (!soundEnabled) {
                            Mix_PauseMusic();
                            Mix_HaltChannel(-1);
                        } else {
                            if(bgm) Mix_ResumeMusic();
                        }
                  } else if (SDL_PointInRect(&mousePoint, &closeSettingRect)) {
                        if (clickSfx && soundEnabled) graphics.play(clickSfx);
                        currentStageState = StageState::PLAYING;
                        if(soundEnabled) Mix_ResumeMusic();
                        }
                  } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                        if (soundEnabled) graphics.play(clickSfx);
                        currentStageState = StageState::PLAYING;
                        if(soundEnabled) Mix_ResumeMusic();
                  }
             }
        }
        if (quitRequested) break;
        if (currentStageState == StageState::PLAYING) {
            const Uint8* keyStates = SDL_GetKeyboardState(NULL);
            playerSwim.handleInput(keyStates);
            playerSwim.update(stage2_platforms, stage2_obstacles);
            for (FishState& fish : fishList) {
                fish.x += fish.dx;
                if (fish.x < 0) {
                    fish.x = 0;
                    fish.dx = FISH_SPEED;
                    fish.facingRight = true;
                }
                else if (fish.x + FISH_WIDTH > SCREEN_WIDTH) {
                    fish.x = SCREEN_WIDTH - FISH_WIDTH; fish.dx = -FISH_SPEED;
                    fish.facingRight = false;
                }
            }

            SDL_Rect playerCurrentRect = playerSwim.getRect();
            for(const SDL_Rect& obs : stage2_obstacles) {
                if (SDL_HasIntersection(&playerCurrentRect, &obs)) {
                     Mix_HaltMusic();
                     Mix_HaltChannel(-1);
                     SDL_Delay(500);
                     cleanupStage2Textures();
                     return StageOutcome::PLAYER_DIED;
                }
            }

            for (const FishState& fish : fishList) {
                SDL_Rect fishRect = { static_cast<int>(fish.x), static_cast<int>(fish.y), FISH_WIDTH, FISH_HEIGHT };
                if (SDL_HasIntersection(&playerCurrentRect, &fishRect)) {
                     Mix_HaltMusic();
                     Mix_HaltChannel(-1);
                     SDL_Delay(500);
                     cleanupStage2Textures();
                     return StageOutcome::PLAYER_DIED;
                }
            }

            if (SDL_HasIntersection(&playerCurrentRect, &STAGE2_GOAL_RECT)) {
                Mix_HaltMusic();
                Mix_HaltChannel(-1);
                SDL_Delay(500);
                cleanupStage2Textures(); return StageOutcome::COMPLETED;
            }
        }
        graphics.prepareScene(mapTexture);
        //Vẽ
        if(fishTexture) {
             for (const FishState& fish : fishList) {
                  SDL_Rect fishDestRect = { static_cast<int>(fish.x), static_cast<int>(fish.y), FISH_WIDTH, FISH_HEIGHT };
                  SDL_RendererFlip fishFlip = (fish.facingRight) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
                  SDL_RenderCopyEx(graphics.renderer, fishTexture, NULL, &fishDestRect, 0.0, NULL, fishFlip);
             }
        }
        playerSwim.render(graphics);
        if (settingButtonTexture) SDL_RenderCopy(graphics.renderer, settingButtonTexture, NULL, &SETTING_BUTTON_RECT);
        if (texHeart) {
             int heartX = 10; int heartY = 10;
             for (int i = 0; i < playerLives; ++i) {
                 SDL_Rect heartDestRect = { heartX, heartY, HEART_ICON_WIDTH, HEART_ICON_HEIGHT };
                 SDL_RenderCopy(graphics.renderer, texHeart, NULL, &heartDestRect);
                 heartX += HEART_ICON_WIDTH + 5;
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

    cleanupStage2Textures();
    Mix_HaltMusic(); Mix_HaltChannel(-1);
    return StageOutcome::QUIT_APPLICATION;
}

#endif /* STAGE2_H */
