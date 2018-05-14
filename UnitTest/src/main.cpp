#include <iostream>
#include <vector>
#include "Ultility\Alias.h"
#include "Evaluator.h"
#include "Individual.h"

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
	std::cin.get();
	return 0;
}