#pragma once
#include <random>
struct RandomGenerator
{
	static std::random_device rd;
	std::mt19937 gen;

	template<typename T>
	static T get_value(const T&& begin, const T&& end)
	{
		std::uniform_real_distribution<T> dis(begin, end);
		return static_cast<T>(dis(rd));
	}
};
