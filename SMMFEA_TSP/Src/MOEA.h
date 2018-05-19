#pragma once
#include "Algorithm.h"
#include "Ultility\IOHandler.h"
#include "Evaluator.h"

class MOEA : public Algorithm
{
public:
	MOEA();
	~MOEA();

	const std::string algorithm_name = "MOEA";
	const int number_of_gens = 100;
	const double mutation_rate = 0.2;

	void init(size_t& pop_sz, size_t& gene_sz, GeneType& base_pert);
	void run(DistMatType& dist_mat, IOHandler& io_handler, int times);
private:
	int a, b; // two cities

	void init_population(GeneType& pert_arr, IdvSPtr& idv, std::mt19937& gen);
	void select_two_cities(size_t& gene_sz);

	auto gen_op(PopType& cur_pop, size_t max_size);

	void crowding_sort(PopType& pop);
	auto fast_non_dominated_sort(PopType& pop);
	void crowding_distance(std::vector<size_t>& front, PopType& pop);
	bool is_dominate(IdvSPtr& a, IdvSPtr& b);

	void select(PopType& inter_pop, PopType& cur_pop);
};

