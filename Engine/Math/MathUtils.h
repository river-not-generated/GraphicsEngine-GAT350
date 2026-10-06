#pragma once

namespace nu::math
{
	constexpr float PI = 3.141592653589f;
	constexpr float HALF_PI = 3.141592653589f / 2.0f;
	constexpr float TWO_PI = 2.0f * PI;

	constexpr float RAD_TO_DEG = 180.0f / PI;
	constexpr float DEG_TO_RAD = PI / 180.0f;

	template<typename T>
	T Min(T a, T b) {
		return (a < b) ? a : b;
	}

	template<typename T>
	T Max(T a, T b) {
		return (a > b) ? a : b;
	}

	template<typename T>
	T Wrap(T min, T max, T value)
	{
		if (value > max) value = min;
		if (value < min) value = max;

		return value;
	}

	template<typename T>
	T Clamp(T min, T max, T value) {
		//return ((value < min) ? min : (value > max) ? max : value);

		if (value > max) value = max;
		if (value < min) value = min;

		return value;
	}
}