#include "pch.h"
#include "Random.h"


namespace nu
{
	std::mt19937& Generator() {
		static std::random_device randomDevice;
		static std::mt19937 generator(randomDevice());

		return generator;
	}

	void SeedRandom(unsigned int seed) {
		Generator().seed(seed);
	}

	int RandomInt() {
		static std::uniform_int_distribution<> dist;
		return dist(Generator());
	}

	/// <summary>
	/// Returns a random integer [0, max)
	/// </summary>
	/// <param name="max">exclusive max value</param>
	/// <returns></returns>
	int RandomInt(int max) {
		std::uniform_int_distribution<> dist(0, max - 1);
		return dist(Generator());
	}

	/// <summary>
	/// Returns a random integer [min, max]
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	int RandomInt(int min, int max) {
		if (min > max) std::swap(min, max);
		std::uniform_int_distribution<> dist(min, max);
		return dist(Generator());
	}

	float RandomFloat() {
		static std::uniform_real_distribution<> distFloat(0.0f, 1.0f);
		return (float) distFloat(Generator());
	}

	float RandomFloat(float max) {
		std::uniform_real_distribution<> distFloat(0.0f, max);
		return (float) distFloat(Generator());
	}

	float RandomFloat(float min, float max) {
		if (min > max) std::swap(min, max);
		std::uniform_real_distribution<> distFloat(min, max);
		return (float) distFloat(Generator());
	}

	bool RandomBool() {
		static std::bernoulli_distribution distBool(0.5f);
		return distBool(Generator());
	}
}