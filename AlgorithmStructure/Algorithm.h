#pragma once

#include <vector>

#include "CrossoverOperator.h"
#include "MutationOperator.h"

class Algorithm
{
public:
	Algorithm(float m_mutation_rate, float m_crossover_rate);
	~Algorithm();

	// Algorithm parameters
	size_t pop_sz;
	float mutation_rate;
	float crossover_rate;
	int gen_number;
	

	CrossoverOperator cross_op;
	MutationOperator mutate_op;

	virtual void init() = 0;
	virtual void run() = 0;

};

