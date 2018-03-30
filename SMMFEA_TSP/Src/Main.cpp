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


using GeneType = std::vector<int>;
using DistMatType = std::vector<float>;
using PosType = GeneType::iterator;

using PopType = std::vector<std::unique_ptr<Individual>>;

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

using IdvUPtr = std::unique_ptr<Individual>;
void init_population(std::vector<int>& pert_arr, std::unique_ptr<Individual>& idv, std::mt19937& gen )
{
	
	std::shuffle(pert_arr.begin(), pert_arr.end(), gen);
	for (auto& val : pert_arr)
	{
		idv->gene.emplace_back(val);
	}
	std::cout << "initialized idv\n";
	/*for (auto& val : idv->gene)
	{
		std::cout << val << " ";
	}
	std::cout << "\n";*/
}


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

int get_rand_int(const int& begin, const int& end)
{
	std::uniform_int_distribution<> dis(begin, end);
	return dis(rd);
}

void proceed_crossover(GeneType p1_gene, GeneType p2_gene, // made a copy
	int first_side, int second_side, 
	Individual* &op1, Individual* &op2)
{
	size_t gene_sz = p1_gene.size();
	//choose random substring
	
	GeneType p1_sub_gene;
	p1_sub_gene.reserve(gene_sz);
	for (auto i = first_side; i <= second_side; i++)
	{
		p1_sub_gene.emplace_back(p1_gene.at(i));
	}
	
	p1_sub_gene.shrink_to_fit();

	std::cout << "p1_sub_gene: ";
	/*for (auto& e : p1_sub_gene)
	{
		std::cout << e << " ";
	}
	std::cout << "\n";*/

	// delete cities in p2 
	// mark those are duplicated  by -1
	GeneType p2_sub_gene;
	p2_sub_gene.reserve(p1_sub_gene.size());

	for (auto i = first_side; i <= second_side; i++)
	{
		p2_sub_gene.emplace_back(p2_gene[i]);
	}
	auto tmp_p2 = p2_gene;
	std::for_each(tmp_p2.begin(), tmp_p2.end(), [p1_sub_gene](auto& e)
	{
		for (auto& c : p1_sub_gene)
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
	/*std::cout << "p2_gene: ";
	for (auto& e : p2_gene) std::cout << e << " ";
	std::cout << "\n";
*/
	std::vector<int> stack;
	stack.reserve(gene_sz);
	//std::cout << "second_side " << p2_gene[second_side] << " " << "end " << p2_gene[p2_gene.size()-1] << "\n";
	for (auto it = second_side; it < p2_gene.size(); it++)
	{
		int val = tmp_p2[it];
		if (val != -1)
		{
			std::cout << val << " ";
			stack.emplace_back(val);
		}
	}
	//std::cout << "\n";
	for (auto it = 0; it < second_side; it++)
	{
		int val = tmp_p2[it];
		if (val != -1)
		{
			std::cout << val << " ";
			stack.emplace_back(val);
		}
	}
	//std::cout << "\n";
	for (auto it = p1_sub_gene.rbegin(); it != p1_sub_gene.rend(); it++)
	{
		if (*it != -1)
		{
			std::cout << *it << " ";
			stack.emplace_back(*it);
		}
	}
	//std::copy(stack.rbegin(), stack.rend(), op2->gene.begin()); --> cannot copy because the gene is empty
	for (auto it = stack.rbegin(); it != stack.rend(); it++)
	{
		op2->gene.emplace_back(*it);
	}
	// now it p1 turn 

	stack.erase(stack.begin(), stack.end());
	auto tmp_p1 = p1_gene;
	std::for_each(tmp_p1.begin(), tmp_p1.end(), [p2_sub_gene](auto& e)
	{
		for (auto& c : p2_sub_gene)
		{
			if (e == c) e = -1;
		}
	});
	for (auto i = second_side; i < gene_sz; i++)
	{
		int val = tmp_p1[i];
		if(val != -1) stack.emplace_back(val);
	}
	for (auto i = 0; i < second_side; i++)
	{
		int val = tmp_p1[i];
		if (val != -1) stack.emplace_back(val);
	}

	for (auto it = p2_sub_gene.rbegin(); it != p2_sub_gene.rend(); it++)
	{
		stack.emplace_back(*it);
	}

	for (auto it = stack.rbegin(); it != stack.rend(); it++)
	{
		op1->gene.emplace_back(*it);
	}
}


auto crossover(IdvUPtr& p1, IdvUPtr& p2)
{
	size_t gene_sz = p1->gene_sz;
	std::vector<Individual*> os_pack;
	os_pack.reserve(2);
	Individual* c1 = new Individual(gene_sz);
	Individual* c2 = new Individual(gene_sz);

	int first_side = get_rand_int(0, gene_sz - 2);
	int second_side = get_rand_int(first_side + 1, gene_sz - 1);

	proceed_crossover(p1->gene, p2->gene, first_side, second_side, c1, c2);

	os_pack.emplace_back(c1);
	os_pack.emplace_back(c2);
	return os_pack;

}
//random swap mutate
void mutate(IdvUPtr& idv)
{
	try
	{
		//pick two random pos
		size_t sz = idv->gene_sz;
		int pos1 = get_rand_int(0, sz);
		int pos2 = get_rand_int(0, sz);

		while (pos2 == pos1) pos2 = get_rand_int(0, sz);

		int a = idv->gene.at(pos1);
		int b = idv->gene.at(pos2);
		//swap 
		a += b;
		b = a - b;
		a = a - b;

		idv->gene[pos1] = a;
		idv->gene[pos2] = b;
	}
	catch (std::out_of_range& oor)
	{
		std::cerr << oor.what() << "\n";
	}
	
}

auto gen_op(PopType& pop, const double& rmp, const double& mutation_rate,
		std::mt19937& gen, size_t max_size)
{
	
	std::uniform_real_distribution<> dis(0.0, 1.0);

	auto r = dis(gen);
	PopType ret;
	ret.reserve(max_size);
	while (ret.size() < max_size)
	{
		// choose two random parent 
		int p1_idx = get_rand_int(0, pop.size() - 1);
		int p2_idx = get_rand_int(0, pop.size() - 1);

		
		// assortative mating
		auto r = dis(gen);
		if (r < rmp)
		{
			//crossover + mutate
			auto os_pack = crossover(pop[p1_idx], pop[p2_idx]);
			IdvUPtr c1{ os_pack[0] };
			IdvUPtr c2{ os_pack[1] };
			mutate(c1);
			mutate(c2);
			ret.emplace_back(std::move(c1));
			ret.emplace_back(std::move(c2));
		}
		else
		{
			// mutate
			mutate(pop[p1_idx]);
			mutate(pop[p2_idx]);
		}
	}

	return ret;
}

void eval(PopType& pop, TaskSOO task_soo, TaskMOO task_moo, 
	DistMatType& dist_mat, size_t gene_sz, 
	int a, int b) // two cities
{
	try
	{
		for (auto& idv : pop)
		{
			auto obj_pack = task_moo.eval(idv->gene, dist_mat, gene_sz, a, b);
			idv->mo_task.val_pack = obj_pack;
			idv->so_task.val = task_soo.eval(obj_pack);
			std::cout << "single: " << idv->so_task.val << "\t"
				<< "multi: f1 " << idv->mo_task.val_pack[0]
				<< " f2 " << idv->mo_task.val_pack[1] << "\n";
		}
	}
	catch (std::out_of_range or)
	{
		std::cerr << or.what() << "\n";
	}
	
}

void compute_rank(PopType& pop)
{

}
void compute_scalar_fitness(PopType& pop)
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

	const size_t POP_SZ = 100;
	//std::array<Individual, POP_SZ> population;
	std::vector<int> base_pert(gene_sz);

	//std::generate(base_pert.begin(), base_pert.end(), [n = 0]() mutable { n++; });
	std::iota(base_pert.begin(), base_pert.end(), 0);

	// construct individuals array
	PopType cur_pop;
	cur_pop.reserve(POP_SZ);
	for (auto i = 0; i < POP_SZ; i++)
	{
		Individual* idv(new Individual(gene_sz));
		std::unique_ptr<Individual> idv_u_ptr{ idv };
		cur_pop.emplace_back(std::move(idv_u_ptr));
		//cur_pop.emplace_back(new Individual(gene_sz));
	}

	std::cout << "done with the input\n";
	// initialize individuals 

	for (auto& idv : cur_pop)
	{
		init_population(base_pert, idv, gen);
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

	//evaluate for each task

	std::cout << "Evaluating...!\n";

	TaskMOO task_moo;
	TaskSOO task_soo;

	eval(cur_pop, task_soo, task_moo, dist_mat, gene_sz, a, b);

	const int number_of_gens = 100;
	int gen_count = 0;

	const double rmp = 0.3;
	const double mutation_rate = 0.2;

	
	// into the loop
	while (true)
	{
		
		// applying genetic operators
		auto offspring = gen_op(cur_pop, rmp, mutation_rate, gen, POP_SZ / 2);
		//evaluate offspring
		eval(offspring, task_soo, task_moo, dist_mat, gene_sz, a, b);
		//concatenate current pop with offspring
		PopType inter_pop;
		inter_pop.reserve(POP_SZ + offspring.size());
		inter_pop.insert(inter_pop.begin(),
			std::make_move_iterator(cur_pop.begin()),
			std::make_move_iterator(cur_pop.end()));
		inter_pop.insert(inter_pop.end(),
			std::make_move_iterator(offspring.begin()),
			std::make_move_iterator(offspring.end()));
		//inter_pop.insert(inter_pop.end(), offspring.begin(), offspring.end());
		// compute rank
		compute_rank(inter_pop);
		// assign scalar fitness
		compute_scalar_fitness(inter_pop);
		// select for next generation
		select();

	}

	std::cin.get();
    return 0;
}

