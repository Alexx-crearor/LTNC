#ifndef PLAYER2_H
#define PLAYER2_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "graphics.h" // Cần cho Graphics và Sprite
#include "defs.h"     // Cần các hằng số chung và hằng số bơi MỚI

using namespace std;

// Tỉ lệ scale (Lấy từ defs.h hoặc giữ nguyên ở đây)
// const float PLAYER_SCALE_FACTOR = 0.4f;

// Struct Player2 dành riêng cho trạng thái Bơi
struct Player2 {

// --- PUBLIC MEMBERS ---
public:

    /* --- Constructor --- */
    Player2(Graphics& graphics, int startX, int startY)
        : x(startX), y(static_cast<float>(startY)), dx(0), velocityY(0.0f),
          facingRight(true), // Hướng nhìn ban đầu
          swimTexture(nullptr),
          renderWidth(0), renderHeight(0)
          // Không cần onGround, jumpRequested
    {
        SDL_Log("Khoi tao Player2 (Swimming)...");

        // Tính kích thước render DỰA TRÊN KÍCH THƯỚC FRAME BƠI
        renderWidth = static_cast<int>(SWIM_FRAME_WIDTH * PLAYER_SCALE_FACTOR);
        renderHeight = static_cast<int>(SWIM_FRAME_HEIGHT * PLAYER_SCALE_FACTOR);
        SDL_Log(" - Kich thuoc render Swim: %d x %d", renderWidth, renderHeight);

        // Load texture BƠI (Dùng MAN_SWIM_FILE từ defs.h)
        swimTexture = graphics.loadTexture(MAN_SWIM_FILE);
        if (!swimTexture) {
            SDL_Log("LOI: Khong tai duoc texture BƠI: %s", MAN_SWIM_FILE);
        } else {
            SDL_Log("OK: Tai xong texture bơi.");
            // Khởi tạo Sprite cho animation bơi
            // (Dùng SWIM_FRAMES, MAN_SWIM từ defs.h)
            swimSprite.init(swimTexture, SWIM_FRAMES, MAN_SWIM);
            SDL_Log("Da khoi tao Sprite animation bơi.");
        }
        SDL_Log("Khoi tao Player2 xong.");
    }

    /* --- Destructor --- */
    ~Player2() {
        SDL_Log("Huy Player2 - Giai phong texture bơi...");
        SDL_DestroyTexture(swimTexture);
        swimTexture = nullptr;
    }

    /* --- Xử lý sự kiện (Có thể không cần cho bơi) --- */
    void handleEvent(const SDL_Event& event) {
        // Tạm thời không làm gì
    }

    /* --- Xử lý input nhấn giữ (Di chuyển ngang + dọc BƠI) --- */
    void handleInput(const Uint8* keyStates) {
        // Di chuyển ngang
        int desired_dx = 0;
        if (keyStates[SDL_SCANCODE_LEFT])  desired_dx -= PLAYER_MOVE_SPEED; // Dùng chung tốc độ ngang
        if (keyStates[SDL_SCANCODE_RIGHT]) desired_dx += PLAYER_MOVE_SPEED;
        if (desired_dx > 0) facingRight = true;
        else if (desired_dx < 0) facingRight = false;
        dx = desired_dx;

        // Di chuyển dọc (Bơi) - Đặt trực tiếp velocityY
        int desired_vy = 0;
        if (keyStates[SDL_SCANCODE_UP]) desired_vy -= SWIM_SPEED; // Dùng SWIM_SPEED
        if (keyStates[SDL_SCANCODE_DOWN]) desired_vy += SWIM_SPEED;
        velocityY = desired_vy; // velocityY giờ là tốc độ bơi tức thời
    }

    /* --- Cập nhật trạng thái (Logic Bơi) --- */
    void update(const vector<SDL_Rect>& platforms, const vector<SDL_Rect>& obstacles) {
        /* Logic bơi: Không trọng lực, không onGround */

        /* 1. Tính toán vị trí X, Y tiếp theo dự kiến */
        int currentX = x;
        float currentY = y;
        int nextX = currentX + dx;
        float nextY = currentY + velocityY; // Cập nhật Y theo tốc độ bơi từ input

        /* 2. Kiểm tra va chạm và điều chỉnh vị trí TIẾP THEO */
        SDL_Rect playerNextRect = { nextX, static_cast<int>(nextY), renderWidth, renderHeight };
        bool collisionOccurred = false;

        /* Va chạm với Platforms (Chặn 4 chiều) */
        for (const SDL_Rect& plat : platforms) {
            if (SDL_HasIntersection(&playerNextRect, &plat)) {
                 SDL_Log("SWIM collision with platform Y=%d", plat.y);
                 collisionOccurred = true;
                 /* Xử lý chặn đơn giản: dừng lại */
                 nextX = currentX;
                 nextY = currentY;
                 velocityY = 0;
                 dx = 0;
                 break;
            }
        }

        /* Va chạm với Obstacles (Chỉ Log) */
        if (!collisionOccurred) {
             SDL_Rect checkRect = { nextX, static_cast<int>(nextY), renderWidth, renderHeight };
             for (const SDL_Rect& obs : obstacles) {
                if (SDL_HasIntersection(&checkRect, &obs)) {
                     SDL_Log(">>> Va cham SWIMMING voi obstacle tai {x=%d, y=%d}", obs.x, obs.y);
                     /* Không chặn */
                     break;
                }
             }
        }

        /* 3. Cập nhật vị trí X, Y cuối cùng */
        x = nextX;
        y = nextY;

        /* 4. Giới hạn trong màn hình */
        if (x < 0) x = 0;
        if (x + renderWidth > SCREEN_WIDTH) x = SCREEN_WIDTH - renderWidth;
        if (y < 0) { y = 0; if (velocityY < 0) velocityY = 0; } // Chặn ở trên
        if (y + renderHeight > SCREEN_HEIGHT) { y = SCREEN_HEIGHT - renderHeight; if (velocityY > 0) velocityY = 0; } // Chặn ở dưới

        /* 5. Cập nhật animation bơi */
         if (dx != 0 || velocityY != 0) { // Tick khi có di chuyển bất kỳ
             swimSprite.tick();
         }

         /* Log trạng thái cuối frame */
         SDL_Log("End Update Swim: y=%.2f, vy=%.2f, dx=%d", y, velocityY, dx);
    }

    /* --- Render --- */
    void render(Graphics& graphics) {
        SDL_Rect renderQuad = { static_cast<int>(x), static_cast<int>(y), renderWidth, renderHeight };
        SDL_RendererFlip flip = (facingRight) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

        /* Chỉ vẽ animation bơi */
        if (swimTexture) {
            const SDL_Rect* clip = swimSprite.getCurrentClip();
            SDL_RenderCopyEx(graphics.renderer, swimTexture, clip, &renderQuad, 0.0, NULL, flip);
        }
        /* Có thể thêm logic vẽ idle bơi nếu dx=0 và vy=0 */
    }

    /* --- Getters --- */
    SDL_Rect getRect() const { return { static_cast<int>(x), static_cast<int>(y), renderWidth, renderHeight }; }
    int getX() const { return x; }
    int getY() const { return static_cast<int>(y); }

/* --- PRIVATE MEMBERS --- */
private:
    int x;
    float y;
    int dx;
    float velocityY;    // Dùng lưu tốc độ bơi lên/xuống
    bool facingRight;

    SDL_Texture* swimTexture;
    // SDL_Texture* swimIdleTexture; // Có thể thêm sau
    Sprite swimSprite; // Đối tượng quản lý animation bơi

    int renderWidth;
    int renderHeight;
};

#endif /* PLAYER2_H */
