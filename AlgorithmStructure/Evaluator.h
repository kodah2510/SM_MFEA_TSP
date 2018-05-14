#pragma once
#include <vector>
#include <array>
#include <memory.h>

#include "Ultility\Alias.h"

#include "Individual.h"


class Evaluator
{
public:
	Evaluator();
	~Evaluator();

	
	inline float eval_so(float f1, float f2);

	auto eval_mo(GeneType& gene, DistMatType& dist_mat, size_t base, int a, int b);

	float f1(GeneType& gene, DistMatType& dist_mat, size_t base, PosType& pos_a, PosType& pos_b);

	float f2(GeneType& gene, DistMatType& dist_mat, size_t base, PosType& pos_a, PosType& pos_b);

	std::vector<PosType> find_pos(GeneType& gene, int a, int b);

	void eval(PopType& cur_pop, DistMatType& dist_mat, size_t& gene_sz, int a, int b);

	void eval(PopType& cur, DistMatType& dist_mat, size_t gene_sz);
};

