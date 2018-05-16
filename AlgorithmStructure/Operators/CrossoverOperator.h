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

		/*int first_side = Ultility::get_rand_int(0, gene_sz - 2);
		int second_side = Ultility::get_rand_int(first_side + 1, gene_sz - 1);*/
		int first_side(0), second_side(0);
		set_side(first_side, second_side);

		proceed_crossover(p1->gene, p2->gene, first_side, second_side, c1, c2);

		os_pack.emplace_back(c1);
		os_pack.emplace_back(c2);
		return os_pack;
	};
	void set_side(int& first, int& second)
	{
		first = 2;
		second = 6;
	}
private:
	void proceed_crossover(GeneType p1_gene, GeneType p2_gene, // made a copy
		int first_side, int second_side,
		Individual* &op1, Individual* &op2) 
	{
		size_t gene_sz = p1_gene.size();
		//choose random substring
		op1->gene.resize(gene_sz);
		op2->gene.resize(gene_sz);

		GeneType p1_sub_gene;
		p1_sub_gene.reserve(gene_sz);
		for (auto i = first_side; i <= second_side; i++)
		{
			p1_sub_gene.emplace_back(p1_gene.at(i));
		}
		p1_sub_gene.shrink_to_fit();

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
		auto tmp_p1 = p1_gene;
		std::for_each(tmp_p1.begin(), tmp_p1.end(), [p2_sub_gene](auto& e)
		{
			for (auto& c : p2_sub_gene)
			{
				if (e == c) e = -1;
			}
		});
		slide_motion(tmp_p2, p1_sub_gene, op2->gene, first_side, second_side);
		slide_motion(tmp_p1, p2_sub_gene, op1->gene, first_side, second_side);

		// now it p1 turn 
	};
	void slide_motion(GeneType& p, GeneType& sub_gene, GeneType& op_gene, int first_side, int second_side)
	{
		GeneType stack;
		auto gene_sz = op_gene.size();
		stack.reserve(gene_sz);
		size_t iter = second_side + 1;
		size_t count(0);
		while (count != gene_sz)
		{
			int val(0);
			(iter >= gene_sz) ? val = p[iter - gene_sz] : val = p[iter];
			if (val != -1) stack.emplace_back(val);
			iter++;
			count++;
		}

		for (auto& i : sub_gene) stack.emplace_back(i);

		size_t idx = second_side + 1;
		for (size_t i = 0; i < stack.size(); i++)
		{
			if (idx >= gene_sz) op_gene[idx - gene_sz] = stack[i];
			else op_gene[idx] = stack[i];
			idx++;
		}

	}
};
