/**
 * @file PvpPlayState.cpp
 * @author TV5 (Truyen)
 * @brief Two-player duel gameplay implementation (best-of-3 stomp duel).
 */

#include "states/PvpPlayState.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "box2d/box2d.h"

#include "core/DisplayConfig.h"
#include "core/GameManager.h"
#include "core/LevelCatalog.h"
#include "core/SoundManager.h"
#include "patterns/EventBus.h"
#include "patterns/EventType.h"
#include "patterns/JumpCommand.h"
#include "patterns/MoveLeftCommand.h"
#include "patterns/MoveRightCommand.h"
#include "patterns/ShootCommand.h"
#include "physics/PhysicsEngine.h"
#include "states/MenuState.h"
#include "states/PauseState.h"
#include "ui/UILayoutHelper.h"

namespace {

constexpr const char* FONT_PATH = "assets/fonts/mario.ttf";
constexpr const char* PVP_ARENA_PATH = "levels/pvp_arena.txt";

constexpr float ROUND_INTRO_DURATION = 1.6f;
constexpr float ROUND_INTRO_GO_WINDOW = 0.6f;
constexpr float ROUND_END_DURATION = 1.4f;
constexpr float KO_SETTLE_TIMEOUT = 1.5f;

constexpr int WINS_TO_TAKE_MATCH = 2;

constexpr float FIRE_STATE_DURATION = 5.0f;
constexpr float FLOWER_COUNTDOWN_MIN_SECONDS = 6.0f;
constexpr float FLOWER_COUNTDOWN_MAX_SECONDS = 12.0f;

constexpr float STUN_DURATION = 0.5f;
constexpr float FIRE_IMMUNITY_DURATION = 1.0f;
constexpr float FIREBALL_KNOCKBACK_UP = 350.f;
constexpr float FIREBALL_KNOCKBACK_PUSH = 150.f;

constexpr unsigned int SCORE_FONT_SIZE = 12;
constexpr unsigned int FIRE_FONT_SIZE = 10;
constexpr unsigned int BANNER_FONT_SIZE = 22;
constexpr unsigned int BANNER_SUB_FONT_SIZE = 11;
constexpr unsigned int LABEL_FONT_SIZE = 8;

const sf::Color GOLD_COLOR(255, 215, 0);
const sf::Color P1_COLOR(120, 190, 255);
const sf::Color P2_COLOR(120, 255, 170);
const sf::Color FIRE_COLOR(255, 140, 40);
const sf::Color BODY_COLOR(235, 240, 255);

const char* characterName(CharacterType type) {
    return type == CharacterType::LUIGI ? "LUIGI" : "MARIO";
}

} // namespace

PvpPlayState::PvpPlayState(CharacterType playerOne, CharacterType playerTwo)
    : m_playerTypes{playerOne, playerTwo} {}

void PvpPlayState::onEnter() {
    m_fontLoaded = m_font.openFromFile(FONT_PATH);
    if (m_fontLoaded) {
        m_font.setSmooth(false);
    }

    m_level = std::make_unique<Level>();
    m_level->setTheme(LevelTheme::OVERWORLD);
    m_level->setCameraVerticalMode(CameraVerticalMode::DEAD_ZONE);
    if (!m_level->loadPvpArena(PVP_ARENA_PATH, m_playerTypes[0],
                               m_playerTypes[1])) {
        std::cerr << "[PvpPlayState] Failed to load the PvP arena '"
                  << PVP_ARENA_PATH << "' — returning to Menu." << std::endl;
        m_level.reset();
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
        return;
    }

    if (const Mario* p1 = m_level->getMario()) {
        m_spawnPositions[0] = p1->getPosition();
    }
    if (const Mario* p2 = m_level->getMario2()) {
        m_spawnPositions[1] = p2->getPosition();
    }

    const auto pedestalTiles = m_level->getTileMap().findTiles('W');
    m_pedestalFound = !pedestalTiles.empty();
    if (m_pedestalFound) {
        m_pedestalPosition =
            TileMap::gridToWorldPosition(pedestalTiles.front());
    }

    rebindCommands();
    initHud();
    resetMatch();
    SoundManager::getInstance().playMusic(MusicId::OVERWORLD);
}

void PvpPlayState::onExit() {
    SoundManager::getInstance().stopMusic();
}

void PvpPlayState::onPause() {
    SoundManager::getInstance().pauseMusic();
}

void PvpPlayState::onResume() {
    EventBus::getInstance().notify(EventType::GAME_RESUMED);
}

void PvpPlayState::rebindCommands() {
    m_p1Input.clear();
    m_p2Input.clear();

    Mario* p1 = m_level ? m_level->getMario() : nullptr;
    Mario* p2 = m_level ? m_level->getMario2() : nullptr;

    // Player one: A/D move, W jump, Left Shift run (polled), X shoot.
    if (p1) {
        m_p1Input.bindKey(sf::Keyboard::Key::A,
                          std::make_unique<MoveLeftCommand>(p1),
                          InputTrigger::Held, InputGroup::Horizontal);
        m_p1Input.bindKey(sf::Keyboard::Key::D,
                          std::make_unique<MoveRightCommand>(p1),
                          InputTrigger::Held, InputGroup::Horizontal);
        m_p1Input.bindKey(sf::Keyboard::Key::W,
                          std::make_unique<JumpCommand>(p1),
                          InputTrigger::Pressed);
        m_p1Input.bindKey(sf::Keyboard::Key::X,
                          std::make_unique<ShootCommand>([this, p1] {
                              if (m_level) {
                                  m_level->requestFireBallShot(*p1);
                              }
                          }),
                          InputTrigger::Pressed);
    }

    // Player two: arrows move, Up jump, Right Shift run (polled), '/' shoot.
    if (p2) {
        m_p2Input.bindKey(sf::Keyboard::Key::Left,
                          std::make_unique<MoveLeftCommand>(p2),
                          InputTrigger::Held, InputGroup::Horizontal);
        m_p2Input.bindKey(sf::Keyboard::Key::Right,
                          std::make_unique<MoveRightCommand>(p2),
                          InputTrigger::Held, InputGroup::Horizontal);
        m_p2Input.bindKey(sf::Keyboard::Key::Up,
                          std::make_unique<JumpCommand>(p2),
                          InputTrigger::Pressed);
        m_p2Input.bindKey(sf::Keyboard::Key::Slash,
                          std::make_unique<ShootCommand>([this, p2] {
                              if (m_level) {
                                  m_level->requestFireBallShot(*p2);
                              }
                          }),
                          InputTrigger::Pressed);
    }
}

Mario* PvpPlayState::fighter(int playerIndex) {
    if (!m_level) {
        return nullptr;
    }
    return playerIndex == 0 ? m_level->getMario() : m_level->getMario2();
}

const Mario* PvpPlayState::fighter(int playerIndex) const {
    if (!m_level) {
        return nullptr;
    }
    return playerIndex == 0 ? m_level->getMario() : m_level->getMario2();
}

int PvpPlayState::fighterIndex(const Mario* mario) const {
    if (mario && m_level && mario == m_level->getMario()) {
        return 0;
    }
    if (mario && m_level && mario == m_level->getMario2()) {
        return 1;
    }
    return -1;
}

int PvpPlayState::getRoundWins(int playerIndex) const {
    return (playerIndex == 0 || playerIndex == 1) ? m_roundWins[playerIndex]
                                                  : 0;
}

bool PvpPlayState::isFlowerOnField() const {
    return m_level && m_level->hasPvpFireFlowerOnField();
}

float PvpPlayState::getFireTimer(int playerIndex) const {
    return (playerIndex == 0 || playerIndex == 1) ? m_fireTimer[playerIndex]
                                                  : 0.f;
}

bool PvpPlayState::isStunned(int playerIndex) const {
    return (playerIndex == 0 || playerIndex == 1) &&
           m_stunTimer[playerIndex] > 0.f;
}

void PvpPlayState::setNextFlowerCountdown(float seconds) {
    m_flowerCountdown = seconds;
}

void PvpPlayState::processEvents(const sf::Event& event) {
    (void)event;
}

void PvpPlayState::processInput(const InputState& inputState) {
    if (!m_level) {
        return;
    }

    if (m_phase == Phase::MATCH_END) {
        if (inputState.wasPressed(sf::Keyboard::Key::Enter)) {
            resetMatch();
        } else if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
            GameManager::getInstance().changeState(
                std::make_unique<MenuState>());
        }
        return;
    }

    if (inputState.wasPressed(sf::Keyboard::Key::Escape)) {
        GameManager::getInstance().pushState(std::make_unique<PauseState>());
        return;
    }

    Mario* p1 = fighter(0);
    Mario* p2 = fighter(1);

    const auto dispatchFighter = [this, &inputState](
        int index, InputHandler& handler, Mario* player,
        sf::Keyboard::Key runKey, sf::Keyboard::Key jumpKey) {
        player->setMoveIntent(0.f);
        player->setVerticalIntent(0.f);
        player->setRunIntent(false);

        if (m_phase != Phase::FIGHT || player->isDying() ||
            player->isTransforming() || m_stunTimer[index] > 0.f) {
            return;
        }

        if (inputState.isHeld(runKey)) {
            player->setRunIntent(true);
        }
        handler.handleInput(inputState);

        if (inputState.wasReleased(jumpKey)) {
            player->releaseJump();
        }
    };

    if (p1) {
        dispatchFighter(0, m_p1Input, p1, sf::Keyboard::Key::LShift,
                        sf::Keyboard::Key::W);
    }
    if (p2) {
        dispatchFighter(1, m_p2Input, p2, sf::Keyboard::Key::RShift,
                        sf::Keyboard::Key::Up);
    }
}

void PvpPlayState::update(float dt) {
    if (!m_level) {
        return;
    }

    // Flower pickup edge: entering a FIRE state starts the 5-second window.
    for (int i = 0; i < 2; ++i) {
        Mario* player = fighter(i);
        if (!player) {
            continue;
        }
        const MarioState state = player->getMarioState();
        const bool onFire = state == MarioState::FIRE_SMALL ||
                            state == MarioState::FIRE_SUPER;
        if (onFire && m_fireTimer[i] <= 0.f) {
            m_fireTimer[i] = FIRE_STATE_DURATION;
        }
    }

    // Timed effects: fire expiry, stun, post-stun fireball immunity.
    for (int i = 0; i < 2; ++i) {
        if (m_stunTimer[i] > 0.f) {
            m_stunTimer[i] = std::max(0.f, m_stunTimer[i] - dt);
            if (m_stunTimer[i] <= 0.f) {
                m_fireImmunityTimer[i] = FIRE_IMMUNITY_DURATION;
            }
        }
        if (m_fireImmunityTimer[i] > 0.f) {
            m_fireImmunityTimer[i] = std::max(0.f, m_fireImmunityTimer[i] - dt);
        }
        if (m_fireTimer[i] > 0.f) {
            m_fireTimer[i] = std::max(0.f, m_fireTimer[i] - dt);
            if (m_fireTimer[i] <= 0.f) {
                if (Mario* player = fighter(i)) {
                    const MarioState state = player->getMarioState();
                    if (state == MarioState::FIRE_SMALL ||
                        state == MarioState::FIRE_SUPER) {
                        player->setMarioState(MarioState::SMALL);
                        SoundManager::getInstance().playSound(SoundId::POWER_DOWN);
                    }
                }
            }
        }
    }

    m_level->update(dt);

    applyFireballHits();
    updateRoundFlow(dt);
    if (m_phase == Phase::FIGHT) {
        updateFireDirector(dt);
    }

    m_blinkTimer += dt;
    refreshHudTexts();
}

void PvpPlayState::updateRoundFlow(float dt) {
    switch (m_phase) {
        case Phase::ROUND_INTRO: {
            m_phaseTimer -= dt;
            if (m_phaseTimer <= 0.f) {
                m_phase = Phase::FIGHT;
            }
            break;
        }
        case Phase::FIGHT: {
            Mario* p1 = fighter(0);
            Mario* p2 = fighter(1);
            if (!p1 || !p2) {
                break;
            }
            const bool p1Down = p1->isDying();
            const bool p2Down = p2->isDying();
            if (p1Down || p2Down) {
                m_roundWinner = p1Down ? 1 : 0;
                ++m_roundWins[m_roundWinner];
                m_phase = Phase::KO_SETTLE;
                m_phaseTimer = KO_SETTLE_TIMEOUT;
            }
            break;
        }
        case Phase::KO_SETTLE: {
            m_phaseTimer -= dt;
            const Mario* loser =
                fighter(m_roundWinner == 0 ? 1 : 0);
            const bool animationDone = loser && loser->isDeathAnimationFinished();
            if (animationDone || m_phaseTimer <= 0.f) {
                m_phase = Phase::ROUND_END;
                m_phaseTimer = ROUND_END_DURATION;
            }
            break;
        }
        case Phase::ROUND_END: {
            m_phaseTimer -= dt;
            if (m_phaseTimer <= 0.f) {
                if (m_roundWinner >= 0 &&
                    m_roundWins[m_roundWinner] >= WINS_TO_TAKE_MATCH) {
                    m_matchWinner = m_roundWinner;
                    m_phase = Phase::MATCH_END;
                } else {
                    ++m_roundNumber;
                    resetRound();
                }
            }
            break;
        }
        case Phase::MATCH_END:
            break;
    }
}

void PvpPlayState::updateFireDirector(float dt) {
    if (!m_level || !m_pedestalFound) {
        return;
    }

    // The countdown only runs while the duel has no flower on the field and
    // nobody is holding fire power.
    if (m_level->hasPvpFireFlowerOnField() || anyFighterInFireState()) {
        return;
    }

    m_flowerCountdown -= dt;
    if (m_flowerCountdown > 0.f) {
        return;
    }

    m_level->spawnPvpFireFlower(m_pedestalPosition);
    SoundManager::getInstance().playSound(SoundId::ITEM);
    scheduleNextFlowerCountdown();
}

void PvpPlayState::applyFireballHits() {
    if (!m_level) {
        return;
    }

    for (const PvpHit& hit : m_level->consumePvpFireballHits()) {
        if (!hit.attacker || !hit.victim) {
            continue;
        }
        if (m_phase != Phase::FIGHT) {
            continue;
        }

        const int victimIndex = fighterIndex(hit.victim);
        if (victimIndex < 0) {
            continue;
        }

        Mario* victim = hit.victim;
        if (victim->isDying() || victim->isCollisionLocked() ||
            m_stunTimer[victimIndex] > 0.f ||
            m_fireImmunityTimer[victimIndex] > 0.f) {
            continue;
        }

        // Launch the victim upward and away from the shooter, then lock his
        // inputs briefly so the shooter can chase the stomp.
        if (b2Body* body = victim->getBody()) {
            const float away =
                (victim->getPosition().x >= hit.attacker->getPosition().x)
                    ? 1.f
                    : -1.f;
            body->SetLinearVelocity(
                b2Vec2(PhysicsEngine::pixelsToMeters(
                           away * FIREBALL_KNOCKBACK_PUSH),
                       -PhysicsEngine::pixelsToMeters(
                           FIREBALL_KNOCKBACK_UP)));
        }
        victim->clearGroundedState();
        victim->setMoveIntent(0.f);

        m_stunTimer[victimIndex] = STUN_DURATION;
        m_level->getCamera().shake(0.2f, 6.f);
        SoundManager::getInstance().playSound(SoundId::ENEMY_FIREBALL);
    }
}

bool PvpPlayState::anyFighterInFireState() const {
    for (int i = 0; i < 2; ++i) {
        const Mario* player = fighter(i);
        if (player) {
            const MarioState state = player->getMarioState();
            if (state == MarioState::FIRE_SMALL ||
                state == MarioState::FIRE_SUPER) {
                return true;
            }
        }
    }
    return false;
}

void PvpPlayState::scheduleNextFlowerCountdown() {
    std::uniform_real_distribution<float> distribution(
        FLOWER_COUNTDOWN_MIN_SECONDS, FLOWER_COUNTDOWN_MAX_SECONDS);
    m_flowerCountdown = distribution(m_rng);
}

void PvpPlayState::resetRound() {
    for (int i = 0; i < 2; ++i) {
        Mario* player = fighter(i);
        if (!player) {
            continue;
        }
        player->respawn(m_spawnPositions[i]);
        if (player->getBody()) {
            player->getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));
        }
        m_fireTimer[i] = 0.f;
        m_stunTimer[i] = 0.f;
        m_fireImmunityTimer[i] = 0.f;
    }

    if (m_level) {
        m_level->clearPvpFireFlower();
    }
    scheduleNextFlowerCountdown();

    m_roundWinner = -1;
    m_phase = Phase::ROUND_INTRO;
    m_phaseTimer = ROUND_INTRO_DURATION;

    // A round KO switched the music to the death theme; bring the duel
    // theme back for the next round.
    SoundManager::getInstance().playMusic(MusicId::OVERWORLD);
}

void PvpPlayState::resetMatch() {
    m_roundWins[0] = 0;
    m_roundWins[1] = 0;
    m_roundNumber = 1;
    m_matchWinner = -1;
    resetRound();
}

namespace {
void alignLeft(sf::Text& text, const sf::Vector2f& pos) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({bounds.position.x, bounds.position.y});
    text.setPosition(pos);
}

void alignRight(sf::Text& text, const sf::Vector2f& pos) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({bounds.position.x + bounds.size.x, bounds.position.y});
    text.setPosition(pos);
}

void alignCenter(sf::Text& text, const sf::Vector2f& pos) {
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y});
    text.setPosition(pos);
}
} // namespace

void PvpPlayState::initHud() {
    if (!m_fontLoaded) {
        return;
    }

    // P1 (Left Column)
    m_p1Header.emplace(m_font, "", 11);
    m_p1Header->setFillColor(P1_COLOR);
    m_p1Header->setOutlineColor(sf::Color::Black);
    m_p1Header->setOutlineThickness(1.2f);

    m_p1WinsText.emplace(m_font, "", 9);
    m_p1WinsText->setFillColor(BODY_COLOR);
    m_p1WinsText->setOutlineColor(sf::Color::Black);
    m_p1WinsText->setOutlineThickness(1.0f);

    m_p1FireText.emplace(m_font, "", 9);
    m_p1FireText->setFillColor(FIRE_COLOR);
    m_p1FireText->setOutlineColor(sf::Color::Black);
    m_p1FireText->setOutlineThickness(1.0f);

    // P2 (Right Column)
    m_p2Header.emplace(m_font, "", 11);
    m_p2Header->setFillColor(P2_COLOR);
    m_p2Header->setOutlineColor(sf::Color::Black);
    m_p2Header->setOutlineThickness(1.2f);

    m_p2WinsText.emplace(m_font, "", 9);
    m_p2WinsText->setFillColor(BODY_COLOR);
    m_p2WinsText->setOutlineColor(sf::Color::Black);
    m_p2WinsText->setOutlineThickness(1.0f);

    m_p2FireText.emplace(m_font, "", 9);
    m_p2FireText->setFillColor(FIRE_COLOR);
    m_p2FireText->setOutlineColor(sf::Color::Black);
    m_p2FireText->setOutlineThickness(1.0f);

    // Center Column (Match score & round)
    m_matchScoreText.emplace(m_font, "", 13);
    m_matchScoreText->setFillColor(GOLD_COLOR);
    m_matchScoreText->setOutlineColor(sf::Color::Black);
    m_matchScoreText->setOutlineThickness(1.5f);

    m_matchRoundText.emplace(m_font, "", 8);
    m_matchRoundText->setFillColor(BODY_COLOR);
    m_matchRoundText->setOutlineColor(sf::Color::Black);
    m_matchRoundText->setOutlineThickness(1.0f);

    // Banners
    m_bannerText.emplace(m_font, "", BANNER_FONT_SIZE);
    m_bannerText->setFillColor(GOLD_COLOR);
    m_bannerText->setOutlineColor(sf::Color::Black);
    m_bannerText->setOutlineThickness(2.f);

    m_bannerSubText.emplace(m_font, "", BANNER_SUB_FONT_SIZE);
    m_bannerSubText->setFillColor(BODY_COLOR);
    m_bannerSubText->setOutlineColor(sf::Color::Black);
    m_bannerSubText->setOutlineThickness(1.f);

    // Overhead tags
    m_p1Label.emplace(m_font, "P1", LABEL_FONT_SIZE);
    m_p1Label->setFillColor(P1_COLOR);
    m_p1Label->setOutlineColor(sf::Color::Black);
    m_p1Label->setOutlineThickness(1.f);

    m_p2Label.emplace(m_font, "P2", LABEL_FONT_SIZE);
    m_p2Label->setFillColor(P2_COLOR);
    m_p2Label->setOutlineColor(sf::Color::Black);
    m_p2Label->setOutlineThickness(1.f);
}

void PvpPlayState::refreshHudTexts() {
    if (!m_fontLoaded) {
        return;
    }

    // P1 Column (Top-Left, X = 40px clear of side wall)
    if (m_p1Header) {
        std::string p1Name = "P1 ";
        p1Name += characterName(m_playerTypes[0]);
        m_p1Header->setString(p1Name);
        alignLeft(*m_p1Header, {40.f, 8.f});
    }
    if (m_p1WinsText) {
        std::string p1Wins = "WINS: " + std::to_string(m_roundWins[0]);
        m_p1WinsText->setString(p1Wins);
        alignLeft(*m_p1WinsText, {40.f, 22.f});
    }
    if (m_p1FireText) {
        if (m_fireTimer[0] > 0.f) {
            char buf[16];
            std::snprintf(buf, sizeof(buf), "FIRE %.1fs", m_fireTimer[0]);
            m_p1FireText->setString(buf);
            m_p1FireText->setFillColor(m_fireTimer[0] <= 1.0f ? GOLD_COLOR : FIRE_COLOR);
            alignLeft(*m_p1FireText, {40.f, 35.f});
        } else {
            m_p1FireText->setString("");
        }
    }

    // P2 Column (Top-Right, X = 600px clear of side wall)
    if (m_p2Header) {
        std::string p2Name = characterName(m_playerTypes[1]);
        p2Name += " P2";
        m_p2Header->setString(p2Name);
        alignRight(*m_p2Header, {600.f, 8.f});
    }
    if (m_p2WinsText) {
        std::string p2Wins = "WINS: " + std::to_string(m_roundWins[1]);
        m_p2WinsText->setString(p2Wins);
        alignRight(*m_p2WinsText, {600.f, 22.f});
    }
    if (m_p2FireText) {
        if (m_fireTimer[1] > 0.f) {
            char buf[16];
            std::snprintf(buf, sizeof(buf), "FIRE %.1fs", m_fireTimer[1]);
            m_p2FireText->setString(buf);
            m_p2FireText->setFillColor(m_fireTimer[1] <= 1.0f ? GOLD_COLOR : FIRE_COLOR);
            alignRight(*m_p2FireText, {600.f, 35.f});
        } else {
            m_p2FireText->setString("");
        }
    }

    // Center Column (Top-Center, X = 320px)
    if (m_matchScoreText) {
        std::string score = std::to_string(m_roundWins[0]) + "  -  " + std::to_string(m_roundWins[1]);
        m_matchScoreText->setString(score);
        alignCenter(*m_matchScoreText, {320.f, 8.f});
    }
    if (m_matchRoundText) {
        std::string roundStr;
        if (m_roundWins[0] == 1 && m_roundWins[1] == 1) {
            roundStr = "FINAL ROUND";
        } else {
            roundStr = "ROUND " + std::to_string(m_roundNumber);
        }
        m_matchRoundText->setString(roundStr);
        alignCenter(*m_matchRoundText, {320.f, 24.f});
    }

    // Banners (Center-Screen, Y = 85px / 112px clear of pedestal)
    if (m_bannerText && m_bannerSubText) {
        std::string banner;
        std::string sub;

        switch (m_phase) {
            case Phase::ROUND_INTRO:
                banner = (m_roundWins[0] == 1 && m_roundWins[1] == 1)
                             ? "FINAL ROUND"
                             : "ROUND " + std::to_string(m_roundNumber);
                sub = m_phaseTimer > ROUND_INTRO_GO_WINDOW ? "READY..."
                                                           : "GO!!";
                break;
            case Phase::FIGHT:
                banner.clear();
                sub.clear();
                break;
            case Phase::KO_SETTLE:
            case Phase::ROUND_END: {
                const std::string who = m_roundWinner == 0 ? "PLAYER 1" : "PLAYER 2";
                banner = who + " SCORES!";
                sub = m_roundWins[m_roundWinner == -1 ? 0 : m_roundWinner] >=
                              WINS_TO_TAKE_MATCH
                          ? "MATCH OVER"
                          : "NEXT ROUND...";
                break;
            }
            case Phase::MATCH_END:
                banner = m_matchWinner == 0 ? "PLAYER 1 WINS!"
                                            : "PLAYER 2 WINS!";
                sub = "ENTER: REMATCH    ESC: MENU";
                break;
        }

        m_bannerText->setString(banner);
        m_bannerSubText->setString(sub);
        alignCenter(*m_bannerText, {320.f, 85.f});
        alignCenter(*m_bannerSubText, {320.f, 112.f});
    }
}

void PvpPlayState::drawWorldLabels(sf::RenderTarget& target) {
    if (!m_level || !m_fontLoaded) {
        return;
    }

    const sf::View& view = m_level->getCamera().getView();
    target.setView(view);
    const float viewTop = view.getCenter().y - (view.getSize().y / 2.0f);

    const auto drawLabel = [&target, viewTop](std::optional<sf::Text>& label,
                                              const Mario* player, float blinkTimer,
                                              bool stunned) {
        if (!label || !player || player->isDying()) {
            return;
        }
        // A stunned fighter's tag flickers so the state is readable.
        if (stunned && std::fmod(blinkTimer, 0.16f) < 0.08f) {
            return;
        }

        const sf::FloatRect bounds = label->getLocalBounds();
        label->setOrigin({bounds.position.x + bounds.size.x / 2.f,
                          bounds.size.y});
        const float targetY = std::max(viewTop + 8.f, player->getPosition().y - 2.f);
        label->setPosition({player->getPosition().x + player->getSize().x / 2.f,
                            targetY});
        target.draw(*label);
    };

    drawLabel(m_p1Label, fighter(0), m_blinkTimer, m_stunTimer[0] > 0.f);
    drawLabel(m_p2Label, fighter(1), m_blinkTimer, m_stunTimer[1] > 0.f);
}

void PvpPlayState::drawHud(sf::RenderTarget& target) {
    if (!m_fontLoaded) {
        return;
    }

    // Player 1
    if (m_p1Header && !m_p1Header->getString().isEmpty()) target.draw(*m_p1Header);
    if (m_p1WinsText && !m_p1WinsText->getString().isEmpty()) target.draw(*m_p1WinsText);
    if (m_p1FireText && !m_p1FireText->getString().isEmpty()) target.draw(*m_p1FireText);

    // Player 2
    if (m_p2Header && !m_p2Header->getString().isEmpty()) target.draw(*m_p2Header);
    if (m_p2WinsText && !m_p2WinsText->getString().isEmpty()) target.draw(*m_p2WinsText);
    if (m_p2FireText && !m_p2FireText->getString().isEmpty()) target.draw(*m_p2FireText);

    // Match info
    if (m_matchScoreText && !m_matchScoreText->getString().isEmpty()) target.draw(*m_matchScoreText);
    if (m_matchRoundText && !m_matchRoundText->getString().isEmpty()) target.draw(*m_matchRoundText);

    // Center banners
    if (m_bannerText && !m_bannerText->getString().isEmpty()) target.draw(*m_bannerText);
    if (m_bannerSubText && !m_bannerSubText->getString().isEmpty()) target.draw(*m_bannerSubText);
}

void PvpPlayState::render(sf::RenderTarget& target) {
    if (!m_level) {
        return;
    }

    m_level->render(target);
    drawWorldLabels(target);

    target.setView(target.getDefaultView());
    drawHud(target);
}
