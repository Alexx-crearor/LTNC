#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <vector>
#include <cmath>
#include <iostream>
#include "graphics.h"
#include "defs.h"

using namespace std;

//Tỉ lệ scale
const float PLAYER_SCALE_FACTOR = 0.4f;

struct Player {

// --- PUBLIC MEMBERS ---
public:

    /* --- Constructor --- */
    Player(Graphics& graphics, int startX, int startY)
        : x(startX),                 // x vẫn là int
          y(static_cast<float>(startY)), // <<<--- Khởi tạo y là float
          dx(0),
          velocityY(0.0f),         // velocityY là float
          onGround(true),
          facingRight(true),
          runTexture(nullptr),
          standTexture(nullptr),
          jumpRequested(false)
    {
        SDL_Log("Khoi tao Player...");
        renderWidth = static_cast<int>(BOBOIBOY_FRAME_WIDTH * PLAYER_SCALE_FACTOR);
        renderHeight = static_cast<int>(BOBOIBOY_FRAME_HEIGHT * PLAYER_SCALE_FACTOR);
        runTexture = graphics.loadTexture(MAN_SPRITE_FILE);
        standTexture = graphics.loadTexture(PLAYER_STAND_FILE);
        if (!runTexture) SDL_Log("LOI: Khong tai duoc texture CHAY: %s", MAN_SPRITE_FILE);
        if (!standTexture) SDL_Log("LOI: Khong tai duoc texture DUNG: %s", PLAYER_STAND_FILE);
        if (runTexture) {
            runSprite.init(runTexture, MAN_FRAMES, MAN_CLIPS);
        }
        SDL_Log("Khoi tao Player xong.");
    }

    /* --- Destructor --- */
    ~Player() {
        SDL_Log("Huy Player - Giai phong textures...");
        SDL_DestroyTexture(runTexture);
        SDL_DestroyTexture(standTexture);
        runTexture = nullptr;
        standTexture = nullptr;
    }

    /* --- Xử lý sự kiện (ví dụ: nhảy) --- */
    void handleEvent(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_UP) {
                jumpRequested = true;
            }
        }
    }

    /* --- Xử lý input (ví dụ: di chuyển) --- */
    void handleInput(const Uint8* keyStates) {
        int desired_dx = 0;
        if (keyStates[SDL_SCANCODE_LEFT])  desired_dx -= PLAYER_MOVE_SPEED;
        if (keyStates[SDL_SCANCODE_RIGHT]) desired_dx += PLAYER_MOVE_SPEED;
        if (desired_dx > 0) facingRight = true;
        else if (desired_dx < 0) facingRight = false;
        dx = desired_dx;
    }

    /* --- Cập nhật trạng thái mỗi frame (Sử dụng float y) --- */
    void update(const vector<SDL_Rect>& platforms, const vector<SDL_Rect>& obstacles) {

        /* 1. Cập nhật X và Va chạm ngang (LOGIC CŨ - CHỈ LOG OBSTACLE) */
        int currentX = x;
        int nextX = currentX + dx;
        SDL_Rect nextXRect = { nextX, static_cast<int>(y), renderWidth, renderHeight }; // <<< cast y to int
        for (const SDL_Rect& obs : obstacles) {
            if (SDL_HasIntersection(&nextXRect, &obs)) {
                 SDL_Log(">>> Va cham NGANG voi obstacle tai {x=%d, y=%d}", obs.x, obs.y);
                break;
            }
        }
        x = nextX; // Cập nhật X
        if (x < 0) x = 0;
        if (x + renderWidth > SCREEN_WIDTH) x = SCREEN_WIDTH - renderWidth;


        /* 2. Cập nhật Y và Va chạm dọc (Sử dụng float y) */
        /* a. Xử lý yêu cầu nhảy (Dùng onGround của frame TRƯỚC) */
        if (jumpRequested && onGround) {
            velocityY = PLAYER_JUMP_FORCE;
            onGround = false; /* Rời mặt đất ngay lập tức */
        }
        jumpRequested = false;

        /* b. Áp dụng trọng lực NẾU KHÔNG ở trên mặt đất (từ frame TRƯỚC) */
        if (!onGround) {
            velocityY += PLAYER_GRAVITY;
        }

        /* c. Tính toán vị trí Y tiếp theo dự kiến (phép cộng float) */
        float currentY = y; // y là float
        float nextY = currentY + velocityY; // <<<--- KHÔNG CÒN ÉP KIỂU (int)

        /* d. Chuẩn bị kiểm tra va chạm */
        /* --->>> Dùng static_cast<int>(nextY) khi tạo Rect <<<--- */
        SDL_Rect nextYRect = { x, static_cast<int>(nextY), renderWidth, renderHeight };
        bool stoppedByPlatform = false;

        /* e. Kiểm tra va chạm Platform tại nextY */
        for (const SDL_Rect& plat : platforms) {
            if (SDL_HasIntersection(&nextYRect, &plat)) {
                if (velocityY >= 0) { /* Đang rơi hoặc đứng yên -> Đáp xuống platform */
                    /* --->>> Ép kiểu float khi gán lại y <<<--- */
                    y = static_cast<float>(plat.y - renderHeight);
                    velocityY = 0.0f;
                    stoppedByPlatform = true;
                    break;
                } else { /* Đang bay lên -> Đập đầu vào platform */
                    /* --->>> Ép kiểu float khi gán lại y <<<--- */
                    y = static_cast<float>(plat.y + plat.h);
                    velocityY = 0;
                    stoppedByPlatform = true; // Bị chặn bởi platform nhưng không phải onGround
                    break;
                }
            }
        }

        /* f. Kiểm tra va chạm Obstacle tại nextY (Chỉ Log) */
        if (!stoppedByPlatform) {
            for (const SDL_Rect& obs : obstacles) {
                if (SDL_HasIntersection(&nextYRect, &obs)) {
                     SDL_Log(">>> Va cham DOC voi obstacle tai {x=%d, y=%d} (khong xu ly)", obs.x, obs.y);
                     /* Nếu muốn chặn khi đập đầu vào obstacle thì thêm xử lý ở đây */
                     if (velocityY < 0) {
                         // y = static_cast<float>(obs.y + obs.h); // Ví dụ chặn
                         // velocityY = 0;
                     }
                     break;
                }
            }
        }

        /* g. Cập nhật vị trí Y cuối cùng NẾU không bị chặn bởi platform */
        if (!stoppedByPlatform) {
            y = nextY;
        }

        /* h. Xác định trạng thái onGround CUỐI CÙNG cho frame này */
        /* Dùng ground check rect với vị trí y cuối cùng */
        /* --->>> Dùng static_cast<int>(y) khi tạo Rect <<<--- */
        SDL_Rect finalGroundCheckRect = { x + 1, static_cast<int>(y) + renderHeight, renderWidth - 2, 1 };
        onGround = false; // Mặc định là false
        for (const SDL_Rect& plat : platforms) {
            if (SDL_HasIntersection(&finalGroundCheckRect, &plat)) {
                onGround = true; break;
            }
        }
        if(!onGround) { /* Check obstacle top */
            for (const SDL_Rect& obs : obstacles) {
                 SDL_Rect obstacleSurface = {obs.x, obs.y, obs.w, 1};
                 if (SDL_HasIntersection(&finalGroundCheckRect, &obstacleSurface)) {
                      onGround = true; break;
                 }
            }
        }
        /* Đảm bảo không onGround nếu đang bay lên */
        if (velocityY < 0) { onGround = false; }


        /* 3. Cập nhật animation */
        if (dx != 0) { runSprite.tick(); }

        /* Log trạng thái cuối frame */
        /* --->>> Dùng %.2f cho y <<<--- */
        SDL_Log("End Update: y=%.2f, vy=%.2f, onGround=%d, dx=%d", y, velocityY, onGround, dx);
    }

    /* --- Render --- */
    void render(Graphics& graphics) {
        /* --->>> Dùng static_cast<int>(x) và static_cast<int>(y) khi tạo Rect <<<--- */
        SDL_Rect renderQuad = { static_cast<int>(x), static_cast<int>(y), renderWidth, renderHeight };
        SDL_RendererFlip flip = (facingRight) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

        if (dx == 0) { /* Đứng yên */
            if (standTexture) {
                 SDL_Rect standSrcRect = {0, 0, 1024, 1024};
                 SDL_RenderCopyEx(graphics.renderer, standTexture, &standSrcRect, &renderQuad, 0.0, NULL, flip);
            } else if (runTexture) { /* Fallback */
                 SDL_Rect firstClipRect = { MAN_CLIPS[0][0], MAN_CLIPS[0][1], MAN_CLIPS[0][2], MAN_CLIPS[0][3] };
                 SDL_RenderCopyEx(graphics.renderer, runTexture, &firstClipRect, &renderQuad, 0.0, NULL, flip);
            }
        } else { /* Chạy ngang */
            if (runTexture) {
                const SDL_Rect* clip = runSprite.getCurrentClip();
                SDL_RenderCopyEx(graphics.renderer, runTexture, clip, &renderQuad, 0.0, NULL, flip);
            }
        }
    }

    /* --- Getters --- */
    SDL_Rect getRect() const {
        /* --->>> Dùng static_cast<int>(x) và static_cast<int>(y) <<<--- */
        return { static_cast<int>(x), static_cast<int>(y), renderWidth, renderHeight };
    }
    int getX() const { return x; }
    int getY() const {
        /* --->>> Ép kiểu về int khi trả về <<<--- */
        return static_cast<int>(y);
    }

/* --- PRIVATE MEMBERS --- */
private:
    int x;              // X vẫn là int
    float y;            // <<<--- ĐÃ LÀ FLOAT
    int dx;
    float velocityY;    // Đã là float
    bool onGround;
    bool facingRight;
    bool jumpRequested;
    SDL_Texture* runTexture;
    SDL_Texture* standTexture;
    Sprite runSprite;
    int renderWidth;
    int renderHeight;
};

#endif /* PLAYER_H */
