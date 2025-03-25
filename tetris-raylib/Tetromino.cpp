#include "Settings.h"
#include "Tetromino.h"
#include "Board.h"

Tetromino::Tetromino(const bool* shape, int dimension, Color color, Board& board)
	:
	shape(shape),
	dimension(dimension),
	color(color),
	pos(board.GetWidth() / 2 - dimension / 2, 0),
	board(board),
	currentRotation(0.0f),
	hasLanded(false),
	timeSinceLastMove(0.0f),
	moveInterval(1.0f)
{
	physics.position = Vec2<float>(pos.GetX(), pos.GetY());
	physics.velocity = Vec2<float>(0.0f, 0.0f);
	physics.centerOfMass = CalculateCenterOfMass();
}

void Tetromino::Draw() const
{
	if (currentMode == PhysicsMode::Grid) {
		DrawGridBased();
	}
	else {
		DrawContinuous();
	}
}

void Tetromino::DrawGridBased() const
{
	for (int y = 0; y < dimension; ++y) {
		for (int x = 0; x < dimension; ++x) {
			if (IsCellAt(x, y)) {
				Vec2<int> cellPos = pos + Vec2<int>(x, y);

				// Only draw if within bounds
				if (cellPos.GetX() >= 0 && cellPos.GetX() < board.GetWidth() &&
					cellPos.GetY() >= 0 && cellPos.GetY() < board.GetHeight()) {
					board.DrawCell(cellPos, color);
				}
			}
		}
	}
}

void Tetromino::DrawContinuous() const
{
	float cellSizeF = static_cast<float>(board.GetCellSize());

	for (int y = 0; y < dimension; ++y) {
		for (int x = 0; x < dimension; ++x) {
			// Still use the same shape data
			if (shape[y * dimension + x]) {
				// Transform the cell to world space with rotation
				float worldX, worldY;
				TransformToWorldSpace(worldX, worldY, x, y);

				// Draw the rotated cell directly
				Rectangle rect = {
					worldX,
					worldY,
					cellSizeF - board.GetPadding(),
					cellSizeF - board.GetPadding()
				};

				DrawRectanglePro(
					rect,                     // Rectangle
					{ cellSizeF / 2, cellSizeF / 2 }, // Origin at center
					currentRotation,          // Rotation angle
					color                     // Color
				);
			}
		}
	}
}

void Tetromino::TransformToWorldSpace(float& outX, float& outY, int cellX, int cellY) const
{
	Vec2<int> boardScreen = board.GetScreenPosition();
	float padding = static_cast<float>(board.GetPadding());
	float cellSizeF = static_cast<float>(board.GetCellSize());

	// Calculate center of the tetromino in grid space
	float centerX = physics.position.GetX() + dimension / 2.0f;
	float centerY = physics.position.GetY() + dimension / 2.0f;

	// Calculate cell position relative to center
	float relX = (cellX - dimension / 2.0f) * cellSizeF;
	float relY = (cellY - dimension / 2.0f) * cellSizeF;

	// Apply rotation (in radians)
	float radians = currentRotation * DEG2RAD;
	float rotatedX = relX * cosf(radians) - relY * sinf(radians);
	float rotatedY = relX * sinf(radians) + relY * cosf(radians);

	// Calculate world position
	outX = boardScreen.GetX() + padding + (centerX * cellSizeF) + rotatedX;
	outY = boardScreen.GetY() + padding + (centerY * cellSizeF) + rotatedY;
}

void Tetromino::HandleBoundariesAndCollisions()
{
	// Save original position
	Vec2<int> originalPos = pos;

	// Apply boundary constraints
	bool positionClamped = ClampPositionToBoundaries();

	// If we're in continuous mode and position wasn't clamped, check for collisions
	if (currentMode == PhysicsMode::Continuous && !positionClamped) {
		if (IsCollidingWithBoard()) {
			// Try reverting to horizontal position but keep vertical
			Vec2<int> verticalOnlyPos = Vec2<int>(originalPos.GetX(), pos.GetY());
			pos = verticalOnlyPos;

			if (IsCollidingWithBoard()) {
				// If still colliding, fully revert
				pos = originalPos;
				physics.position.SetY(static_cast<float>(originalPos.GetY()));

				if (physics.isStable) {
					// If stable, land the piece
					hasLanded = true;
					currentMode = PhysicsMode::Grid; // Switch back to grid mode when landing
					currentRotation = round(currentRotation / 90.0f) * 90.0f; // Snap to 90 degrees
				}
				else {
					// If unstable, bounce
					physics.velocity.SetY(-physics.velocity.GetY() * settings::restitution);
				}
			}
			else {
				// Update physics X position to match
				physics.position.SetX(static_cast<float>(originalPos.GetX()));
			}
		}
	}
}

bool Tetromino::ClampPositionToBoundaries()
{
	bool positionClamped = false;

	// Clamp X position
	if (pos.GetX() < 0) {
		pos.SetX(0);
		physics.position.SetX(0.0f);
		physics.velocity.SetX(0.0f);
		positionClamped = true;
	}

	// Find the maximum X extent
	int maxX = FindRightmostCell();

	if (pos.GetX() + maxX >= board.GetWidth()) {
		pos.SetX(board.GetWidth() - maxX - 1);
		physics.position.SetX(static_cast<float>(board.GetWidth() - maxX - 1));
		physics.velocity.SetX(0.0f);
		positionClamped = true;
	}

	// Clamp Y position at top
	if (pos.GetY() < 0) {
		pos.SetY(0);
		physics.position.SetY(0.0f);
		physics.velocity.SetY(0.0f);
		positionClamped = true;
	}

	return positionClamped;

}

int Tetromino::FindRightmostCell() const
{
	int maxX = 0;
	for (int y = 0; y < dimension; ++y) {
		for (int x = 0; x < dimension; ++x) {
			if (IsCellAt(x, y) && x > maxX) {
				maxX = x;
			}
		}
	}
	return maxX;
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
	if (currentMode == PhysicsMode::Grid) {
		switch (currentGridRotation) {
		case GridRotation::Zero:
			return shape[y * dimension + x];
		case GridRotation::Ninety:
			return shape[dimension * (dimension - 1) - dimension * x + y];
		case GridRotation::OneEighty:
			return shape[dimension * (dimension - y) - (x + 1)];
		case GridRotation::TwoSeventy:
			return shape[(dimension - 1) + dimension * x - y];
		default:
			return false;
		}

	}
	else {
		// In continuous mode, use the original shape data
		// We'll handle rotation in the drawing method
		return (x >= 0 && x < dimension && y >= 0 && y < dimension) ?
			shape[y * dimension + x] : false;
	}

}


void Tetromino::CheckCollisionBeforeRotation()
{
	// Save the original position
	Vec2<int> originalPos = pos;
	float originalRotation = currentRotation;

	// Define the offset sequence to try (wall kick)
	const std::vector<Vec2<int>> offsets = {
		Vec2<int>(0, 0),    // No offset (check if it already fits)
		Vec2<int>(1, 0),    // Right by 1
		Vec2<int>(-1, 0),   // Left by 1
		Vec2<int>(2, 0),    // Right by 2
		Vec2<int>(-2, 0)    // Left by 2
	};

	// Try each offset
	for (const auto& offset : offsets)
	{
		pos = originalPos + offset;
		if (!IsCollidingWithBoard())
			return;  // Success! We found a position that works
	}

	// If all offsets failed, revert to the original position
	pos = originalPos;
	currentRotation = originalRotation;
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

Vec2<float> Tetromino::CalculateCenterOfMass() const
{
	float totalMass = 0.0f;
	float sumX = 0.0f;
	float sumY = 0.0f;

	for (int y = 0; y < dimension; ++y)
	{
		for (int x = 0; x < dimension; ++x)
		{
			if (IsCellAt(x, y))
			{
				totalMass += 1.0f;
				sumX += x;
				sumY += y;
			}
		}
	}

	if (totalMass > 0.0f)
	{
		return Vec2<float>(sumX / totalMass, sumY / totalMass);
	}

	return Vec2<float>(dimension / 2.0f, dimension / 2.0f);
}

bool Tetromino::IsStable() const
{
	// A piece is stable when its center of mass is above a support
	Vec2<float> worldCoM = physics.position + physics.centerOfMass;

	// Find supports (blocks directly below)
	std::vector<float> supportXPositions;

	for (int y = 0; y < dimension; ++y) {
		for (int x = 0; x < dimension; ++x) {
			if (IsCellAt(x, y)) {
				Vec2<int> cellPos = pos + Vec2<int>(x, y);
				Vec2<int> cellBelow = cellPos + Vec2<int>(0, 1);

				// If we hit the bottom of the board or another block
				if (cellBelow.GetY() >= board.GetHeight() ||
					(cellBelow.GetY() < board.GetHeight() && board.CellExists(cellBelow))) {
					supportXPositions.push_back(cellPos.GetX() + 0.5f); // Center of cell
				}
			}
		}
	}

	if (supportXPositions.empty()) {
		return false; // No support, not stable
	}

	// Find the support range
	float minSupport = *std::min_element(supportXPositions.begin(), supportXPositions.end());
	float maxSupport = *std::max_element(supportXPositions.begin(), supportXPositions.end());

	// Expanded support range (allow slight offset)
	float expandedMin = minSupport - settings::stability_threshold;
	float expandedMax = maxSupport + settings::stability_threshold;

	// Center of mass must be within the support range to be stable
	return worldCoM.GetX() >= expandedMin && worldCoM.GetX() <= expandedMax;
}

float Tetromino::CalculateTorque() const
{
	Vec2<float> worldCoM = physics.position + physics.centerOfMass;

	// Find the average support position
	float avgSupportX = 0.0f;
	int supportCount = 0;

	for (int y = 0; y < dimension; ++y) {
		for (int x = 0; x < dimension; ++x) {
			if (IsCellAt(x, y)) {
				Vec2<int> cellPos = pos + Vec2<int>(x, y);
				Vec2<int> cellBelow = cellPos + Vec2<int>(0, 1);

				if (cellBelow.GetY() >= board.GetHeight() ||
					(cellBelow.GetY() < board.GetHeight() && board.CellExists(cellBelow))) {
					avgSupportX += cellPos.GetX() + 0.5f;
					supportCount++;
				}
			}
		}
	}

	if (supportCount > 0) {
		avgSupportX /= supportCount;

		// Calculate torque based on distance from center of mass to support
		// The further the center of mass is from support, the greater the torque
		float leverage = worldCoM.GetX() - avgSupportX;
		return leverage * 10.0f; // Scale factor for rotation speed
	}

	return 0.0f;

}

float Tetromino::NormalizeAngle(float angle) const
{
	float result = fmodf(angle, 360.0f);
	if (result < 0.0f) {
		result += 360.0f;  // Handle negative angles
	}
	return result;
}

void Tetromino::Update(float deltaTime)
{
	if (hasLanded) {
		return;
	}

	timeSinceLastMove += deltaTime;

	if (currentMode == PhysicsMode::Grid)
	{
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

	else
	{
		// Determine if we should be in physics mode
		physics.isStable = IsStable();
		bool shouldUsePhysics = !physics.isStable;

		if (!shouldUsePhysics) {
			// When switching back to grid, snap rotation to nearest 90 degrees
			currentRotation = round(currentRotation / 90.0f) * 90.0f;
			currentMode = PhysicsMode::Grid;
		}

		// Apply physics calculations
		float torque = CalculateTorque();
		physics.angularVelocity += torque * deltaTime;
		physics.angularVelocity *= settings::friction;

		if (fabs(physics.angularVelocity) > 0.1f) {
			currentRotation = NormalizeAngle(currentRotation + physics.angularVelocity * deltaTime);
		}

		// Apply gravity and velocity updates
		physics.velocity.SetY(physics.velocity.GetY() + settings::gravity * deltaTime);
		physics.velocity.SetX(physics.velocity.GetX() * settings::friction);

		physics.position.SetX(physics.position.GetX() + physics.velocity.GetX() * deltaTime);
		physics.position.SetY(physics.position.GetY() + physics.velocity.GetY() * deltaTime);

		// Update integer position
		pos = Vec2<int>(round(physics.position.GetX()), round(physics.position.GetY()));

		// Handle collisions and boundaries
		HandleBoundariesAndCollisions();
	}
}


void Tetromino::RotateClockwise()
{
	if (currentMode == PhysicsMode::Grid)
	{
		currentGridRotation = static_cast<GridRotation>((static_cast<int>(currentGridRotation) + 90) % 360);
		CheckCollisionBeforeRotation();
	}
	else
	{
		float oldAngle = currentRotation;
		currentRotation = NormalizeAngle(currentRotation + 1.0f);

		if (IsCollidingWithBoard()) {
			currentRotation = oldAngle;
			CheckCollisionBeforeRotation();
		}
	}
}

void Tetromino::RotateCounterClockwise()
{
	if (currentMode == PhysicsMode::Grid)
	{
		if (currentGridRotation == GridRotation::Zero)
		{
			currentGridRotation = GridRotation::TwoSeventy;
			CheckCollisionBeforeRotation();
			return;
		}
		currentGridRotation = static_cast<GridRotation>((static_cast<int>(currentGridRotation) - 90) % 360);
		CheckCollisionBeforeRotation();

	}
	else
	{
		float oldAngle = currentRotation;
		currentRotation = NormalizeAngle(currentRotation - 1.0f);

		if (IsCollidingWithBoard()) {
			currentRotation = oldAngle;
			CheckCollisionBeforeRotation();
		}
	}
}

void Tetromino::MoveLeft() {
	pos += Vec2<int>(-1, 0); // Move the Tetromino left
	physics.position.SetX(physics.position.GetX() - 1.0f);

	if (IsCollidingWithBoard()) {
		pos -= Vec2<int>(-1, 0); // Revert the position if collision detected
		physics.position.SetX(physics.position.GetX() + 1.0f);
	}
}

void Tetromino::MoveRight() {
	pos += Vec2<int>(1, 0); // Move the Tetromino right
	physics.position.SetX(physics.position.GetX() + 1.0f);

	if (IsCollidingWithBoard()) {
		pos -= Vec2<int>(1, 0); // Revert the position if collision detected
		physics.position.SetX(physics.position.GetX() - 1.0f);
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

void Tetromino::Reset()
{
	pos = Vec2<int>(board.GetWidth() / 2 - dimension / 2, 0);
	hasLanded = false;
	timeSinceLastMove = 0.0f;
	currentRotation = 0.0f;
	physics.position = Vec2<float>(pos.GetX(), pos.GetY());
	physics.velocity = Vec2<float>(0.0f, 0.0f);
	physics.angularVelocity = 0.0f;
	physics.isStable = true;
}
