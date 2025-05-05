#ifndef STAGE3_H
#define STAGE3_H

/* --- Includes --- */
#include "graphics.h" // Lớp/Struct quản lý đồ họa
#include "defs.h"     // Chứa các hằng số và Enums dùng chung
#include "Player.h"   // Dùng Player struct (đi bộ/nhảy)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>   // Thư viện xử lý Font chữ
#include <SDL_mixer.h> // Thư viện xử lý âm thanh
#include <iostream>
#include <vector>
#include <string>
#include <sstream>   // Dùng cho chuyển đổi số sang chuỗi (đồng hồ)
#include <iomanip>   // Dùng cho định dạng số (đồng hồ)
#include <cstdlib>   // Dùng cho rand(), srand()
#include <ctime>     // Dùng cho time()

using namespace std; // Theo yêu cầu sử dụng namespace std

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

/* Mặt đất cho stage 3 */
const vector<SDL_Rect> stage3_platforms = { { 0, 540, SCREEN_WIDTH, 60 } };
const vector<SDL_Rect> stage3_obstacles = {}; // Không có obstacle tĩnh

/* Vị trí bắt đầu Player */
const float STAGE3_PLAYER_SCALE = 0.4f; // Cần đồng bộ với Player.h
const int STAGE3_RENDER_WIDTH = static_cast<int>(BOBOIBOY_FRAME_WIDTH * STAGE3_PLAYER_SCALE);
const int STAGE3_RENDER_HEIGHT = static_cast<int>(BOBOIBOY_FRAME_HEIGHT * STAGE3_PLAYER_SCALE);
const int PLAYER_START_X_S3 = SCREEN_WIDTH / 2 - STAGE3_RENDER_WIDTH / 2;
const int PLAYER_START_Y_S3 = stage3_platforms[0].y - STAGE3_RENDER_HEIGHT;

/* Cấu trúc Thiên thạch */
struct Meteor { float x, y; float vy; };


/* --- Hàm thực thi màn chơi Stage 3 --- */
inline StageOutcome stage3(Graphics& graphics, bool& soundEnabled, int playerLives,
                           SDL_Texture* texHeart, Mix_Chunk* clickSfx, Mix_Chunk* notiSfx,
                           Mix_Music* bgm, Mix_Chunk* clockSfx) { // <<< Đầy đủ tham số

    SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    srand(time(0));

    /* --- 1. Tải tài nguyên đặc thù của Stage 3 --- */
    SDL_Log("Loading Stage 3 resources...");
    SDL_Texture* mapTexture = graphics.loadTexture(MAP_STAGE3_FILE);
    SDL_Texture* meteorTexture = graphics.loadTexture(METEOR_SPRITE_FILE);
    TTF_Font* timerFont = TTF_OpenFont(FONT_FILE, FONT_SIZE);
    SDL_Texture* settingButtonTexture = graphics.loadTexture(SETTING_BUTTON_FILE);
    SDL_Texture* setting1Texture = graphics.loadTexture("menu\\5.png");
    SDL_Texture* setting2Texture = graphics.loadTexture("menu\\6.png");

    /* Hàm lambda dọn dẹp tài nguyên của stage này */
    auto cleanupStage3Textures = [&]() {
         SDL_DestroyTexture(mapTexture); SDL_DestroyTexture(meteorTexture); TTF_CloseFont(timerFont);
         SDL_DestroyTexture(settingButtonTexture); SDL_DestroyTexture(setting1Texture); SDL_DestroyTexture(setting2Texture);
         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); SDL_SetRenderDrawBlendMode(graphics.renderer, SDL_BLENDMODE_NONE);
         SDL_Log("Cleaned up Stage 3 textures and font.");
    };

    /* Kiểm tra lỗi load */
    if (!mapTexture || !meteorTexture || !timerFont || !settingButtonTexture || !setting1Texture || !setting2Texture) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,"Error loading resources for Stage 3!");
        if (!timerFont) SDL_Log("TTF Error: %s", TTF_GetError());
        cleanupStage3Textures();
        return StageOutcome::QUIT_APPLICATION; // Thoát nếu lỗi nghiêm trọng
    }

    /* --- 2. Khởi tạo đối tượng và trạng thái --- */
    Player boboiboy(graphics, PLAYER_START_X_S3, PLAYER_START_Y_S3); // Dùng Player thường

    vector<Meteor> meteors; // Danh sách thiên thạch
    Uint32 lastSpawnTime = SDL_GetTicks(); // Thời điểm tạo thiên thạch cuối
    Uint32 startTime = SDL_GetTicks();    // Thời điểm bắt đầu màn chơi
    Uint32 pauseStartTime = 0;          // Thời điểm bắt đầu pause
    Uint32 totalPausedTime = 0;         // Tổng thời gian đã pause

    StageState currentStageState = StageState::PLAYING; // Trạng thái màn chơi
    bool quitRequested = false;                       // Cờ yêu cầu thoát game
    SDL_Event event;                                  // Biến sự kiện
    int clockChannel = -1;                            // Kênh đang phát tiếng đồng hồ

    /* Bắt đầu âm thanh (nếu bật) */
    if (soundEnabled) {
        graphics.play(bgm); // Chơi nhạc nền stage 3
        if (clockSfx) clockChannel = Mix_PlayChannel(-1, clockSfx, -1); // Chơi lặp lại clock
        if (clockChannel == -1 && clockSfx) { SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,"Could not play clock sound"); }
    }

    /* *** Vòng lặp chính Stage 3 *** */
    SDL_Log("Entering Stage 3 loop...");
    while (!quitRequested) {

        Uint32 frameStartTime = SDL_GetTicks(); // Thời điểm bắt đầu frame hiện tại

        /* --- Tính toán thời gian còn lại (có trừ thời gian pause) --- */
        float elapsedSeconds = 0;
        if (currentStageState == StageState::PLAYING) {
             elapsedSeconds = (frameStartTime - startTime - totalPausedTime) / 1000.0f;
        } else { // Đang PAUSED_SETTINGS
             elapsedSeconds = (pauseStartTime - startTime - totalPausedTime) / 1000.0f;
        }
        double timeRemaining = STAGE3_DURATION_S - elapsedSeconds;
        if (timeRemaining < 0) timeRemaining = 0;

        /* --- Kiểm tra hết giờ --- */
        if (currentStageState == StageState::PLAYING && timeRemaining <= 0) {
            SDL_Log("Stage 3 Complete (Time Up)!");
            Mix_HaltChannel(clockChannel); Mix_HaltMusic(); // Dừng âm thanh
            cleanupStage3Textures();
            return StageOutcome::COMPLETED; // Trả về thắng
        }

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
                         currentStageState = StageState::PAUSED_SETTINGS; pauseStartTime = SDL_GetTicks();
                         Mix_PauseMusic(); if (clockChannel != -1) Mix_Pause(clockChannel); SDL_Log("Game Paused");
                     } else { boboiboy.handleEvent(event); }
                 } else { boboiboy.handleEvent(event); }
             } else { /* PAUSED_SETTINGS */
                  if (event.type == SDL_MOUSEBUTTONDOWN) {
                      int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY); SDL_Point mousePoint = {mouseX, mouseY};
                      SDL_Rect musicTickRect = {503, 351, 585 - 503, 426 - 351}; /* !!! Kiểm tra tọa độ !!! */
                      SDL_Rect closeSettingRect = {527, 233, 585 - 527, 288 - 233}; /* !!! Kiểm tra tọa độ !!! */
                      if (SDL_PointInRect(&mousePoint, &musicTickRect)) {
                          if (clickSfx && soundEnabled) graphics.play(clickSfx);
                          soundEnabled = !soundEnabled; SDL_Log("Sound Toggled: %s", soundEnabled ? "ON" : "OFF");
                          if (!soundEnabled) { Mix_PauseMusic(); if (clockChannel != -1) Mix_Pause(clockChannel); Mix_HaltChannel(-1); } // Dừng hẳn SFX khác
                          else { Mix_ResumeMusic(); if (clockChannel != -1) Mix_Resume(clockChannel); if(bgm) graphics.play(bgm); /* Đảm bảo nhạc nền chạy */} // Chơi lại nhạc nền nếu resume không đủ
                      } else if (SDL_PointInRect(&mousePoint, &closeSettingRect)) {
                          if (clickSfx && soundEnabled) graphics.play(clickSfx); totalPausedTime += SDL_GetTicks() - pauseStartTime; currentStageState = StageState::PLAYING;
                          if(soundEnabled) { Mix_ResumeMusic(); if (clockChannel != -1) Mix_Resume(clockChannel); } SDL_Log("Settings Closed");
                      }
                  } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                      if (clickSfx && soundEnabled) graphics.play(clickSfx); totalPausedTime += SDL_GetTicks() - pauseStartTime; currentStageState = StageState::PLAYING;
                      if(soundEnabled) { Mix_ResumeMusic(); if (clockChannel != -1) Mix_Resume(clockChannel); } SDL_Log("Settings Closed via ESC");
                  }
             }
        } // Kết thúc PollEvent
        if (quitRequested) break;


        /* --- Chỉ Update khi đang PLAYING --- */
        if (currentStageState == StageState::PLAYING) {
            /* Input Player */
            const Uint8* keyStates = SDL_GetKeyboardState(NULL);
            boboiboy.handleInput(keyStates);

            /* Update Player (chỉ có platform mặt đất, không có obstacles tĩnh) */
            boboiboy.update(stage3_platforms, {}); /* Truyền vector obstacles rỗng */

            /* Tạo Thiên thạch mới */
            if (frameStartTime - lastSpawnTime > METEOR_SPAWN_INTERVAL_MS) {
                Meteor newMeteor;
                newMeteor.x = rand() % (SCREEN_WIDTH - METEOR_WIDTH);
                newMeteor.y = -METEOR_HEIGHT; // Bắt đầu từ trên
                newMeteor.vy = METEOR_MIN_SPEED + (float(rand()) / RAND_MAX) * (METEOR_MAX_SPEED - METEOR_MIN_SPEED);
                meteors.push_back(newMeteor);
                lastSpawnTime = frameStartTime; // Dùng thời điểm bắt đầu frame
            }

            /* Cập nhật & Xóa Thiên thạch */
            for (int i = meteors.size() - 1; i >= 0; --i) {
                meteors[i].y += meteors[i].vy;
                if (meteors[i].y > SCREEN_HEIGHT) {
                    meteors.erase(meteors.begin() + i);
                }
            }

            /* --- Va chạm Player - Thiên thạch --- */
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
        } /* Kết thúc if PLAYING */


        /* --- Render --- */
        graphics.prepareScene(mapTexture); /* Vẽ nền */

        /* Vẽ Thiên thạch */
        if(meteorTexture) {
             SDL_Rect meteorDestRect;
             for (const Meteor& meteor : meteors) {
                  meteorDestRect = { static_cast<int>(meteor.x), static_cast<int>(meteor.y), METEOR_WIDTH, METEOR_HEIGHT };
                  SDL_RenderCopy(graphics.renderer, meteorTexture, NULL, &meteorDestRect);
             }
         }

        /* Vẽ Player */
        boboiboy.render(graphics);

        /* Vẽ nút Setting */
        if (settingButtonTexture) SDL_RenderCopy(graphics.renderer, settingButtonTexture, NULL, &SETTING_BUTTON_RECT);

        /* Vẽ màn hình Settings nếu Pause */
        if (currentStageState == StageState::PAUSED_SETTINGS) {
            SDL_Texture* settingTexture = soundEnabled ? setting1Texture : setting2Texture;
            int settingW, settingH; SDL_QueryTexture(settingTexture, NULL, NULL, &settingW, &settingH);
            SDL_Rect settingDest = {SCREEN_WIDTH / 2 - settingW / 2, SCREEN_HEIGHT/ 2 - settingH/ 2, settingW, settingH};
            SDL_RenderCopy(graphics.renderer, settingTexture, NULL, &settingDest);
        }

        /* Vẽ Đồng hồ */
        SDL_Color textColor = { 0, 0, 0, 255 }; /* Màu đen */
        int totalSecondsRemaining = static_cast<int>(ceil(timeRemaining));
        int minutes = totalSecondsRemaining / 60; int seconds = totalSecondsRemaining % 60;
        stringstream timeStream; timeStream << minutes << ":" << setfill('0') << setw(2) << seconds;
        string timeText = timeStream.str();
        SDL_Surface* textSurface = TTF_RenderText_Blended(timerFont, timeText.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(graphics.renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {0,0,0,0}; SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
                textRect.x = SCREEN_WIDTH / 2 - textRect.w / 2; textRect.y = 10;
                SDL_RenderCopy(graphics.renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            } else { SDL_Log("SDL Error: Create Texture failed - %s", SDL_GetError());}
            SDL_FreeSurface(textSurface);
        } else { SDL_Log("TTF Error: RenderText failed - %s", TTF_GetError());}

        /* Vẽ HUD Trái Tim */
        if (texHeart) {
            int heartX = 10; int heartY = 10;
            for (int i = 0; i < playerLives; ++i) {
                SDL_Rect heartDestRect = { heartX, heartY, HEART_ICON_WIDTH, HEART_ICON_HEIGHT };
                SDL_RenderCopy(graphics.renderer, texHeart, NULL, &heartDestRect);
                heartX += HEART_ICON_WIDTH + 5;
            }
        }

        graphics.presentScene();

        /* --- Delay --- */
        SDL_Delay(16);

    } /* --- Kết thúc vòng lặp chính --- */

    /* --- 5. Dọn dẹp tài nguyên Stage 3 --- */
    cleanupStage3Textures();
    SDL_Log("Exiting Stage 3 function.");
    /* Dừng âm thanh lần cuối */
    Mix_HaltChannel(clockChannel); Mix_HaltMusic();
    return StageOutcome::QUIT_APPLICATION; // Trả về Quit nếu thoát vòng lặp không phải do Complete
}

#endif /* STAGE3_H */
