/**
 * @file SoundManager.h
 * @author TV5 (Truyền)
 * @brief Singleton audio manager — loads, caches, and plays sound effects & music
 * @note Week 1 — skeleton only, full implementation in later weeks
 */

#pragma once

// 1. Standard library
#include <string>
#include <unordered_map>

// 2. SFML
#include <SFML/Audio.hpp>

// ============================================================
// PATTERN: Singleton
// Reason: only one audio device / mixer should exist at a time;
//         any class can play sounds without owning the manager.
// ============================================================

class SoundManager {
public:
    // ── 1. Singleton access ──────────────────────────────────
    static SoundManager& getInstance();

    // Delete copy / move to enforce single instance
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    SoundManager(SoundManager&&) = delete;
    SoundManager& operator=(SoundManager&&) = delete;

    // ── 2. Public methods ────────────────────────────────────

    /// Load a sound effect (.wav) and cache it under `id`
    /// @return true on success
    bool loadSound(const std::string& id, const std::string& filepath);

    /// Load background music (.ogg) from `filepath`
    /// @return true on success
    bool loadMusic(const std::string& filepath);

    /// Play a previously loaded sound effect by `id`
    void playSound(const std::string& id);

    /// Play the currently loaded background music (looped)
    void playMusic();

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

private:
    // ── 4. Private constructor (Singleton) ───────────────────
    SoundManager();
    ~SoundManager();

    // ── 5. Private members ───────────────────────────────────

    /// Cached sound buffers keyed by string id (e.g. "jump", "coin")
    std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;

    /// Active sf::Sound instances (one per buffer, reused)
    std::unordered_map<std::string, sf::Sound> m_sounds;

    /// Background music stream (only one track at a time)
    sf::Music m_music;

    float m_soundVolume;
    float m_musicVolume;
};
