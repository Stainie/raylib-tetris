#pragma once
#include "Vec2.h"
#include <string>

namespace settings
{
	// Window settings
#ifdef PLATFORM_WEB
	constexpr int screenWidth = 400;
	constexpr int screenHeight = 700;
	constexpr int cellSize = 28;

	// Board position for web (centered horizontally)
	// Board is 10 cells * 28px = 280px wide
	// Center: (400 - 280) / 2 = 60px from left
	// Top: 60px for score area
	constexpr int boardPosX = 60;
	constexpr int boardPosY = 60;
#else
	constexpr int screenWidth = 800;
	constexpr int screenHeight = 600;
	constexpr int cellSize = 30;

	// Board position for desktop
	// Board is 10 cells * 30px = 300px wide
	// Position to left side, leaving room for score on right
	constexpr int boardPosX = 200;
	constexpr int boardPosY = 50;
#endif

	inline constexpr int fps = 60;
	inline const std::string title = "Tetris";

	// Board settings
	inline constexpr int boardPadding = 2;
	inline constexpr Vec2<int> boardPosition{ boardPosX, boardPosY };
	inline constexpr Vec2<int> boardWidthHeight{ 10, 20 };

	// Game settings
	inline constexpr int initialDropInterval = 1;
	inline constexpr float timeIntervalSpeedUp = 60.0f;
}