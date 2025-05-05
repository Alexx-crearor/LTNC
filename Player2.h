#ifndef PLAYER2_H
#define PLAYER2_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>

#include <vector>
#include <cmath>
#include <iostream>

#include "graphics.h"
#include "defs.h"

using namespace std;

struct Player2 {
public:
    //constructor
    Player2(Graphics& graphics, int startX, int startY)
        : x(startX), y(static_cast<float>(startY)), dx(0), velocityY(0.0f),
          facingRight(true),
          swimTexture(nullptr),
          renderWidth(0), renderHeight(0)
    {
        renderWidth = static_cast<int>(SWIM_FRAME_WIDTH * PLAYER_SCALE_FACTOR);
        renderHeight = static_cast<int>(SWIM_FRAME_HEIGHT * PLAYER_SCALE_FACTOR);
        swimTexture = graphics.loadTexture(MAN_SWIM_FILE);
        swimSprite.init(swimTexture, SWIM_FRAMES, MAN_SWIM);
    }

    //destructor
    ~Player2() {
        SDL_Log("Huy Player2 - Giai phong texture bơi...");
        SDL_DestroyTexture(swimTexture);
        swimTexture = nullptr;
    }

    void handleInput(const Uint8* keyStates) {
        int desired_dx = 0;
        if (keyStates[SDL_SCANCODE_LEFT])  desired_dx -= PLAYER_MOVE_SPEED;
        if (keyStates[SDL_SCANCODE_RIGHT]) desired_dx += PLAYER_MOVE_SPEED;
        if (desired_dx > 0) facingRight = true;
        else if (desired_dx < 0) facingRight = false;
        dx = desired_dx;
        int desired_vy = 0;
        if (keyStates[SDL_SCANCODE_UP]) desired_vy -= SWIM_SPEED;
        if (keyStates[SDL_SCANCODE_DOWN]) desired_vy += SWIM_SPEED;
        velocityY = desired_vy;
    }

    void update(const vector<SDL_Rect>& platforms, const vector<SDL_Rect>& obstacles) {
        int currentX = x;
        float currentY = y;
        int nextX = currentX + dx;
        float nextY = currentY + velocityY;

        SDL_Rect playerNextRect = { nextX, static_cast<int>(nextY), renderWidth, renderHeight };
        bool collisionOccurred = false;

        for (const SDL_Rect& plat : platforms) {
            if (SDL_HasIntersection(&playerNextRect, &plat)) {
                 SDL_Log("SWIM collision with platform Y=%d", plat.y);
                 collisionOccurred = true;
                 nextX = currentX;
                 nextY = currentY;
                 velocityY = 0;
                 dx = 0;
                 break;
            }
        }

        x = nextX;
        y = nextY;

        //giới hạn
        if (x < 0) x = 0;
        if (x + renderWidth > SCREEN_WIDTH) x = SCREEN_WIDTH - renderWidth;
        if (y < 0) {
            y = 0;
            if (velocityY < 0) velocityY = 0;
        }
        if (y + renderHeight > SCREEN_HEIGHT) {
            y = SCREEN_HEIGHT - renderHeight;
            if (velocityY > 0) velocityY = 0;
        }
         if (dx != 0 || velocityY != 0) {
             swimSprite.tick();
         }
    }

    void render(Graphics& graphics) {
        SDL_Rect renderQuad = { static_cast<int>(x), static_cast<int>(y), renderWidth, renderHeight };
        SDL_RendererFlip flip = (facingRight) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

        if (swimTexture) {
            const SDL_Rect* clip = swimSprite.getCurrentClip();
            SDL_RenderCopyEx(graphics.renderer, swimTexture, clip, &renderQuad, 0.0, NULL, flip);
        }
    }

    SDL_Rect getRect() const { return { static_cast<int>(x), static_cast<int>(y), renderWidth, renderHeight }; }
    int getX() const { return x; }
    int getY() const { return static_cast<int>(y); }

private:
    int x;
    float y;
    int dx;
    float velocityY;
    bool facingRight;

    SDL_Texture* swimTexture;
    Sprite swimSprite;

    int renderWidth;
    int renderHeight;
};

#endif /* PLAYER2_H */
