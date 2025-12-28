#include <assert.h>
#include "Game.h"
#include "raylib.h"
#include "Settings.h"
#include "GameUtils.h"
#include "GameState.h"

Game::Game(int width, int height, int fps, std::string title)
	: board(settings::boardPosition, settings::boardWidthHeight, settings::cellSize, settings::boardPadding),
	speedLevel(settings::initialDropInterval)
{
	assert(!GetWindowHandle());	// Make sure we don't already have a window
	SetTargetFPS(fps);
	InitWindow(width, height, title.c_str());
	InitTouchControls();
}

Game::~Game() noexcept
{
	assert(GetWindowHandle());	// Already closed?
	CloseWindow();
}

bool Game::ShouldClose() const
{
	return WindowShouldClose() || isGameOver;
}

void Game::IncreaseDifficulty(float newDropInterval)
{
	if (currentTetromino)
	{
		currentTetromino->SetDropInterval(newDropInterval);
	}
}

void Game::Tick()
{
	BeginDrawing();
	Update();
	Draw();
	EndDrawing();
}

void Game::Draw()
{
	ClearBackground(BLACK);

	switch (currentState)
	{
	case GameState::MainMenu:
		DrawMainMenu();
		break;
	case GameState::Gameplay:
		DrawGameplay();
		break;
	case GameState::Pause:
		DrawPause();
		break;
	default:
		break;
	}
}

void Game::DrawMainMenu()
{
	float screenW = static_cast<float>(GetScreenWidth());

	// Title - centered
	const char* title = "TETRIS";
	int titleWidth = MeasureText(title, 60);
	DrawText(title, (int)(screenW - titleWidth) / 2, 80, 60, WHITE);

	// Start button
	DrawRectangleRec(startBtn, DARKGRAY);
	DrawRectangleLinesEx(startBtn, 3, WHITE);
	const char* startText = "TAP TO START";
	int startTextWidth = MeasureText(startText, 20);
	DrawText(startText,
		(int)(startBtn.x + (startBtn.width - startTextWidth) / 2),
		(int)(startBtn.y + (startBtn.height - 20) / 2),
		20, WHITE);

	// Instructions
	const char* instructions = "Swipe or use buttons to play";
	int instrWidth = MeasureText(instructions, 16);
	DrawText(instructions, (int)(screenW - instrWidth) / 2, (int)(startBtn.y + startBtn.height + 30), 16, GRAY);

	// Keyboard hint (for desktop testing)
	DrawText("Or press ENTER to start", 10, GetScreenHeight() - 30, 16, DARKGRAY);
}

void Game::DrawGameplay()
{
	ClearBackground(BLACK);
	DrawText(std::to_string(static_cast<int>(elapsedTime)).c_str(), 10, 10, 20, WHITE);
	DrawText(("Speed Level: " + std::to_string(speedLevel)).c_str(), 10, 30, 20, WHITE);
	board.Draw();
	if (currentTetromino)
	{
		currentTetromino->Draw();
	}

#ifdef PLATFORM_WEB
	DrawTouchControls();
#endif // PLATFORM_WEB

}

void Game::DrawPause()
{
	float screenW = static_cast<float>(GetScreenWidth());

	// Title
	const char* title = "PAUSED";
	int titleWidth = MeasureText(title, 50);
	DrawText(title, (int)(screenW - titleWidth) / 2, 100, 50, WHITE);

	// Resume button
	DrawRectangleRec(resumeBtn, DARKGREEN);
	DrawRectangleLinesEx(resumeBtn, 3, WHITE);
	const char* resumeText = "RESUME";
	int resumeTextWidth = MeasureText(resumeText, 24);
	DrawText(resumeText,
		(int)(resumeBtn.x + (resumeBtn.width - resumeTextWidth) / 2),
		(int)(resumeBtn.y + (resumeBtn.height - 24) / 2),
		24, WHITE);

	// Restart button
	DrawRectangleRec(restartBtn, DARKGRAY);
	DrawRectangleLinesEx(restartBtn, 3, WHITE);
	const char* restartText = "RESTART";
	int restartTextWidth = MeasureText(restartText, 24);
	DrawText(restartText,
		(int)(restartBtn.x + (restartBtn.width - restartTextWidth) / 2),
		(int)(restartBtn.y + (restartBtn.height - 24) / 2),
		24, WHITE);

	// Keyboard hints
	DrawText("P - Resume | R - Restart", 10, GetScreenHeight() - 30, 16, DARKGRAY);
}

void Game::Update()
{
	// Update touch cooldown
	if (touchCooldown > 0)
	{
		touchCooldown -= GetFrameTime();
	}

	// Reset touch state when no longer touching
	if (GetTouchPointCount() == 0)
	{
		isTouching = false;
	}

	switch (currentState)
	{
	case GameState::MainMenu:
		UpdateMainMenu();
		break;
	case GameState::Gameplay:
		UpdateGameplay();
		break;
	case GameState::Pause:
		UpdatePause();
		break;
	default:
		break;
	}
}

void Game::UpdateMainMenu()
{
	// Touch input - start game
	if (!isTouching && IsButtonTouched(startBtn))
	{
		currentState = GameState::Gameplay;
		isTouching = true;
	}

	// Keyboard input (for desktop testing)
	if (IsKeyPressed(KEY_ENTER))
	{
		currentState = GameState::Gameplay;
	}
	else if (IsKeyPressed(KEY_F))
	{
		ToggleFullscreen();
	}
	else if (IsKeyPressed(KEY_ESCAPE))
	{
		CloseWindow();
	}
}

void Game::UpdateGameplay()
{
	if (board.IsTopRowOccupied()) {
		isGameOver = true;
	}

	float deltaTime = GetFrameTime();
	elapsedTime += deltaTime;

	// Increase speed level every 60 seconds
	if (elapsedTime >= settings::timeIntervalSpeedUp * speedLevel) {
		speedLevel++;
		if (currentTetromino) {
			currentTetromino->SetDropInterval(std::max(0.1f, 1.0f - 0.1f * (speedLevel - 1)));
		}
	}

	if (!currentTetromino || currentTetromino->HasLanded()) {
		if (currentTetromino)
		{
			currentTetromino->AddToBoard();
		}
		currentTetromino = GenerateRandomTetromino(board);
	}

	// Handle touch input
	HandleGameplayTouchInput();

	// Keyboard input (for desktop testing)
	if (IsKeyPressed(KEY_RIGHT))
	{
		currentTetromino->MoveRight();
	}
	else if (IsKeyPressed(KEY_LEFT))
	{
		currentTetromino->MoveLeft();
	}
	else if (IsKeyPressed(KEY_DOWN))
	{
		currentTetromino->RotateClockwise();
	}
	else if (IsKeyPressed(KEY_UP))
	{
		currentTetromino->RotateCounterClockwise();
	}
	else if (IsKeyPressed(KEY_SPACE))
	{
		currentTetromino->Drop();
	}
	else if (IsKeyPressed(KEY_R))
	{
		board.Reset();
		currentTetromino->Reset();
	}
	else if (IsKeyPressed(KEY_P))
	{
		currentState = GameState::Pause;
	}
	else if (IsKeyPressed(KEY_ESCAPE))
	{
		CloseWindow();
	}
	else if (IsKeyPressed(KEY_F))
	{
		ToggleFullscreen();
	}

	currentTetromino->Update(deltaTime);
	board.Update();
}


void Game::UpdatePause()
{
	// Touch input
	if (!isTouching)
	{
		if (IsButtonTouched(resumeBtn))
		{
			currentState = GameState::Gameplay;
			isTouching = true;
		}
		else if (IsButtonTouched(restartBtn))
		{
			board.Reset();
			if (currentTetromino) currentTetromino->Reset();
			elapsedTime = 0.0f;
			speedLevel = settings::initialDropInterval;
			currentState = GameState::Gameplay;
			isTouching = true;
		}
	}

	// Keyboard input (for desktop testing)
	if (IsKeyPressed(KEY_P))
	{
		currentState = GameState::Gameplay;
	}
	else if (IsKeyPressed(KEY_F))
	{
		ToggleFullscreen();
	}
	else if (IsKeyPressed(KEY_R))
	{
		board.Reset();
		if (currentTetromino) currentTetromino->Reset();
		elapsedTime = 0.0f;
		speedLevel = settings::initialDropInterval;
		currentState = GameState::Gameplay;
	}
	else if (IsKeyPressed(KEY_ESCAPE))
	{
		CloseWindow();
	}
}

void Game::InitTouchControls()
{
	float screenW = static_cast<float>(GetScreenWidth());
	float screenH = static_cast<float>(GetScreenHeight());

	// Gameplay buttons - positioned at bottom of screen
	float btnWidth = 70;
	float btnHeight = 70;
	float padding = 15;
	float bottomY = screenH - btnHeight - padding;

	// Left side: movement buttons
	leftBtn = { padding, bottomY, btnWidth, btnHeight };
	rightBtn = { padding + btnWidth + 10, bottomY, btnWidth, btnHeight };

	// Right side: rotation and drop buttons
	rotateLeftBtn = { screenW - btnWidth * 3 - padding - 20, bottomY, btnWidth, btnHeight };
	rotateRightBtn = { screenW - btnWidth * 2 - padding - 10, bottomY, btnWidth, btnHeight };
	dropBtn = { screenW - btnWidth - padding, bottomY, btnWidth, btnHeight };

	// Pause button - top right corner
	pauseBtn = { screenW - 60, 10, 50, 50 };

	// Menu buttons - centered on screen
	float menuBtnWidth = 200;
	float menuBtnHeight = 60;
	float centerX = (screenW - menuBtnWidth) / 2;

	startBtn = { centerX, screenH / 2 - 30, menuBtnWidth, menuBtnHeight };
	resumeBtn = { centerX, screenH / 2 - 80, menuBtnWidth, menuBtnHeight };
	restartBtn = { centerX, screenH / 2, menuBtnWidth, menuBtnHeight };
}

bool Game::IsButtonTouched(Rectangle btn)
{
	if (GetTouchPointCount() > 0)
	{
		Vector2 touchPos = GetTouchPosition(0);
		return CheckCollisionPointRec(touchPos, btn);
	}
	return false;
}


void Game::HandleGameplayTouchInput()
{
	if (!currentTetromino || isTouching || touchCooldown > 0) return;

	if (IsButtonTouched(leftBtn))
	{
		currentTetromino->MoveLeft();
		isTouching = true;
		touchCooldown = TOUCH_COOLDOWN_TIME;
	}
	else if (IsButtonTouched(rightBtn))
	{
		currentTetromino->MoveRight();
		isTouching = true;
		touchCooldown = TOUCH_COOLDOWN_TIME;
	}
	else if (IsButtonTouched(rotateLeftBtn))
	{
		currentTetromino->RotateCounterClockwise();
		isTouching = true;
		touchCooldown = TOUCH_COOLDOWN_TIME;
	}
	else if (IsButtonTouched(rotateRightBtn))
	{
		currentTetromino->RotateClockwise();
		isTouching = true;
		touchCooldown = TOUCH_COOLDOWN_TIME;
	}
	else if (IsButtonTouched(dropBtn))
	{
		currentTetromino->Drop();
		isTouching = true;
		touchCooldown = TOUCH_COOLDOWN_TIME;
	}
	else if (IsButtonTouched(pauseBtn))
	{
		currentState = GameState::Pause;
		isTouching = true;
	}
}

void Game::DrawTouchControls()
{
	Color btnColor = Fade(DARKGRAY, 0.7f);
	Color btnBorder = Fade(WHITE, 0.5f);

	// Left button
	DrawRectangleRec(leftBtn, btnColor);
	DrawRectangleLinesEx(leftBtn, 2, btnBorder);
	DrawText("<", (int)(leftBtn.x + 25), (int)(leftBtn.y + 20), 30, WHITE);

	// Right button
	DrawRectangleRec(rightBtn, btnColor);
	DrawRectangleLinesEx(rightBtn, 2, btnBorder);
	DrawText(">", (int)(rightBtn.x + 25), (int)(rightBtn.y + 20), 30, WHITE);

	// Rotate left button
	DrawRectangleRec(rotateLeftBtn, btnColor);
	DrawRectangleLinesEx(rotateLeftBtn, 2, btnBorder);
	DrawText("CCW", (int)(rotateLeftBtn.x + 12), (int)(rotateLeftBtn.y + 25), 20, WHITE);

	// Rotate right button
	DrawRectangleRec(rotateRightBtn, btnColor);
	DrawRectangleLinesEx(rotateRightBtn, 2, btnBorder);
	DrawText("CW", (int)(rotateRightBtn.x + 18), (int)(rotateRightBtn.y + 25), 20, WHITE);

	// Drop button
	DrawRectangleRec(dropBtn, Fade(MAROON, 0.7f));
	DrawRectangleLinesEx(dropBtn, 2, btnBorder);
	DrawText("v", (int)(dropBtn.x + 28), (int)(dropBtn.y + 20), 30, WHITE);

	// Pause button
	DrawRectangleRec(pauseBtn, btnColor);
	DrawRectangleLinesEx(pauseBtn, 2, btnBorder);
	DrawText("||", (int)(pauseBtn.x + 15), (int)(pauseBtn.y + 12), 25, WHITE);
}