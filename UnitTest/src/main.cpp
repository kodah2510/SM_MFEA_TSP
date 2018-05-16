#include <iostream>
#include <vector>
#include "Ultility\Alias.h"
#include "Evaluator.h"
#include "Individual.h"
#include "Operators\CrossoverOperator.h"
#include "Operators\MutationOperator.h"

int main()
{
	Evaluator evaluator;
	std::vector<float> dist_mat = { 10, 10, 10, 10, 10, 
									10, 10, 10, 10, 10,
									10, 10, 10, 10, 10,
									10, 10, 10, 10, 10,
									10, 10, 10, 10, 10,};
	IdvSPtr test{ new Individual(5) };
	test->gene = { 3, 0, 1, 4, 2 };
	PopType pop;
	pop.emplace_back(test);

	int a = 0;
	int b = 4;



	evaluator.eval(pop, dist_mat, 5);
	std::cout << test->so_task.val << "\n";
	evaluator.eval(pop, dist_mat, test->gene_sz, a, b);
	std::cout << test->so_task.val << "\n";

	IdvSPtr p1{ new Individual(10) };
	IdvSPtr p2{ new Individual(10) };

	p1->gene = { 9, 8, 4, 5, 6, 7, 1, 3, 2, 0 };
	p2->gene = { 8, 7, 1, 2, 3, 0, 9, 5, 4, 6 };

	CrossoverOperator* crossover_op = new OrderCrossoverOperator();
	MutationOperator* mutate_op = new TworsMutationOperator();


	auto res = crossover_op->crossover(p1, p2);
	for (auto& idv : res)
	{
		auto gene = idv->gene;
		for (auto& g : gene)
		{
			std::cout << g << " ";
		}
		std::cout << "\n";
	}
	/*for (auto& i : p1->gene) std::cout << i << " ";
	std::cout << "\n";
	auto res = mutate_op->mutate(p1);
	for (auto& i : res->gene) std::cout << i << " ";*/
	
	std::cin.get();
	return 0;
}