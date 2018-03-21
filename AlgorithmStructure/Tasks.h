#pragma once
#include <array>

struct SOTask
{
	float val;
};

template<size_t num_obj>
struct MOTask
{
	std::array<float, num_obj> val_pack;
};