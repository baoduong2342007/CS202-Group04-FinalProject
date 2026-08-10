/**
 * @file AnimationSystem.h
 * @author TV2 (Nhật)
 * @brief Manages animation states and handles the logic for updating sprite texture rectangles over time.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <string>

/**
 * @brief Defines a single animation state, containing all frames and timing information.
 */
struct Animation {
    // Replaces the magic "0.1f" and acts as a safe fallback to prevent infinite update loops
    static constexpr float DEFAULT_FRAME_DURATION = 0.1f;

    std::vector<sf::IntRect> frames;
    float frameDuration = DEFAULT_FRAME_DURATION; 
    bool isLooping = true;
};

class AnimationSystem {
public:
    // 1. Constructor / Destructor
    AnimationSystem() = default;
    ~AnimationSystem() = default;

    // Prevent copying to ensure animation state isn't accidentally duplicated
    AnimationSystem(const AnimationSystem&) = delete;
    AnimationSystem& operator=(const AnimationSystem&) = delete;

    // 2. Overrides
    // (None currently required for this class)

    // 3. Public methods
    /**
     * @brief Automatically generates an Animation struct by slicing a uniform grid spritesheet.
     * @param startX The X pixel coordinate of the first frame.
     * @param startY The Y pixel coordinate of the first frame.
     * @param frameWidth The width of a single frame in pixels.
     * @param frameHeight The height of a single frame in pixels.
     * @param frameCount The total number of frames in this animation.
     * @param frameDuration The time in seconds each frame should be displayed.
     * @param isLooping True if the animation should loop; false otherwise.
     * @return A fully populated Animation struct ready to be added to the system.
     */
    static Animation createGridAnimation(
        int startX, 
        int startY, 
        int frameWidth, 
        int frameHeight, 
        int frameCount, 
        float frameDuration, 
        bool isLooping = true,
        int spacingX = 0,
        int spacingY = 0
    );

    /**
     * @brief Creates an Animation from an explicit list of frame rects.
     * Use this when sprites have irregular spacing and cannot be sliced from a uniform grid.
     * @param frames The list of texture rects, one per frame.
     * @param frameDuration The time in seconds each frame should be displayed.
     * @param isLooping True if the animation should loop; false otherwise.
     * @return A fully populated Animation struct ready to be added to the system.
     */
    static Animation createManualAnimation(
        const std::vector<sf::IntRect>& frames,
        float frameDuration,
        bool isLooping = true
    );

    /**
     * @brief Registers a new animation state to the system.
     * @param name The unique string identifier for the animation (e.g., "Run").
     * @param animation The Animation struct containing frame data and settings.
     */
    void addAnimation(const std::string& name, const Animation& animation);

    /**
     * @brief Starts playing the specified animation from the first frame.
     * @param name The string identifier of the animation to play.
     */
    void play(const std::string& name);

    /**
     * @brief Halts the current animation and resets the frame index to zero.
     */
    void stop();

    /**
     * @brief Freezes the current animation timer without resetting the frame index.
     */
    void pause();

    /**
     * @brief Resumes playback of a paused animation.
     */
    void resume();

    /**
     * @brief Updates the animation timer and modifies the sprite's texture rectangle if a frame advances.
     * @param dt The time elapsed since the last engine update, in seconds.
     * @param sprite A reference to the sf::Sprite that will be animated.
     */
    void update(float dt, sf::Sprite& sprite);

    /**
     * @brief Checks if the current non-looping animation has finished playing.
     * @return true if the animation has finished, false otherwise.
     */
    bool isFinished() const;

    // 4. Getters / Setters
    // (None currently required for this class)

private:
    // 5. Private methods
    // (None currently required for this class)

    // 6. Private members
    static constexpr float RESET_TIME = 0.0f;

    std::unordered_map<std::string, Animation> m_animations; 
    std::string m_currentAnimation;                          
    unsigned int m_currentFrame = 0;                         
    float m_elapsedTime = RESET_TIME;                              
    bool m_isPlaying = false;                                
};