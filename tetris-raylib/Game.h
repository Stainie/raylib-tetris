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
	void DrawPause();
	void Update();
	void UpdateMainMenu();
	void UpdateGameplay();
	void UpdatePause();
	void HandleTouchInput();
	void DrawTouchControls();
	void InitTouchControls();

	Board board;
	bool isGameOver = false;
	float elapsedTime = 0.0f;
	int speedLevel;
	GameState currentState = GameState::MainMenu;
	Vector2 touchStartPos;
	bool isTouching = false;
	float touchStartTime = 0.0f;

	Rectangle leftBtn;
	Rectangle rightBtn;
	Rectangle rotateBtn;
	Rectangle dropBtn;

	std::unique_ptr<Tetromino> currentTetromino;
};
