#include "Board.h"
#include <cassert>

Board::Cell::Cell() : bExists(false), c(WHITE)
{
}

void Board::Cell::SetColor(Color c)
{
	this->c = c;
}

void Board::Cell::Add()
{
	bExists = true;
}

void Board::Cell::Remove()
{
	bExists = false;
}

bool Board::Cell::Exists() const
{
	return bExists;
}

Color Board::Cell::GetColor() const
{
	return c;
}

Board::Board(Vec2<int> screenPos, Vec2<int> widthHeight, int cellSize, int padding)
	: screenPos(screenPos), width(widthHeight.GetX()), height(widthHeight.GetY()), cellSize(cellSize), padding(padding)
{
	assert(width > 0 && height > 0);
	assert(cellSize > 0);
	cells.resize(width * height);
}

bool Board::IsTopRowOccupied() const {
	for (int x = 0; x < width; ++x) {
		if (CellExists(Vec2<int>(x, 0))) {
			return true;
		}
	}
	return false;
}

void Board::DrawCell(Vec2<int> pos) const
{
	const Cell& cell = cells[pos.GetY() * width + pos.GetX()];

	DrawCell(pos, cell.GetColor());
}

void Board::DrawCell(Vec2<int> pos, Color color) const
{
	// Add safety check before assertion
	if (pos.GetX() < 0 || pos.GetX() >= width || pos.GetY() < 0 || pos.GetY() >= height) {
		return; // Skip drawing out-of-bounds cells
	}

	// Original code continues...
	assert(pos.GetX() >= 0 && pos.GetX() < width && pos.GetY() >= 0 && pos.GetY() < height);
	Vec2<int> topLeft = screenPos + padding + (pos * cellSize);
	Vec2<int> paddedWidthHeight = Vec2<int>(cellSize, cellSize) - padding;

	rayCpp::DrawRectangle(topLeft, paddedWidthHeight, color);
}

void Board::Draw() const
{
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x) {
			if (CellExists({ x,y }))
				DrawCell(Vec2<int>(x, y));
		}
	DrawBorder();
}

void Board::Update()
{
	for (int y = height - 1; y >= 0; --y) {
		bool bRowFull = true;
		for (int x = 0; x < width; ++x) {
			if (!CellExists({ x,y })) {
				bRowFull = false;
				break;
			}
		}
		if (bRowFull) {
			for (int x = 0; x < width; ++x) {
				RemoveCell(Vec2<int>(x, y));
			}
			for (int y2 = y - 1; y2 >= 0; --y2) {
				for (int x = 0; x < width; ++x) {
					if (CellExists({ x,y2 })) {
						Color c = cells[y2 * width + x].GetColor();
						RemoveCell(Vec2<int>(x, y2));
						SetCell({ x,y2 + 1 }, c);
					}
				}
			}
			++y;
		}
	}
}

void Board::DrawBorder() const
{
	Vec2<int> topLeft = screenPos - (cellSize / 2);
	Vec2<int> widthHeight = Vec2<int>(width, height) * cellSize + cellSize;
	rayCpp::DrawRectangleLinesEx(topLeft, widthHeight, cellSize / 2, WHITE);
}

bool Board::CellExists(Vec2<int> pos) const
{
	assert(pos.GetX() >= 0 && pos.GetX() < width && pos.GetY() >= 0 && pos.GetY() < height);
	return cells[pos.GetY() * width + pos.GetX()].Exists();
}

void Board::SetCell(Vec2<int> pos, Color c)
{
	assert(pos.GetX() >= 0 && pos.GetX() < width && pos.GetY() >= 0 && pos.GetY() < height);
	cells[pos.GetY() * width + pos.GetX()].SetColor(c);
	cells[pos.GetY() * width + pos.GetX()].Add();
}

void Board::RemoveCell(Vec2<int> pos)
{
	assert(pos.GetX() >= 0 && pos.GetX() < width && pos.GetY() >= 0 && pos.GetY() < height);
	cells[pos.GetY() * width + pos.GetX()].Remove();
}

int Board::GetWidth() const
{
	return width;
}

int Board::GetHeight() const
{
	return height;
}

int Board::GetPadding() const
{
	return padding;
}

int Board::GetCellSize() const
{
	return cellSize;
}

Vec2<int> Board::GetScreenPosition() const
{
	return screenPos;
}

void Board::Reset()
{
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x) {
			RemoveCell(Vec2<int>(x, y));
		}
}
