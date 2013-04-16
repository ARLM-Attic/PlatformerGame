#pragma once
#include "util/Fixed.hpp"
#include <bitset>

struct InputButtons {
	enum Enum {
		LEFT,
		RIGHT,
		UP,
		DOWN,
		SHOOT,
		MAX
	};

	typedef std::bitset<MAX> Bitset;

	Bitset pressed;
	Bitset held;
	Bitset released;
};
