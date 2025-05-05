#ifndef _GRAPHICS__H
#define _GRAPHICS__H

#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "defs.h"

struct Sprite {
    SDL_Texture* texture;
    std::vector<SDL_Rect> clips;
    int currentFrame = 0;

    void init(SDL_Texture* _texture, int frames, const int _clips [][4]) {
        texture = _texture;

        SDL_Rect clip;
        for (int i = 0; i < frames; i++) {
            clip.x = _clips[i][0];
            clip.y = _clips[i][1];
            clip.w = _clips[i][2];
            clip.h = _clips[i][3];
            clips.push_back(clip);
        }
    }
    void tick() {
        currentFrame = (currentFrame + 1) % clips.size();
    }

    const SDL_Rect* getCurrentClip() const {
        return &(clips[currentFrame]);
    }
};

struct ScrollingBackground {
    SDL_Texture* texture;
    int scrollingOffset = 0;
    int width, height;

    void setTexture(SDL_Texture* _texture) {
        texture = _texture;
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    }

    void scroll(int distance) {
        scrollingOffset -= distance;
        if( scrollingOffset < 0 ) { scrollingOffset = width; }
    }
};


struct Graphics {
    //Định nghĩa cấu trúc
    SDL_Renderer *renderer;
    SDL_Window *window;

    //Báo lỗi và thoát chương trình
    void logErrorAndExit(const char* msg, const char* error)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "%s: %s", msg, error);
        SDL_Quit();
    }

    //Khởi tạo SDL và tạo cửa sổ
    void init() {

        if (TTF_Init() == -1) {
            logErrorAndExit("SDL_ttf could not initialize! SDL_ttf Error: ",
                             TTF_GetError());
        }

        //Lỗi thì thoát
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            logErrorAndExit("SDL_Init", SDL_GetError());

        //Tạo cửa sổ
        window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == nullptr)
                   logErrorAndExit("CreateWindow", SDL_GetError());

        //Khởi tạo SDL_image
        if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
            logErrorAndExit( "SDL_image error:", IMG_GetError());

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        //renderer = SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(window));

        if (renderer == nullptr)
             logErrorAndExit("CreateRenderer", SDL_GetError());

        //Cải thiện chất lượng
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
        {
            logErrorAndExit( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        }
    }

    void prepareImg(SDL_Texture * background)
    {
        SDL_RenderCopy( renderer, background, NULL, NULL);
    }
    void prepareScene()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
    //Chuẩn bị và hiện thị cảnh
    void prepareScene(SDL_Texture * background)
    {
        SDL_RenderClear(renderer);//Xóa màn hình hiện tại
        SDL_RenderCopy( renderer, background, NULL, NULL);//Vẽ trong tưởng tượng, chưa vẽ lên màn hình
    }
    void presentScene()
    {
        SDL_RenderPresent(renderer);//Cập nhật nội dung hiện thị lên màn hình
    }

    //Tải ảnh vào texture
    SDL_Texture *loadTexture(const char *filename)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);
        //Tải ảnh và load vào texture
        SDL_Texture *texture = IMG_LoadTexture(renderer, filename);

        //Nếu tải lỗi, ghi log lỗi chứ không thoát chương trình
        if (texture == NULL)
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                  SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());

        return texture;
    }

    //Vẽ texture lên màn hình
    void renderTexture(SDL_Texture *texture, int x, int y)
    {
        SDL_Rect dest;
        //Đặt vị trí vẽ tại
        dest.x = x;
        dest.y = y;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);//Lấy kích thước texture
        //Vẽ texture lên màn hình
        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    //Dọn dẹp và thoát chương trình
    void quit()
    {
        TTF_Quit();
        IMG_Quit(); //Giải phóng SDL image

        SDL_DestroyRenderer(renderer);//Xóa renderer
        SDL_DestroyWindow(window);//Xóa cửa sổ
        SDL_Quit(); //Tắt SDL
    }
    void render(const ScrollingBackground& background) {
        renderTexture(background.texture, background.scrollingOffset, 0);
        renderTexture(background.texture, background.scrollingOffset - background.width, 0);
    }
    void render(int x, int y, const Sprite& sprite) {
        const SDL_Rect* clip = sprite.getCurrentClip();
        SDL_Rect renderQuad = {x, y, clip->w, clip->h};
        SDL_RenderCopy(renderer, sprite.texture, clip, &renderQuad);
    }
    Mix_Music *loadMusic(const char* path)
    {
        Mix_Music *gMusic = Mix_LoadMUS(path);
        if (gMusic == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                           "Could not load music! SDL_mixer Error: %s", Mix_GetError());
        }
        return gMusic;
    }
    void play(Mix_Music *gMusic)
    {
        if (gMusic == nullptr) return;

        if (Mix_PlayingMusic() == 0) {
            Mix_PlayMusic( gMusic, -1 );
        }
        else if( Mix_PausedMusic() == 1 ) {
            Mix_ResumeMusic();
        }
    }

    Mix_Chunk* loadSound(const char* path) {
        Mix_Chunk* gChunk = Mix_LoadWAV(path);
        if (gChunk == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "Could not load sound! SDL_mixer Error: %s", Mix_GetError());
        }
        return gChunk;
    }
    void play(Mix_Chunk* gChunk) {
        if (gChunk != nullptr) {
            Mix_PlayChannel( -1, gChunk, 0 );
        }
    }
};


#endif // _GRAPHICS__H
