#pragma once
#include "Vec2.h"

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
}