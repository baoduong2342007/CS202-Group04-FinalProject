/**
 * @file PvpPlayState.h
 * @author TV5 (Truyen)
 * @brief Two-player duel gameplay state: best-of-3 head-stomp arena with a
 *        contested timed fire flower.
 * @note The arena fits one logical screen, so the camera stays pinned. Only a
 *       head-stomp wins a round; the fire flower grants a 5-second FIRE state
 *       whose fireballs knock the opponent airborne instead of scoring.
 */

#pragma once

#include <memory>
#include <optional>
#include <random>

#include <SFML/Graphics.hpp>

#include "entities/Mario.h"
#include "level/Level.h"
#include "patterns/InputHandler.h"
#include "states/IGameState.h"

class PvpPlayState final : public IGameState {
public:
    // 1. Constructor / Destructor
    explicit PvpPlayState(CharacterType playerOne = CharacterType::MARIO,
                          CharacterType playerTwo = CharacterType::LUIGI);
    ~PvpPlayState() override = default;

    /// Round/match state machine, exposed for tests and HUD debugging.
    enum class Phase {
        ROUND_INTRO,   ///< "ROUND N — READY / GO", gameplay input frozen
        FIGHT,         ///< duel active; flower director runs
        KO_SETTLE,     ///< loser death animation; winner frozen
        ROUND_END,     ///< round banner; then next round or match end
        MATCH_END      ///< winner banner; ENTER rematches, ESC exits
    };

    // 2. Override methods
    void onEnter() override;
    void onExit() override;
    void onPause() override;
    void onResume() override;

    void processEvents(const sf::Event& event) override;
    void processInput(const InputState& inputState) override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) override;

    // 3. Getters and Helpers
    Phase getPhase() const { return m_phase; }
    /// Non-owning access to the duel arena.
    Level* getLevel() { return m_level.get(); }
    const Level* getLevel() const { return m_level.get(); }
    int getRoundNumber() const { return m_roundNumber; }
    /// Round wins for fighter index 0 (player one) or 1 (player two).
    int getRoundWins(int playerIndex) const;
    /// Fighter index that won the match, or -1 while it is still undecided.
    int getMatchWinner() const { return m_matchWinner; }
    bool isFlowerOnField() const;
    /// Remaining FIRE seconds for a fighter; 0 when he is not on fire.
    float getFireTimer(int playerIndex) const;
    bool isStunned(int playerIndex) const;
    /// Override the next fire-flower spawn countdown.
    void setNextFlowerCountdown(float seconds);

private:
    // 5. Private methods
    void rebindCommands();
    Mario* fighter(int playerIndex);
    const Mario* fighter(int playerIndex) const;
    int fighterIndex(const Mario* mario) const;

    void updateRoundFlow(float dt);
    void updateFireDirector(float dt);
    void applyFireballHits();
    void scheduleNextFlowerCountdown();
    bool anyFighterInFireState() const;
    void resetRound();
    void resetMatch();

    void initHud();
    void refreshHudTexts();
    void drawWorldLabels(sf::RenderTarget& target);
    void drawHud(sf::RenderTarget& target);

    // 6. Private members
    CharacterType m_playerTypes[2] = {CharacterType::MARIO,
                                      CharacterType::LUIGI};
    int m_roundWins[2] = {0, 0};
    int m_roundNumber = 1;
    Phase m_phase = Phase::ROUND_INTRO;
    float m_phaseTimer = 0.f;
    /// Fighter index that scored the current (or most recent) round.
    int m_roundWinner = -1;
    int m_matchWinner = -1;

    std::unique_ptr<Level> m_level;
    InputHandler m_p1Input;
    InputHandler m_p2Input;

    sf::Vector2f m_spawnPositions[2] = {{0.f, 0.f}, {0.f, 0.f}};
    sf::Vector2f m_pedestalPosition{0.f, 0.f};
    bool m_pedestalFound = false;

    /// Counts down only while no flower is on the field and no fighter is in
    /// the FIRE state; reaching zero spawns the flower on the pedestal.
    float m_flowerCountdown = 0.f;
    std::mt19937 m_rng{std::random_device{}()};

    float m_fireTimer[2] = {0.f, 0.f};
    float m_stunTimer[2] = {0.f, 0.f};
    float m_fireImmunityTimer[2] = {0.f, 0.f};

    // HUD
    sf::Font m_font;
    bool m_fontLoaded = false;
    std::optional<sf::Text> m_p1Header;
    std::optional<sf::Text> m_p1WinsText;
    std::optional<sf::Text> m_p1FireText;
    std::optional<sf::Text> m_p2Header;
    std::optional<sf::Text> m_p2WinsText;
    std::optional<sf::Text> m_p2FireText;
    std::optional<sf::Text> m_matchScoreText;
    std::optional<sf::Text> m_matchRoundText;
    std::optional<sf::Text> m_bannerText;
    std::optional<sf::Text> m_bannerSubText;
    std::optional<sf::Text> m_p1Label;
    std::optional<sf::Text> m_p2Label;
    float m_blinkTimer = 0.f;
};
