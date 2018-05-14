#pragma once
#include "../Ultility/Alias.h"
#include "../Individual.h"
class CrossoverOperator
{
public:
	CrossoverOperator() {};
	~CrossoverOperator() {};

	virtual std::vector<Individual*> crossover(IdvSPtr& idv1, IdvSPtr& idv2) = 0;
};

class OrderCrossoverOperator :
	public CrossoverOperator
{
public:
	OrderCrossoverOperator() {};
	~OrderCrossoverOperator() {};

	std::vector<Individual*> crossover(IdvSPtr& p1, IdvSPtr& p2)
	{
		size_t gene_sz = p1->gene_sz;
		std::vector<Individual*> os_pack;
		os_pack.reserve(2);
		Individual* c1 = new Individual(gene_sz);
		Individual* c2 = new Individual(gene_sz);

		int first_side = Ultility::get_rand_int(0, gene_sz - 2);
		int second_side = Ultility::get_rand_int(first_side + 1, gene_sz - 1);

		proceed_crossover(p1->gene, p2->gene, first_side, second_side, c1, c2);

		os_pack.emplace_back(c1);
		os_pack.emplace_back(c2);
		return os_pack;
	};
private:
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

		//std::cout << "p1_sub_gene: ";
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
		std::vector<int> stack;
		stack.reserve(gene_sz);
		for (size_t it = second_side; it < p2_gene.size(); it++)
		{
			int val = tmp_p2[it];
			if (val != -1)
			{
				stack.emplace_back(val);
			}
		}
		//std::cout << "\n";
		for (auto it = 0; it < second_side; it++)
		{
			int val = tmp_p2[it];
			if (val != -1)
			{
				//std::cout << val << " ";
				stack.emplace_back(val);
			}
		}
		//std::cout << "\n";
		for (auto it = p1_sub_gene.rbegin(); it != p1_sub_gene.rend(); it++)
		{
			if (*it != -1)
			{
				//std::cout << *it << " ";
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
		for (size_t i = second_side; i < gene_sz; i++)
		{
			int val = tmp_p1[i];
			if (val != -1) stack.emplace_back(val);
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
	};
};

