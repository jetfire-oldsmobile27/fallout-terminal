#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <random>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    
    void play_tick() const;
    void play_select() const;
    void play_back() const;

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

private:
    void load_sounds();
    void cleanup();

    std::vector<Mix_Chunk*> click_sounds_; 
    Mix_Chunk* select_sound_ = nullptr;
    Mix_Chunk* back_sound_ = nullptr;
    bool initialized_ = false;
    mutable std::mt19937 rng_;
};