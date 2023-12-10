#pragma once
#include <string>
#include <memory>
#include "Board.h"
#include "Tetromino.h"
#include "GameState.h"

class Game
{
public:
	Game(int width, int height, int fps, std::string title);
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	~Game() noexcept;

	bool ShouldClose() const;

	void IncreaseDifficulty(float newDropInterval);

	void Tick();
private:
	void Draw();
	void DrawMainMenu();
	void DrawGameplay();
	void Update();
	void UpdateMainMenu();
	void UpdateGameplay();

	Board board;
	bool isGameOver = false;
	float elapsedTime = 0.0f;
	int speedLevel;
	GameState currentState = GameState::MainMenu;

	std::unique_ptr<Tetromino> currentTetromino;
};
