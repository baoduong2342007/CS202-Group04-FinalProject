/**
 * @file AnimationSystem.cpp
 * @author TV2 (Nhật)
 * @brief Implementation of sprite animation handling.
 */

#include "core/AnimationSystem.h"
#include <iostream>

Animation AnimationSystem::createGridAnimation(
    int startX, int startY, int frameWidth, int frameHeight, 
    int frameCount, float frameDuration, bool isLooping,
    int spacingX, int spacingY) 
{
    Animation anim;
    anim.frameDuration = frameDuration;
    anim.isLooping = isLooping;

    // INVARIANT CHECK: Prevent invalid dimensions or counts
    if (frameCount <= 0 || frameWidth <= 0 || frameHeight <= 0) {
        std::cerr << "[AnimationSystem] Error: Invalid grid dimensions or frame count!" << std::endl;
        return anim; 
    }

    // INVARIANT CHECK: Prevent infinite loops in the update cycle
    if (frameDuration <= RESET_TIME) {
        std::cerr << "[AnimationSystem] Warning: Frame duration must be > 0. Clamping to default." << std::endl;
        anim.frameDuration = Animation::DEFAULT_FRAME_DURATION;
    }

    for (int i = 0; i < frameCount; ++i) {
        int x = startX + i * (frameWidth + spacingX);
        int y = startY + i * spacingY;
        
        anim.frames.push_back(sf::IntRect({x, y}, {frameWidth, frameHeight}));
    }

    return anim;
}

Animation AnimationSystem::createManualAnimation(
    const std::vector<sf::IntRect>& frames,
    float frameDuration,
    bool isLooping)
{
    Animation anim;
    anim.frameDuration = frameDuration;
    anim.isLooping = isLooping;
    anim.frames = frames;

    if (frameDuration <= RESET_TIME) {
        std::cerr << "[AnimationSystem] Warning: Frame duration must be > 0. Clamping to default." << std::endl;
        anim.frameDuration = Animation::DEFAULT_FRAME_DURATION;
    }

    return anim;
}

void AnimationSystem::addAnimation(const std::string& name, const Animation& animation) {
    // INVARIANT CHECK: Prevent adding completely empty or invalid animations
    if (animation.frames.empty() || animation.frameDuration <= RESET_TIME) {
        std::cerr << "[AnimationSystem] Error: Attempting to add invalid animation '" << name << "'!" << std::endl;
        return;
    }
    m_animations[name] = animation;
}

void AnimationSystem::play(const std::string& name) {
    auto it = m_animations.find(name);
    if (it == m_animations.end()) {
        std::cerr << "[AnimationSystem] Error: Animation '" << name << "' not found!" << std::endl;
        return;
    }

    if (m_currentAnimation != name) {
        m_currentAnimation = name;
        m_currentFrame = 0;
        m_elapsedTime = RESET_TIME;
    }
    
    m_isPlaying = true;
}



void AnimationSystem::advance(float dt) {
    if (!m_isPlaying || m_currentAnimation.empty()) {
        return;
    }

    auto it = m_animations.find(m_currentAnimation);
    if (it == m_animations.end()) {
        return;
    }

    const Animation& anim = it->second;
    
    // Safety check against empty frame vectors (double-checking just in case)
    if (anim.frames.empty()) {
        return;
    }

    m_elapsedTime += dt;

    // Advance frames if enough time has passed. 
    // The previous checks ensure anim.frameDuration > RESET_TIME, preventing an infinite loop here.
    while (m_elapsedTime >= anim.frameDuration) {
        m_elapsedTime -= anim.frameDuration;
        m_currentFrame++;

        if (m_currentFrame >= anim.frames.size()) {
            if (anim.isLooping) {
                m_currentFrame = 0;
            } else {
                m_currentFrame = anim.frames.size() - 1;
                m_isPlaying = false;
            }
        }
    }

}

void AnimationSystem::update(float dt, sf::Sprite& sprite) {
    advance(dt);
    const auto animation = m_animations.find(m_currentAnimation);
    if (animation != m_animations.end() && !animation->second.frames.empty()) {
        sprite.setTextureRect(animation->second.frames[m_currentFrame]);
    }
}

void AnimationSystem::update(float dt) {
    advance(dt);
}

bool AnimationSystem::isFinished() const {
    if (m_currentAnimation.empty()) return false;
    auto it = m_animations.find(m_currentAnimation);
    if (it == m_animations.end()) return false;
    return !m_isPlaying && !it->second.isLooping;
}
