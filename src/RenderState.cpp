#include "RenderState.hpp"
#include "Context.hpp"
#include "entity/ComponentManager.hpp"
#include "objects/GameState.hpp"
#include "render/text.hpp"
#include "hud.hpp"
#include "gl/gl_1_5.h"
#include <boost/format.hpp>

void RenderState::clearBuffers() {
	for (SpriteBuffer& buffer : sprite_buffers) {
		buffer.clear();
	}
}

void RenderState::drawSprites() {
	const GameState& game_state = *context.game_state;

	for (const SpriteComponent& sprite : context.component_mgr->component_pool_SpriteComponent) {
		const PositionComponent* pos = findInChain<PositionComponent>(sprite);
		assert(pos);

		Sprite spr;
		spr.img = sprite.image;
		spr.pos = game_state.camera.transform(pos->position) - sprite.origin;
		sprite_buffers[sprite.layer].append(spr);
	}
}

void RenderState::drawTileLayers() {
	const GameState& game_state = *context.game_state;
	game_state.player_layer.draw(sprite_buffers[RenderState::LAYER_TILES_FG], game_state.camera);
}

void RenderState::drawFrametime() {
	static boost::format frametime_format("%6.3f");
	const GameState& game_state = *context.game_state;

	const std::string fps_text = "FPS: " + str(frametime_format % game_state.fps);
	const std::string min_text = "MIN: " + str(frametime_format % (game_state.frametime_min * 1000.0f));
	const std::string avg_text = "AVG: " + str(frametime_format % (game_state.frametime_avg * 1000.0f));
	const std::string max_text = "MAX: " + str(frametime_format % (game_state.frametime_max * 1000.0f));

	SpriteBuffer& ui_buffer = sprite_buffers[RenderState::LAYER_UI];
	drawString(WINDOW_WIDTH, 0*8, fps_text, ui_buffer, ui_font, TextAlignment::right, color_white);
	drawString(WINDOW_WIDTH, 1*8, min_text, ui_buffer, ui_font, TextAlignment::right, color_white);
	drawString(WINDOW_WIDTH, 2*8, avg_text, ui_buffer, ui_font, TextAlignment::right, color_white);
	drawString(WINDOW_WIDTH, 3*8, max_text, ui_buffer, ui_font, TextAlignment::right, color_white);
}

void RenderState::submitSprites() {
	glClear(GL_COLOR_BUFFER_BIT);
	for (const SpriteBuffer& buffer : sprite_buffers) {
		buffer.draw(sprite_buffer_indices);
	}
}