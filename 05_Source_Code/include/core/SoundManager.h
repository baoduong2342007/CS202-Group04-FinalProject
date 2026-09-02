/**
 * @file SoundManager.h
 * @author TV5 (Truyen)
 * @brief Singleton audio manager — loads, caches, and plays sound effects & music
 * @note Week 1 — skeleton only, full implementation in later weeks
 */

#pragma once

// 1. Standard library
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// 2. SFML
#include <SFML/Audio.hpp>

// 3. Project headers
#include "core/LevelCatalog.h"
#include "patterns/IObserver.h"
#include "patterns/Subscription.h"

// ============================================================
// PATTERN: Singleton & Observer (Subscriber)
// Reason: only one audio device / mixer should exist at a time;
//         any class can play sounds without owning the manager.
//         listens to EventBus for decoupled audio feedback.
// ============================================================

/**
 * Stable identifiers for every sound effect known to the game.
 *
 * The values and metadata are generated from SoundManifest.def.  Callers
 * should use SoundId for fixed game cues; string lookup is retained only for
 * validated compatibility with external/configuration input.
 */
enum class SoundId : std::uint8_t {
#define SOUND_MANIFEST_ENTRY(token, key, path) token,
#include "core/SoundManifest.def"
#undef SOUND_MANIFEST_ENTRY
    COUNT,
};

struct SoundManifestEntry {
    SoundId id;
    const char* key;
    const char* relativePath;
};

inline constexpr std::array<SoundManifestEntry,
                            static_cast<std::size_t>(SoundId::COUNT)>
    kSoundManifest = {{
#define SOUND_MANIFEST_ENTRY(token, key, path) \
    SoundManifestEntry{SoundId::token, key, path},
#include "core/SoundManifest.def"
#undef SOUND_MANIFEST_ENTRY
}};

/** Return the canonical manifest rows in stable enum order. */
inline constexpr const auto& soundManifest() noexcept {
    return kSoundManifest;
}

/** Find metadata for a typed ID, returning null for an invalid enum value. */
inline constexpr const SoundManifestEntry* findSoundManifestEntry(
    SoundId id) noexcept {
    for (const auto& entry : kSoundManifest) {
        if (entry.id == id) {
            return &entry;
        }
    }
    return nullptr;
}

/**
 * Resolve a canonical key or one of the two legacy typo keys.
 *
 * `pipepowerdown` was an old spelling of the canonical `powerdown` cue, and
 * `kickkill` was a legacy shell defeat spelling.  They intentionally resolve
 * to existing semantic IDs rather than creating duplicate manifest rows.
 */
inline constexpr std::optional<SoundId> soundIdFromKey(
    std::string_view key) noexcept {
    for (const auto& entry : kSoundManifest) {
        if (key == entry.key) {
            return entry.id;
        }
    }
    if (key == "pipepowerdown") {
        return SoundId::POWER_DOWN;
    }
    if (key == "kickkill") {
        return SoundId::SHELL_KILL;
    }
    return std::nullopt;
}

struct SoundIdHash {
    std::size_t operator()(SoundId sound) const noexcept {
        return static_cast<std::size_t>(sound);
    }
};

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
    void onNotify(const GameEvent& event) override;

    // ── 3. Public methods ────────────────────────────────────

    /// Load a manifest sound effect using its canonical path.
    /// @return true on success; false is observable for invalid/unavailable
    /// entries and does not throw during normal startup.
    bool loadSound(SoundId id);

    /// Load a typed sound effect from an explicit path (for controlled
    /// replacement).  Default registration always uses the manifest.
    bool loadSound(SoundId id, const std::string& filepath);

    /// Compatibility loader for canonical keys and the two legacy aliases.
    bool loadSound(const std::string& id, const std::string& filepath);

    /// Load background music (for example `.flac`) from `filepath`
    /// @return true on success
    bool loadMusic(const std::string& filepath);

    /// Register and load a named track from the central MusicId catalog.
    bool loadMusic(MusicId id, const std::string& filepath);

    /// Play a previously loaded sound effect by its typed ID.
    /// @return false when the ID is invalid or its asset is unavailable.
    bool playSound(SoundId id);

    /// Compatibility playback for canonical keys and the two legacy aliases.
    bool playSound(const std::string& id);

    /// Play the currently loaded background music (looped)
    void playMusic();

    /// Switch to a named track. Replaying the current paused track resumes it.
    void playMusic(MusicId id);

    /// Set the track to restore after a temporary Star override.
    void setLevelMusic(MusicId id);
    void playStarMusic();
    void restoreLevelMusic();
    bool isStarMusicActive() const;
    std::optional<MusicId> getLevelMusicId() const { return m_levelMusicId; }

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

    bool isSoundLoaded(SoundId id) const;
    bool isSoundLoaded(const std::string& id) const;
    bool isMusicLoaded() const { return m_musicLoaded; }
    std::optional<MusicId> getCurrentMusicId() const { return m_currentMusicId; }
    std::size_t getSoundPlayRequestCount(SoundId id) const;
    std::size_t getSoundPlayRequestCount(const std::string& id) const;
    void resetDiagnosticCounters() { m_soundPlayRequests.clear(); }

    /// The most recent observable SFX failure, or an empty string when none
    /// has occurred since construction/clearDiagnostic().
    const std::string& getLastDiagnostic() const noexcept {
        return m_lastDiagnostic;
    }
    void clearDiagnostic() noexcept { m_lastDiagnostic.clear(); }

private:
    // ── 4. Private constructor (Singleton) ───────────────────
    SoundManager();
    ~SoundManager();

    // ── 5. Private members ───────────────────────────────────

    static constexpr std::size_t SOUND_VOICE_COUNT = 4;

    /// Cached sound buffers keyed by the typed manifest ID.
    /// Sound objects below only hold non-owning references to these buffers.
    std::unordered_map<SoundId, std::unique_ptr<sf::SoundBuffer>, SoundIdHash>
        m_soundBuffers;

    /// Four independent voices prevent rapid coin/stomp sounds from cutting
    /// one another off. A fifth simultaneous request is safely dropped.
    std::unordered_map<SoundId, std::vector<std::unique_ptr<sf::Sound>>,
                       SoundIdHash>
        m_soundVoices;
    std::unordered_map<SoundId, std::size_t, SoundIdHash> m_voiceCursors;
    std::unordered_map<SoundId, std::size_t, SoundIdHash> m_soundPlayRequests;

    std::string m_lastDiagnostic;

    /// Background music stream (only one track at a time)
    sf::Music m_music;
    std::unordered_map<MusicId, std::string, MusicIdHash> m_musicPaths;
    std::optional<MusicId> m_currentMusicId;
    std::optional<MusicId> m_levelMusicId;
    bool m_musicLoaded = false;

    float m_soundVolume;
    float m_musicVolume;

    void registerDefaultMusicPaths();
    bool openMusic(MusicId id);
    bool failSound(const std::string& diagnostic);

    // Keeping these tokens as members makes the manager's observer lifetime
    // explicit and removes destructor-time raw-pointer unsubscriptions.
    std::vector<Subscription> m_eventSubscriptions;
};
