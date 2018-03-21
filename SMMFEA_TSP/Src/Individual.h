#pragma once
#include <vector>
#include <array>
#include "Tasks.h"

class Individual
{
public:
	Individual();
	Individual(size_t m_gene_sz);
	~Individual();
	// variables first , functions after

	size_t gene_sz;
	std::vector<int> gene;
	SOTask so_task;
	MOTask<2> mo_task; 

	void init();
	//void add_task(Task& m_task);
};


