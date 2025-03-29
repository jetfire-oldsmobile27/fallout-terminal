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
    
    load_sounds();
    initialized_ = true;
}

AudioManager::~AudioManager() {
    if(initialized_) cleanup();
}

void AudioManager::load_sounds() {
    click_sound_ = Mix_LoadWAV("sounds/click.wav");
    if(!click_sound_) std::cerr << "Failed to load tick sound\n";

    select_sound_ = Mix_LoadWAV("sounds/select.wav");
    if(!select_sound_) std::cerr << "Failed to load select sound\n";

    back_sound_ = Mix_LoadWAV("sounds/back.wav");
    if(!back_sound_) std::cerr << "Failed to load back sound\n";
}

void AudioManager::cleanup() {
    if(click_sound_) Mix_FreeChunk(click_sound_);
    if(select_sound_) Mix_FreeChunk(select_sound_);
    if(back_sound_) Mix_FreeChunk(back_sound_);
    
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void AudioManager::play_tick() const {
    if(click_sound_) Mix_PlayChannel(-1, click_sound_, 0);
}

void AudioManager::play_select() const {
    if(select_sound_) Mix_PlayChannel(-1, select_sound_, 0);
}

void AudioManager::play_back() const {
    if(back_sound_) Mix_PlayChannel(-1, back_sound_, 0);
}