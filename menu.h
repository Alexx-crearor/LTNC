#ifndef MENU_H
#define MENU_H

#include "graphics.h"
#include "defs.h"     // Chứa enum MenuResult, AppState, StageOutcome, StageState
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std;

/* MenuResult lấy từ defs.h */

/* --- Hàm hiển thị và xử lý Menu --- */
/* <<< Đã cập nhật chữ ký hàm >>> */
inline MenuResult menu(Graphics& graphics, bool& soundEnabled, Mix_Chunk* clickSfx, Mix_Chunk* notiSfx, Mix_Music* menuMusicBgm) {
    SDL_Log("Entering menu function...");

    /* --- Tải tài nguyên cho Menu --- */
    ScrollingBackground background;
    SDL_Texture* bgTexture = graphics.loadTexture("bg\\menu.png");
    /* ... load menu1, menu2, menu3, menu4 ... */
    SDL_Texture* menu1 = graphics.loadTexture("menu\\1.png");
    SDL_Texture* menu2 = graphics.loadTexture("menu\\2.png");
    SDL_Texture* menu3 = graphics.loadTexture("menu\\3.png");
    SDL_Texture* menu4 = graphics.loadTexture("menu\\4.png");
    SDL_Texture* setting2Texture = graphics.loadTexture("menu\\5.png"); // Ảnh có tick (Sound ON)
    SDL_Texture* setting1Texture = graphics.loadTexture("menu\\6.png"); // Ảnh không tick (Sound OFF)
    SDL_Texture* setting3Texture = graphics.loadTexture("menu\\7.png"); // Ảnh trống

    /* Kiểm tra lỗi load */
    bool allLoaded = bgTexture && menu1 && menu2 && menu3 && menu4 && setting1Texture && setting2Texture && setting3Texture;
    if (!allLoaded) { /* ... xử lý lỗi ... */ return MenuResult::QUIT_APPLICATION; }
    background.setTexture(bgTexture);

    /* --- Biến trạng thái Menu --- */
    SDL_Texture* menuSetting = setting3Texture;
    SDL_Texture* menuTexture = menu1;
    int x, y;
    MenuResult finalOutcome = MenuResult::QUIT_APPLICATION;
    int menuState = 0;
    SDL_Event e;

    /* --- Vòng lặp Menu --- */
    while (true) {
        /* --- Xử lý sự kiện --- */
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { finalOutcome = MenuResult::QUIT_APPLICATION; goto cleanup_and_return; }
            else if ((e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) && menuState == 0) {
                 if (clickSfx && soundEnabled) graphics.play(clickSfx);
                 menuState = 1; menuTexture = menu2;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                SDL_Point mousePoint = {x, y};

                if (menuState == 1) { /* Màn hình chọn chính */
                    SDL_Rect startRect = {310, 288, 486-310, 359-288};
                    SDL_Rect settingRect = {310, 403, 486-310, 479-403};
                    if (SDL_PointInRect(&mousePoint, &startRect)) { /* Click Start */
                        if (clickSfx && soundEnabled) graphics.play(clickSfx);
                        finalOutcome = MenuResult::START_GAME; goto cleanup_and_return;
                    } else if (SDL_PointInRect(&mousePoint, &settingRect)) { /* Click Setting */
                        if (clickSfx && soundEnabled) graphics.play(clickSfx);
                        if (notiSfx && soundEnabled) graphics.play(notiSfx);
                        menuState = 3; menuTexture = menu2;
                        /* Cập nhật ảnh setting khi mở dựa vào soundEnabled */
                        menuSetting = soundEnabled ? setting1Texture : setting2Texture; // <<< SỬA LẠI ĐÂY CHO ĐÚNG
                    }
                } else if (menuState == 3) { /* Màn hình setting */
                    SDL_Rect musicTickRect = {503, 351, 585 - 503, 426 - 351}; // !!! Kiểm tra tọa độ
                    SDL_Rect closeSettingRect = {527, 233, 585 - 527, 288 - 233}; // !!! Kiểm tra tọa độ
                    if (SDL_PointInRect(&mousePoint, &musicTickRect)) { /* Click Toggle */
                         if (clickSfx && soundEnabled) graphics.play(clickSfx); // Chơi sound trước khi thay đổi
                         soundEnabled = !soundEnabled; // <<< Thay đổi biến tham chiếu
                         SDL_Log("Sound Toggled via Menu: %s", soundEnabled ? "ON" : "OFF");
                         if (!soundEnabled) {
                             Mix_HaltMusic(); Mix_HaltChannel(-1); // <<< Dừng âm thanh
                         } else {
                             if(menuMusicBgm) graphics.play(menuMusicBgm); // <<< Phát lại nhạc menu
                         }
                         /* Cập nhật ảnh setting sau khi toggle */
                         menuSetting = soundEnabled ? setting1Texture : setting2Texture; // <<< SỬA LẠI ĐÂY CHO ĐÚNG

                    } else if (SDL_PointInRect(&mousePoint, &closeSettingRect)) { /* Click Close */
                         if (clickSfx && soundEnabled) graphics.play(clickSfx);
                         menuState = 1; menuTexture = menu2; menuSetting = setting3Texture;
                         SDL_Log("Settings Closed in Menu");
                    }
                }
            } // kết thúc xử lý click chuột
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE && menuState == 3) {
                 if (clickSfx && soundEnabled) graphics.play(clickSfx);
                 menuState = 1; menuTexture = menu2; menuSetting = setting3Texture;
                 SDL_Log("Settings Closed via ESC in Menu");
            }
        } // kết thúc vòng lặp PollEvent

        /* --- Xử lý Hover --- */
        if (menuState == 1) { /* ... code hover như cũ ... */
             SDL_GetMouseState(&x, &y); SDL_Rect startRect = {310, 288, 486-310, 359-288}; SDL_Rect settingRect = {310, 403, 486-310, 479-403}; SDL_Point mousePoint = {x, y};
             if (SDL_PointInRect(&mousePoint, &startRect)) { menuTexture = menu3; } else if (SDL_PointInRect(&mousePoint, &settingRect)) { menuTexture = menu4; } else { menuTexture = menu2; }
        }

        /* --- Render --- */
        graphics.prepareScene();
        background.scroll(1); graphics.render(background);
        graphics.renderTexture(menuTexture, 0, 0);
        /* Vẽ ảnh setting phù hợp */
        if (menuState == 3) {
            /* Luôn chọn ảnh dựa trên trạng thái soundEnabled mới nhất */
             menuSetting = soundEnabled ? setting1Texture : setting2Texture; // <<< SỬA LẠI ĐÂY CHO ĐÚNG
             if (menuSetting) { // Kiểm tra null trước khi vẽ
                 int settingW, settingH; SDL_QueryTexture(menuSetting, NULL, NULL, &settingW, &settingH);
                 SDL_Rect settingDest = {SCREEN_WIDTH / 2 - settingW / 2, SCREEN_HEIGHT/ 2 - settingH/ 2, settingW, settingH};
                 SDL_RenderCopy(graphics.renderer, menuSetting, NULL, &settingDest);
             }
        }
        graphics.presentScene();

        SDL_Delay(10);
    } // Kết thúc vòng lặp chính (while true)

cleanup_and_return:
    SDL_Log("Exiting menu function, performing cleanup...");
    /* Dọn dẹp textures */
    SDL_DestroyTexture(bgTexture); SDL_DestroyTexture(menu1); SDL_DestroyTexture(menu2); SDL_DestroyTexture(menu3); SDL_DestroyTexture(menu4);
    SDL_DestroyTexture(setting1Texture); SDL_DestroyTexture(setting2Texture); SDL_DestroyTexture(setting3Texture);

    return finalOutcome;
}

#endif /* MENU_H */
