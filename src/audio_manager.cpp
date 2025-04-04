#include "audio_manager.h"
#include <iostream>
#include <algorithm>

AudioManager::AudioManager() : rng_(std::random_device{}()) {
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL audio init error: " << SDL_GetError() << "\n";
        return;
    }

    Mix_AllocateChannels(16); 
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL mixer error: " << Mix_GetError() << "\n";
        return;
    }
    std::cout << "Audio initialized successfully\n";
    load_sounds();
    initialized_ = true;
}

AudioManager::~AudioManager() {
    if(initialized_) {
        cleanup();
        Mix_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}

void AudioManager::load_sounds() {
    const std::vector<std::string> click_files = {
        "sounds/click1.wav", "sounds/click2.wav",
        "sounds/click3.wav", "sounds/click4.wav",
        "sounds/click5.wav", "sounds/click6.wav",
        "sounds/click7.wav", "sounds/click8.wav"
    };

    for (const auto& file : click_files) {
        Mix_Chunk* sound = Mix_LoadWAV(file.c_str());
        if (sound) {
            click_sounds_.push_back(sound);
        } else {
            std::cerr << "Failed to load click sound: " << file << "\n";
        }
    }

    select_sound_ = Mix_LoadWAV("sounds/select.wav");
    back_sound_ = Mix_LoadWAV("sounds/back.wav");
}

void AudioManager::cleanup() {
    for (auto sound : click_sounds_) {
        Mix_FreeChunk(sound);
    }
    click_sounds_.clear();

    if (select_sound_) Mix_FreeChunk(select_sound_);
    if (back_sound_) Mix_FreeChunk(back_sound_);
}

void AudioManager::play_tick() const {
    if (!click_sounds_.empty()) {
        const Uint32 now = SDL_GetTicks();
        if (now - last_tick_time_ > 20) { 
            std::uniform_int_distribution<size_t> dist(0, click_sounds_.size()-1);
            Mix_PlayChannelTimed(-1, click_sounds_[dist(rng_)], 0, 50); 
            last_tick_time_ = now;
        }
    }
}

void AudioManager::play_select() const {
    if (select_sound_) Mix_PlayChannel(-1, select_sound_, 0);
}

void AudioManager::play_back() const {
    if (back_sound_) Mix_PlayChannel(-1, back_sound_, 0);
}

void AudioManager::play_tick_async() const {
    std::lock_guard<std::mutex> lock(sound_mutex_);
    active_sounds_.remove_if([](auto& f) {
        return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    });
    active_sounds_.push_back(std::async(std::launch::async, [this]{ play_tick(); }));
}

void AudioManager::play_select_async() const {
    std::lock_guard<std::mutex> lock(sound_mutex_);
    active_sounds_.remove_if([](auto& f) {
        return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    });
    active_sounds_.push_back(std::async(std::launch::async, [this]{ play_select(); }));
}

void AudioManager::play_back_async() const {
    std::lock_guard<std::mutex> lock(sound_mutex_);
    active_sounds_.remove_if([](auto& f) {
        return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    });
    active_sounds_.push_back(std::async(std::launch::async, [this]{ play_back(); }));
}