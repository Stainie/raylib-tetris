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
	DrawText("Tetris", 10, 10, 20, WHITE);
	DrawText("Press Enter to start", 10, 30, 20, WHITE);
	DrawText("Press F to toggle fullscreen", 10, 50, 20, WHITE);
	DrawText("Press ESC to exit", 10, 70, 20, WHITE);
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
	DrawText("PAUSED", 10, 10, 20, WHITE);
	DrawText("Press P to resume", 10, 30, 20, WHITE);
	DrawText("Press F to toggle fullscreen", 10, 50, 20, WHITE);
	DrawText("Press R to restart", 10, 70, 20, WHITE);
	DrawText("Press ESC to exit", 10, 90, 20, WHITE);
}

void Game::Update()
{
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
	if (IsKeyPressed(KEY_ENTER)) { // Example condition to start the game
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

	float deltaTime = GetFrameTime(); // Get the time elapsed since the last frame
	elapsedTime += deltaTime;

	// Increase speed level every 60 seconds
	if (elapsedTime >= settings::timeIntervalSpeedUp * speedLevel) {
		speedLevel++;
		// Update the Tetromino drop interval based on the new speed level
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

	HandleTouchInput();

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
		currentTetromino->Reset();
		currentState = GameState::Gameplay;
	}
	else if (IsKeyPressed(KEY_ESCAPE))
	{
		CloseWindow();
	}
}

void Game::InitTouchControls()
{
	// Position buttons at bottom of screen
	float btnWidth = 80;
	float btnHeight = 80;
	float padding = 20;
	float bottomY = GetScreenHeight() - btnHeight - padding;

	leftBtn = { padding, bottomY, btnWidth, btnHeight };
	rightBtn = { padding + btnWidth + 10, bottomY, btnWidth, btnHeight };
	rotateBtn = { GetScreenWidth() - btnWidth * 2 - padding - 10, bottomY, btnWidth, btnHeight };
	dropBtn = { GetScreenWidth() - btnWidth - padding, bottomY, btnWidth, btnHeight };
}

void Game::HandleTouchInput()
{
	if (!currentTetromino) return;

	// Get touch count
	int touchCount = GetTouchPointCount();

	if (touchCount > 0)
	{
		Vector2 touchPos = GetTouchPosition(0);

		// Check button presses
		if (CheckCollisionPointRec(touchPos, leftBtn))
		{
			if (!isTouching) {
				currentTetromino->MoveLeft();
				isTouching = true;
			}
		}
		else if (CheckCollisionPointRec(touchPos, rightBtn))
		{
			if (!isTouching) {
				currentTetromino->MoveRight();
				isTouching = true;
			}
		}
		else if (CheckCollisionPointRec(touchPos, rotateBtn))
		{
			if (!isTouching) {
				currentTetromino->RotateClockwise();
				isTouching = true;
			}
		}
		else if (CheckCollisionPointRec(touchPos, dropBtn))
		{
			if (!isTouching) {
				currentTetromino->Drop();
				isTouching = true;
			}
		}
	}
	else
	{
		isTouching = false;
	}

	// Alternative: Gesture-based controls
	int gesture = GetGestureDetected();
	if (gesture == GESTURE_SWIPE_LEFT) currentTetromino->MoveLeft();
	if (gesture == GESTURE_SWIPE_RIGHT) currentTetromino->MoveRight();
	if (gesture == GESTURE_SWIPE_DOWN) currentTetromino->Drop();
	if (gesture == GESTURE_TAP) currentTetromino->RotateClockwise();
}

void Game::DrawTouchControls()
{
	// Semi-transparent buttons
	DrawRectangleRec(leftBtn, Fade(GRAY, 0.5f));
	DrawRectangleRec(rightBtn, Fade(GRAY, 0.5f));
	DrawRectangleRec(rotateBtn, Fade(GRAY, 0.5f));
	DrawRectangleRec(dropBtn, Fade(GRAY, 0.5f));

	// Button labels
	DrawText("<", leftBtn.x + 30, leftBtn.y + 25, 30, WHITE);
	DrawText(">", rightBtn.x + 30, rightBtn.y + 25, 30, WHITE);
	DrawText("R", rotateBtn.x + 30, rotateBtn.y + 25, 30, WHITE);
	DrawText("v", dropBtn.x + 30, dropBtn.y + 25, 30, WHITE);
}
