#include "audio_manager.h"

AudioManager::AudioManager() {
    if (SDL_Init(SDL_INIT_AUDIO) != 0) { 
        std::cerr << "SDL audio init error: " << SDL_GetError() << "\n";
        return;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL mixer error: " << Mix_GetError() << "\n";
        return;
    }
    
    loadSounds();
    initialized = true;
}

AudioManager::~AudioManager() {
    if(initialized) cleanup();
}

void AudioManager::loadSounds() {
    tickSound = Mix_LoadWAV("sounds/tick.wav");
    if(!tickSound) std::cerr << "Failed to load tick sound\n";

    selectSound = Mix_LoadWAV("sounds/select.wav");
    if(!selectSound) std::cerr << "Failed to load select sound\n";

    backSound = Mix_LoadWAV("sounds/back.wav");
    if(!backSound) std::cerr << "Failed to load back sound\n";
}

void AudioManager::cleanup() {
    if(tickSound) Mix_FreeChunk(tickSound);
    if(selectSound) Mix_FreeChunk(selectSound);
    if(backSound) Mix_FreeChunk(backSound);
    
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioManager::playTick() const {
    if(tickSound) Mix_PlayChannel(-1, tickSound, 0);
}

void AudioManager::playSelect() const {
    if(selectSound) Mix_PlayChannel(-1, selectSound, 0);
}

void AudioManager::playBack() const {
    if(backSound) Mix_PlayChannel(-1, backSound, 0);
}