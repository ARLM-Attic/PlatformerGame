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

		DEBUG_UP,
		DEBUG_DOWN,
		DEBUG_LEFT,
		DEBUG_RIGHT,
		DEBUG_ENTER,
		MAX
	};

	typedef std::bitset<MAX> Bitset;

	Bitset pressed;
	Bitset held;
	Bitset released;
};
