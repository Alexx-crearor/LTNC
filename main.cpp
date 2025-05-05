/* --- main.cpp --- */

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib> // Thêm nếu dùng rand/srand trong main (mặc dù nên dùng trong stage)
#include <ctime>   // Thêm nếu dùng time trong main

// --- Include các file header của project ---
#include "defs.h"     // Chứa các hằng số và Enums dùng chung
#include "graphics.h" // Quản lý đồ họa
#include "Player.h"   // Struct Player (đi bộ/nhảy)
#include "Player2.h"  // Struct Player2 (bơi)
#include "menu.h"     // Hàm menu()
#include "stages1.h"   // Hàm stage1()
#include "stages2.h"   // Hàm stage2()
#include "stages3.h"   // Hàm stage3()

using namespace std;

/* AppState, StageOutcome, MenuResult lấy từ defs.h */

int main(int argc, char* argv[]) {
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
    SDL_Log("Starting Application...");

    /* --- Khởi tạo Graphics (Bao gồm SDL, IMG, TTF, Mixer) --- */
    Graphics graphics;
    graphics.init(); // Giả sử init là void và tự xử lý lỗi bên trong
    SDL_Log("Graphics initialization called.");

    /* --- Khai báo và Load Âm thanh + Texture dùng chung --- */
    SDL_Log("Loading shared assets in main...");
    /* Âm thanh */
    Mix_Music* menuMusic = graphics.loadMusic("sound\\entering_sound.mp3");
    Mix_Music* stage1Music = graphics.loadMusic("sound\\stage1_sound.mp3");
    Mix_Music* stage2Music = graphics.loadMusic("sound\\stage2_sound.mp3");
    Mix_Music* stage3Music = graphics.loadMusic("sound\\stage3_sound.mp3");
    Mix_Music* endMusic = graphics.loadMusic("sound\\end_music.mp3"); // <<< Nhạc end game
    Mix_Chunk* clockSound = graphics.loadSound("sound\\ticking_sound.wav"); // <<< Đã sửa thành Chunk/wav
    Mix_Chunk* mouseclickSound = graphics.loadSound("sound\\mouseclick_sound.wav");
    Mix_Chunk* notificationSound = graphics.loadSound("sound\\notification_sound.wav");
    Mix_Chunk* deathSound = graphics.loadSound("sound\\death_sound.wav");
    /* Mix_Chunk* jumpSound = graphics.loadSound("sound\\jump_sound.wav"); */ // <<< Bỏ comment nếu có và dùng
    /* !!! Thêm kiểm tra lỗi load cho từng file ở đây nếu cần !!! */

    /* Texture */
    SDL_Texture* texHeart = graphics.loadTexture(HEART_ICON);
    SDL_Texture* texNoti[6] = {nullptr};
    texNoti[5] = graphics.loadTexture(NOTI_5_LIFE);
    texNoti[4] = graphics.loadTexture(NOTI_4_LIFE);
    texNoti[3] = graphics.loadTexture(NOTI_3_LIFE);
    texNoti[2] = graphics.loadTexture(NOTI_2_LIFE);
    texNoti[1] = graphics.loadTexture(NOTI_1_LIFE);
    texNoti[0] = graphics.loadTexture(NOTI_0_LIFE);
    SDL_Texture* endScreenTexture = graphics.loadTexture("stages\\end.png"); // <<< Load ảnh end game
    /* !!! Thêm kiểm tra lỗi load cho từng file ở đây nếu cần !!! */
    SDL_Log("Shared assets loading attempted.");


    /* --- Khởi tạo trạng thái Game --- */
    AppState currentState = AppState::MENU;
    bool isRunning = true;
    bool soundEnabled = true;
    int playerLives = 5;
    int currentPlayingChannel = -1; // Kênh đang chơi âm thanh lặp

    /* --- Vòng lặp Game Chính --- */
    SDL_Log("Starting main game loop...");
    while (isRunning) {
        MenuResult menuOutcome;
        StageOutcome stageOutcome;

        /* --- State Machine --- */
        /* Thêm {} cho mỗi case để tránh lỗi "crosses initialization" */
        switch (currentState) {
            case AppState::MENU: {
                SDL_Log("Entering MENU state...");
                Mix_HaltChannel(-1); Mix_HaltMusic();
                if (soundEnabled) graphics.play(menuMusic);
                /* <<< Gọi menu với đủ tham số >>> */
                menuOutcome = menu(graphics, soundEnabled, mouseclickSound, notificationSound, menuMusic);
                if (menuOutcome == MenuResult::START_GAME) {
                    playerLives = 5; // Reset mạng
                    SDL_Log("Menu -> START_GAME");
                    /* Hiển thị thông báo 5 mạng */
                    if (texNoti[5]) {
                        graphics.prepareScene(); SDL_RenderCopy(graphics.renderer, texNoti[5], NULL, NULL);
                        graphics.presentScene(); SDL_Delay(2000);
                    }
                    currentState = AppState::STAGE_1;
                } else { /* QUIT_APPLICATION */
                    currentState = AppState::QUITTING;
                }
                break;
            } // Kết thúc case MENU

            case AppState::STAGE_1: {
                SDL_Log("Entering STAGE_1 state (Lives: %d)...", playerLives);
                Mix_HaltChannel(-1); Mix_HaltMusic();
                if (soundEnabled) graphics.play(stage1Music);
                /* <<< Gọi stage1 với đủ tham số >>> */
                stageOutcome = stage1(graphics, soundEnabled, playerLives, texHeart, mouseclickSound, notificationSound, stage1Music);
                switch (stageOutcome) {
                    case StageOutcome::PLAYER_DIED:
                        Mix_HaltMusic(); // Dừng nhạc ngay khi chết
                        if (soundEnabled) graphics.play(deathSound); // Phát âm thanh chết
                        playerLives--;
                        SDL_Log("Player died! Lives left: %d", playerLives);
                        /* Hiển thị thông báo mạng còn lại */
                        if (playerLives >= 0 && playerLives <= 5 && texNoti[playerLives]) {
                            graphics.prepareScene(); SDL_RenderCopy(graphics.renderer, texNoti[playerLives], NULL, NULL);
                            graphics.presentScene(); SDL_Delay(2000);
                        }
                        if (playerLives <= 0) { currentState = AppState::GAME_OVER; }
                        /* else { currentState = AppState::STAGE_1; } // Giữ nguyên state để chơi lại */
                        break;
                    case StageOutcome::COMPLETED: currentState = AppState::STAGE_2; break;
                    case StageOutcome::GO_TO_MENU: currentState = AppState::MENU; break;
                    case StageOutcome::QUIT_APPLICATION: default: currentState = AppState::QUITTING; break;
                 }
                 SDL_Log("Exiting STAGE_1 state -> AppState %d", static_cast<int>(currentState));
                 break;
            } // Kết thúc case STAGE_1

            case AppState::STAGE_2: {
                 SDL_Log("Entering STAGE_2 state (Lives: %d)...", playerLives);
                 Mix_HaltChannel(-1); Mix_HaltMusic();
                 if (soundEnabled) graphics.play(stage2Music);
                 /* <<< Gọi stage2 với đủ tham số >>> */
                 stageOutcome = stage2(graphics, soundEnabled, playerLives, texHeart, mouseclickSound, notificationSound, stage2Music);
                 switch (stageOutcome) {
                     case StageOutcome::PLAYER_DIED: Mix_HaltMusic(); if(soundEnabled) graphics.play(deathSound); playerLives--; if(playerLives>=0 && texNoti[playerLives]){/*...*/ SDL_Delay(2000);} if(playerLives<=0) currentState = AppState::GAME_OVER; break;
                     case StageOutcome::COMPLETED: currentState = AppState::STAGE_3; break;
                     case StageOutcome::GO_TO_MENU: currentState = AppState::MENU; break;
                     case StageOutcome::QUIT_APPLICATION: default: currentState = AppState::QUITTING; break;
                 }
                 SDL_Log("Exiting STAGE_2 state -> AppState %d", static_cast<int>(currentState));
                 break;
            } // Kết thúc case STAGE_2

             case AppState::STAGE_3: {
                 SDL_Log("Entering STAGE_3 state (Lives: %d)...", playerLives);
                 Mix_HaltChannel(-1); Mix_HaltMusic();
                 currentPlayingChannel = -1;
                 if (soundEnabled) {
                    graphics.play(stage3Music);
                    if (clockSound) currentPlayingChannel = Mix_PlayChannel(-1, clockSound, -1);
                 }
                 /* <<< Gọi stage3 với đủ tham số >>> */
                 stageOutcome = stage3(graphics, soundEnabled, playerLives, texHeart, mouseclickSound, notificationSound, stage3Music, clockSound); // Truyền cả clock vào nếu stage3 cần pause/resume nó
                 /* Dừng âm thanh khi thoát stage 3 */
                 if (currentPlayingChannel != -1) Mix_HaltChannel(currentPlayingChannel); else Mix_HaltChannel(-1);
                 Mix_HaltMusic();
                 switch (stageOutcome) {
                    case StageOutcome::COMPLETED: currentState = AppState::GAME_WON; break; // Thắng!
                    case StageOutcome::PLAYER_DIED: if(soundEnabled) graphics.play(deathSound); playerLives--; if(playerLives>=0 && texNoti[playerLives]){/*...*/ SDL_Delay(2000);} if(playerLives<=0) currentState = AppState::GAME_OVER; break;
                    case StageOutcome::GO_TO_MENU: currentState = AppState::MENU; break;
                    case StageOutcome::QUIT_APPLICATION: default: currentState = AppState::QUITTING; break;
                 }
                 SDL_Log("Exiting STAGE_3 state -> AppState %d", static_cast<int>(currentState));
                 break;
            } // Kết thúc case STAGE_3

            case AppState::GAME_OVER: {
                SDL_Log("Entering GAME_OVER state...");
                Mix_HaltChannel(-1); Mix_HaltMusic(); // Dừng mọi âm thanh
                /* Hiển thị ảnh 0 life nếu có */
                if (texNoti[0]) {
                     graphics.prepareScene();
                     SDL_RenderCopy(graphics.renderer, texNoti[0], NULL, NULL); // Vẽ toàn màn hình
                     graphics.presentScene();
                     SDL_Delay(3000);
                } else { SDL_Delay(3000); } /* Delay nếu không có ảnh */
                currentState = AppState::MENU; /* Quay về menu */
                SDL_Log(" -> Returning to MENU from GAME_OVER");
                break;
            } // Kết thúc case GAME_OVER

            case AppState::GAME_WON: {
                 SDL_Log("Entering GAME_WON state...");
                 Mix_HaltChannel(-1); Mix_HaltMusic();
                 if (soundEnabled) graphics.play(endMusic); // <<< Chơi nhạc kết thúc

                 bool waitingForInput = true; // Biến cục bộ trong case này
                 SDL_Event waitEvent;

                 while(waitingForInput && isRunning) { // <<< Sửa: Dùng isRunning
                      while (SDL_PollEvent(&waitEvent)) {
                           if (waitEvent.type == SDL_QUIT) {
                               isRunning = false; // <<< Sửa: Đặt isRunning thành false
                               waitingForInput = false;
                               break;
                           }
                           if (waitEvent.type == SDL_KEYDOWN || waitEvent.type == SDL_MOUSEBUTTONDOWN) {
                               waitingForInput = false;
                               break;
                           }
                      }
                      /* Render màn hình kết thúc */
                      graphics.prepareScene();
                      if (endScreenTexture) {
                           SDL_RenderCopy(graphics.renderer, endScreenTexture, NULL, NULL);
                      } else { /* Vẽ chữ YOU WON! nếu không có ảnh */ }
                      graphics.presentScene();
                      SDL_Delay(10);
                 } /* Kết thúc vòng lặp chờ input */

                 Mix_HaltMusic(); /* Dừng nhạc kết thúc */

                 /* --- SỬA LẠI LOGIC KIỂM TRA Ở ĐÂY --- */
                 if (isRunning) { /* Nếu không phải do nhấn Quit cửa sổ */
                     currentState = AppState::MENU;
                     SDL_Log(" -> Returning to MENU from GAME_WON");
                 } else { /* Nếu nhấn Quit cửa sổ */
                     currentState = AppState::QUITTING;
                     SDL_Log(" -> Quitting from GAME_WON");
                 }
                 /* --- HẾT PHẦN SỬA --- */
                 break;
            } // Kết thúc case GAME_WON

            case AppState::QUITTING: {
                SDL_Log("Entering QUITTING state...");
                isRunning = false;
                break;
            } // Kết thúc case QUITTING

            default: {
                SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Error: Unknown AppState!");
                isRunning = false;
                break;
            }
        } // Kết thúc switch

    } // --- Kết thúc vòng lặp while(isRunning) ---


    // --- Dọn dẹp và Thoát ---
    SDL_Log("Cleaning up assets and exiting application...");
    /* Giải phóng âm thanh */
    Mix_FreeMusic(menuMusic); Mix_FreeMusic(stage1Music); Mix_FreeMusic(stage2Music); Mix_FreeMusic(stage3Music); Mix_FreeMusic(endMusic);
    Mix_FreeChunk(clockSound); Mix_FreeChunk(mouseclickSound); Mix_FreeChunk(notificationSound); Mix_FreeChunk(deathSound);
    /* Mix_FreeChunk(jumpSound); */ // <<< Xóa dòng này nếu bạn không load jumpSound
    /* Giải phóng textures HUD/Thông báo/End */
    SDL_DestroyTexture(texHeart); for (int i=0; i<6; ++i) { SDL_DestroyTexture(texNoti[i]); }
    SDL_DestroyTexture(endScreenTexture);
    /* Gọi hàm quit tổng của Graphics (nên gọi Mix_CloseAudio, TTF_Quit, SDL_Quit bên trong) */
    graphics.quit();
    SDL_Log("Application finished.");
    return 0;
}
