#include "Tetromino.h"
#include "Board.h"

Tetromino::Tetromino(const bool* shape, int dimension, Color color, Board& board)
	:
	shape(shape),
	dimension(dimension),
	color(color),
	pos(board.GetWidth() / 2 - dimension / 2, 0),
	board(board),
	currentRotation(Rotation::Zero),
	hasLanded(false),
	timeSinceLastMove(0.0f),
	moveInterval(1.0f)
{
}

void Tetromino::Draw() const
{
	for (int y = 0; y < dimension; ++y)
	{
		for (int x = 0; x < dimension; ++x)
		{
			if (IsCellAt(x, y)) {
				board.DrawCell(pos + Vec2<int>(x, y), color);
			}
		}
	}
}

Vec2<int> Tetromino::GetLastPos() const {
	int highestX = 0;
	int highestY = 0;

	for (int y = 0; y < dimension; ++y) {
		for (int x = 0; x < dimension; ++x) {
			if (IsCellAt(x, y)) {
				highestX = std::max(highestX, x);
				highestY = std::max(highestY, y);
			}
		}
	}

	return pos + Vec2<int>(highestX, highestY);
}

bool Tetromino::IsCellAt(int x, int y) const
{
	switch (currentRotation) {
	case Rotation::Zero:
		return shape[y * dimension + x];
	case Rotation::Ninety:
		return shape[dimension * (dimension - 1) - dimension * x + y];
	case Rotation::OneEighty:
		return shape[dimension * (dimension - y) - (x + 1)];
	case Rotation::TwoSeventy:
		return shape[(dimension - 1) + dimension * x - y];
	default:
		return false;
	}

}

bool Tetromino::IsCollidingWithBoard() const {
	for (int y = 0; y < dimension; ++y) {
		for (int x = 0; x < dimension; ++x) {
			if (IsCellAt(x, y)) {
				Vec2<int> cellPos = pos + Vec2<int>(x, y);
				if (cellPos.GetX() < 0 || cellPos.GetX() >= board.GetWidth() ||
					cellPos.GetY() < 0 || cellPos.GetY() >= board.GetHeight() ||
					board.CellExists(cellPos)) {
					return true;
				}
			}
		}
	}
	return false;
}

void Tetromino::Update(float deltaTime) {
	if (hasLanded) {
		return;
	}

	timeSinceLastMove += deltaTime;

	if (timeSinceLastMove >= moveInterval) {
		timeSinceLastMove = 0.0f;

		pos += Vec2<int>(0, 1);

		// Temporarily move the Tetromino down for collision check
		if (IsCollidingWithBoard()) {
			// Revert the position if collision detected
			pos -= Vec2<int>(0, 1);
			hasLanded = true;
		}
	}
}


void Tetromino::RotateClockwise()
{
	currentRotation = static_cast<Rotation>((static_cast<int>(currentRotation) + 90) % 360);
}

void Tetromino::RotateCounterClockwise()
{
	if (currentRotation == Rotation::Zero)
	{
		currentRotation = Rotation::TwoSeventy;
		return;
	}
	currentRotation = static_cast<Rotation>((static_cast<int>(currentRotation) - 90) % 360);
}

void Tetromino::MoveLeft() {
	pos += Vec2<int>(-1, 0); // Move the Tetromino left

	if (IsCollidingWithBoard()) {
		pos -= Vec2<int>(-1, 0); // Revert the position if collision detected
	}
}

void Tetromino::MoveRight() {
	pos += Vec2<int>(1, 0); // Move the Tetromino right

	if (IsCollidingWithBoard()) {
		pos -= Vec2<int>(1, 0); // Revert the position if collision detected
	}
}


void Tetromino::Drop() {
	pos += Vec2<int>(0, 1); // Move the Tetromino down

	if (IsCollidingWithBoard()) {
		pos -= Vec2<int>(0, 1); // Revert the position if collision detected
	}
}

void Tetromino::SetDropInterval(float interval)
{
	moveInterval = interval;
}

void Tetromino::AddToBoard() const {
	for (int y = 0; y < dimension; ++y) {
		for (int x = 0; x < dimension; ++x) {
			if (IsCellAt(x, y)) {
				board.SetCell(pos + Vec2<int>(x, y), color);
			}
		}
	}
}

bool Tetromino::HasLanded() const
{
	return hasLanded;
}
