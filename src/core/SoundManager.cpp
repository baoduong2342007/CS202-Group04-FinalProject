/**
 * @file SoundManager.cpp
 * @author TV5 (Truyền)
 * @brief Singleton audio manager with named music tracks and SFX voice pool.
 */

#include "core/SoundManager.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <utility>

#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr float DEFAULT_SOUND_VOLUME = 80.f;
constexpr float DEFAULT_MUSIC_VOLUME = 70.f;

bool isLevelMusicId(MusicId id) {
    return id == MusicId::OVERWORLD || id == MusicId::UNDERGROUND ||
           id == MusicId::CASTLE || id == MusicId::UNDERWATER;
}

bool tryOpenMusic(sf::Music& candidate, const std::string& filepath) noexcept {
    try {
        return candidate.openFromFile(filepath);
    } catch (const sf::Exception&) {
        return false;
    } catch (const std::exception&) {
        return false;
    } catch (...) {
        return false;
    }
}
} // namespace

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager()
    : m_soundVolume(DEFAULT_SOUND_VOLUME),
      m_musicVolume(DEFAULT_MUSIC_VOLUME) {
    EventBus& bus = EventBus::getInstance();
    m_eventSubscriptions.reserve(21);
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_JUMPED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::COIN_COLLECTED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::ENEMY_STOMPED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_DIED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_POWER_UP, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_POWER_DOWN, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_STAR_COLLECTED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::GAME_PAUSED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::GAME_RESUMED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::LEVEL_COMPLETED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::FIREBALL_SHOT, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::SHELL_KICKED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::ENEMY_DEFEATED_BY_SHELL, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::ENEMY_DEFEATED_BY_FIREBALL, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::ENEMY_DEFEATED_BY_STAR, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::ENEMY_DEFEATED_BY_BLOCK, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::BLOCK_BUMPED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::BRICK_BROKEN, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::ITEM_EMERGED, this));
    m_eventSubscriptions.emplace_back(
        bus.subscribe(EventType::ONE_UP_COLLECTED, this));

    // Event-to-SFX registration is derived exclusively from the canonical
    // manifest.  Paths are executable-relative so the same mapping works
    // from a clean CMake build directory and from the source tree.
    for (const SoundManifestEntry& entry : soundManifest()) {
        loadSound(entry.id);
    }

    registerDefaultMusicPaths();
    setLevelMusic(MusicId::OVERWORLD);
    loadMusic(MusicId::OVERWORLD, "assets/sounds/music/overworld.flac");
}

SoundManager::~SoundManager() {
    m_eventSubscriptions.clear();
}

void SoundManager::onNotify(const GameEvent& eventData) {
    const EventType event = eventData.type;
    switch (event) {
        case EventType::PLAYER_JUMPED:
            playSound(SoundId::JUMP);
            break;
        case EventType::COIN_COLLECTED:
            playSound(SoundId::COIN);
            break;
        case EventType::ENEMY_STOMPED:
            playSound(SoundId::STOMP);
            break;
        case EventType::ENEMY_DEFEATED_BY_SHELL:
            playSound(SoundId::SHELL_KILL);
            break;
        case EventType::ENEMY_DEFEATED_BY_FIREBALL:
            playSound(SoundId::ENEMY_FIREBALL);
            break;
        case EventType::ENEMY_DEFEATED_BY_STAR:
            playSound(SoundId::ENEMY_STAR);
            break;
        case EventType::ENEMY_DEFEATED_BY_BLOCK:
            playSound(SoundId::STOMP);
            break;
        case EventType::PLAYER_DIED:
            playSound(SoundId::DEATH);
            playMusic(MusicId::DEATH);
            break;
        case EventType::PLAYER_POWER_UP:
            playSound(SoundId::POWER_UP);
            break;
        case EventType::PLAYER_POWER_DOWN:
            playSound(SoundId::POWER_DOWN);
            break;
        case EventType::PLAYER_STAR_COLLECTED:
            playStarMusic();
            break;
        case EventType::PLAYER_INVINCIBILITY_EXPIRED:
            // This event is the Star lifecycle edge; damage grace has its own
            // independent timer and does not publish a music event.
            if (isStarMusicActive()) {
                restoreLevelMusic();
            }
            break;
        case EventType::FIREBALL_SHOT:
            playSound(SoundId::FIREBALL);
            break;
        case EventType::SHELL_KICKED:
            playSound(SoundId::SHELL_KICK);
            break;
        case EventType::BLOCK_BUMPED:
            playSound(SoundId::BUMP);
            break;
        case EventType::BRICK_BROKEN:
            playSound(SoundId::BRICK);
            break;
        case EventType::ITEM_EMERGED:
            playSound(SoundId::ITEM);
            break;
        case EventType::ONE_UP_COLLECTED:
            playSound(SoundId::ONE_UP);
            break;
        case EventType::GAME_PAUSED:
            playSound(SoundId::PAUSE);
            pauseMusic();
            break;
        case EventType::GAME_RESUMED:
            playMusic();
            break;
        case EventType::LEVEL_COMPLETED:
            playSound(SoundId::FLAGPOLE);
            break;
        default:
            break;
    }
}

bool SoundManager::failSound(const std::string& diagnostic) {
    m_lastDiagnostic = diagnostic;
    std::cerr << "[SoundManager] " << diagnostic << "\n";
    return false;
}

bool SoundManager::loadSound(SoundId id) {
    const SoundManifestEntry* entry = findSoundManifestEntry(id);
    if (entry == nullptr) {
        return failSound("Cannot load invalid sound ID " +
                         std::to_string(static_cast<int>(id)));
    }
    return loadSound(id, entry->relativePath);
}

bool SoundManager::loadSound(SoundId id, const std::string& filepath) {
    const SoundManifestEntry* entry = findSoundManifestEntry(id);
    if (entry == nullptr) {
        return failSound("Cannot load invalid sound ID " +
                         std::to_string(static_cast<int>(id)));
    }

    try {
        // Construct replacements first. If the file is invalid, the existing
        // voices remain usable instead of being replaced by an empty entry.
        auto buffer = std::make_unique<sf::SoundBuffer>(filepath);
        std::vector<std::unique_ptr<sf::Sound>> voices;
        voices.reserve(SOUND_VOICE_COUNT);
        for (std::size_t index = 0; index < SOUND_VOICE_COUNT; ++index) {
            auto voice = std::make_unique<sf::Sound>(*buffer);
            voice->setVolume(m_soundVolume);
            voices.push_back(std::move(voice));
        }

        // Voices refer to the buffer, so replace voices before replacing the
        // buffer they currently reference.
        m_soundVoices[id] = std::move(voices);
        m_voiceCursors[id] = 0;
        m_soundBuffers[id] = std::move(buffer);
        return true;
    } catch (const sf::Exception& e) {
        return failSound("Failed to load sound '" + std::string(entry->key) +
                         "' from " + filepath + ": " + e.what());
    } catch (const std::exception& e) {
        return failSound("Failed to load sound '" + std::string(entry->key) +
                         "' from " + filepath + ": " + e.what());
    } catch (...) {
        return failSound("Failed to load sound '" + std::string(entry->key) +
                         "' from " + filepath + ": unknown error");
    }
}

bool SoundManager::loadSound(const std::string& id,
                             const std::string& filepath) {
    const std::optional<SoundId> soundId = soundIdFromKey(id);
    if (!soundId) {
        return failSound("Unknown sound key '" + id + "'");
    }
    return loadSound(*soundId, filepath);
}

bool SoundManager::playSound(SoundId id) {
    const SoundManifestEntry* entry = findSoundManifestEntry(id);
    if (entry == nullptr) {
        return failSound("Cannot play invalid sound ID " +
                         std::to_string(static_cast<int>(id)));
    }

    auto it = m_soundVoices.find(id);
    if (it == m_soundVoices.end() || it->second.empty()) {
        return failSound("Sound '" + std::string(entry->key) +
                         "' is unavailable (asset was not loaded)");
    }

    try {
        ++m_soundPlayRequests[id];

        auto& voices = it->second;
        std::size_t& cursor = m_voiceCursors[id];
        const std::size_t start = cursor % voices.size();

        for (std::size_t offset = 0; offset < voices.size(); ++offset) {
            const std::size_t index = (start + offset) % voices.size();
            if (voices[index]->getStatus() == sf::Sound::Status::Playing) {
                continue;
            }

            voices[index]->play();
            cursor = (index + 1) % voices.size();
            return true;
        }
        // All voices are busy. Drop this request instead of restarting one
        // and cutting off an already audible sound. The request itself was
        // accepted and is still counted for deterministic gameplay tests.
        return true;
    } catch (const sf::Exception& e) {
        return failSound("Failed to play sound '" + std::string(entry->key) +
                         "': " + e.what());
    } catch (const std::exception& e) {
        return failSound("Failed to play sound '" + std::string(entry->key) +
                         "': " + e.what());
    } catch (...) {
        return failSound("Failed to play sound '" + std::string(entry->key) +
                         "': unknown error");
    }
}

bool SoundManager::playSound(const std::string& id) {
    const std::optional<SoundId> soundId = soundIdFromKey(id);
    if (!soundId) {
        return failSound("Unknown sound key '" + id + "'");
    }
    return playSound(*soundId);
}

std::size_t SoundManager::getSoundPlayRequestCount(SoundId id) const {
    const auto request = m_soundPlayRequests.find(id);
    return request == m_soundPlayRequests.end() ? 0u : request->second;
}

std::size_t SoundManager::getSoundPlayRequestCount(
    const std::string& id) const {
    const std::optional<SoundId> soundId = soundIdFromKey(id);
    return soundId ? getSoundPlayRequestCount(*soundId) : 0u;
}

bool SoundManager::loadMusic(const std::string& filepath) {
    // Open into a temporary first. SFML can leave an sf::Music instance's
    // decoder unusable after a failed open; keeping the candidate isolated
    // lets us stop/reset the active stream without poisoning future retries.
    sf::Music candidate;
    if (!tryOpenMusic(candidate, filepath)) {
        std::cerr << "[SoundManager] Failed to load music from "
                  << filepath << "\n";
        // An unsuccessful replacement must not leave the previous stream
        // looking active to callers (or continue playing in the background).
        if (m_musicLoaded) {
            m_music.stop();
        }
        m_musicLoaded = false;
        m_currentMusicId.reset();
        return false;
    }

    if (m_musicLoaded) {
        // Stop before replacing a stream. SFML's streaming worker may still be
        // reading the current decoder while the move assignment swaps it.
        m_music.stop();
    }
    m_music = std::move(candidate);
    m_currentMusicId.reset();
    m_musicLoaded = true;
    m_music.setVolume(m_musicVolume);
    m_music.setLooping(true);
    return true;
}

bool SoundManager::loadMusic(MusicId id, const std::string& filepath) {
    m_musicPaths[id] = filepath;
    if (isLevelMusicId(id)) {
        setLevelMusic(id);
    }

    const bool loaded = openMusic(id);
    if (loaded && isLevelMusicId(id)) {
        // Commit a level transition only after the stream is usable.
        m_levelMusicId = id;
    }
    return loaded;
}

void SoundManager::playMusic() {
    if (m_musicLoaded) {
        // sf::Music::play() resumes a paused stream and starts a stopped
        // stream from its beginning.
        m_music.play();
    }
}

void SoundManager::playMusic(MusicId id) {
    if (!m_musicLoaded || !m_currentMusicId || *m_currentMusicId != id) {
        if (!openMusic(id)) {
            return;
        }
    }

    if (isLevelMusicId(id)) {
        // Do not overwrite a valid Star-resume context before openMusic()
        // succeeds. This also handles PlayState's explicit setLevelMusic()
        // call before playMusic().
        m_levelMusicId = id;
    }
    m_music.play();
}

void SoundManager::setLevelMusic(MusicId id) {
    if (isStarMusicActive()) {
        // A level transition can be requested while Star is playing. Keep the
        // current level as the resumable context until the replacement track
        // has opened successfully; a missing replacement must not erase it.
        return;
    }

    m_levelMusicId = id;
}

void SoundManager::playStarMusic() {
    playMusic(MusicId::STAR);
}

void SoundManager::restoreLevelMusic() {
    if (m_levelMusicId) {
        playMusic(*m_levelMusicId);
    }
}

bool SoundManager::isStarMusicActive() const {
    return m_currentMusicId && *m_currentMusicId == MusicId::STAR;
}

void SoundManager::stopMusic() {
    m_music.stop();
}

void SoundManager::pauseMusic() {
    if (m_music.getStatus() == sf::Music::Status::Playing) {
        m_music.pause();
    }
}

void SoundManager::setSoundVolume(float volume) {
    m_soundVolume = clampVolume(volume);
    for (auto& [id, voices] : m_soundVoices) {
        (void)id;
        for (auto& voice : voices) {
            voice->setVolume(m_soundVolume);
        }
    }
}

void SoundManager::setMusicVolume(float volume) {
    m_musicVolume = clampVolume(volume);
    m_music.setVolume(m_musicVolume);
}

float SoundManager::getSoundVolume() const {
    return m_soundVolume;
}

float SoundManager::getMusicVolume() const {
    return m_musicVolume;
}

float SoundManager::clampVolume(float volume) {
    if (!std::isfinite(volume)) {
        return 0.f;
    }
    return std::clamp(volume, 0.f, 100.f);
}

bool SoundManager::isSoundLoaded(SoundId id) const {
    return m_soundVoices.find(id) != m_soundVoices.end();
}

bool SoundManager::isSoundLoaded(const std::string& id) const {
    const std::optional<SoundId> soundId = soundIdFromKey(id);
    return soundId && isSoundLoaded(*soundId);
}

void SoundManager::registerDefaultMusicPaths() {
    m_musicPaths[MusicId::OVERWORLD] = "assets/sounds/music/overworld.flac";
    m_musicPaths[MusicId::UNDERGROUND] = "assets/sounds/music/underground.flac";
    m_musicPaths[MusicId::CASTLE] = "assets/sounds/music/castle.flac";
    m_musicPaths[MusicId::UNDERWATER] = "assets/sounds/music/underwater.flac";
    m_musicPaths[MusicId::STAR] = "assets/sounds/music/invincible.flac";
    m_musicPaths[MusicId::DEATH] = "assets/sounds/music/death.flac";
    m_musicPaths[MusicId::GAME_OVER] = "assets/sounds/music/gameover.flac";
    m_musicPaths[MusicId::WIN] = "assets/sounds/music/level_complete.flac";
}

bool SoundManager::openMusic(MusicId id) {
    if (m_musicLoaded) {
        // Stop before replacing a stream. SFML's streaming worker may still be
        // reading the current decoder while openFromFile() swaps resources.
        m_music.stop();
    }

    const auto pathIt = m_musicPaths.find(id);
    if (pathIt == m_musicPaths.end()) {
        std::cerr << "[SoundManager] Failed to load music id "
                  << static_cast<int>(id)
                  << ": no path is registered\n";
        m_musicLoaded = false;
        m_currentMusicId.reset();
        return false;
    }

    // Open into a temporary first. SFML can leave an sf::Music instance's
    // decoder unusable after a failed open; keeping the candidate isolated
    // lets us stop/reset the active stream without poisoning future retries.
    sf::Music candidate;
    if (!tryOpenMusic(candidate, pathIt->second)) {
        std::cerr << "[SoundManager] Failed to load music id "
                  << static_cast<int>(id) << " from " << pathIt->second
                  << "\n";
        m_musicLoaded = false;
        m_currentMusicId.reset();
        return false;
    }

    m_music = std::move(candidate);
    m_music.setVolume(m_musicVolume);
    m_music.setLooping(true);
    m_currentMusicId = id;
    m_musicLoaded = true;
    return true;
}
