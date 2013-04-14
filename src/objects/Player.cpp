#include "Player.hpp"

#include "RenderState.hpp"
#include "render/SpriteDb.hpp"
#include "Camera.hpp"

void Player::init(const SpriteDb& sprite_db) {
	image = sprite_db.lookup("player_stand");
}

void Player::update(GameState& game_state, const InputButtons::Bitset& input) {
	static const int movement_speed = 4;

	if (input.at(InputButtons::LEFT)) {
		pos.x -= 2;
	}
	if (input.at(InputButtons::RIGHT)) {
		pos.x += 2;
	}
	if (input.at(InputButtons::UP)) {
		pos.y -= 2;
	}
	if (input.at(InputButtons::DOWN)) {
		pos.y += 2;
	}
}

void Player::draw(SpriteBuffer& buffer, const Camera& camera) const {
	Sprite spr;
	spr.img = image;
	spr.setPos(camera.transform(pos));
	buffer.append(spr);
}