#pragma once
#include <random>
#include "typeinfo.h"

class RandomGenerator
{
public:
	static f32 uniform_real(f32 a = 0.0f, f32 b = 1.0f)
	{
		return a + (b - a) * mUniform0_1Dist(mRandomGenerator);
	}

	static f32 signed_uniform_real(f32 a = -1.0f, f32 b = 1.0f)
	{
		return a + (b - a) * mUniform0_1Dist(mRandomGenerator);
	}

	static f32 normal(f32 mu = 0.0f, f32 sigma = 1.0f)
	{
		return mu + sigma * mNormalDist(mRandomGenerator);
	}

	static std::random_device rd;
	static std::mt19937 mRandomGenerator;
	static std::normal_distribution<f32> mNormalDist;
	static std::uniform_real_distribution<f32> mUniform0_1Dist;
};

inline std::random_device RandomGenerator::rd;
inline std::mt19937 RandomGenerator::mRandomGenerator(rd());
inline std::normal_distribution<float> RandomGenerator::mNormalDist(0.0f, 1.0f);
inline std::uniform_real_distribution<f32> RandomGenerator::mUniform0_1Dist(0.0f, 1.0f);