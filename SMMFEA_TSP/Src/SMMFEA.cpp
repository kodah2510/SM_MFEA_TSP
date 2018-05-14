#include "stdafx.h"
#include "SMMFEA.h"

SMMFEA::SMMFEA()
{
}

SMMFEA::~SMMFEA()
{
}

void SMMFEA::init(size_t& m_pop_sz, size_t& m_gene_sz, GeneType& base_pert)
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

auto SMMFEA::gen_op(PopType & pop, const double & rmp, const double & mutation_rate, size_t max_size)
{
	std::uniform_real_distribution<> dis(0.0, 1.0);
	std::mt19937 gen;
	PopType ret;
	ret.reserve(max_size);
	while (ret.size() < max_size)
	{
		 //choose two random parent 
		int p1_idx = Ultility::get_rand_int(0, pop.size() - 1);
		int p2_idx = Ultility::get_rand_int(0, pop.size() - 1);

		 //assortative mating
		auto r = dis(gen);
		auto p1 = pop[p1_idx];
		auto p2 = pop[p2_idx];
		if (r < rmp || p1->skill_factor == p2->skill_factor)
		{
			//crossover + mutate
			auto os_pack = cross_op->crossover(pop[p1_idx], pop[p2_idx]);
			IdvSPtr c1{ os_pack[0] };
			IdvSPtr c2{ os_pack[1] };
			auto sf_rand = dis(gen);
			if (sf_rand < 0.5)
			{
				c1->skill_factor = p1->skill_factor;
				c2->skill_factor = p2->skill_factor;
			}
			else
			{
				c1->skill_factor = p2->skill_factor;
				c2->skill_factor = p1->skill_factor;
			}
			auto mut_rand = dis(gen);
			if (mut_rand < mutation_rate)
			{
				auto mutated_c1 = mutate_op->mutate(c1);
				auto mutated_c2 = mutate_op->mutate(c2);
				ret.emplace_back(mutated_c1);
				ret.emplace_back(mutated_c2);
			}
			else
			{
				ret.emplace_back(c1);
				ret.emplace_back(c2);
			}
		}
		else
		{
			auto c1 = mutate_op->mutate(pop[p1_idx]);
			auto c2 = mutate_op->mutate(pop[p2_idx]);

			ret.emplace_back(c1);
			ret.emplace_back(c2);
		}
	}
	return ret;
}

bool SMMFEA::is_dominate(IdvSPtr & a, IdvSPtr & b)
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
void SMMFEA::crowding_distance(std::vector<size_t>& front, PopType & pop)
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
auto SMMFEA::fast_non_dominated_sort(PopType & pop)
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
				/*std::cout << p->mo_task.val_pack[0] << " " << q->mo_task.val_pack[0] << "\n"
					<< p->mo_task.val_pack[1] << " " << q->mo_task.val_pack[1] << "\n";*/
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

	return fronts;
}
void SMMFEA::compute_mo_rank(PopType & pop)
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
			return (a->mo_task.cd > b->mo_task.cd);
		}
		else return false;
	});

	/*for (auto& idv : pop)
	{
		std::cout << idv->so_task.val << "\n";
	}*/

	for (size_t i = 0; i < pop.size(); i++)
	{
		pop[i]->mo_task.rank = i + 1;
	}
}

void SMMFEA::compute_so_rank(PopType & pop)
{
	std::sort(pop.begin(), pop.end(), [](IdvSPtr& a, IdvSPtr& b)
	{
		return a->so_task.val < b->so_task.val;
	});

	//assign rank 
	for (size_t i = 0; i < pop.size(); i++)
	{
		pop[i]->so_task.rank = i + 1; // avoid rank is 0
	}
}
void SMMFEA::compute_rank(PopType & pop)
{
	compute_so_rank(pop);
	/*for (auto& idv : pop)
	{
		std::cout << idv->so_task.rank << "\n";
	}
	std::cout << "\n";*/
	compute_mo_rank(pop);
	/*for (auto& idv : pop)
	{
		std::cout << idv->mo_task.rank << "\n";
	}*/
}
void SMMFEA::compute_scalar_fitness(PopType & pop)
{
	for (auto& idv : pop)
	{
		if (idv->so_task.rank < idv->mo_task.rank)
		{
			idv->skill_factor = SKILL_FACTOR::SO;
			idv->scalar_fitness = 1.0 / idv->so_task.rank;
		}
		else
		{
			idv->skill_factor = SKILL_FACTOR::MO;
			idv->scalar_fitness = 1.0 / idv->mo_task.rank;
		}
		//std::cout << idv->scalar_fitness << " ";
	}
}

void SMMFEA::select(PopType & inter_pop, PopType & cur_pop)
{
	std::sort(inter_pop.begin(), inter_pop.end(), [](IdvSPtr& a, IdvSPtr& b)
	{
		return a->scalar_fitness > b->scalar_fitness;
	});

	//select
	auto sz = cur_pop.size();
	cur_pop.clear();
	cur_pop.insert(cur_pop.begin(), inter_pop.begin(), inter_pop.begin() + sz);
}

void SMMFEA::run(DistMatType& dist_mat, IOHandler& io_handler)
{
	//output file setup
	auto problem_name = io_handler.get_problem_name();
	std::ofstream of_dv;
	of_dv.open(".\\Result\\SMMFEA\\" + name + "_" + problem_name + "_dv.txt");

	std::ofstream of_front;
	of_front.open(".\\Result\\SMMFEA\\" + name + "_" + problem_name + "_front.txt");

	//evaluating
	std::cout << "Evaluating...!\n";
	Evaluator evaluator;
	evaluator.eval(cur_pop, dist_mat, gene_sz, a, b);
	int gen_count = 0;

	compute_rank(cur_pop);
	compute_scalar_fitness(cur_pop);

	while (gen_count < number_of_gens)
	{
		// applying genetic operators
		PopType offspring = gen_op(cur_pop, rmp, mutation_rate, pop_sz / 2);

		//refinement
		//ls_op->run(offspring, dist_mat, gene_sz);

		//evaluate offspring
		evaluator.eval(offspring, dist_mat, gene_sz, a, b);

		//concatenate current pop with offspring
		PopType inter_pop;
		inter_pop.reserve(pop_sz + offspring.size());
		inter_pop.insert(	inter_pop.begin(),
							std::make_move_iterator(cur_pop.begin()), 
							std::make_move_iterator(cur_pop.end()));

		inter_pop.insert(	inter_pop.end(),
							std::make_move_iterator(offspring.begin()),
							std::make_move_iterator(offspring.end()));

		//print_file(inter_pop);

		// compute rank
		compute_rank(inter_pop);
		// assign scalar fitness
		compute_scalar_fitness(inter_pop);

	/*	for (auto& idv : inter_pop)
		{
			std::cout << idv->scalar_fitness << " ";
		}
		std::cout << "\n";*/
		// select for next generation
		select(inter_pop, cur_pop);

		/*for (auto& idv : cur_pop)
		{
			std::cout << idv->scalar_fitness << " ";
		}
		std::cout << "\n";*/

		std::cout << "generation " << gen_count << "\n";

		auto idv1 = cur_pop[0];
		auto idv2 = cur_pop[1];

		std::cout	<< "so: "			<< std::setw(7)		<< idv1->so_task.val << "\t" 
					<< "mo: "			<< std::setw(10)	<< idv1->mo_task.val_pack[0] << " " << idv1->mo_task.val_pack[1] << "\t" 
					<< "skill factor "	<< std::setw(2)		<< idv1->skill_factor << "\n";

		std::cout	<< "so: "			<< std::setw(7)		<< idv2->so_task.val << "\t"
					<< "mo: "			<< std::setw(10)	<< idv2->mo_task.val_pack[0] << " " << idv2->mo_task.val_pack[1] << "\t"
					<< "skill factor "	<< std::setw(2)		<< idv2->skill_factor << "\n";

		//for (auto& idv : cur_pop)
		//{
		//	std::cout << idv->so_task.val << " " << idv->skill_factor << " " << idv->scalar_fitness  << "\n";
		//}
		gen_count++;

		//record the dist_val and f1 f2 of the first fron
		of_dv << idv2->so_task.val  << "\n";
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

void SMMFEA::set_crossover_operator(CrossoverOperator* co)
{
	cross_op = co;
}

void SMMFEA::set_mutate_operator(MutationOperator* mo)
{
	mutate_op = mo;
}

void SMMFEA::set_localsearch_operator(LocalSearchOperator* lo)
{
	ls_op = lo;
}

void SMMFEA::init_population(GeneType & pert_arr, IdvSPtr & idv, std::mt19937 & gen)
{
	std::shuffle(pert_arr.begin(), pert_arr.end(), gen);
	for (auto& val : pert_arr)
	{
		idv->gene.emplace_back(val);
	}
}

void SMMFEA::select_two_cities(size_t& gene_sz)
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

