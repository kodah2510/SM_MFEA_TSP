#pragma once
#include "Algorithm.h"
#include "Ultility\Alias.h"
#include "Operators\CrossoverOperator.h"
#include "Operators\MutationOperator.h"
#include "Operators\LocalSearchOperator.h"

#include "Evaluator.h"

class SOEA : public Algorithm
{
public:
	SOEA();
	~SOEA();


	const int number_of_gens = 100;
	const std::string algorithm_name = "SOEA";

	const double mutation_rate = 0.2;
	

	void init(size_t& pop_sz, size_t& gene_sz, GeneType& base_pert);
	void run(DistMatType& dist_mat, IOHandler& io_handler, int times);
private:
	void init_population(GeneType& pert_arr, IdvSPtr& idv, std::mt19937& gen);
	auto gen_op(PopType& pop, size_t max_size);
	void select(PopType& inter_pop, PopType& cur_pop);
};

