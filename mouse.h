#ifndef MOUSE_H
#define MOUSE_H


struct Mouse{

    SDL_Texture *texMouse1, *texMouse2;
    SDL_Rect rect;
    SDL_Rect point;

    bool in = 0;

    void createMouse(const Graphics& graphics){
    texMouse1 = IMG_LoadTexture(graphics.renderer, "img\\mouse 1.png");
    texMouse2 = IMG_LoadTexture(graphics.renderer, "img\\mouse 2.png");
    rect.w = 40;
    rect.h = 40;
    point.w = 1;
    point.h = 1;
    }

    void update(){
        SDL_GetMouseState(&rect.x , &rect.y);
        point.x = rect.x;
        point.y = rect.y;
    }

    void draw(const Graphics& graphics){
        if(in) SDL_RenderCopy(graphics.renderer, texMouse2, NULL, &rect);
        else SDL_RenderCopy(graphics.renderer, texMouse1, NULL, &rect);
    }


};

#endif
