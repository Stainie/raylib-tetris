#pragma once
#include "Vec2.h"

namespace settings
{
	// Window settings
#ifdef PLATFORM_WEB
	constexpr int screenWidth = 400;   // Narrower for portrait
	constexpr int screenHeight = 700;  // Taller
	constexpr int cellSize = 28;       // Slightly smaller cells
#else
	constexpr int screenWidth = 800;
	constexpr int screenHeight = 600;
	constexpr int cellSize = 30;
#endif
	inline constexpr int fps = 60;
	inline const std::string title = "Tetris";

	// Board settings
	inline constexpr int boardPadding = 2;
	inline constexpr Vec2<int> boardPosition{200, 200};
	inline constexpr Vec2<int> boardWidthHeight{10, 20};

	// Game settings
	inline constexpr float initialDropInterval = 1.0f;
	inline constexpr float timeIntervalSpeedUp = 60.0f;
}