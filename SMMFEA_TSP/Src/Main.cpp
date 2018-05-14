// SMMFEATSP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <array>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <regex>
#include <algorithm>
#include <numeric>
#include <random>
#include <memory>

#include "Individual.h"
#include "Ultility\IOHandler.h"
#include "Evaluator.h"
#include "Operators\CrossoverOperator.h"
#include "Operators\MutationOperator.h"
#include "Operators\LocalSearchOperator.h"


#include "SMMFEA.h"
#include "SOEA.h"
#include "MOEA.h"

int main(int argv, char** args)
{
	//read input
	std::vector<float> dist_mat;
	size_t gene_sz = 0;
	const std::string folder_name = "Input\\";
	std::vector<std::string> input_files = {
		/*"eil51.txt",
		"bier127.txt",
		"ch130.txt",
		"kroA150.txt",
		"kroA200.txt",
		"kroB150.txt",
		"kroB200.txt",
		"lin105.txt",
		"pr76.txt",
		"pr107.txt",
		"pr136.txt",
		"pr144.txt",
		"pr152.txt",
		"pr226.txt",
		"pr264.txt",
		"pr299.txt",*/
		"rat195.txt"
	};
	for (auto& input_file : input_files)
	{
		IOHandler io_handler;
		dist_mat = io_handler.read_mat(folder_name + input_file, gene_sz);
		//done with the input


		// Individual initializing
		size_t pop_sz = 100;
		std::vector<int> base_pert(gene_sz);
		std::iota(base_pert.begin(), base_pert.end(), 0);

		std::cout << "SMMFEA\n";
		SMMFEA smmfea;
		smmfea.set_crossover_operator(new OrderCrossoverOperator());
		smmfea.set_mutate_operator(new TworsMutationOperator());
		smmfea.set_localsearch_operator(new LS2optOperator());

		smmfea.init(pop_sz, gene_sz, base_pert);
		smmfea.run(dist_mat, io_handler);

		std::cout << "SOEA\n";
		SOEA soea;
		soea.set_crossover_operator(new OrderCrossoverOperator());
		soea.set_mutate_operator(new TworsMutationOperator());
		soea.set_localsearch_operator(new LS2optOperator());

		soea.init(pop_sz, gene_sz, base_pert);
		soea.run(dist_mat, io_handler);

		std::cout << "MOEA\n";
		MOEA moea;
		moea.set_crossover_operator(new OrderCrossoverOperator());
		moea.set_mutate_operator(new TworsMutationOperator());
		moea.set_localsearch_operator(new LS2optOperator());

		moea.init(pop_sz, gene_sz, base_pert);
		moea.run(dist_mat, io_handler);

	}
	
	
	std::cin.get();
    return 0;
}

