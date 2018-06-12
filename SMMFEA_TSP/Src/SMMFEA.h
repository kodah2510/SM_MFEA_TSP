#pragma once
#include <vector>
#include <random>
#include <iostream>
#include <iomanip>

#include "Algorithm.h"
#include "Ultility\Alias.h"
#include "Evaluator.h"
#include "Ultility\IOHandler.h"
class SMMFEA : 
	public Algorithm
{
public:
	SMMFEA();

	~SMMFEA();

	const std::string name = "SMMFEA";

	//setting up algorithm's parameters
	const int number_of_gens = 100;

	const double rmp = 0.3;
	const double mutation_rate = 0.2;

	void init(size_t& POP_SZ, size_t& gene_sz, GeneType& base_pert);
	void run(DistMatType& dist_mat, IOHandler& io_handler, int times);

	void set_crossover_operator(CrossoverOperator* co);
	void set_mutate_operator(MutationOperator* mo);
	void set_localsearch_operator(LocalSearchOperator* lo);
private:

	int a, b; // two cities

	//init
	void init_population(GeneType& pert_arr, IdvSPtr& idv, std::mt19937& gen);
	void select_two_cities(size_t& gene_sz);
	//run
	auto gen_op(PopType& cur_pop, const double& rmp, const double& mutation_rate, size_t max_size);
	void compute_rank(PopType& inter_pop);
	void compute_scalar_fitness(PopType& inter_pop);
	void select(PopType& inter_pop, PopType& cur_pop);
	void compute_so_rank(PopType& pop);
	void compute_mo_rank(PopType& pop);
	//compute mo rank
	auto fast_non_dominated_sort(PopType& pop);
	void crowding_distance(std::vector<size_t>& front, PopType& pop);
	bool is_dominate(IdvSPtr& a, IdvSPtr& b);
	//additional improve
	void niche_enforcement(std::vector<size_t>& front, PopType& pop);

};
