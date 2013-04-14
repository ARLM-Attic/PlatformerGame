#include "hud.hpp"
#include "objects/GameState.hpp"
#include "RenderState.hpp"
#include "render/text.hpp"

FontInfo ui_font(' ', 8, 8, 0, 0, 16, 6);
const Color hud_color = {49, 209, 17, 0};

void initUiFont() {
	ui_font = FontInfo(' ', 8, 8, 0, 0, 16, 6);
}
