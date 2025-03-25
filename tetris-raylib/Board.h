#pragma once
#include "raylibCpp.h"
#include <vector>
#include "Vec2.h"

class Board
{
private:
	class Cell
	{
	public:
		Cell();
		void SetColor(Color c);
		void Add();
		void Remove();
		bool Exists() const;

		Color GetColor() const;
	private:
		bool bExists;
		Color c;
	};
public:
	Board(Vec2<int> screenPos, Vec2<int> widthHeight, int cellSize, int padding);
	void DrawCell(Vec2<int> pos) const;
	void DrawCell(Vec2<int> pos, Color color) const;
	void Draw() const;
	void Update();
	void DrawBorder() const;
	bool CellExists(Vec2<int> pos) const;
	bool IsTopRowOccupied() const;
	void SetCell(Vec2<int> pos, Color c);
	void RemoveCell(Vec2<int> pos);
	int GetWidth() const;
	int GetHeight() const;
	int GetPadding() const;
	int GetCellSize() const;
	Vec2<int> GetScreenPosition() const;

	void Reset();
private:
	std::vector<Cell> cells;
	const int width;
	const int height;
	const int cellSize;
	Vec2<int> screenPos;
	int padding;
};