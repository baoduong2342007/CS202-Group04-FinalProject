/**
 * @file PauseState.h
 * @author TV1 (Dương)
 * @brief Pause State
 */
#pragma once
#include "states/IGameState.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include "ui/UIMenuWidget.h"

enum class PauseVolumeSelection {
    MUSIC,
    SOUND
};

class PauseState : public IGameState {
public:
    // 1. Constructor / Destructor
    /**
     * @brief Constructs the PauseState, loading the font and configuring the pause text.
     */
    PauseState();
    ~PauseState() override = default;

    // 2. Override methods
    /**
     * @brief Triggered when the game enters the pause state. Freezes relevant game systems.
     */
    void onEnter() override;

    /**
     * @brief Triggered when the game exits the pause state. Resumes frozen systems.
     */
    void onExit() override;

    /**
     * @brief Processes window events.
     * @param event The SFML event to process.
     */
    void processEvents(const sf::Event& event) override;

    /**
     * @brief Processes user input. Specifically looks for the unpause command.
     * @param inputState The current input state.
     */
    void processInput(const InputState& inputState) override;

    /**
     * @brief Updates the pause state.
     * @param dt Delta time in seconds.
     */
    void update(float dt) override;

    /**
     * @brief Renders the pause overlay text.
     * @param target The render target to draw to.
     */
    void render(sf::RenderTarget& target) override;

    /**
     * @brief Indicates that this state does not replace the underlying state, but draws on top of it.
     * @return true Always returns true for PauseState.
     */
    bool isOverlay() const override { return true; }

private:
    // 5. Private methods
    void refreshText();
    void adjustSelectedVolume(float delta);

    // 6. Private members
    sf::Font m_font;                             ///< Font for rendering text
    bool m_fontLoaded;                           ///< True if font loaded successfully
    std::optional<sf::Text> m_titleText;         ///< "PAUSED" text
    std::optional<sf::Text> m_volumeText;        ///< Volume text overlay
    PauseVolumeSelection m_selectedVolume = PauseVolumeSelection::MUSIC; ///< Currently selected volume to adjust
    std::unique_ptr<UIMenuWidget> m_menu;        ///< The UI menu for the state
};
