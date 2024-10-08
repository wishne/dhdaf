#include "includes.h"

std::string wstring_to_string(const std::wstring& wstr) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
}

void Slider::draw() {
	Rect area{ m_parent->GetElementsRect() };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };

	Color color1 = Color(17, 17, 17);
	Color color2 = Color(17, 17, 17);
	if (g_gui.m_open) {
		color1.a() = 50;
		color2.a() = 255;
	}
	else {
		color1.a() = 0;
		color2.a() = 0;
	}

	// get gui color.
	Color color = g_gui.m_color;
	color.a() = m_parent->m_alpha;

	// convert
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(m_precision) << m_value << wstring_to_string(m_suffix);
	std::string combined_label = m_label + " (" + ss.str() + ") ";

	// draw label with value.
	if (m_use_label)
		render::menu_shade.string(p.x, p.y - 2, { 205, 205, 205, m_parent->m_alpha }, combined_label.c_str());

	// outline.
	render::rect(p.x, p.y + m_offset, m_w - SLIDER_X_OFFSET, SLIDER_HEIGHT, { 18, 18, 18, m_parent->m_alpha });
	render::rect(p.x + 1, p.y + m_offset + 1, m_w - SLIDER_X_OFFSET - 2, SLIDER_HEIGHT - 2, { 26, 26, 26, m_parent->m_alpha });

	// background.
	Color wtf = color;
	wtf.r() = std::clamp(wtf.r() * .8, 0.0, 255.0);
	wtf.g() = std::clamp(wtf.g() * .8, 0.0, 255.0);
	wtf.b() = std::clamp(wtf.b() * .8, 0.0, 255.0);

	// bar.
	render::rect_filled(p.x + 2, p.y + m_offset + 2, std::max(m_fill - 2, 1) - 2, SLIDER_HEIGHT - 4, color);

}

void Slider::think() {
	Rect  area{ m_parent->GetElementsRect() };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };

	// how many steps do we have?
	float steps = (m_max - m_min) / m_step;

	// compute the amount of pixels for one step.
	float pixels = (m_w - SLIDER_X_OFFSET) / steps;

	// clamp the current value.
	math::clamp(m_value, m_min, m_max);

	// compute the fill ratio.
	m_fill = (int)std::floor(std::max(((m_value - m_min) / m_step) * pixels, 0.f));

	// we are draggin this mofo!
	if (m_drag) {
		// left mouse is still down.
		if (g_input.GetKeyState(VK_LBUTTON)) {

			// compute the new value.
			float updated = m_min + (((g_input.m_mouse.x - SLIDER_X_OFFSET) - p.x) / pixels * m_step);

			// set updated value to closest step.
			float remainder = std::fmod(updated, m_step);

			if (remainder > (m_step / 2))
				updated += m_step - remainder;

			else
				updated -= remainder;

			m_value = updated;

			// clamp the value.
			math::clamp(m_value, m_min, m_max);

			if (m_callback)
				m_callback();
		}

		// left mouse has been released.
		else
			m_drag = false;
	}
}

void Slider::click() {
	Rect  area{ m_parent->GetElementsRect() };
	Point p{ area.x + m_pos.x, area.y + m_pos.y };

	// get slider area.
	Rect slider{ p.x + SLIDER_X_OFFSET, p.y + m_offset, m_w - SLIDER_X_OFFSET, SLIDER_HEIGHT };

	// detect dragging.
	if (g_input.IsCursorInRect(slider))
		m_drag = true;

	// get incrementor area.
	//Rect increment{ l.x + m_w - SLIDER_OFFSET + 1, l.y + SLIDER_OFFSET_Y + 1, 6, 6 };

	// get decrementor area.
	//Rect decrement{ l.x - 6, l.y + SLIDER_OFFSET_Y + 1, 6, 6 };

	// increment value.
	//else if( g_input.is_mouse_in_area( increment ) )
	//	m_value += m_step;

	// decrement value.
	//else if( g_input.is_mouse_in_area( decrement ) )
	//	m_value -= m_step;

	// clamp the updated value.
	math::clamp(m_value, m_min, m_max);
}