#pragma once
#include <assert.h>
#include "Settings.h"
#include "Vec2.h"
#include "raylibCpp.h"
#include "Board.h"

class Tetromino
{
public:
	enum class GridRotation
	{
		Zero = 0,
		Ninety = 90,
		OneEighty = 180,
		TwoSeventy = 270
	};
public:
	Tetromino(const bool* shape, int dimension, Color color, Board& board);
	void Draw() const;
	void Update(float deltaTime);
	void RotateClockwise();
	void RotateCounterClockwise();
	void MoveLeft();
	void MoveRight();
	void Drop();
	void SetDropInterval(float interval);
	void AddToBoard() const;
	bool HasLanded() const;
	void Reset();
private:
	struct Physics
	{
		Vec2<float> position;
		Vec2<float> velocity;
		Vec2<float> centerOfMass;
		float angularVelocity = 0.0f; // degrees/second
		bool isStable = true;
	};

	Physics physics;
	settings::PhysicsMode currentMode = settings::currentMode;

	void DrawGridBased() const;
	void DrawContinuous() const;
	void TransformToWorldSpace(float& outX, float& outY, int cellX, int cellY) const;
	void HandleBoundariesAndCollisions();
	bool ClampPositionToBoundaries();
	int FindRightmostCell() const;
	Vec2<float> CalculateCenterOfMass() const;
	bool IsStable() const;
	float CalculateTorque() const;

	Vec2<int> pos;
	Vec2<int> GetLastPos() const;
	bool IsCellAt(int x, int y) const;
	void CheckCollisionBeforeRotation();
	bool IsCollidingWithBoard() const;
	float NormalizeAngle(float angle) const;
	float currentRotation = 0.0f;
	GridRotation currentGridRotation = GridRotation::Zero;
	bool hasLanded;
	float timeSinceLastMove;
	float moveInterval;
	const bool* shape;
	const int dimension;
	const Color color;
	Board& board;
};

class StraightTetromino : public Tetromino
{
public:
	StraightTetromino(Board& board)
		: Tetromino(shape, dimension, color, board)
	{
		static_assert(sizeof(shape) / sizeof(bool) == dimension * dimension);
	}
private:
	static constexpr bool shape[] =
	{ 0,0,0,0,
	1,1,1,1,
	0,0,0,0,
	0,0,0,0 };
	static constexpr Color color = BLUE;
	static constexpr int dimension = 4;
};

class SquareTetromino : public Tetromino
{
public:
	SquareTetromino(Board& board)
		: Tetromino(shape, dimension, color, board)
	{
		static_assert(sizeof(shape) / sizeof(bool) == dimension * dimension);
	}
private:
	static constexpr bool shape[] = {
			1,1,1,1
	};
	static constexpr Color color = YELLOW;
	static constexpr int dimension = 2;
};

class TeeTetromino : public Tetromino
{
public:
	TeeTetromino(Board& board)
		: Tetromino(shape, dimension, color, board)
	{
		static_assert(sizeof(shape) / sizeof(bool) == dimension * dimension);
	}
private:
	static constexpr bool shape[] = {
			0,1,0,
			1,1,1,
			0,0,0
	};
	static constexpr Color color = PURPLE;
	static constexpr int dimension = 3;
};

class JayTetromino : public Tetromino
{
public:
	JayTetromino(Board& board)
		: Tetromino(shape, dimension, color, board)
	{
		static_assert(sizeof(shape) / sizeof(bool) == dimension * dimension);
	}
private:
	static constexpr bool shape[] = {
			1,0,0,
			1,1,1,
			0,0,0
	};
	static constexpr Color color = ORANGE;
	static constexpr int dimension = 3;
};

class EllTetromino : public Tetromino
{
public:
	EllTetromino(Board& board)
		: Tetromino(shape, dimension, color, board)
	{
		static_assert(sizeof(shape) / sizeof(bool) == dimension * dimension);
	}
private:
	static constexpr bool shape[] = {
			0,0,1,
			1,1,1,
			0,0,0
	};
	static constexpr Color color = GREEN;
	static constexpr int dimension = 3;
};

class SkewSTetromino : public Tetromino
{
public:
	SkewSTetromino(Board& board)
		: Tetromino(shape, dimension, color, board)
	{
		static_assert(sizeof(shape) / sizeof(bool) == dimension * dimension);
	}
private:
	static constexpr bool shape[] = {
			0,1,1,
			1,1,0,
			0,0,0
	};
	static constexpr Color color = RED;
	static constexpr int dimension = 3;
};

class SkewZTetromino : public Tetromino
{
public:
	SkewZTetromino(Board& board)
		: Tetromino(shape, dimension, color, board)
	{
		static_assert(sizeof(shape) / sizeof(bool) == dimension * dimension);
	}
private:
	static constexpr bool shape[] = {
		1,1,0,
		0,1,1,
		0,0,0
	};
	static constexpr Color color = MAROON;
	static constexpr int dimension = 3;
};