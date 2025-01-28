/* -----------------------------------------------------------------------------------------------
Author: Harrison Day
Date: 01/24/25
Description: This header file defines the sfmod namespace, which includes utility modules
for enhancing SFML-based applications. It features:

1. Style Flags:
   - Provides animation style flags (e.g., Reverse, PingPong) for flexible control.

2. Animation Class:
   - Manages frame-based animations with adjustable framerate, looping, and style options.
   - Supports adding frames from file paths or preloaded textures.

3. AnimationModule Class:
   - Handles multiple animations, allowing dynamic switching and updating.

4. CollisionModule Class:
   - Implements AABB collision detection, calculating precise offsets for resolving collisions.
   - Includes directional flags for detecting collision sides (top, bottom, left, right).

This library modularizes essential components, streamlining the development of SFML projects.
----------------------------------------------------------------------------------------------- */

#ifndef SFMLMODULES_HPP
#define SFMLMODULES_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace sfmod {

namespace Style {
    enum : unsigned int {
        None        = 0,        // No style
        Reverse     = 1 << 0,   // 0010
        PingPong    = 1 << 1,   // 0100 
    };
} // namespeace Style

class Animation {
public:
    void addFrame(const std::string& image);
    void addFrame(const sf::Texture& tex);
    void setFrameRate(unsigned int fps);
    void setStyle(unsigned int style);
    void removeStyle(unsigned int style);
    void setLoops(unsigned int loops);
    void update(double delta_time);
    const sf::Texture& getCurrentFrame() const;
    void reset();

private:
    std::vector<sf::Texture> m_frameVector;
    sf::Texture m_currentFrame;
    double m_frameTimer = 0.0;
    unsigned int m_frameRate = 12;
    unsigned int m_style = Style::None;
    int m_frameIndex = 0;
    int m_loops = 0;
    int m_maxLoops = -1;

    bool hasStyle(unsigned int style) const;
};

class AnimationModule {
public:
    void addFrame(const std::string& animation_name, const std::string& image);
    void addFrame(const std::string& animation_name, const sf::Texture& tex);
    void addAnimation(Animation newAnimation, const std::string& animation_name);
    void setCurrentAnimation(const std::string& animation_name);
    void setAnimationFramerate(const std::string& animation_name, unsigned int fps);
    sf::Texture update(double delta_time);
    sf::Texture getCurrentFrame();

private:
    std::unordered_map<std::string, Animation> m_animations;
    Animation* m_currentAnimation;
};

class CollisionModule {
public:
    sf::Vector2f getCollisionOffset(sf::Rect<float> entity_rect, std::vector<sf::Rect<float>> collision_rects);

    const bool top() const;
    const bool bottom() const;
    const bool left() const;
    const bool right() const;

private:
    bool m_top = false;
    bool m_bottom = false;
    bool m_left = false;
    bool m_right = false;

    sf::Vector2f entity_movement;
};

// --------------------------------------------- Animation Implementation
void Animation::update(double delta_time) {
    if (m_frameTimer >= 1.0/m_frameRate && (m_loops < m_maxLoops * 2 || m_maxLoops == -1)) {
        if (hasStyle(Style::PingPong) && m_frameIndex + 1 > m_frameVector.size() - 1 && !hasStyle(Style::Reverse)) setStyle(Style::Reverse);
        if (hasStyle(Style::PingPong) && m_frameIndex - 1 < 0 && hasStyle(Style::Reverse)) removeStyle(Style::Reverse);
        if (hasStyle(Style::Reverse)) m_frameIndex = (m_frameIndex - 1 < 0) ? m_frameVector.size() - 1 : m_frameIndex - 1;
        else m_frameIndex = (m_frameIndex + 1 > m_frameVector.size() - 1) ? 0 : m_frameIndex + 1;
        if ((m_frameIndex == 0 || m_frameIndex == m_frameVector.size() - 1) && m_maxLoops != -1) m_loops++;
        if (m_loops == m_maxLoops * 2 && !hasStyle(Style::PingPong)) m_frameIndex = (hasStyle(Style::Reverse)) ? 0 : m_frameVector.size() - 1;
        m_frameTimer = 0.0;
    }

    m_frameTimer += delta_time;
}

void Animation::addFrame(const std::string& image) {
    m_frameVector.push_back(sf::Texture(image));
}

void Animation::addFrame(const sf::Texture& tex) {
    m_frameVector.push_back(tex);
}

void Animation::setFrameRate(unsigned int fps) {
    m_frameRate = (fps > 0) ? fps : 12;
}

void Animation::setStyle(unsigned int style) {
    m_style |= style;

    if (hasStyle(Style::Reverse))
        m_frameIndex = m_frameVector.size() - 1;
}

void Animation::removeStyle(unsigned int style) {
    if (hasStyle(style))
        m_style ^= style;
}

void Animation::setLoops(unsigned int loops) {
    m_maxLoops = loops;
}

void Animation::reset() {
    m_frameIndex = (hasStyle(Style::Reverse)) ? m_frameVector.size() - 1 : 0;
}

const sf::Texture& Animation::getCurrentFrame() const {
    return m_frameVector[m_frameIndex];
}

bool Animation::hasStyle(unsigned int style) const {
    return m_style & style;
}

// --------------------------------------------- AnimationModule Implementation
sf::Texture AnimationModule::update(double delta_time) {
    m_currentAnimation->update(delta_time);
    return m_currentAnimation->getCurrentFrame();
}

void AnimationModule::addFrame(const std::string& animation_name, const std::string& image) {
    m_animations[animation_name].addFrame(image);
}

void AnimationModule::addFrame(const std::string& animation_name, const sf::Texture& tex) {
    m_animations[animation_name].addFrame(tex);
}

void AnimationModule::addAnimation(Animation newAnimation, const std::string& animation_name) {
    m_animations[animation_name] = newAnimation;
    m_currentAnimation = &m_animations[animation_name];
}

void AnimationModule::setCurrentAnimation(const std::string& animation_name) {
    m_currentAnimation->reset();
    m_currentAnimation = &m_animations[animation_name];
}

void AnimationModule::setAnimationFramerate(const std::string& animation_name, unsigned int fps) {
    m_animations[animation_name].setFrameRate(fps);
}

sf::Texture AnimationModule::getCurrentFrame() {
    return m_currentAnimation->getCurrentFrame();
}

sf::Vector2f CollisionModule::getCollisionOffset(sf::Rect<float> entity_rect, std::vector<sf::Rect<float>> collision_rects) {
    if (collision_rects.empty())
        return {0.f, 0.f};

    sf::Vector2f cumulativeOffset(0.f, 0.f);
    bool collisionResolved;

    // Loop until no collisions, accumulating the entity's offset for resolution
    do {
        collisionResolved = false;
        const sf::FloatRect* bestRect = nullptr;
        float maxOverlap = 0.f;

        for (const auto& rect : collision_rects) {
            if (auto i = entity_rect.findIntersection(rect)) {
                sf::Vector2f iSize = i->size;
                float overlap = std::max(iSize.x, iSize.y);

                if (overlap > maxOverlap) {
                    maxOverlap = overlap;
                    bestRect = &rect;
                }
            }
        }

        // test and resolve collision with the rect that overlaps the player the most
        if (bestRect) {
            if (auto i = entity_rect.findIntersection(*bestRect)) {
                sf::Vector2f iSize = i->size;
                sf::Vector2f offset(0.f, 0.f);

                if ((int)iSize.x == (int)iSize.y)
                    offset.y = (i->position.y > entity_rect.position.y) ? -iSize.y : iSize.y;
                else if ((int)iSize.x > (int)iSize.y)
                    offset.y = (i->position.y > entity_rect.position.y) ? -iSize.y : iSize.y;
                else
                    offset.x = (i->position.x > entity_rect.position.x) ? -iSize.x : iSize.x;

                entity_rect.position += offset;
                cumulativeOffset += offset;
                collisionResolved = true;
            }
        }

    } while (collisionResolved);

    // determine collision directions
    m_bottom = cumulativeOffset.y < 0; m_top = cumulativeOffset.y > 0;
    m_left = cumulativeOffset.x > 0; m_right = cumulativeOffset.x < 0;

    return cumulativeOffset;
}

const bool CollisionModule::top() const { return m_top; }
const bool CollisionModule::bottom() const { return m_bottom; }
const bool CollisionModule::left() const { return m_left; }
const bool CollisionModule::right() const { return m_right; }

} // namespace sfmod

#endif //!SFMLMODULES_HPP