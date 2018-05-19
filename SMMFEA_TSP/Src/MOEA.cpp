#include "stdafx.h"
#include "MOEA.h"


MOEA::MOEA()
{
}


MOEA::~MOEA()
{
}

void MOEA::init(size_t & m_pop_sz, size_t & m_gene_sz, GeneType & base_pert)
{
	pop_sz = m_pop_sz;
	gene_sz = m_gene_sz;
	cur_pop.reserve(pop_sz);
	for (size_t i = 0; i < pop_sz; i++)
	{
		Individual* idv(new Individual(gene_sz));
		std::unique_ptr<Individual> idv_u_ptr{ idv };
		cur_pop.emplace_back(std::move(idv_u_ptr));
		//cur_pop.emplace_back(new Individual(gene_sz));
	}

	std::random_device rd;
	std::mt19937 gen(rd());

	for (auto& idv : cur_pop)
	{
		init_population(base_pert, idv, gen);
	}

	std::cout << "done with initializing\n";

	// select two cities a and b
	select_two_cities(gene_sz);
}
auto MOEA::gen_op(PopType & pop, size_t max_size)
{
	std::uniform_real_distribution<> dis(0.0, 1.0);
	std::mt19937 gen;
	auto r = dis(gen);
	PopType ret;
	ret.reserve(max_size);

	while (ret.size() < max_size)
	{
		int p1_idx = Ultility::get_rand_int(0, pop.size() - 1);
		int p2_idx = Ultility::get_rand_int(0, pop.size() - 1);
		//crossover
		auto os_pack = cross_op->crossover(pop[p1_idx], pop[p2_idx]);
		IdvSPtr c1{ os_pack[0] };
		IdvSPtr c2{ os_pack[1] };

		auto r = dis(gen);
		if (r < mutation_rate)
		{
			auto mutate_c1 = mutate_op->mutate(c1);
			ret.emplace_back(mutate_c1);
			ret.emplace_back(c2);
			continue;
		}
		r = dis(gen);
		if (r < mutation_rate)
		{
			auto mutate_c2 = mutate_op->mutate(c2);
			ret.emplace_back(mutate_c2);
			ret.emplace_back(c1);
			continue;
		}
		ret.emplace_back(c1);
		ret.emplace_back(c2);
	}
	return ret;
}

void MOEA::run(DistMatType & dist_mat, IOHandler & io_handler, int times)
{
	auto problem_name = io_handler.get_problem_name();
	std::ofstream of_dv;
	of_dv.open(	".\\Result\\MOEA\\" + algorithm_name + "_" + problem_name + "_dv_" + std::to_string(times) + ".txt");

	std::ofstream of_front;
	of_front.open(	".\\Result\\MOEA\\" + algorithm_name + "_" + problem_name + "_front_" +std::to_string(times) +".txt");

	Evaluator evaluator;
	evaluator.eval(cur_pop, dist_mat, gene_sz, a, b);
	int gen_count = 0;

	
	while (gen_count < number_of_gens)
	{
		//generating
		PopType offspring = gen_op(cur_pop, pop_sz / 2);
		//refinement
		//ls_op->run(offspring, dist_mat, gene_sz);
		//eval
		evaluator.eval(offspring, dist_mat, gene_sz, a, b);

		//concat
		PopType inter_pop;
		inter_pop.reserve(pop_sz + offspring.size());
		inter_pop.insert(inter_pop.begin(),
			std::make_move_iterator(cur_pop.begin()),
			std::make_move_iterator(cur_pop.end()));
		inter_pop.insert(inter_pop.end(),
			std::make_move_iterator(offspring.begin()),
			std::make_move_iterator(offspring.end()));

		crowding_sort(inter_pop);
		select(inter_pop, cur_pop);
		
		std::cout << "generation " << gen_count << "\n";
		gen_count++;

		auto	idv = cur_pop[0];
		double	min = INFINITY;
		for (auto& idv : cur_pop)
		{
			if (idv->mo_task.nf == 0)
			{
				/*auto f1 = idv->mo_task.val_pack[0];
				auto f2 = idv->mo_task.val_pack[0];*/
			/*	of_dv << idv->mo_task.val_pack[0] << " " << idv->mo_task.val_pack[1] << "\n";*/
				auto cur_val = idv->so_task.val;
				if (cur_val < min)
				{
					min = idv->so_task.val;
				}
			}
		}
		of_dv << min << "\n";
		std::cout << min << "\n";
	}

	for (auto& idv : cur_pop)
	{
		if (idv->mo_task.nf == 0)
		{
			of_front << idv->mo_task.val_pack[0] << " " << idv->mo_task.val_pack[1] << "\n";
		}
	}
	of_dv.close();
	of_front.close();
}

void MOEA::init_population(GeneType & pert_arr, IdvSPtr & idv, std::mt19937 & gen)
{
	std::shuffle(pert_arr.begin(), pert_arr.end(), gen);
	for (auto& val : pert_arr)
	{
		idv->gene.emplace_back(val);
	}
}

void MOEA::select_two_cities(size_t & gene_sz)
{
	std::random_device rd;
	std::uniform_int_distribution<int> dist(0, gene_sz - 1);
	a = dist(rd);
	b = dist(rd);
	while (a == b)
	{
		b = dist(rd);
	}
	std::cout << "two selected cities: " << a << " " << b << "\n";
}

auto MOEA::fast_non_dominated_sort(PopType & pop)
{
	std::vector<std::vector<size_t>> S;
	S.resize(pop.size());
	std::vector<int> n;
	n.resize(pop.size());

	std::vector<std::vector<size_t>> fronts;
	fronts.resize(5);
	for (auto& f : fronts)
	{
		f.reserve(pop.size());
	}

	for (size_t i = 0; i < pop.size() - 1; i++)
	{
		//np is already initialized
		auto p = pop[i];
		S[i].reserve(pop.size());
		for (size_t j = 0; j < pop.size(); j++)
		{
			if (j == i) continue;
			auto q = pop[j];
			if (is_dominate(p, q)) // if p dominate q
			{
				//Sp[i].emplace_back(q);
				S[i].emplace_back(j);
			}
			else if (is_dominate(q, p))
			{
				n[i]++;
			}
		}
		//std::cout << n[i] << " ";
		if (n[i] == 0)
		{
			p->mo_task.nf = 0;
			fronts[0].emplace_back(i);
		}
		S[i].shrink_to_fit();
	}

	auto fc = 0; // front_counter
	while (!fronts[fc].empty())
	{
		std::vector<size_t> Q;
		Q.reserve(pop.size());
		for (auto& p : fronts[fc]) // p is index
		{
			for (auto& q : S[p]) // q is index
			{
				auto nq = n[q];
				nq--;
				if (nq == 0)
				{
					pop[q]->mo_task.nf++;
					Q.emplace_back(q);
				}
			}
		}
		fc++;
		for (auto& q : Q)
		{
			fronts[fc].emplace_back(q);
		}
	}
	for (size_t i = 0; i < pop.size(); i++)
	{
		if (pop[i]->mo_task.nf != 0) continue;
		else
		{
			if (std::find(fronts[0].begin(), fronts[0].end(), i) == std::end(fronts[0]))
			{
				pop[i]->mo_task.nf = 100;
			}
		}
	}
	return fronts;
}
void MOEA::crowding_sort(PopType & pop)
{
	auto fronts = fast_non_dominated_sort(pop);
	for (auto& f : fronts)
	{
		if (f.size() != 0)
		{
			crowding_distance(f, pop);
			/*std::cout << "out cd \n";
			for (auto& idx : f)
			{
			std::cout <<  pop[idx]->mo_task.cd << "\n";
			}*/
		}
	}

	/*for (auto& idv : pop)
	{
	std::cout << "mo: " << "\t" << idv->mo_task.nf << "\t " << idv->mo_task.cd << "\n";
	}*/

	std::sort(pop.begin(), pop.end(), [](IdvSPtr& a, IdvSPtr& b)
	{
		if (a->mo_task.nf < b->mo_task.nf)
		{
			return true;
		}
		else if (a->mo_task.nf == b->mo_task.nf)
		{
			if (a->mo_task.cd > b->mo_task.cd)
				return true;
			else return false;
		}
		else return false;
	});
}



void MOEA::crowding_distance(std::vector<size_t>& front, PopType & pop)
{
	size_t sz = front.size();

	size_t num_obj = pop[0]->mo_task.val_pack.size();

	for (size_t i = 0; i < num_obj; i++)
	{
		std::sort(front.begin(), front.end(), [i, pop](auto& a, auto& b)
		{
			return pop[a]->mo_task.val_pack[i] < pop[b]->mo_task.val_pack[i];
		});

		double f_max = pop.at(front[sz - 1])->mo_task.val_pack[i];
		double f_min = pop.at(front[0])->mo_task.val_pack[i];
		/*std::cout << "f_max: " << f_max << "\tf_min: " << f_min << "\n";
		std::cout << "val_pack \n";
		for(auto& idv: front)
		{
		std::cout << pop.at(idv)->mo_task.val_pack[i] << "\n";
		}*/
		//std::cout << "in the cd function\n";
		pop[front[0]]->mo_task.cd = pop[front[sz - 1]]->mo_task.cd = INFINITY;
		for (size_t j = 1; j < sz - 1; j++)
		{
			pop[front[j]]->mo_task.cd += (pop[front[j + 1]]->mo_task.val_pack[i] - pop[front[j - 1]]->mo_task.val_pack[i]) / (f_max - f_min);
			//std::cout << pop[front[j]]->mo_task.cd << "\n";
		}
	}
}

bool MOEA::is_dominate(IdvSPtr & a, IdvSPtr & b)
{
	bool state = false;

	for (size_t i = 0; i < a->mo_task.val_pack.size(); i++)
	{
		auto fa = a->mo_task.val_pack[i];
		auto fb = b->mo_task.val_pack[i];
		//std::cout << fa << " " << fb << "\n";
		if (fa < fb)
		{
			state = true;
		}
		else if (fa > fb)
		{
			return false;
		}
	}
	return state;
}

void MOEA::select(PopType & inter_pop, PopType & cur_pop)
{
	auto sz = cur_pop.size();
	cur_pop.clear();
	cur_pop.insert(cur_pop.begin(), inter_pop.begin(), inter_pop.begin() + sz);
}
