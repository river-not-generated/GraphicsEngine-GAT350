#pragma once

#include <cmath>
#include <cassert>

namespace nu
{
	struct Vector2 {
		float x, y;

		Vector2() = default;
		Vector2(float x, float y) : x{ x }, y{ y } {};
		Vector2(int x, int y) : x{ (float) x }, y{ (float) y } {}
		Vector2(unsigned int x, unsigned int y) : x{ (float) x }, y{ (float) y } {}
		explicit Vector2(float v) : x{ v }, y{ v } {};


		float operator [] (unsigned int i) const {
			// halt the program if i isn't less than 2
			assert(i < 2);
			return (&x)[i];
		}

		float& operator [] (unsigned int i) {
			assert(i < 2);
			return (&x)[i];
		}

		bool operator == (const Vector2& v) const {
			return (this->x == v.x && this->y == v.y);
		}

		bool operator != (const Vector2& v) const {
			return !(*this == v);
		}


		// OPERATIONS WITH OTHER VECTORS
		Vector2 operator + (const Vector2& v) const {
			return Vector2{ this->x + v.x, this->y + v.y };
		}
		Vector2 operator - (const Vector2& v) const {
			return Vector2{ this->x - v.x, this->y - v.y };
		}
		Vector2 operator * (const Vector2& v) const {
			return Vector2{ this->x * v.x, this->y * v.y };
		}
		Vector2 operator / (const Vector2& v) const {
			return Vector2{ this->x / v.x, this->y / v.y };
		}

		Vector2 operator += (const Vector2& v) {
			this->x += v.x;
			this->y += v.y;
			return *this;
		}
		Vector2 operator -= (const Vector2& v) {
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		}
		Vector2 operator *= (const Vector2& v) {
			this->x *= v.x;
			this->y *= v.y;
			return *this;
		}
		Vector2 operator /= (const Vector2& v) {
			this->x /= v.x;
			this->y /= v.y;
			return *this;
		}


		// OPERATIONS WITH FLOATS
		Vector2 operator + (float num) const {
			return Vector2{ this->x + num, this->y + num };
		}
		Vector2 operator - (float num) const {
			return Vector2{ this->x - num, this->y - num };
		}
		Vector2 operator * (float num) const {
			return Vector2{ this->x * num, this->y * num };
		}
		Vector2 operator / (float num) const {
			return Vector2{ this->x / num, this->y / num };
		}

		Vector2 operator += (float num) {
			this->x += num;
			this->y += num;
			return *this;
		}
		Vector2 operator -= (float num) {
			this->x -= num;
			this->y -= num;
			return *this;
		}
		Vector2 operator *= (float num) {
			this->x *= num;
			this->y *= num;
			return *this;
		}
		Vector2 operator /= (float num) {
			this->x /= num;
			this->y /= num;
			return *this;
		}



		// a^2 + b^2 = c^2
		float LengthSqr() const {
			return pow(x, 2.0f) + pow(y, 2.0f);
		}

		// returns c
		float Length() const {
			return std::sqrt(LengthSqr());
		}

		float Distance(const Vector2& v) const { return (v - *this).Length(); }

		// convert to unit vector
		Vector2 Normalized() const {
			return *this / Length();
		}

		// dot product, returns the cosine between two unit vectors
		float Dot(const Vector2& v) {
			return ((this->x * v.x) + (this->y * v.y));
		}

		// returns the angle of the vector relative to the origin
		float Angle() const {
			return std::atan2(this->y, this->x);
		}

		// returns the angle between two vectors in degrees
		float AngleBetween(const Vector2& v) {
			return std::acos(Dot(v));
		}

		Vector2 Rotate(float radians) {
			float x = this->x * std::cos(radians) - this->y * std::sin(radians);
			float y = this->x * std::sin(radians) + this->y * std::cos(radians);

			return { x, y };
		}
	};
}