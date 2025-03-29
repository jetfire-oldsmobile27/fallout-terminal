#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    
    void playTick() const;
    void playSelect() const;
    void playBack() const;

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

private:
    void loadSounds();
    void cleanup();

    Mix_Chunk* tickSound = nullptr;
    Mix_Chunk* selectSound = nullptr;
    Mix_Chunk* backSound = nullptr;
    bool initialized = false;
};