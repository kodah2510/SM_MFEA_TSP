#include "Evaluator.h"
#include <iostream>

Evaluator::Evaluator(){}
Evaluator::~Evaluator(){}

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

void Evaluator::eval(PopType& cur_pop, DistMatType& dist_mat, size_t& gene_sz, int a, int b)
{
	/*std::ofstream output_file;
	output_file.open("eval.txt");*/
	
	for (auto& idv : cur_pop)
	{
		//skill factor missing 
		auto obj_pack = eval_mo(idv->gene, dist_mat, gene_sz, a, b);
		idv->mo_task.val_pack = obj_pack;
		idv->so_task.val = eval_so(obj_pack[0], obj_pack[1]);
		/*std::cout << "single: " << idv->so_task.val << "\t"
		<< "multi: f1 " << idv->mo_task.val_pack[0]
		<< " f2 " << idv->mo_task.val_pack[1] << "\n";*/
	}
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
	if (pos_a == gene.begin())
	{
		for (auto it = pos_b; it != gene.end() - 1; it++)
		{
			auto row = *it;
			auto col = *(it + 1);
			ret += dist_mat[row * base + col];
		}
	}
	else if(pos_b == gene.end())
	{
		for (auto it = gene.begin(); it != pos_a - 1; it++)
		{
			auto row = *it;
			auto col = *(it + 1);

			ret += dist_mat[row * base + col];
		}
	}
	else
	{
		for (auto it = gene.begin(); it != pos_a - 1; it++)
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
	}
	auto end = gene[gene.size() - 1];
	auto begin = gene[0];
	ret += dist_mat[end * base + begin];

	return ret;
}
// find the position of two cities a and b
std::vector<PosType> Evaluator::find_pos(GeneType & gene, int a, int b)
{
	auto pos_a = std::find(gene.begin(), gene.end(), a);
	auto pos_b = std::find(gene.begin(), gene.end(), b);

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

void Evaluator::eval(PopType & cur_pop, DistMatType & dist_mat, size_t gene_sz)
{
	for (auto& idv : cur_pop)
	{
		float res = 0;
		for (size_t i = 0; i < gene_sz - 1; i++)
		{
			auto gene = idv->gene;
			auto row = gene[i];
			auto col = gene[i + 1];
			res += dist_mat[row*gene_sz + col];
		}
		res += dist_mat[idv->gene[gene_sz - 1] * gene_sz + idv->gene[0]];
		idv->so_task.val = res;
	}
}
