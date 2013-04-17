#include "DebugConstant.hpp"
#include "objects/Camera.hpp"
#include "render/SpriteBuffer.hpp"
#include "render/text.hpp"
#include "game_types.hpp"

DebugConstantBase* debug_constant_list_head = nullptr;
DebugConstantBase* debug_constant_list_tail = nullptr;

static DebugConstantBase* selected_entry = nullptr;
static bool menu_enabled = false;

std::string toUpper(std::string str) {
	for (char& c : str) {
		c = ::toupper(c);
	}
	return str;
}

void drawDebugMenu(SpriteBuffer& ui_buffer, const FontInfo& font_info) {
	if (!menu_enabled)
		return;

	int y = 0;

	for (DebugConstantBase* p = debug_constant_list_head; p != nullptr; p = p->next) {
		char scroll_char = ' ';
		if (p == debug_constant_list_head && p->next == nullptr) {
			scroll_char = 0x79;
		} else if (y == 0) {
			if (p == debug_constant_list_head) {
				scroll_char = 0x77;
			} else {
				scroll_char = 0x78;
			}
		} else if (p->next == nullptr || y == WINDOW_HEIGHT / font_info.char_h) {
			if (p->next == nullptr) {
				scroll_char = 0x75;
			} else {
				scroll_char = 0x76;
			}
		}

		char cursor_char = selected_entry == p ? 0x6C : ' ';

		std::string entry_text = std::string(1, scroll_char) + std::string(1, cursor_char) + toUpper(std::string(p->name)) + ": " + p->toString_();
		drawString(0, y*font_info.char_h, entry_text, ui_buffer, font_info, TextAlignment::left, color_white);
		y++;
	}
}

void updateDebugMenu(const InputButtons& input) {
	if (input.pressed[InputButtons::DEBUG_ENTER]) {
		menu_enabled = !menu_enabled;
	}

	if (debug_constant_list_head == debug_constant_list_tail)
		menu_enabled = false;
	if (!menu_enabled)
		return;

	if (selected_entry == nullptr) {
		selected_entry = debug_constant_list_head;
	}

	if (input.pressed[InputButtons::DEBUG_UP]) {
		selected_entry = selected_entry->prev;
		if (selected_entry == nullptr) {
			selected_entry = debug_constant_list_tail;
		}
	} else if (input.pressed[InputButtons::DEBUG_DOWN]) {
		selected_entry = selected_entry->next;
		if (selected_entry == nullptr) {
			selected_entry = debug_constant_list_head;
		}
	}

	if (input.held[InputButtons::DEBUG_LEFT]) {
		selected_entry->decrement_();
	}
	if (input.held[InputButtons::DEBUG_RIGHT]) {
		selected_entry->increment_();
	}
}
