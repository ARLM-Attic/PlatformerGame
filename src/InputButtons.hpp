#pragma once
#include "util/vec2.hpp"
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

		EDITOR_TOGGLE,
		EDITOR_PICK_TILE,
		EDITOR_PLACE_TILE,
		EDITOR_CHOOSE_TILE,
		EDITOR_NEXT_LAYER,
		EDITOR_SCROLL_LEFT,
		EDITOR_SCROLL_RIGHT,
		EDITOR_SCROLL_UP,
		EDITOR_SCROLL_DOWN,

		MAX
	};

	typedef std::bitset<MAX> Bitset;

	Bitset pressed;
	Bitset held;
	Bitset released;

	ivec2 mouse_pos;
	void warpMouse(ivec2 new_pos);
};
