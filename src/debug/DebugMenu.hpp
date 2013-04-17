#pragma once

struct SpriteBuffer;
struct FontInfo;

void drawDebugMenu(SpriteBuffer& ui_buffer, const FontInfo& font_info);
void updateDebugMenu(const InputButtons& input);
