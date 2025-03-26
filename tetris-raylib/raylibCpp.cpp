#include "raylibCpp.h"
#include "Settings.h"
#include <assert.h>

void rayCpp::DrawRectangle(Vec2<int> pos, Vec2<int> widthHeight, Color color)
{
	assert(pos.GetX() >= 0 && pos.GetY() >= 0 && pos.GetX() < GetScreenWidth() && pos.GetY() < GetScreenHeight());
	DrawRectangle(pos.GetX(), pos.GetY(), widthHeight.GetX(), widthHeight.GetY(), color);
}

void rayCpp::DrawRectangleLinesEx(Vec2<int> pos, Vec2<int> widthHeight, int lineThick, Color color)
{
	assert(pos.GetX() >= 0 && pos.GetY() >= 0 && pos.GetX() < GetScreenWidth() && pos.GetY() < GetScreenHeight() && lineThick > 0);
	DrawRectangleLinesEx({ (float)pos.GetX(), (float)pos.GetY(), (float)widthHeight.GetX(), (float)widthHeight.GetY() }, (float)lineThick, color);
}

bool rayCpp::IsKeyAction(int key)
{
	if (settings::currentMode == settings::PhysicsMode::Grid)
		return IsKeyPressed(key);
	else
		return IsKeyDown(key);
}
