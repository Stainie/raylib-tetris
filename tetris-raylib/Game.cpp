#include <assert.h>
#include "Game.h"
#include "raylib.h"
#include "Settings.h"
#include "GameUtils.h"

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
	DrawText(std::to_string(static_cast<int>(elapsedTime)).c_str(), 10, 10, 20, WHITE);
	DrawText(("Speed Level: " + std::to_string(speedLevel)).c_str(), 10, 30, 20, WHITE);
	board.Draw();
	if (currentTetromino)
	{
		currentTetromino->Draw();
	}
}

void Game::Update()
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
	/*else if (IsKeyPressed(KEY_R))
	{
		board.Reset();
	}
	else if (IsKeyPressed(KEY_P))
	{
		board.Pause();
	}
	else if (IsKeyPressed(KEY_C))
	{
		board.Continue();
	}*/
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
