#pragma once
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

    // Основные методы управления звуками
    void play_tick() const;
    void play_select() const;
    void play_back() const;
    
    // Асинхронные версии методов
    void play_tick_async() const;
    void play_select_async() const;
    void play_back_async() const;

    // Запрет копирования
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

private:
    void load_sounds();
    void cleanup();

    // Звуковые ресурсы
    std::vector<Mix_Chunk*> click_sounds_;
    Mix_Chunk* select_sound_ = nullptr;
    Mix_Chunk* back_sound_ = nullptr;
    
    // Состояние системы
    bool initialized_ = false;
    mutable std::mt19937 rng_;
    mutable std::atomic<Uint32> last_tick_time_{0};
    mutable std::list<std::future<void>> active_sounds_;
    mutable std::mutex sound_mutex_;
};