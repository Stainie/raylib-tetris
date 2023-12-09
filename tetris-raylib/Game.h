#pragma once
#include <string>
#include <memory>
#include "Board.h"
#include "Tetromino.h"

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
	void Update();

	Board board;
	bool isGameOver = false;
	float elapsedTime = 0.0f;
	int speedLevel;

	std::unique_ptr<Tetromino> currentTetromino;
};
