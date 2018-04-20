#include <iostream>

#include "Individual.h"


Individual::Individual()
{
}

Individual::Individual(size_t m_gene_sz) : gene_sz(m_gene_sz)
{
	std::cout << "Individual constructed !\n";
	gene.reserve(m_gene_sz);

}


Individual::~Individual()
{
	gene.clear();
}

void Individual::init()
{

}