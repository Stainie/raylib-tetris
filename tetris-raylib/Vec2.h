#pragma once

template<typename T>
class Vec2
{
public:
	Vec2() = default;
	constexpr Vec2(T x, T y) : x(x), y(y) {}

	constexpr T GetX() const { return x; }
	constexpr T GetY() const { return y; }
	constexpr void SetX(T x) { this->x = x; }
	constexpr void SetY(T y) { this->y = y; }
public:
	constexpr bool operator==(const Vec2& other) const
	{
		return x == other.x && y == other.y;
	}
	constexpr Vec2 operator+(const Vec2& other) const
	{
		return Vec2(x + other.x, y + other.y);
	}
	constexpr Vec2 operator+(const int other) const
	{
		return Vec2(x + other, y + other);
	}
	constexpr Vec2& operator+=(const Vec2& other)
	{
		return *this = *this + other;
	}
	constexpr Vec2 operator-(const Vec2& other) const
	{
		return Vec2(x - other.x, y - other.y);
	}
	constexpr Vec2 operator-(const int other) const
	{
		return Vec2(x - other, y - other);
	}
	constexpr Vec2& operator-=(const Vec2& other)
	{
		return *this = *this - other;
	}
	constexpr Vec2 operator*(const Vec2& other) const
	{
		return Vec2(x * other.x, y * other.y);
	}
	constexpr Vec2 operator*(const int other) const
	{
		return Vec2(x * other, y * other);
	}

private:
	T x;
	T y;
};