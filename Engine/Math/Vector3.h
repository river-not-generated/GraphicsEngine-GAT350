#pragma once

#include <cmath>
#include <cassert>

namespace nu
{
	struct Vector3 {
		union {
			struct { float x, y, z; };
			// RGB shares the same space in memory as XYZ
			struct { float r, g, b; };
		};

		Vector3() = default;
		Vector3(float x, float y, float z) : x{ x }, y{ y }, z{ z } {};
		explicit Vector3(float v) : x{ v }, y{ v }, z{ v } {};


		float operator [] (unsigned int i) const {
			assert(i < 3);
			return (&x)[i];
		}

		float& operator [] (unsigned int i) {
			assert(i < 3);
			return (&x)[i];
		}

		bool operator == (const Vector3& v) const {
			return (this->x == v.x
				&& this->y == v.y
				&& this->z == v.z);
		}

		bool operator != (const Vector3& v) const {
			return !(*this == v);
		}


		// OPERATIONS WITH OTHER VECTORS
		Vector3 operator + (const Vector3& v) const {
			return Vector3{ this->x + v.x, this->y + v.y, this->z + v.z };
		}
		Vector3 operator - (const Vector3& v) const {
			return Vector3{ this->x - v.x, this->y - v.y, this->z - v.z };
		}
		Vector3 operator * (const Vector3& v) const {
			return Vector3{ this->x * v.x, this->y * v.y, this->z * v.z };
		}
		Vector3 operator / (const Vector3& v) const {
			return Vector3{ this->x / v.x, this->y / v.y, this->z / v.z };
		}

		Vector3 operator += (const Vector3& v) {
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}
		Vector3 operator -= (const Vector3& v) {
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}
		Vector3 operator *= (const Vector3& v) {
			this->x *= v.x;
			this->y *= v.y;
			this->z *= v.z;
			return *this;
		}
		Vector3 operator /= (const Vector3& v) {
			this->x /= v.x;
			this->y /= v.y;
			this->z /= v.z;
			return *this;
		}


		// OPERATIONS WITH FLOATS
		Vector3 operator + (float num) const {
			return Vector3{ this->x + num, this->y + num, this->z + num };
		}
		Vector3 operator - (float num) const {
			return Vector3{ this->x - num, this->y - num, this->z - num };
		}
		Vector3 operator * (float num) const {
			return Vector3{ this->x * num, this->y * num, this->z * num };
		}
		Vector3 operator / (float num) const {
			return Vector3{ this->x / num, this->y / num, this->z / num };
		}

		Vector3 operator += (float num) {
			this->x += num;
			this->y += num;
			this->z += num;
			return *this;
		}
		Vector3 operator -= (float num) {
			this->x -= num;
			this->y -= num;
			this->z -= num;
			return *this;
		}
		Vector3 operator *= (float num) {
			this->x *= num;
			this->y *= num;
			this->z *= num;
			return *this;
		}
		Vector3 operator /= (float num) {
			this->x /= num;
			this->y /= num;
			this->z /= num;
			return *this;
		}


		float LengthSqr() const {
			return pow(x, 2.0f) + pow(y, 2.0f) + pow(z, 2.0f);
		}

		float Length() const {
			return std::sqrt(LengthSqr());
		}

		// convert to unit vector
		Vector3 Normalized() const {
			return *this / Length();
		}

		// dot product, returns the cosine between two unit vectors
		float Dot(const Vector3& v) {
			return ((this->x * v.x) + (this->y * v.y) + (this->z * v.z));
		}
	};

	// alias for the class (Colour and Vector3 are the same thing)
	using Colour = Vector3;
}