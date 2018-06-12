#include "stdafx.h"
#include "SOEA.h"


SOEA::SOEA()
{
}


SOEA::~SOEA()
{
}

void SOEA::init(size_t & m_pop_sz, size_t & m_gene_sz, GeneType & base_pert)
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
}
auto SOEA::gen_op(PopType& pop, size_t max_size)
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
void SOEA::run(DistMatType& dist_mat, IOHandler& io_handler, int times)
{
	auto problem_name = io_handler.get_problem_name();
	std::ofstream of_dv;
	of_dv.open(".\\Result\\SOEA\\" + algorithm_name + "_" + problem_name + "_dv_"+std::to_string(times)+".txt");

	std::cout << "evaluating current pop...\n";
	Evaluator evaluator;
	evaluator.eval(cur_pop, dist_mat, gene_sz);
	int gen_count = 0;

	while (gen_count < number_of_gens)
	{
		//generating
		PopType offspring = gen_op(cur_pop, pop_sz / 2);
		//refinement
		ls_op->run(offspring, dist_mat, gene_sz);
		//eval
		evaluator.eval(offspring, dist_mat, gene_sz);

		//concatenate 
		PopType inter_pop;
		inter_pop.reserve(pop_sz + offspring.size());
		inter_pop.insert(inter_pop.begin(),
			std::make_move_iterator(cur_pop.begin()),
			std::make_move_iterator(cur_pop.end()));
		inter_pop.insert(inter_pop.end(),
			std::make_move_iterator(offspring.begin()),
			std::make_move_iterator(offspring.end()));

		//select for next gen
		select(inter_pop, cur_pop);

		auto idv = cur_pop[0];
		std::cout << "generation "	<< gen_count		<< "\n";
		std::cout << "dv: "			<< idv->so_task.val << "\n";

		of_dv << idv->so_task.val << "\n";

		gen_count++;
	}
	of_dv.close();
	std::cout << "done!\n";
}

void SOEA::init_population(GeneType & pert_arr, IdvSPtr & idv, std::mt19937 & gen)
{
	std::shuffle(pert_arr.begin(), pert_arr.end(), gen);
	for (auto& val : pert_arr)
	{
		idv->gene.emplace_back(val);
	}
}

void SOEA::select(PopType & inter_pop, PopType & cur_pop)
{
	std::sort(inter_pop.begin(), inter_pop.end(), [](IdvSPtr& a, IdvSPtr& b)
	{
		return a->so_task.val < b->so_task.val;
	});

	auto sz = cur_pop.size();
	cur_pop.clear();
	cur_pop.insert(cur_pop.begin(), inter_pop.begin(), inter_pop.begin() + sz);
}
