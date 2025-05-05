#ifndef STAGE3_H
#define STAGE3_H

/* --- Includes --- */
#include "graphics.h"
#include "defs.h"
#include "Player.h" // <<< Dùng Player (đi bộ/nhảy)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h> // <<< Thêm include
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

/* StageOutcome, StageState lấy từ defs.h */

/* --- Hằng số cấu hình Stage 3 --- */
/* ... (Các hằng số MAP, METEOR, FONT, DURATION, PLATFORM, OBSTACLES, START POS như cũ) ... */
const char* MAP_STAGE3_FILE = "stages\\stage3.png";
const char* METEOR_SPRITE_FILE = "bg\\meteor.png";
const char* FONT_FILE = "font\\Pixel_Sans_Serif.ttf";
const int FONT_SIZE = 120;
const double STAGE3_DURATION_S = 60.0;
const int METEOR_WIDTH = 60; const int METEOR_HEIGHT = 100;
const float METEOR_MIN_SPEED = 2.0f; const float METEOR_MAX_SPEED = 5.0f;
const int METEOR_SPAWN_INTERVAL_MS = 1000;
const vector<SDL_Rect> stage3_platforms = { { 0, 540, SCREEN_WIDTH, 60 } };
const vector<SDL_Rect> stage3_obstacles = {};
const float STAGE3_PLAYER_SCALE = 0.4f;
const int STAGE3_RENDER_WIDTH = static_cast<int>(BOBOIBOY_FRAME_WIDTH * STAGE3_PLAYER_SCALE);
const int STAGE3_RENDER_HEIGHT = static_cast<int>(BOBOIBOY_FRAME_HEIGHT * STAGE3_PLAYER_SCALE);
const int PLAYER_START_X_S3 = SCREEN_WIDTH / 2 - STAGE3_RENDER_WIDTH / 2;
const int PLAYER_START_Y_S3 = stage3_platforms[0].y - STAGE3_RENDER_HEIGHT;
struct Meteor { float x, y; float vy; };


/* --- Hàm thực thi màn chơi Stage 3 --- */
/* <<< SỬA CHỮ KÝ HÀM >>> */
inline StageOutcome stage3(Graphics& graphics, bool& soundEnabled, int playerLives, SDL_Texture* texHeart, Mix_Chunk* clickSfx, Mix_Chunk* notiSfx, Mix_Music* bgm, Mix_Chunk* clockSfx) { // <<< Thêm bgm, clockSfx
    SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    srand(time(0));

    /* --- 1. Tải tài nguyên --- */
    SDL_Log("Loading Stage 3 resources...");
    SDL_Texture* mapTexture = graphics.loadTexture(MAP_STAGE3_FILE);
    SDL_Texture* meteorTexture = graphics.loadTexture(METEOR_SPRITE_FILE);
    TTF_Font* timerFont = TTF_OpenFont(FONT_FILE, FONT_SIZE);
    SDL_Texture* settingButtonTexture = graphics.loadTexture(SETTING_BUTTON_FILE);
    SDL_Texture* setting1Texture = graphics.loadTexture("menu\\5.png");
    SDL_Texture* setting2Texture = graphics.loadTexture("menu\\6.png");

    /* Hàm dọn dẹp */
    auto cleanupStage3Textures = [&]() { /* ... dọn dẹp map, meteor, font, button, settings ... */
         SDL_DestroyTexture(mapTexture); SDL_DestroyTexture(meteorTexture); TTF_CloseFont(timerFont);
         SDL_DestroyTexture(settingButtonTexture); SDL_DestroyTexture(setting1Texture); SDL_DestroyTexture(setting2Texture);
         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_NONE);
    };

    if (!mapTexture || !meteorTexture || !timerFont || !settingButtonTexture || !setting1Texture || !setting2Texture) { /* ... xử lý lỗi ... */ cleanupStage3Textures(); return StageOutcome::QUIT_APPLICATION; }

    /* --- 2. Khởi tạo --- */
    Player boboiboy(graphics, PLAYER_START_X_S3, PLAYER_START_Y_S3); // <<< Dùng Player thường
    vector<Meteor> meteors;
    Uint32 lastSpawnTime = SDL_GetTicks(); Uint32 startTime = SDL_GetTicks();
    Uint32 pauseStartTime = 0; Uint32 totalPausedTime = 0;

    StageState currentStageState = StageState::PLAYING;
    bool quitRequested = false;
    SDL_Event event;
    int clockChannel = -1; // Kênh đang chơi tiếng đồng hồ

    /* --- Bắt đầu chơi nhạc nền và clock nếu sound on --- */
    if (soundEnabled) {
        graphics.play(bgm); // Chơi nhạc nền stage 3
        if (clockSfx) clockChannel = Mix_PlayChannel(-1, clockSfx, -1); // Chơi lặp lại clock
    }

    /* *** Vòng lặp chính Stage 3 *** */
    SDL_Log("Entering Stage 3 loop...");
    while (!quitRequested) {

        Uint32 currentTime = SDL_GetTicks();
        float elapsedSeconds = 0;
        if (currentStageState == StageState::PLAYING) {
             elapsedSeconds = (currentTime - startTime - totalPausedTime) / 1000.0f;
        } else { elapsedSeconds = (pauseStartTime - startTime - totalPausedTime) / 1000.0f; }
        double timeRemaining = STAGE3_DURATION_S - elapsedSeconds;
        if (timeRemaining < 0) timeRemaining = 0;

        /* --- Kiểm tra hết giờ --- */
        if (currentStageState == StageState::PLAYING && timeRemaining <= 0) {
            SDL_Log("Stage 3 Complete (Time Up)!");
            Mix_HaltChannel(clockChannel); Mix_HaltMusic(); // Dừng âm thanh trước khi thoát
            cleanupStage3Textures(); return StageOutcome::COMPLETED;
        }

        /* --- Xử lý sự kiện --- */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { quitRequested = true; break; }

            /* Xử lý Input dựa trên State (Giống stage 1 & 2) */
             if (currentStageState == StageState::PLAYING) { /* ... kiểm tra click setting ... */
                 if (event.type == SDL_MOUSEBUTTONDOWN) {
                     int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
                     if (SDL_PointInRect(&mousePoint, &SETTING_BUTTON_RECT)) {
                         if (clickSfx && soundEnabled) graphics.play(clickSfx); if (notiSfx && soundEnabled) graphics.play(notiSfx);
                         currentStageState = StageState::PAUSED_SETTINGS; pauseStartTime = SDL_GetTicks();
                         Mix_PauseMusic(); if (clockChannel != -1) Mix_Pause(clockChannel); /* Pause clock */ SDL_Log("Game Paused");
                     } else { boboiboy.handleEvent(event); }
                 } else { boboiboy.handleEvent(event); }
             } else { /* PAUSED_SETTINGS */ /* ... xử lý click trong setting hoặc ESC ... */
                   if (event.type == SDL_MOUSEBUTTONDOWN) {
                       int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
                       SDL_Rect musicTickRect = {503, 351, 585 - 503, 426 - 351}; SDL_Rect closeSettingRect = {527, 233, 585 - 527, 288 - 233};
                       if (SDL_PointInRect(&mousePoint, &musicTickRect)) {
                           if (clickSfx && soundEnabled) graphics.play(clickSfx);
                           soundEnabled = !soundEnabled; SDL_Log("Sound Toggled: %s", soundEnabled ? "ON" : "OFF");
                           if (!soundEnabled) { Mix_PauseMusic(); if (clockChannel != -1) Mix_Pause(clockChannel); Mix_HaltChannel(-1); /* Dừng SFX khác */}
                           else { Mix_ResumeMusic(); if (clockChannel != -1) Mix_Resume(clockChannel); /* Resume clock */ }
                       } else if (SDL_PointInRect(&mousePoint, &closeSettingRect)) {
                           if (clickSfx && soundEnabled) graphics.play(clickSfx); totalPausedTime += SDL_GetTicks() - pauseStartTime; currentStageState = StageState::PLAYING;
                           if(soundEnabled) { Mix_ResumeMusic(); if (clockChannel != -1) Mix_Resume(clockChannel); /* Resume clock */} SDL_Log("Settings Closed");
                       }
                   } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                      if (clickSfx && soundEnabled) graphics.play(clickSfx); totalPausedTime += SDL_GetTicks() - pauseStartTime; currentStageState = StageState::PLAYING;
                      if(soundEnabled) { Mix_ResumeMusic(); if (clockChannel != -1) Mix_Resume(clockChannel); /* Resume clock */} SDL_Log("Settings Closed via ESC");
                   }
             }
        }
        if (quitRequested) break;

        /* --- Chỉ Update khi đang PLAYING --- */
        if (currentStageState == StageState::PLAYING) {
            /* Input Player */
            const Uint8* keyStates = SDL_GetKeyboardState(NULL);
            boboiboy.handleInput(keyStates);
            /* Update Player */
            boboiboy.update(stage3_platforms, {}); /* obstacles rỗng */
            /* Tạo Thiên thạch mới */
            if (currentTime - lastSpawnTime > METEOR_SPAWN_INTERVAL_MS) { /* ... code spawn meteor ... */ }
            /* Cập nhật & Xóa Thiên thạch */
            for (int i = meteors.size() - 1; i >= 0; --i) { /* ... code update & erase meteor ... */ }

            /* --- Va chạm Player - Thiên thạch --- */
            SDL_Rect playerRect = boboiboy.getRect();
            for (const Meteor& meteor : meteors) {
                 SDL_Rect meteorRect = { static_cast<int>(meteor.x), static_cast<int>(meteor.y), METEOR_WIDTH, METEOR_HEIGHT };
                 if (SDL_HasIntersection(&playerRect, &meteorRect)) {
                     SDL_Log(">>> PLAYER VA CHAM METEOR -> DIED!");
                     Mix_HaltChannel(clockChannel); Mix_HaltMusic(); /* Dừng âm thanh */
                     cleanupStage3Textures(); return StageOutcome::PLAYER_DIED;
                 }
             }
        } /* Kết thúc if PLAYING */

        /* --- Render --- */
        graphics.prepareScene(mapTexture);
        /* Vẽ Thiên thạch */
        if(meteorTexture) { /* ... code vẽ meteor ... */ }
        /* Vẽ Player */
        boboiboy.render(graphics);
        /* Vẽ nút Setting */
        if (settingButtonTexture) SDL_RenderCopy(graphics.renderer, settingButtonTexture, NULL, &SETTING_BUTTON_RECT);
        /* Vẽ màn hình Settings nếu Pause */
        if (currentStageState == StageState::PAUSED_SETTINGS) { /* ... code vẽ setting overlay ... */ }
        /* Vẽ Đồng hồ */
        SDL_Color textColor = { 0, 0, 0, 255 }; /* ... code vẽ đồng hồ ... */
        /* Vẽ HUD Trái Tim */
        if (texHeart) { /* ... code vẽ trái tim ... */ }
        graphics.presentScene();

        /* --- Delay --- */
        SDL_Delay(16);

    } /* --- Kết thúc vòng lặp chính --- */

    /* --- Dọn dẹp --- */
    cleanupStage3Textures();
    SDL_Log("Exiting Stage 3 function.");
    Mix_HaltChannel(clockChannel); Mix_HaltMusic(); /* Dừng âm thanh lần cuối */
    return StageOutcome::QUIT_APPLICATION;
}

#endif /* STAGE3_H */
