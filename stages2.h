#ifndef STAGE2_H
#define STAGE2_H

/* --- Includes --- */
#include "graphics.h"
#include "defs.h"     // Chứa enums, hằng số chung, bơi, nút setting...
#include "Player2.h"  // <<< Dùng Player2 cho màn bơi
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <SDL_mixer.h>

using namespace std;

/* StageOutcome, StageState lấy từ defs.h */

/* --- Hằng số cấu hình Stage 2 --- */
const char* MAP_STAGE2_FILE = "states\\stage2.png";
const float STAGE2_PLAYER_SCALE_FACTOR = 0.4f;
const int STAGE2_RENDER_WIDTH = static_cast<int>(SWIM_FRAME_WIDTH * STAGE2_PLAYER_SCALE_FACTOR);
const int STAGE2_RENDER_HEIGHT = static_cast<int>(SWIM_FRAME_HEIGHT * STAGE2_PLAYER_SCALE_FACTOR);
const vector<SDL_Rect> stage2_platforms = { { 0, 159, 645, 60 }, { 154, 368, 645, 60 } };
const vector<SDL_Rect> stage2_obstacles = { { 433, 107, 63, 52 }, { 465, 70, 28, 37 }, { 281, 220, 17, 79 }, { 378, 429, 123, 45 }, { 423, 475, 69, 29 }, { 443, 504, 21, 16 } };
const SDL_Rect STAGE2_GOAL_RECT = { 0, 75, 66, 84 };
const int STAGE2_START_X = SCREEN_WIDTH - STAGE2_RENDER_WIDTH - 30;
const int STAGE2_START_Y = SCREEN_HEIGHT - STAGE2_RENDER_HEIGHT - 20;
const char* FISH_SPRITE_FILE = "bg/fish.png"; //<<< Sửa lại nếu cần
const int FISH_WIDTH        = 45;
const int FISH_HEIGHT       = 30;
const int FISH_SPEED        = 2;
const int NUMBER_OF_FISH    = 5;
struct FishState { float x, y; int dx; bool facingRight; };


/* --- Hàm thực thi màn chơi Stage 2 --- */
/* <<< SỬA CHỮ KÝ HÀM >>> */
inline StageOutcome stage2(Graphics& graphics, bool& soundEnabled, int playerLives, SDL_Texture* texHeart, Mix_Chunk* clickSfx, Mix_Chunk* notiSfx, Mix_Music* bgm) {
    SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    srand(time(0));

    /* --- Tải tài nguyên --- */
    SDL_Log("Loading Stage 2 resources...");
    SDL_Texture* mapTexture = graphics.loadTexture(MAP_STAGE2_FILE);
    SDL_Texture* fishTexture = graphics.loadTexture(FISH_SPRITE_FILE);
    SDL_Texture* settingButtonTexture = graphics.loadTexture(SETTING_BUTTON_FILE);
    SDL_Texture* setting2Texture = graphics.loadTexture("menu\\5.png");
    SDL_Texture* setting1Texture = graphics.loadTexture("menu\\6.png");

    auto cleanupStage2Textures = [&]() {
         SDL_DestroyTexture(mapTexture); SDL_DestroyTexture(fishTexture);
         SDL_DestroyTexture(settingButtonTexture); SDL_DestroyTexture(setting1Texture);
         SDL_DestroyTexture(setting2Texture);
         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_NONE);
    };

    if (!mapTexture || !fishTexture || !settingButtonTexture || !setting1Texture || !setting2Texture) { /* ... xử lý lỗi ... */ cleanupStage2Textures(); return StageOutcome::QUIT_APPLICATION; }

    /* --- Khởi tạo --- */
    Player2 playerSwim(graphics, STAGE2_START_X, STAGE2_START_Y); // <<< Dùng Player2
    vector<FishState> fishList;
    for (int i = 0; i < NUMBER_OF_FISH; ++i) { /* ... code khởi tạo cá ... */ }

    StageState currentStageState = StageState::PLAYING;
    bool quitRequested = false;
    SDL_Event event;
    Uint32 pauseStartTime = 0;
    Uint32 totalPausedTime = 0;

    /* *** Vòng lặp chính Stage 2 *** */
    while (!quitRequested) {

        /* --- Xử lý sự kiện --- */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { quitRequested = true; break; }

            /* Xử lý Input dựa trên State */
            if (currentStageState == StageState::PLAYING) {
                 if (event.type == SDL_MOUSEBUTTONDOWN) {
                     int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
                     if (SDL_PointInRect(&mousePoint, &SETTING_BUTTON_RECT)) {
                         if (clickSfx && soundEnabled) graphics.play(clickSfx);
                         if (notiSfx && soundEnabled) graphics.play(notiSfx);
                         currentStageState = StageState::PAUSED_SETTINGS; pauseStartTime = SDL_GetTicks(); Mix_PauseMusic(); SDL_Log("Game Paused");
                     } else { /* playerSwim.handleEvent(event); */ }
                 } else { /* playerSwim.handleEvent(event); */ }
            } else { /* PAUSED_SETTINGS */
                 if (event.type == SDL_MOUSEBUTTONDOWN) {
                     int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
                     SDL_Rect musicTickRect = {503, 351, 585 - 503, 426 - 351}; SDL_Rect closeSettingRect = {527, 233, 585 - 527, 288 - 233}; // !!! Kiểm tra tọa độ
                     if (SDL_PointInRect(&mousePoint, &musicTickRect)) {
                         if (clickSfx && soundEnabled) graphics.play(clickSfx);
                         soundEnabled = !soundEnabled; SDL_Log("Sound Toggled: %s", soundEnabled ? "ON" : "OFF");
                         if (!soundEnabled) { Mix_PauseMusic(); Mix_HaltChannel(-1); } else { if (bgm) Mix_ResumeMusic(); } // <<< Dùng bgm
                     } else if (SDL_PointInRect(&mousePoint, &closeSettingRect)) {
                         if (clickSfx && soundEnabled) graphics.play(clickSfx);
                         totalPausedTime += SDL_GetTicks() - pauseStartTime; currentStageState = StageState::PLAYING;
                         if(soundEnabled) Mix_ResumeMusic(); SDL_Log("Settings Closed");
                     }
                 } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                     if (clickSfx && soundEnabled) graphics.play(clickSfx);
                     totalPausedTime += SDL_GetTicks() - pauseStartTime; currentStageState = StageState::PLAYING;
                     if(soundEnabled) Mix_ResumeMusic(); SDL_Log("Settings Closed via ESC");
                 }
            }
        }
        if (quitRequested) break;


        /* --- Chỉ Update khi đang PLAYING --- */
        if (currentStageState == StageState::PLAYING) {
            const Uint8* keyStates = SDL_GetKeyboardState(NULL);
            playerSwim.handleInput(keyStates);
            playerSwim.update(stage2_platforms, stage2_obstacles);
            for (FishState& fish : fishList) { /* ... update cá ... */ }

            /* --- KIỂM TRA VA CHẠM CHẾT NGƯỜI (Obstacle + Fish) --- */
            SDL_Rect playerCurrentRect = playerSwim.getRect();
            for(const SDL_Rect& obs : stage2_obstacles) {
                if (SDL_HasIntersection(&playerCurrentRect, &obs)) {
                     SDL_Log(">>> PLAYER VA CHAM OBSTACLE -> DIED!");
                     cleanupStage2Textures(); return StageOutcome::PLAYER_DIED;
                }
            }
            for (const FishState& fish : fishList) {
                SDL_Rect fishRect = { static_cast<int>(fish.x), static_cast<int>(fish.y), FISH_WIDTH, FISH_HEIGHT };
                if (SDL_HasIntersection(&playerCurrentRect, &fishRect)) {
                     SDL_Log(">>> PLAYER VA CHAM FISH -> DIED!");
                     cleanupStage2Textures(); return StageOutcome::PLAYER_DIED;
                }
            }

            /* Kiểm tra hoàn thành */
            if (SDL_HasIntersection(&playerCurrentRect, &STAGE2_GOAL_RECT)) {
                SDL_Log("Stage 2 Complete!"); SDL_Delay(500);
                cleanupStage2Textures(); return StageOutcome::COMPLETED;
            }
        }

        /* --- Render --- */
        graphics.prepareScene(mapTexture);
        /* Vẽ Debug (tùy chọn) */
        if(fishTexture) { /* ... code vẽ cá ... */ }
        playerSwim.render(graphics); // <<< Vẽ Player2
        if (settingButtonTexture) SDL_RenderCopy(graphics.renderer, settingButtonTexture, NULL, &SETTING_BUTTON_RECT);
        /* --- Vẽ HUD Trái Tim --- */
        if (texHeart) { /* ... code vẽ trái tim như stage 1 ... */ }
        /* Vẽ Setting Overlay nếu Pause */
        if (currentStageState == StageState::PAUSED_SETTINGS) { /* ... code vẽ setting overlay ... */ }
        graphics.presentScene();

        /* --- Delay --- */
        SDL_Delay(16);

    } /* --- Kết thúc vòng lặp chính --- */

    /* --- Dọn dẹp --- */
    cleanupStage2Textures();
    SDL_Log("Exiting Stage 2 function.");
    /* Dừng âm thanh lần cuối */
    Mix_HaltMusic(); Mix_HaltChannel(-1);
    return StageOutcome::QUIT_APPLICATION;
}

#endif /* STAGE2_H */
