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

//scale
const float PLAYER_SCALE_FACTOR = 0.4f;

struct Player {

public:

    //Constructor
    Player(Graphics& graphics, int startX, int startY)
        : x(startX),
          y(static_cast<float>(startY)),
          dx(0),
          velocityY(0.0f),
          onGround(true),
          facingRight(true),
          runTexture(nullptr),
          standTexture(nullptr),
          jumpRequested(false)
    {
        renderWidth = static_cast<int>(BOBOIBOY_FRAME_WIDTH * PLAYER_SCALE_FACTOR);
        renderHeight = static_cast<int>(BOBOIBOY_FRAME_HEIGHT * PLAYER_SCALE_FACTOR);
        runTexture = graphics.loadTexture(MAN_SPRITE_FILE);
        standTexture = graphics.loadTexture(PLAYER_STAND_FILE);

        if (runTexture) {
            runSprite.init(runTexture, MAN_FRAMES, MAN_CLIPS);
        }

    }

    //Destructor
    ~Player() {
        SDL_Log("Huy Player - Giai phong textures...");
        SDL_DestroyTexture(runTexture);
        SDL_DestroyTexture(standTexture);
        runTexture = nullptr;
        standTexture = nullptr;
    }

    // nhảy
    void handleEvent(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_UP) {
                jumpRequested = true;
            }
        }
    }

    //di chuyển
    void handleInput(const Uint8* keyStates) {
        int desired_dx = 0;
        if (keyStates[SDL_SCANCODE_LEFT])  desired_dx -= PLAYER_MOVE_SPEED;
        if (keyStates[SDL_SCANCODE_RIGHT]) desired_dx += PLAYER_MOVE_SPEED;
        if (desired_dx > 0) facingRight = true;
        else if (desired_dx < 0) facingRight = false;
        dx = desired_dx;
    }

    void update(const vector<SDL_Rect>& platforms, const vector<SDL_Rect>& obstacles) {

        //tính toán x
        int currentX = x;
        int nextX = currentX + dx;
        SDL_Rect nextXRect = { nextX, static_cast<int>(y), renderWidth, renderHeight };

        x = nextX;
        //giới hạn
        if (x < 0) x = 0;
        if (x + renderWidth > SCREEN_WIDTH) x = SCREEN_WIDTH - renderWidth;


        //tính toán y
        if (jumpRequested && onGround) {
            velocityY = PLAYER_JUMP_FORCE;
            onGround = false;
        }
        jumpRequested = false;

        if (!onGround) {
            velocityY += PLAYER_GRAVITY;
        }

        float currentY = y;
        float nextY = currentY + velocityY;
        SDL_Rect nextYRect = { x, static_cast<int>(nextY), renderWidth, renderHeight };
        bool stoppedByPlatform = false;

        // see the future
        for (const SDL_Rect& plat : platforms) {
            if (SDL_HasIntersection(&nextYRect, &plat)) {
                if (velocityY >= 0) {
                    y = static_cast<float>(plat.y - renderHeight);
                    velocityY = 0.0f;
                    stoppedByPlatform = true;
                    break;
                } else {
                    y = static_cast<float>(plat.y + plat.h);
                    velocityY = 0;
                    stoppedByPlatform = true;
                    break;
                }
            }
        }

        if (!stoppedByPlatform) {
            y = nextY;
        }
        SDL_Rect finalGroundCheckRect = { x + 1, static_cast<int>(y) + renderHeight, renderWidth - 2, 1 };
        onGround = false;
        for (const SDL_Rect& plat : platforms) {
            if (SDL_HasIntersection(&finalGroundCheckRect, &plat)) {
                onGround = true; break;
            }
        }

        if(!onGround) {
            for (const SDL_Rect& obs : obstacles) {
                 SDL_Rect obstacleSurface = {obs.x, obs.y, obs.w, 1};
                 if (SDL_HasIntersection(&finalGroundCheckRect, &obstacleSurface)) {
                      onGround = true;
                      break;
                 }
            }
        }
        if (velocityY < 0) { onGround = false; }
        if (dx != 0) { runSprite.tick(); }

    }

    /* --- Render --- */
    void render(Graphics& graphics) {
        SDL_Rect renderQuad = { static_cast<int>(x), static_cast<int>(y), renderWidth, renderHeight };
        SDL_RendererFlip flip = (facingRight) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

        if (dx == 0) {
            if (standTexture) {
                 SDL_Rect standSrcRect = {0, 0, 1024, 1024};
                 SDL_RenderCopyEx(graphics.renderer, standTexture, &standSrcRect, &renderQuad, 0.0, NULL, flip);
            } else if (runTexture) {
                 SDL_Rect firstClipRect = { MAN_CLIPS[0][0], MAN_CLIPS[0][1], MAN_CLIPS[0][2], MAN_CLIPS[0][3] };
                 SDL_RenderCopyEx(graphics.renderer, runTexture, &firstClipRect, &renderQuad, 0.0, NULL, flip);
            }
        } else {
            if (runTexture) {
                const SDL_Rect* clip = runSprite.getCurrentClip();
                SDL_RenderCopyEx(graphics.renderer, runTexture, clip, &renderQuad, 0.0, NULL, flip);
            }
        }
    }

    /* --- Getters --- */
    SDL_Rect getRect() const {
        return { static_cast<int>(x), static_cast<int>(y), renderWidth, renderHeight };
    }
    int getX() const { return x; }
    int getY() const {
        return static_cast<int>(y);
    }

private:
    int x;
    float y;
    int dx;
    float velocityY;
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
