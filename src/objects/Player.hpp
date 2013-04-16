#pragma once

#include "util/Position.hpp"
#include "render/Sprite.hpp"
#include "game_types.hpp"

struct GameState;
struct SpriteDb;
struct SpriteBuffer;
struct Camera;

struct Player {
	Position pos;
	bool on_ground;
	ivec2 size;

	int facing_direction;
	float move_velocity;
	float jump_velocity;

	IntRect image;

	void init(const SpriteDb& sprite_db);
	void update(GameState& game_state, const InputButtons::Bitset& input);
	void draw(SpriteBuffer& buffer, const Camera& camera) const;
};
