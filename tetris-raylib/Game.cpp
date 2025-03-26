#include <assert.h>
#include "Game.h"
#include "raylibCpp.h"
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
	DrawText(settings::title, 5, 5, 50, LIGHTGRAY);
	DrawText("Press Enter to start", 10, 70, 20, WHITE);
	DrawText("Press F to toggle fullscreen", 10, 90, 20, WHITE);
	DrawText("Press ESC to exit", 10, 110, 20, WHITE);
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

	if (IsKeyPressed(KEY_RIGHT))
	{
		currentTetromino->MoveRight();
	}
	else if (IsKeyPressed(KEY_LEFT))
	{
		currentTetromino->MoveLeft();
	}
	else if (rayCpp::IsKeyAction(KEY_DOWN))
	{
		currentTetromino->RotateClockwise();
	}
	else if (rayCpp::IsKeyAction(KEY_UP))
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
