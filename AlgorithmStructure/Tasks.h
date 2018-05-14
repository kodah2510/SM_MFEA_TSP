#pragma once
#include <array>
#include <memory>

#include "Individual.h"


struct SOTask
{
	float val = INFINITY;
	int rank;
};

template<size_t num_obj>
struct MOTask
{
	std::array<float, num_obj> val_pack;
	int rank;
	int nf;
	double cd = 0;
};