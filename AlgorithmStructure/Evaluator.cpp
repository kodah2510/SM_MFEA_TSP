#include "Evaluator.h"
#include <iostream>

Evaluator::Evaluator()
{
}


Evaluator::~Evaluator()
{
}

inline float Evaluator::eval_so(float f1, float f2)
{
	return f1 + f2;
}

auto Evaluator::eval_mo(GeneType & gene, DistMatType & dist_mat, size_t base, int a, int b)
{
	auto pos_pack = find_pos(gene, a, b);

	auto f1_val = f1(gene, dist_mat, base, pos_pack[0], pos_pack[1]);
	auto f2_val = f2(gene, dist_mat, base, pos_pack[0], pos_pack[1]);
	std::array<decltype(f1_val), 2> ret = { f1_val, f2_val };
	return ret;
}

float Evaluator::f1(GeneType & gene, DistMatType & dist_mat, size_t base, PosType & pos_a, PosType & pos_b)
{
	auto  ret = 0.0f;
	for (auto it = pos_a; it != pos_b - 1; it++)
	{
		auto row = *it;
		auto col = *(it + 1);

		ret += dist_mat[row * base + col];
	}
	return ret;
}

float Evaluator::f2(GeneType & gene, DistMatType & dist_mat, size_t base, PosType & pos_a, PosType & pos_b)
{
	auto ret = 0.0f;
	for (auto it = gene.begin(); it != pos_a; it++)
	{
		auto row = *it;
		auto col = *(it + 1);

		ret += dist_mat[row * base + col];
	}

	for (auto it = pos_b; it != gene.end() - 1; it++)
	{
		auto row = *it;
		auto col = *(it + 1);
		ret += dist_mat[row * base + col];
	}
	return ret;
}

std::vector<PosType> Evaluator::find_pos(GeneType & gene, int a, int b)
{
	auto pos_a = std::find(gene.begin(), gene.end(), a);
	auto pos_b = std::find(gene.begin(), gene.end(), b);
	for (auto& v : gene)
	{
		std::cout << v << " ";
	}
	std::cout << "\n";

	std::vector<decltype(pos_a)> pos_pack;
	pos_pack.reserve(2);

	if (pos_a < pos_b)
	{
		pos_pack.emplace_back(pos_a);
		pos_pack.emplace_back(pos_b);
	}
	else
	{
		pos_pack.emplace_back(pos_b);
		pos_pack.emplace_back(pos_a);
	}

	return pos_pack;
}
