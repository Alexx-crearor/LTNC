#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "def.h"
#include "graphics.h"

using namespace std;



SDL_Texture *loadTexture(const char *filename, SDL_Renderer* renderer)
{
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                     "Loading %s", filename);

	SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
	if (texture == NULL) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "Load texture %s", IMG_GetError());
      }

	return texture;
}

//Hiện ảnh ở 1 vị trí cụ thể
void renderTexture(SDL_Texture *texture, int x, int y, SDL_Renderer* renderer) {
    SDL_Rect dest;
    dest.x=x;
    dest.y=y;
    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

    SDL_RenderCopy(renderer, texture, NULL, &dest);
}


int main(int agrc, char* argv[]) {
    /*
    //Khởi tại môi trường đồ họa
    SDL_Window* window=initSDL(SCREEN_WIDTH, SCREEN_HIGHT, WINDOW_TITLE);
    SDL_Renderer* renderer= createRenderer(window);

    SDL_Texture* background = loadTexture("bikiniBottom.jpg",renderer);
    //Hiện ảnh trên toàn cửa sổ
    SDL_RenderCopy(renderer,background,NULL,NULL);
    //Xóa màn hình
    SDL_RenderClear(renderer);

    //Vẽ gì đó
    //drawSomething(window, renderer);

    SDL_RenderPresent(renderer);
    waitUntilKeyPressed();
    SDL_DestroyRenderer(background);
    background=NULL;

    //Hiện bản vẽ ra màn hình
    //Khi chạy tại mô trường bthg
    SDL_RenderPresent(renderer);
    //Khi chạy trong máy ảo
    SDL_UpdateWindowSurface(window);

    //Đợi phím bất kỳ ấn trước khi đóng tất cả
    waitUntilKeyPressed();
    quitSDL(window, renderer);

    SDL_GetModState(&x, &y);
    */

    Graphics graphics;
    graphics.init();
    SDL_Rect rect;
    rect.x=100;
    rect.y=100;
    rect.h=100;
    rect.w=100;
    SDL_SetRenderDrawColor(graphics.renderer, 255, 255, 255, 0 );
    SDL_RenderFillRect(graphics.renderer, &rect);
    SDL_RenderPresent(graphics.renderer);

    SDL_Event event;
    int x, y;

    SDL_GetMouseState(&x, &y);
    cerr << ((x > 100 && y > 100 && x < 200 && y < 200) ? "In\n" : "Out\n");

    SDL_PollEvent(&event);





    return 0;

}
