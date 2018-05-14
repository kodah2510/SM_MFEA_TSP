#pragma once
#include "../Ultility/Alias.h"
#include "../Individual.h"

#include <iostream>
class MutationOperator
{
public:
	MutationOperator() {};
	~MutationOperator() {};

	virtual IdvSPtr mutate(IdvSPtr& idv) = 0;
};

class TworsMutationOperator :
	public MutationOperator
{
public:
	TworsMutationOperator() {};
	~TworsMutationOperator() {};
	
	IdvSPtr mutate(IdvSPtr& idv) 
	{
		
		IdvSPtr new_idv{ new Individual(idv->gene_sz) };
		new_idv->gene = idv->gene;
		//pick two random pos
		size_t sz = idv->gene_sz;
		int pos1 = Ultility::get_rand_int(0, sz - 1);
		int pos2 = Ultility::get_rand_int(0, sz - 1);

		while (pos2 == pos1) pos2 = Ultility::get_rand_int(0, sz - 1);

		int a = new_idv->gene.at(pos1);
		int b = new_idv->gene.at(pos2);
		//swap 
		a += b;
		b = a - b;
		a = a - b;

		new_idv->gene[pos1] = a;
		new_idv->gene[pos2] = b;
		new_idv->skill_factor = idv->skill_factor;
		return new_idv;
	};
};

