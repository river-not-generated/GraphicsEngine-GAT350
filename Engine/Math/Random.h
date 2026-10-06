#pragma once

#include <cstdlib>
#include <random>

namespace nu
{
	std::mt19937& Generator();
	void SeedRandom(unsigned int seed);
	int RandomInt();
	int RandomInt(int max);
	int RandomInt(int min, int max);
	float RandomFloat();
	float RandomFloat(float max);
	float RandomFloat(float min, float max);
	bool RandomBool();
}