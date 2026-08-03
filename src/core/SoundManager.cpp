/**
 * @file SoundManager.cpp
 * @author TV5 (Truyền)
 * @brief Singleton audio manager — loads, caches, and plays sound effects & music
 * @note Week 2 — subscribes to EventBus for automatic playback on game events
 */

#include "core/SoundManager.h"

#ifdef DEBUG
#include <iostream>
#endif
#include "patterns/EventBus.h"
#include "patterns/EventType.h"

// ============================================================
// PATTERN: Singleton & Observer (Subscriber)
// Reason: only one audio device / mixer should exist at a time;
//         any class can play sounds without owning the manager.
//         Listens to EventBus for decoupled audio feedback.
// ============================================================

namespace {
constexpr float DEFAULT_SOUND_VOLUME = 100.f;
constexpr float DEFAULT_MUSIC_VOLUME = 50.f;
} // namespace

// ── Singleton access ─────────────────────────────────────────

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

// ── Constructor / Destructor ─────────────────────────────────

SoundManager::SoundManager()
    : m_soundVolume(DEFAULT_SOUND_VOLUME),
      m_musicVolume(DEFAULT_MUSIC_VOLUME) {
    // Subscribe to game events for automatic sound playback
    EventBus& bus = EventBus::getInstance();
    bus.subscribe(EventType::PLAYER_JUMPED, this);
    bus.subscribe(EventType::COIN_COLLECTED, this);
    bus.subscribe(EventType::ENEMY_STOMPED, this);
    bus.subscribe(EventType::PLAYER_DIED, this);
    bus.subscribe(EventType::PLAYER_POWER_UP, this);
    bus.subscribe(EventType::PLAYER_POWER_DOWN, this);
    bus.subscribe(EventType::GAME_PAUSED, this);

    // Preload sound effects
    loadSound("jump",    "assets/sounds/effects/jump.wav");
    loadSound("coin",    "assets/sounds/effects/coin.wav");
    loadSound("stomp",   "assets/sounds/effects/stompswim.wav");
    loadSound("death",   "assets/sounds/effects/death.wav");
    loadSound("powerup", "assets/sounds/effects/powerup.wav");

    // Preload background music
    loadMusic("assets/sounds/music/overworld.flac");
}

SoundManager::~SoundManager() {
    // Unsubscribe from all events to prevent dangling pointer
    EventBus& bus = EventBus::getInstance();
    bus.unsubscribe(EventType::PLAYER_JUMPED, this);
    bus.unsubscribe(EventType::COIN_COLLECTED, this);
    bus.unsubscribe(EventType::ENEMY_STOMPED, this);
    bus.unsubscribe(EventType::PLAYER_DIED, this);
    bus.unsubscribe(EventType::PLAYER_POWER_UP, this);
    bus.unsubscribe(EventType::PLAYER_POWER_DOWN, this);
    bus.unsubscribe(EventType::GAME_PAUSED, this);
}

// ── IObserver ────────────────────────────────────────────────

void SoundManager::onNotify(EventType event) {
    switch (event) {
        case EventType::PLAYER_JUMPED:
            playSound("jump");
            break;
        case EventType::COIN_COLLECTED:
            playSound("coin");
            break;
        case EventType::ENEMY_STOMPED:
            playSound("stomp");
            break;
        case EventType::PLAYER_DIED:
            playSound("death");
            break;
        case EventType::PLAYER_POWER_UP:
            playSound("powerup");
            break;
        case EventType::PLAYER_POWER_DOWN:
            playSound("powerdown");
            break;
        case EventType::GAME_PAUSED:
            pauseMusic();
            break;
        default:
            break;
    }
}

// ── Sound effects ────────────────────────────────────────────

bool SoundManager::loadSound(const std::string& id,
                             const std::string& filepath) {
    try {
        // Construct SoundBuffer from file (SFML 3 throws on failure)
        sf::SoundBuffer buffer(filepath);

        // Store buffer first, then create Sound referencing it
        auto [bufIt, bufInserted] =
            m_soundBuffers.emplace(id, std::move(buffer));
        if (!bufInserted) {
            // Key already exists — overwrite the buffer
            bufIt->second = sf::SoundBuffer(filepath);
            // Erase old Sound so we rebuild it with the new buffer
            m_sounds.erase(id);
        }

        // Create the Sound object referencing the stored buffer
        m_sounds.emplace(std::piecewise_construct,
                         std::forward_as_tuple(id),
                         std::forward_as_tuple(bufIt->second));

        // Apply current volume
        m_sounds.at(id).setVolume(m_soundVolume);
        return true;
    } catch (const sf::Exception& e) {
#ifdef DEBUG
        std::cerr << "[SoundManager] Failed to load sound '" << id
                  << "' from " << filepath << ": " << e.what() << "\n";
#endif
        return false;
    } catch (const std::exception& e) {
#ifdef DEBUG
        std::cerr << "[SoundManager] Failed to load sound '" << id
                  << "' from " << filepath << ": " << e.what() << "\n";
#endif
        return false;
    }
}

void SoundManager::playSound(const std::string& id) {
    auto it = m_sounds.find(id);
    if (it == m_sounds.end()) {
        // Sound not loaded — silently ignore (asset may not exist yet)
        return;
    }
    it->second.play();
}

// ── Background music ─────────────────────────────────────────

bool SoundManager::loadMusic(const std::string& filepath) {
    if (!m_music.openFromFile(filepath)) {
#ifdef DEBUG
        std::cerr << "[SoundManager] Failed to load music from "
                  << filepath << "\n";
#endif
        return false;
    }
    m_music.setVolume(m_musicVolume);
    m_music.setLooping(true);
    return true;
}

void SoundManager::playMusic() {
    m_music.play();
}

void SoundManager::stopMusic() {
    m_music.stop();
}

void SoundManager::pauseMusic() {
    m_music.pause();
}

// ── Volume control ───────────────────────────────────────────

void SoundManager::setSoundVolume(float volume) {
    m_soundVolume = volume;
    for (auto& [id, sound] : m_sounds) {
        sound.setVolume(m_soundVolume);
    }
}

void SoundManager::setMusicVolume(float volume) {
    m_musicVolume = volume;
    m_music.setVolume(m_musicVolume);
}

float SoundManager::getSoundVolume() const {
    return m_soundVolume;
}

float SoundManager::getMusicVolume() const {
    return m_musicVolume;
}
