#pragma once
#include "../Ultility/Alias.h"
class LocalSearchOperator 
{
public:
	LocalSearchOperator() {};
	~LocalSearchOperator() {};

	virtual void run(PopType& os, DistMatType& dist_mat, size_t gene_sz) = 0;
};


class LS2optOperator :
	public LocalSearchOperator
{
public:
	LS2optOperator() {};
	~LS2optOperator() {};
	//ref from wikipedia 2-opt https://en.wikipedia.org/wiki/2-opt
	void run(PopType& os, DistMatType& dist_mat, size_t gene_sz)
	{
		for (auto& idv : os)
		{
			//std::cout << idv << "\n";
			size_t improve = 0;
			auto best_dist = cal_dist(idv->gene, dist_mat, gene_sz);
			while (improve < 50)
			{
				for (size_t i = 1; i < gene_sz - 1; i++)
				{
					for (size_t k = i + 1; k < gene_sz; k++)
					{
						auto new_route = opt_swap(idv->gene, i, k);
						auto new_dist = cal_dist(new_route, dist_mat, gene_sz);
						if (improve == 50) break;

						if (new_dist < best_dist)
						{
							//std::cout << "improved!\n";
							improve = 0;
							idv->gene = new_route;
							best_dist = new_dist;
						}
					}
				}
				improve++;
			}
		}
	}
private:
	float cal_dist(GeneType& route, DistMatType& dist_mat, size_t gene_sz) 
	{
		float res = 0;
		for (size_t i = 0; i < gene_sz - 1; i++)
		{
			auto row = route[i];
			auto col = route[i + 1];
			res += dist_mat[row*gene_sz + col];
		}
		res += dist_mat[route[gene_sz - 1] * gene_sz];
		return res;
	};
	GeneType opt_swap(GeneType& route, size_t i, size_t k)
	{
		GeneType res;
		res.reserve(route.size());
		for (size_t it = 0; it <= i - 1; it++)
		{
			res.emplace_back(route[it]);
		}
		for (size_t it = k; it >= i; it--)
		{
			res.emplace_back(route[it]);
		}
		for (size_t it = k + 1; it < route.size(); it++)
		{
			res.emplace_back(route[it]);
		}
		return res;
	};

};


