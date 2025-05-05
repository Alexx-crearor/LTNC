#ifndef STAGE3_H
#define STAGE3_H

#include "graphics.h"
#include "defs.h"
#include "Player.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

const char* MAP_STAGE3_FILE = "stages\\stage3.png";
const char* METEOR_SPRITE_FILE = "bg\\meteor.png";
const char* FONT_FILE = "font\\Pixel_Sans_Serif.ttf";
const int FONT_SIZE = 120;
const double STAGE3_DURATION_S = 60.0;
const int METEOR_WIDTH = 60;
const int METEOR_HEIGHT = 100;
const float METEOR_MIN_SPEED = 2.0f;
const float METEOR_MAX_SPEED = 5.0f;
const int METEOR_SPAWN_INTERVAL_MS = 1000;

const vector<SDL_Rect> stage3_platforms = { { 0, 540, SCREEN_WIDTH, 60 } };

const float STAGE3_PLAYER_SCALE = 0.4f;
const int STAGE3_RENDER_WIDTH = static_cast<int>(BOBOIBOY_FRAME_WIDTH * STAGE3_PLAYER_SCALE);
const int STAGE3_RENDER_HEIGHT = static_cast<int>(BOBOIBOY_FRAME_HEIGHT * STAGE3_PLAYER_SCALE);
const int PLAYER_START_X_S3 = SCREEN_WIDTH / 2 - STAGE3_RENDER_WIDTH / 2;
const int PLAYER_START_Y_S3 = stage3_platforms[0].y - STAGE3_RENDER_HEIGHT;

struct Meteor { float x, y; float vy; };

inline StageOutcome stage3(Graphics& graphics, bool& soundEnabled, int playerLives,SDL_Texture* texHeart, Mix_Chunk* clickSfx, Mix_Chunk* notiSfx, Mix_Music* bgm, Mix_Chunk* clockSfx) {
    SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    srand(time(0));

    SDL_Log("Loading Stage 3 resources...");
    SDL_Texture* mapTexture = graphics.loadTexture(MAP_STAGE3_FILE);
    SDL_Texture* meteorTexture = graphics.loadTexture(METEOR_SPRITE_FILE);
    TTF_Font* timerFont = TTF_OpenFont(FONT_FILE, FONT_SIZE);
    SDL_Texture* settingButtonTexture = graphics.loadTexture(SETTING_BUTTON_FILE);
    SDL_Texture* setting1Texture = graphics.loadTexture("menu\\5.png");
    SDL_Texture* setting2Texture = graphics.loadTexture("menu\\6.png");

    auto cleanupStage3Textures = [&]() {
         SDL_DestroyTexture(mapTexture);
         SDL_DestroyTexture(meteorTexture);
         TTF_CloseFont(timerFont);
         SDL_DestroyTexture(settingButtonTexture);
         SDL_DestroyTexture(setting1Texture);
         SDL_DestroyTexture(setting2Texture);
         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
         SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_NONE);;
    };

    Player boboiboy(graphics, PLAYER_START_X_S3, PLAYER_START_Y_S3);

    vector<Meteor> meteors;
    Uint32 lastSpawnTime = SDL_GetTicks();
    Uint32 startTime = SDL_GetTicks();
    Uint32 pauseStartTime = 0;
    Uint32 totalPausedTime = 0;

    StageState currentStageState = StageState::PLAYING;
    bool quitRequested = false;
    SDL_Event event;
    int clockChannel = -1;

    if (soundEnabled) {
        graphics.play(bgm);
        clockChannel = Mix_PlayChannel(-1, clockSfx, -1);
    }
    while (!quitRequested) {

        Uint32 frameStartTime = SDL_GetTicks();
        float elapsedSeconds = 0;
        if (currentStageState == StageState::PLAYING) {
             elapsedSeconds = (frameStartTime - startTime - totalPausedTime) / 1000.0f;
        } else {
             elapsedSeconds = (pauseStartTime - startTime - totalPausedTime) / 1000.0f;
        }
        double timeRemaining = STAGE3_DURATION_S - elapsedSeconds;
        if (timeRemaining < 0) timeRemaining = 0;

        if (currentStageState == StageState::PLAYING && timeRemaining <= 0) {
            Mix_HaltChannel(clockChannel);
            Mix_HaltMusic();
            cleanupStage3Textures();
            return StageOutcome::COMPLETED;
        }
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quitRequested = true;
                break;
            }
            if (currentStageState == StageState::PLAYING) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    SDL_Point mousePoint = {mouseX, mouseY};
                    if (SDL_PointInRect(&mousePoint, &SETTING_BUTTON_RECT)) {
                        if (soundEnabled) graphics.play(clickSfx);
                        if (soundEnabled) graphics.play(notiSfx);
                        currentStageState = StageState::PAUSED_SETTINGS;
                        pauseStartTime = SDL_GetTicks();
                        Mix_PauseMusic();
                        if (clockChannel != -1) Mix_Pause(clockChannel);
                    } else {
                     boboiboy.handleEvent(event);
                    }
                } else {
                    boboiboy.handleEvent(event);
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
                                if (clockChannel != -1) {
                                        Mix_Pause(clockChannel);
                                    Mix_HaltChannel(-1);
                                }
                          } else {
                                Mix_ResumeMusic();
                                if (clockChannel != -1) Mix_Resume(clockChannel);
                                if(bgm) graphics.play(bgm);
                            }
                      } else if (SDL_PointInRect(&mousePoint, &closeSettingRect)) {
                          if (soundEnabled) {
                                graphics.play(clickSfx);
                                totalPausedTime += SDL_GetTicks() - pauseStartTime;
                                currentStageState = StageState::PLAYING;
                          }
                          if(soundEnabled) {
                                Mix_ResumeMusic();
                                if (clockChannel != -1) Mix_Resume(clockChannel);
                          }
                      }
                  } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                      if (clickSfx && soundEnabled) graphics.play(clickSfx); totalPausedTime += SDL_GetTicks() - pauseStartTime; currentStageState = StageState::PLAYING;
                      if(soundEnabled) {
                            Mix_ResumeMusic();
                            if (clockChannel != -1) Mix_Resume(clockChannel);
                      }
                  }
             }
        }
        if (quitRequested) break;


        if (currentStageState == StageState::PLAYING) {
            const Uint8* keyStates = SDL_GetKeyboardState(NULL);
            boboiboy.handleInput(keyStates);

            // Update Player
            boboiboy.update(stage3_platforms, {});

            // Tạo Thiên thạch mới
            if (frameStartTime - lastSpawnTime > METEOR_SPAWN_INTERVAL_MS) {
                Meteor newMeteor;
                newMeteor.x = rand() % (SCREEN_WIDTH - METEOR_WIDTH);
                newMeteor.y = -METEOR_HEIGHT; // Bắt đầu từ trên
                newMeteor.vy = METEOR_MIN_SPEED + (float(rand()) / RAND_MAX) * (METEOR_MAX_SPEED - METEOR_MIN_SPEED);
                meteors.push_back(newMeteor);
                lastSpawnTime = frameStartTime; // Dùng thời điểm bắt đầu frame
            }

            //cập nhật && xóa thiên thạch
            for (int i = meteors.size() - 1; i >= 0; --i) {
                meteors[i].y += meteors[i].vy;
                if (meteors[i].y > SCREEN_HEIGHT) {
                    meteors.erase(meteors.begin() + i);
                }
            }

            SDL_Rect playerRect = boboiboy.getRect();
            for (const Meteor& meteor : meteors) {
                 SDL_Rect meteorRect = { static_cast<int>(meteor.x), static_cast<int>(meteor.y), METEOR_WIDTH, METEOR_HEIGHT };
                 if (SDL_HasIntersection(&playerRect, &meteorRect)) {
                     SDL_Log(">>> PLAYER VA CHAM METEOR -> DIED!");
                     Mix_HaltChannel(clockChannel); Mix_HaltMusic(); /* Dừng âm thanh */
                     cleanupStage3Textures(); // Dọn dẹp
                     return StageOutcome::PLAYER_DIED; // Trả về chết
                 }
             }
        }

        graphics.prepareScene(mapTexture);

        if(meteorTexture) {
             SDL_Rect meteorDestRect;
             for (const Meteor& meteor : meteors) {
                  meteorDestRect = { static_cast<int>(meteor.x), static_cast<int>(meteor.y), METEOR_WIDTH, METEOR_HEIGHT };
                  SDL_RenderCopy(graphics.renderer, meteorTexture, NULL, &meteorDestRect);
             }
         }


        boboiboy.render(graphics);

        if (settingButtonTexture) SDL_RenderCopy(graphics.renderer, settingButtonTexture, NULL, &SETTING_BUTTON_RECT);

        if (currentStageState == StageState::PAUSED_SETTINGS) {
            SDL_Texture* settingTexture = soundEnabled ? setting1Texture : setting2Texture;
            int settingW, settingH;
            SDL_QueryTexture(settingTexture, NULL, NULL, &settingW, &settingH);
            SDL_Rect settingDest = {SCREEN_WIDTH / 2 - settingW / 2, SCREEN_HEIGHT/ 2 - settingH/ 2, settingW, settingH};
            SDL_RenderCopy(graphics.renderer, settingTexture, NULL, &settingDest);
        }


        SDL_Color textColor = { 0, 0, 0, 255 };
        int totalSecondsRemaining = static_cast<int>(ceil(timeRemaining));
        int minutes = totalSecondsRemaining / 60; int seconds = totalSecondsRemaining % 60;
        stringstream timeStream;
        timeStream << minutes << ":" << setfill('0') << setw(2) << seconds;
        string timeText = timeStream.str();
        SDL_Surface* textSurface = TTF_RenderText_Blended(timerFont, timeText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(graphics.renderer, textSurface);
        SDL_Rect textRect = {0,0,0,0};
        SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
        textRect.x = SCREEN_WIDTH / 2 - textRect.w / 2; textRect.y = 10;
        SDL_RenderCopy(graphics.renderer, textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);

        SDL_FreeSurface(textSurface);


        if (texHeart) {
            int heartX = 10; int heartY = 10;
            for (int i = 0; i < playerLives; ++i) {
                SDL_Rect heartDestRect = { heartX, heartY, HEART_ICON_WIDTH, HEART_ICON_HEIGHT };
                SDL_RenderCopy(graphics.renderer, texHeart, NULL, &heartDestRect);
                heartX += HEART_ICON_WIDTH + 5;
            }
        }

        graphics.presentScene();
        SDL_Delay(16);

    }
    cleanupStage3Textures();

    Mix_HaltChannel(clockChannel);
    Mix_HaltMusic();
    return StageOutcome::QUIT_APPLICATION;
}

#endif /* STAGE3_H */
