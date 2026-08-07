/**
 * @file SoundManager.h
 * @author TV5 (Truyền)
 * @brief Singleton audio manager — loads, caches, and plays sound effects & music
 * @note Week 1 — skeleton only, full implementation in later weeks
 */

#pragma once

// 1. Standard library
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// 2. SFML
#include <SFML/Audio.hpp>

// 3. Project headers
#include "core/LevelCatalog.h"
#include "patterns/IObserver.h"

// ============================================================
// PATTERN: Singleton & Observer (Subscriber)
// Reason: only one audio device / mixer should exist at a time;
//         any class can play sounds without owning the manager.
//         listens to EventBus for decoupled audio feedback.
// ============================================================

struct MusicIdHash {
    std::size_t operator()(MusicId music) const noexcept {
        return static_cast<std::size_t>(music);
    }
};

class SoundManager : public IObserver {
public:
    // ── 1. Singleton access ──────────────────────────────────
    static SoundManager& getInstance();

    // Delete copy / move to enforce single instance
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    SoundManager(SoundManager&&) = delete;
    SoundManager& operator=(SoundManager&&) = delete;

    // ── 2. Override methods (IObserver) ──────────────────────
    /// @brief IObserver implementation: handles global events from EventBus
    /// @note TV5: Subscribe in init/constructor -> EventBus::getInstance().subscribe(EventType::PLAYER_JUMPED, this);
    void onNotify(EventType event) override;

    // ── 3. Public methods ────────────────────────────────────

    /// Load a sound effect (.wav) and cache it under `id`
    /// @return true on success
    bool loadSound(const std::string& id, const std::string& filepath);

    /// Load background music (for example `.flac`) from `filepath`
    /// @return true on success
    bool loadMusic(const std::string& filepath);

    /// Register and load a named track from the central MusicId catalog.
    bool loadMusic(MusicId id, const std::string& filepath);

    /// Play a previously loaded sound effect by `id`
    void playSound(const std::string& id);

    /// Play the currently loaded background music (looped)
    void playMusic();

    /// Switch to a named track. Replaying the current paused track resumes it.
    void playMusic(MusicId id);

    /// Stop the currently playing background music
    void stopMusic();

    /// Pause the currently playing background music
    void pauseMusic();

    // ── 3. Getters / Setters ─────────────────────────────────

    /// Set the master volume for sound effects (0–100)
    void setSoundVolume(float volume);

    /// Set the volume for background music (0–100)
    void setMusicVolume(float volume);

    /// Get the current sound effects volume
    float getSoundVolume() const;

    /// Get the current music volume
    float getMusicVolume() const;

    /// Keep every public volume API inside the valid SFML range.
    static float clampVolume(float volume);

    bool isSoundLoaded(const std::string& id) const;
    bool isMusicLoaded() const { return m_musicLoaded; }

private:
    // ── 4. Private constructor (Singleton) ───────────────────
    SoundManager();
    ~SoundManager();

    // ── 5. Private members ───────────────────────────────────

    static constexpr std::size_t SOUND_VOICE_COUNT = 4;

    /// Cached sound buffers keyed by string id (e.g. "jump", "coin").
    /// Sound objects below only hold non-owning references to these buffers.
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> m_soundBuffers;

    /// Four independent voices prevent rapid coin/stomp sounds from cutting
    /// one another off. A fifth simultaneous request is safely dropped.
    std::unordered_map<std::string, std::vector<std::unique_ptr<sf::Sound>>> m_soundVoices;
    std::unordered_map<std::string, std::size_t> m_voiceCursors;

    /// Background music stream (only one track at a time)
    sf::Music m_music;
    std::unordered_map<MusicId, std::string, MusicIdHash> m_musicPaths;
    std::optional<MusicId> m_currentMusicId;
    bool m_musicLoaded = false;

    float m_soundVolume;
    float m_musicVolume;

    void registerDefaultMusicPaths();
    bool openMusic(MusicId id);
};
