/**
 * @file SoundManager.cpp
 * @author TV5 (Truyền)
 * @brief Singleton audio manager with named music tracks and SFX voice pool.
 */

#include "core/SoundManager.h"

#include <algorithm>
#include <cmath>
#include <utility>

#ifdef DEBUG
#include <iostream>
#endif

#include "patterns/EventBus.h"
#include "patterns/EventType.h"

namespace {
constexpr float DEFAULT_SOUND_VOLUME = 80.f;
constexpr float DEFAULT_MUSIC_VOLUME = 70.f;
} // namespace

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager()
    : m_soundVolume(DEFAULT_SOUND_VOLUME),
      m_musicVolume(DEFAULT_MUSIC_VOLUME) {
    EventBus& bus = EventBus::getInstance();
    bus.subscribe(EventType::PLAYER_JUMPED, this);
    bus.subscribe(EventType::COIN_COLLECTED, this);
    bus.subscribe(EventType::ENEMY_STOMPED, this);
    bus.subscribe(EventType::PLAYER_DIED, this);
    bus.subscribe(EventType::PLAYER_POWER_UP, this);
    bus.subscribe(EventType::PLAYER_POWER_DOWN, this);
    bus.subscribe(EventType::PLAYER_STAR_COLLECTED, this);
    bus.subscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this);
    bus.subscribe(EventType::GAME_PAUSED, this);
    bus.subscribe(EventType::GAME_RESUMED, this);
    bus.subscribe(EventType::LEVEL_COMPLETED, this);
    bus.subscribe(EventType::FIREBALL_SHOT, this);
    bus.subscribe(EventType::SHELL_KICKED, this);
    bus.subscribe(EventType::ENEMY_DEFEATED_BY_SHELL, this);
    bus.subscribe(EventType::ENEMY_DEFEATED_BY_FIREBALL, this);
    bus.subscribe(EventType::ENEMY_DEFEATED_BY_STAR, this);
    bus.subscribe(EventType::BLOCK_BUMPED, this);
    bus.subscribe(EventType::BRICK_BROKEN, this);
    bus.subscribe(EventType::ITEM_EMERGED, this);
    bus.subscribe(EventType::ONE_UP_COLLECTED, this);

    // Event-to-SFX catalog. All paths are relative to the executable so the
    // same mapping works from a clean CMake build directory.
    loadSound("jump", "assets/sounds/effects/jump.wav");
    loadSound("jumpsmall", "assets/sounds/effects/jumpsmall.wav");
    loadSound("coin", "assets/sounds/effects/coin.wav");
    loadSound("stomp", "assets/sounds/effects/stompswim.wav");
    loadSound("kick", "assets/sounds/effects/kickkill.wav");
    loadSound("death", "assets/sounds/effects/death.wav");
    loadSound("gameover", "assets/sounds/effects/gameover.wav");
    loadSound("powerup", "assets/sounds/effects/powerup.wav");
    loadSound("powerdown", "assets/sounds/effects/pipepowerdown.wav");
    loadSound("fireball", "assets/sounds/effects/fireball.wav");
    // Separate logical cues keep shell kick and shell kill from sharing an
    // EventBus/SFX path. The current packaged sample is reused until a
    // dedicated shell-kill recording is supplied.
    loadSound("shell_kick", "assets/sounds/effects/kickkill.wav");
    loadSound("shell_kill", "assets/sounds/effects/kickkill.wav");
    loadSound("enemy_fireball", "assets/sounds/effects/kickkill.wav");
    loadSound("enemy_star", "assets/sounds/effects/kickkill.wav");
    loadSound("flagpole", "assets/sounds/effects/flagpole.wav");
    loadSound("brick", "assets/sounds/effects/brick.wav");
    loadSound("bump", "assets/sounds/effects/bump.wav");
    loadSound("item", "assets/sounds/effects/item.wav");
    loadSound("oneup", "assets/sounds/effects/1up.wav");
    loadSound("pause", "assets/sounds/effects/pause.wav");
    loadSound("hurryup", "assets/sounds/effects/hurryup.wav");

    registerDefaultMusicPaths();
    setLevelMusic(MusicId::OVERWORLD);
    loadMusic(MusicId::OVERWORLD, "assets/sounds/music/overworld.flac");
}

SoundManager::~SoundManager() {
    EventBus& bus = EventBus::getInstance();
    bus.unsubscribe(EventType::PLAYER_JUMPED, this);
    bus.unsubscribe(EventType::COIN_COLLECTED, this);
    bus.unsubscribe(EventType::ENEMY_STOMPED, this);
    bus.unsubscribe(EventType::PLAYER_DIED, this);
    bus.unsubscribe(EventType::PLAYER_POWER_UP, this);
    bus.unsubscribe(EventType::PLAYER_POWER_DOWN, this);
    bus.unsubscribe(EventType::PLAYER_STAR_COLLECTED, this);
    bus.unsubscribe(EventType::PLAYER_INVINCIBILITY_EXPIRED, this);
    bus.unsubscribe(EventType::GAME_PAUSED, this);
    bus.unsubscribe(EventType::GAME_RESUMED, this);
    bus.unsubscribe(EventType::LEVEL_COMPLETED, this);
    bus.unsubscribe(EventType::FIREBALL_SHOT, this);
    bus.unsubscribe(EventType::SHELL_KICKED, this);
    bus.unsubscribe(EventType::ENEMY_DEFEATED_BY_SHELL, this);
    bus.unsubscribe(EventType::ENEMY_DEFEATED_BY_FIREBALL, this);
    bus.unsubscribe(EventType::ENEMY_DEFEATED_BY_STAR, this);
    bus.unsubscribe(EventType::BLOCK_BUMPED, this);
    bus.unsubscribe(EventType::BRICK_BROKEN, this);
    bus.unsubscribe(EventType::ITEM_EMERGED, this);
    bus.unsubscribe(EventType::ONE_UP_COLLECTED, this);
}

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
        case EventType::ENEMY_DEFEATED_BY_SHELL:
            playSound("shell_kill");
            break;
        case EventType::ENEMY_DEFEATED_BY_FIREBALL:
            playSound("enemy_fireball");
            break;
        case EventType::ENEMY_DEFEATED_BY_STAR:
            playSound("enemy_star");
            break;
        case EventType::PLAYER_DIED:
            playSound("death");
            playMusic(MusicId::DEATH);
            break;
        case EventType::PLAYER_POWER_UP:
            playSound("powerup");
            break;
        case EventType::PLAYER_POWER_DOWN:
            playSound("powerdown");
            break;
        case EventType::PLAYER_STAR_COLLECTED:
            playStarMusic();
            break;
        case EventType::PLAYER_INVINCIBILITY_EXPIRED:
            // Damage-grace expiry also uses this event. Only a Star override
            // may restore the level track; otherwise an unrelated grace timer
            // would interrupt death/GameOver music.
            if (isStarMusicActive()) {
                restoreLevelMusic();
            }
            break;
        case EventType::FIREBALL_SHOT:
            playSound("fireball");
            break;
        case EventType::SHELL_KICKED:
            playSound("shell_kick");
            break;
        case EventType::BLOCK_BUMPED:
            playSound("bump");
            break;
        case EventType::BRICK_BROKEN:
            playSound("brick");
            break;
        case EventType::ITEM_EMERGED:
            playSound("item");
            break;
        case EventType::ONE_UP_COLLECTED:
            playSound("oneup");
            break;
        case EventType::GAME_PAUSED:
            playSound("pause");
            pauseMusic();
            break;
        case EventType::GAME_RESUMED:
            playMusic();
            break;
        case EventType::LEVEL_COMPLETED:
            playSound("flagpole");
            break;
        default:
            break;
    }
}

bool SoundManager::loadSound(const std::string& id,
                             const std::string& filepath) {
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
    auto it = m_soundVoices.find(id);
    if (it == m_soundVoices.end() || it->second.empty()) {
        return;
    }

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
        return;
    }
    // All voices are busy. Drop this request instead of restarting one and
    // cutting off an already audible sound.
}

bool SoundManager::loadMusic(const std::string& filepath) {
    if (!m_music.openFromFile(filepath)) {
#ifdef DEBUG
        std::cerr << "[SoundManager] Failed to load music from "
                  << filepath << "\n";
#endif
        m_musicLoaded = false;
        return false;
    }

    m_currentMusicId.reset();
    m_musicLoaded = true;
    m_music.setVolume(m_musicVolume);
    m_music.setLooping(true);
    return true;
}

bool SoundManager::loadMusic(MusicId id, const std::string& filepath) {
    m_musicPaths[id] = filepath;
    if (id == MusicId::OVERWORLD || id == MusicId::UNDERGROUND || id == MusicId::CASTLE) {
        setLevelMusic(id);
    }
    return openMusic(id);
}

void SoundManager::playMusic() {
    if (m_musicLoaded) {
        // sf::Music::play() resumes a paused stream and starts a stopped
        // stream from its beginning.
        m_music.play();
    }
}

void SoundManager::playMusic(MusicId id) {
    if (id == MusicId::OVERWORLD || id == MusicId::UNDERGROUND || id == MusicId::CASTLE) {
        setLevelMusic(id);
    }

    if (!m_musicLoaded || !m_currentMusicId || *m_currentMusicId != id) {
        if (!openMusic(id)) {
            return;
        }
    }
    m_music.play();
}

void SoundManager::setLevelMusic(MusicId id) {
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

bool SoundManager::isSoundLoaded(const std::string& id) const {
    return m_soundVoices.find(id) != m_soundVoices.end();
}

void SoundManager::registerDefaultMusicPaths() {
    m_musicPaths[MusicId::OVERWORLD] = "assets/sounds/music/overworld.flac";
    m_musicPaths[MusicId::UNDERGROUND] = "assets/sounds/music/underground.flac";
    m_musicPaths[MusicId::CASTLE] = "assets/sounds/music/castle.flac";
    m_musicPaths[MusicId::STAR] = "assets/sounds/music/invincible.flac";
    m_musicPaths[MusicId::DEATH] = "assets/sounds/music/death.flac";
    m_musicPaths[MusicId::GAME_OVER] = "assets/sounds/music/gameover.flac";
    m_musicPaths[MusicId::WIN] = "assets/sounds/music/level_complete.flac";
}

bool SoundManager::openMusic(MusicId id) {
    const auto pathIt = m_musicPaths.find(id);
    if (pathIt == m_musicPaths.end() || !m_music.openFromFile(pathIt->second)) {
#ifdef DEBUG
        std::cerr << "[SoundManager] Failed to load music id "
                  << static_cast<int>(id) << "\n";
#endif
        m_musicLoaded = false;
        return false;
    }

    m_music.setVolume(m_musicVolume);
    m_music.setLooping(true);
    m_currentMusicId = id;
    m_musicLoaded = true;
    return true;
}
