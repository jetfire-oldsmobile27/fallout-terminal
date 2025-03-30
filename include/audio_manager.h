#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <random>
#include <future>
#include <chrono>
#include <atomic>
#include <list>
#include <mutex>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void play_tick() const;
    void play_select() const;
    void play_back() const;
    
    void play_tick_async() const;
    void play_select_async() const;
    void play_back_async() const;

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

private:
    void load_sounds();

    using ChunkPtr = std::unique_ptr<Mix_Chunk, decltype(&Mix_FreeChunk)>;
    
    std::vector<ChunkPtr> click_sounds_;
    ChunkPtr select_sound_{nullptr, Mix_FreeChunk};
    ChunkPtr back_sound_{nullptr, Mix_FreeChunk};
    
    bool initialized_ = false;
    mutable std::mt19937 rng_;
    mutable std::atomic<Uint32> last_tick_time_{0};
    mutable std::list<std::future<void>> active_sounds_;
    mutable std::mutex sound_mutex_;
};