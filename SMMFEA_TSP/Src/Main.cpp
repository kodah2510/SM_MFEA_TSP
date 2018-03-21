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

std::random_device rd;
std::mt19937 gen(rd());

void split_token(std::string& line, std::vector<float>& values_pack)
{
	std::regex ws_re("\\s+");
	std::for_each(
		std::sregex_token_iterator(line.begin(), line.end(), ws_re, -1),
		std::sregex_token_iterator(),
		[&values_pack](const std::string& token)
		{
			try
			{
				values_pack.emplace_back(std::stof(token, nullptr));
			}
			catch (const std::invalid_argument)
			{
				std::cerr << "ignoring tab sequence\n" ;
			}
		}
	);
}


void init_population(std::vector<int>& pert_arr, Individual& idv, std::mt19937& gen )
{
	
	std::shuffle(pert_arr.begin(), pert_arr.end(), gen);
	for (auto& val : pert_arr)
	{
		idv.gene.emplace_back(val);
	}
	std::cout << "initialized idv\n";
	for (auto& val : idv.gene)
	{
		std::cout << val << " ";
	}
	std::cout << "\n";
}

using GeneType = std::vector<int>;
using DistMatType = std::vector<float>;
using PosType = GeneType::iterator;
auto find_pos(GeneType& gene, int a, int b)
{
	auto pos_a = std::find(gene.begin(), gene.end(), a);
	auto pos_b = std::find(gene.begin(), gene.end(), b);
	//for (auto& v : gene)
	//{
	//	std::cout << v << " ";
	//}
	//std::cout << "\n";
	
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
struct TaskSOO
{
	auto eval(std::array<float, 2>& f)
	{
		return f[0] + f[1];
	}
};
struct TaskMOO
{
	//find f1 and f2
	auto eval(GeneType& gene, DistMatType& dist_mat, size_t base, int a, int b)
	{	
		auto pos_pack = find_pos(gene, a, b);

		auto f1_val = f1(gene, dist_mat, base, pos_pack[0], pos_pack[1]);
		auto f2_val = f2(gene, dist_mat, base, pos_pack[0], pos_pack[1]);

		std::array<decltype(f1_val), 2> ret = { f1_val, f2_val };
		return ret;
	}
	float f1(GeneType& gene, DistMatType& dist_mat, size_t base, PosType& pos_a, PosType& pos_b)
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
	float f2(GeneType& gene, DistMatType& dist_mat, size_t base, PosType& pos_a, PosType& pos_b)
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
};
Individual& pick_rand_parent(std::vector<Individual>& pop, std::mt19937& gen)
{
	std::uniform_int_distribution<> dis(0, pop.size());
	auto r = dis(gen);
	return pop[r];
}
int get_rand_int(const int& begin, const int& end)
{
	std::uniform_int_distribution<> dis(begin, end);
	return dis(rd);
}
void proceed_crossover(GeneType& p1_gene, GeneType& p2_gene, int first_side, int second_side, 
	Individual& op1, Individual& op2)
{
	size_t gene_sz = p1_gene.size();
	//choose random substring
	

	GeneType sub_gene;
	sub_gene.reserve(gene_sz);
	std::copy(&p1_gene[first_side], &p1_gene[second_side], sub_gene);
	sub_gene.shrink_to_fit();

	// delete cities in p2 
	// mark those are duplicated  by -1
	GeneType p2_sub;
	p2_sub.reserve(sub_gene.size());

	std::copy(&p2_gene[first_side], &p2_gene[second_side], p2_sub);

	std::for_each(p2_gene.begin(), p2_gene.end(), [sub_gene](auto& e)
	{
		for (auto& c : sub_gene)
		{
			if (e == c) e = -1;
		}
	});

	// slide motion
	// need a stack 
	/*
	Example:
	8 * 1 '2 3 0' 9 * 4 *
	9 4 8 1
	*/
	std::vector<int> stack;
	for (auto it = p2_gene.begin() + second_side; it != p2_gene.end(); it++)
	{
		stack.emplace_back(*it);
	}

	for (auto it = p2_gene.begin(); it != p2_gene.begin() + first_side; it++)
	{
		stack.emplace_back(*it);
	}

	for (auto it = sub_gene.rbegin(); it != sub_gene.rend(); it++)
	{
		stack.emplace_back(*it);
	}

	for (auto it = p2_sub.rbegin(); it != p2_sub.rend(); it++)
	{
		stack.emplace_back(*it);
	}

	std::copy(stack.rbegin(), stack.rend(), op2.gene.begin());

	// now it p1 turn 

	stack.empty();
	for (auto it = p1_gene.begin() + second_side; it != p1_gene.end(); it++)
	{
		stack.emplace_back(*it);
	}

	for (auto it = p1_gene.begin(); it != p1_gene.begin() + first_side; it++)
	{
		stack.emplace_back(*it);
	}

	for (auto it = p2_sub.rbegin(); it != p2_sub.rend(); it++)
	{
		stack.emplace_back(*it);
	}

	for (auto it = sub_gene.rbegin(); it != sub_gene.rend(); it++)
	{
		stack.emplace_back(*it);
	}
	
	std::copy(stack.rbegin(), stack.rend(), op1.gene.begin());
}
auto crossover(Individual& p1, Individual& p2)
{
	size_t gene_sz = p1.gene_sz;
	std::array<Individual, 2> offspring_pack;
	Individual c1{ gene_sz };
	Individual c2{ gene_sz };

	int first_side = get_rand_int(0, gene_sz);
	int second_side = get_rand_int(first_side, gene_sz);

	proceed_crossover(p1.gene, p2.gene, first_side, second_side, c1, c2);

	offspring_pack[0] = c1;
	offspring_pack[1] = c2;
	return offspring_pack;

}
//random swap mutate
auto mutate(Individual& idv)
{
	//pick two random pos
	size_t sz = idv.gene_sz;
	int pos1 = get_rand_int(0, sz);
	int pos2 = get_rand_int(0, sz);

	while (pos2 == pos1) pos2 = get_rand_int(0, sz);
	
	int a = idv.gene.at(pos1);
	int b = idv.gene.at(pos2);
	//swap 
	a += b;
	b = a - b;
	a = a - b;

	idv.gene[pos1] = a;
	idv.gene[pos2] = b;
}

std::vector<Individual> gen_op(std::vector<Individual>& pop, 
		const float& rmp, const float& mutation_rate, 
		std::mt19937& gen, size_t max_size)
{
	
	std::uniform_real_distribution<> dis(0.0, 1.0);

	auto r = dis(gen);
	std::vector<Individual> ret;
	ret.reserve(max_size);
	while (ret.size() < max_size)
	{
		// choose two random parent 
		auto p1 = pick_rand_parent(pop, gen);
		auto p2 = pick_rand_parent(pop, gen);
		// assortative mating
		auto r = dis(gen);
		if (r < rmp)
		{
			// crossover + mutate
			auto os_pack = crossover(p1, p2);
			for (auto& idv : os_pack)
			{
				mutate(idv);
			}
		}
		else
		{
			// mutate
			mutate(p1);
			mutate(p2);
		}
	}

	return ret;
}


void eval(std::vector<Individual>& pop)
{

}

void compute_rank(std::vector<Individual>& pop)
{

}
void compute_scalar_fitness(std::vector<Individual>& pop)
{

}
void select()
{

}
int main(int argv, char** args)
{
	//read input
	const std::string filename = "Input\\att48_d.txt";
	std::ifstream file(filename);
	std::string line;

	std::vector<float> dist_mat{};
	size_t gene_sz;
	if (file.is_open())
	{
		//assign number_of_cities 
		std::string first_line;
		std::getline(file, first_line);
		/*
		// split string using string.find() and string.substr()

		const std::string delimiter = "\x09";
		
		auto start = 0U;
		auto end = firstLine.find(delimiter);
		std::string token;
		try
		{
			while (end != std::string::npos)
			{
				token = firstLine.substr(start, end - start);
				start = end + delimiter.length();
				end = firstLine.find(delimiter, start);
				std::cout << token << "\n";
				firstLineInput.emplace_back(std::stof(token, nullptr));
			}
		}
		catch (const std::invalid_argument& is)
		{
			std::cerr << "Invalid argument: " << is.what() << "\n";
		}*/
		std::vector<float> tmp_pack;
		tmp_pack.reserve(1000);
		split_token(first_line, tmp_pack);
	
		for (auto& val : tmp_pack)
		{
			std::cout << val << "\n";
		}

		//finishing read the first line 
		//initialize the distance matrix
		gene_sz = tmp_pack.size();
		dist_mat.reserve(gene_sz * gene_sz);

		for (auto& val : tmp_pack)
		{
			dist_mat.emplace_back(val);
		}

		while (std::getline(file, line))
		{
			split_token(line, dist_mat);
		}

		std::cout << "dist_mat\n";
		for (auto& val : dist_mat)
		{
			std::cout << val << "\n";
		}
	}
	else
	{
		std::cerr << "Cannot read input file\n";
	}
	//done with the input
	// Individual initializing 
	Individual idv(gene_sz);

	

	const size_t POP_SZ = 100;
	//std::array<Individual, POP_SZ> population;
	std::vector<int> base_pert(gene_sz);

	//std::generate(base_pert.begin(), base_pert.end(), [n = 0]() mutable { n++; });
	std::iota(base_pert.begin(), base_pert.end(), 0);

	// construct individuals array
	Individual pop[POP_SZ];
	for (auto i = 0; i < POP_SZ; i++)
	{
		pop[i] = Individual(gene_sz);
	}

	
	for (auto& i : base_pert)
	{
		std::cout << i << " ";
	}
	
	std::cout << "done with the input\n";
	// initialize individuals 

	for (auto& idv : pop)
	{
		init_population(base_pert,idv, gen);
		//std::cout << idv.id << "\n";
	}

	std::cout << "done with initializing\n";

	// select two cities a and b

	std::uniform_int_distribution<int> dist(0, gene_sz);
	int a = dist(rd);
	int b = dist(rd);
	while (a == b)
	{
		b = dist(rd);
	}

	std::cout << "two selected cities: " << a << " " << b << "\n";

//td::array<float, POP_SZ> idv_dist_val;


	////test evaluation function for SOO
	//auto val = eval_soo(pop[0].gene, dist_mat, gene_sz);
	//std::cout << val << "\n";


	//evaluate for each task

	std::cout << "Evaluating...!\n";

	TaskMOO task_moo;
	TaskSOO task_soo;

	for (auto& idv : pop)
	{
		auto obj_pack = task_moo.eval(idv.gene, dist_mat, gene_sz, a, b);
		idv.mo_task.val_pack = obj_pack;
		idv.so_task.val = task_soo.eval(obj_pack);
		std::cout	<< "single: " << idv.so_task.val << "\t" 
					<< "multi: f1 "  << idv.mo_task.val_pack[0] << " f2 " << idv.mo_task.val_pack[1] << "\n";
	}



	const int number_of_gens = 100;
	int gen_count = 0;

	const float rmp = 0.3;
	const float mutation_rate = 0.2;

	
	// into the loop
	while (true)
	{
		std::vector<Individual> v_pop;
		v_pop.data = pop;
		auto offspring = gen_op(v_pop, rmp, mutation_rate, gen, POP_SZ / 2);
		eval(offspring);
		std::vector<Individual> inter_pop;
		inter_pop.reserve(POP_SZ + offspring.size());
		std::copy(&pop[0], &pop[POP_SZ - 1], inter_pop);
		inter_pop.insert(inter_pop.end(), offspring.begin(), offspring.end());

		compute_rank(inter_pop);
		compute_scalar_fitness(inter_pop);
		select();

	}

	std::cin.get();
    return 0;
}

