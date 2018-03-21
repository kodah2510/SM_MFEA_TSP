#pragma once
#include <vector>
#include <memory>
using GeneType = std::vector<int>;
struct Individual
{
	size_t gene_sz;
	std::vector<int> gene;
};

struct Population
{
	std::vector<std::unique_ptr<Individual>> idv_arr;
};
struct DistanceMatrix
{
	std::vector<float> val_arr;
	size_t base;
};