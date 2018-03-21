#pragma once
#include "Algorithm.h"
#include "CrossoverOperator.h"
#include "MutationOperator.h"
#include <vector>
using IdType = std::size_t;
class SMMFEA : 
	public Algorithm
{
public:
	SMMFEA(float m_mutate_rate, float m_crossover_rate);

	~SMMFEA();

	
	

	void init();
	void run();
	


};
