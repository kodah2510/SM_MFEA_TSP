#pragma once
#include <vector>
#include <memory>
#include <random>

#include "../Individual.h"

using GeneType = std::vector<int>;
using DistMatType = std::vector<float>;
using PosType = GeneType::iterator;
using IdvSPtr = std::shared_ptr<Individual>;
using PopType = std::vector<IdvSPtr>;

namespace Ultility 
{
	inline int get_rand_int(const int& begin, const int& end)
	{
		std::random_device rd;
		std::uniform_int_distribution<> dis(begin, end);
		return dis(rd);
	}
}