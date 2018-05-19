#pragma once

#include "Ultility\Alias.h"
#include "Ultility\IOHandler.h"

#include "Operators\CrossoverOperator.h"
#include "Operators\MutationOperator.h"
#include "Operators\LocalSearchOperator.h"

class Algorithm
{
public:
	Algorithm();
	~Algorithm();

	PopType		cur_pop;
	size_t		gene_sz;
	size_t		pop_sz;


	void set_crossover_operator(CrossoverOperator* cross_op);
	void set_mutate_operator(MutationOperator* mutate_op);
	void set_localsearch_operator(LocalSearchOperator* m_ls_op); 
	
	virtual void init(size_t& POP_SZ, size_t& gene_sz, GeneType& base_pert)	= 0;
	virtual void run(DistMatType& dist_mat, IOHandler& io_handler, int times)	= 0;
	
protected:
	CrossoverOperator*		cross_op;
	MutationOperator*		mutate_op;
	LocalSearchOperator*	ls_op;

};

