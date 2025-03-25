#pragma once
#include "Vec2.h"
#include <string>

namespace settings
{
	// Window settings
	inline constexpr int screenWidth = 800;
	inline constexpr int screenHeight = 600;
	inline constexpr int fps = 60;
	inline const std::string title = "Tetris";

	// Board settings
	inline constexpr int cellSize = 15;
	inline constexpr int boardPadding = 2;
	inline constexpr Vec2<int> boardPosition{200, 200};
	inline constexpr Vec2<int> boardWidthHeight{10, 20};

	// Game settings
	inline constexpr float initialDropInterval = 1.0f;
	inline constexpr float timeIntervalSpeedUp = 60.0f;
	inline constexpr float rotationSpeed = 120.0f;
	inline constexpr float gravity = 1.0f; // pixels/second^2
	inline constexpr float friction = 0.9f;
	inline constexpr float restitution = 0.2f; // Bounciness
	inline constexpr float stability_threshold = 0.8f; // How much the tetromino can tilt before it becomes unstable (how far from center of gravity)
}