# SFMLModules.hpp

## Overview
`SFMLModules.hpp` is a header-only library designed to extend and enhance SFML-based applications. It provides essential utility modules for animations, multi-animation management, and collision detection, streamlining the development process.

### Features
1. **Style Flags**:
   - Animation style modifiers (`sfmod::Style`) for flexible control.
   - Supports styles such as:
     - `Reverse`: Reverses animation playback.
     - `PingPong`: Alternates forward and reverse playback.

2. **Animation Class**:
   - Manages frame-based animations with:
     - Adjustable framerate.
     - Looping and style options.
   - Frames can be added from file paths or preloaded textures.
   - Includes reset and update functionality for dynamic animations.

3. **AnimationModule Class**:
   - Handles multiple animations, allowing for:
     - Dynamic switching between animations.
     - Independent framerate control for each animation.
   - Simplifies managing complex animation states.

4. **CollisionModule Class**:
   - Implements Axis-Aligned Bounding Box (AABB) collision detection.
   - Calculates precise offsets for resolving collisions.
   - Provides flags to determine collision sides (top, bottom, left, right).

---

## Installation
1. Clone or download this repository.
2. Include `SFMLModules.hpp` in your project.
3. Ensure that SFML is properly configured in your project.

---

## Usage

### Animation
```cpp
#include "SFMLModules.hpp"

sfmod::Animation anim;
anim.addFrame("frame1.png");
anim.addFrame("frame2.png");
anim.setFrameRate(24);
anim.setStyle(sfmod::Style::PingPong);

double deltaTime = 0.016; // Example delta time
anim.update(deltaTime);
sf::Texture currentFrame = anim.getCurrentFrame();

### AnimationModule
```cpp
sfmod::AnimationModule animModule;

animModule.addFrame("Walk", "walk1.png");
animModule.addFrame("Walk", "walk2.png");
animModule.setCurrentAnimation("Walk");

double deltaTime = 0.016; // Example delta time
sf::Texture currentFrame = animModule.update(deltaTime);

### AnimationModule
```cpp
sfmod::CollisionModule collision;

sf::FloatRect entityRect(10.f, 10.f, 50.f, 50.f);
std::vector<sf::FloatRect> collisionRects = {
    sf::FloatRect(0.f, 0.f, 100.f, 100.f),
    sf::FloatRect(20.f, 20.f, 30.f, 30.f)
};

sf::Vector2f offset = collision.getCollisionOffset(entityRect, collisionRects);

if (collision.bottom()) {
    std::cout << "Collision at the bottom!" << std::endl;
}