#include "Algorithm.h"



Algorithm::Algorithm()
{
}


Algorithm::~Algorithm()
{
}

void Algorithm::set_crossover_operator(CrossoverOperator * m_cross_op)
{
	cross_op = { m_cross_op };
}

void Algorithm::set_mutate_operator(MutationOperator * m_mutate_op)
{
	mutate_op = { m_mutate_op };
}

void Algorithm::set_localsearch_operator(LocalSearchOperator * m_ls_op)
{
	ls_op = { m_ls_op };
}
