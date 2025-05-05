#ifndef EFFECTS_H
#define EFFECTS_H

#include <vector>
#include <string>
#include <sstream>
#include <SDL.h>
#include <SDL_mixer.h>
#include "graphics.h"
#include "defs.h"

using namespace std;


inline vector<SDL_Texture*> loadTransitionFrames(Graphics& graphics, const string& prefixPath, int frameCount) {
    vector<SDL_Texture*> frames;
    frames.reserve(frameCount);

    for (int i = 1; i <= frameCount; ++i) {
        stringstream ss;
        ss << prefixPath << i << ".png";
        SDL_Texture* frame = graphics.loadTexture(ss.str().c_str());
        frames.push_back(frame);
    }
    return frames;
}

inline void changeScene(Graphics& graphics, const vector<SDL_Texture*>& frames, Mix_Chunk* effectSound, bool soundEnabled) {
    int frameDelay = 50;
    if (soundEnabled) { graphics.play(effectSound); }
    for (SDL_Texture* frameTex : frames) {
        if (frameTex == nullptr) continue;
        graphics.prepareScene();
        SDL_RenderCopy(graphics.renderer, frameTex, NULL, NULL);
        graphics.presentScene();
        cout<<"hi"<<" ";
        SDL_Delay(frameDelay);
    }
}

inline void dechangeScene(Graphics& graphics,const vector<SDL_Texture*>& frames, Mix_Chunk* effectSound, bool soundEnabled) {
    int frameDelay = 50;
    if (soundEnabled) { graphics.play(effectSound); }

    for (auto it = frames.rbegin(); it != frames.rend(); ++it) {
        SDL_Texture* frameTex = *it;
        if (frameTex == nullptr) continue;
        graphics.prepareScene();
        SDL_RenderCopy(graphics.renderer, frameTex, NULL, NULL);
        graphics.presentScene();
        SDL_Delay(frameDelay);
    }
     graphics.prepareScene();
     graphics.presentScene();
     SDL_Delay(100);
}


inline void cleanupTransitionFrames(vector<SDL_Texture*>& frames) {
    for(SDL_Texture* tex : frames) {
        SDL_DestroyTexture(tex);
    }
    frames.clear();
}

#endif // EFFECTS_H
