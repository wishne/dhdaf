#include "includes.h"

#define TAB_X_OFFSET 0
#define TAB_HEIGHT 20
#define TAB_PADDING 10
#define ICON_SPACING 5

void Form::draw() {
	m_alpha = 0xff;
	if (!m_open)
		return;

	// get gui color.
	Color color = g_gui.m_color;
	color.a() = m_alpha;
	Color color2 = g_gui.m_color;
	Color color3 = g_gui.m_color;
	Color color4 = colors::white;

	if (m_open) {
		color2.a() = 50;
		color3.a() = 120;
		color4.a() = 120;
	}
	else if (!m_open) {
		color2.a() = 0;
		color3.a() = 0;
		color4.a() = 0;
	}

	// background.
	int top_bar_size = 22;
	render::rect_filled(m_x, m_y, m_width, m_height, { 10,9,10, m_alpha });
	render::rect_filled(m_x, m_y, m_width, top_bar_size, { 19,16,19, m_alpha });
	render::gradient(m_x, m_y + top_bar_size, m_width / 2, 1, { 0,0,0,0 }, color, true);
	render::gradient(m_x + m_width / 2, m_y + top_bar_size, m_width / 2, 1, color, { 0,0,0,0 }, true);
	render::FontSize_t text_size = render::menu_shade.size(XOR("naphack"));
	render::menu_shade.string(m_x + m_width / 2, m_y + (top_bar_size / 2) - (text_size.m_height / 2), color4, XOR("naphack"), render::ALIGN_CENTER);
	render::rect(m_x + 1, m_y + 1, m_width - 2, m_height - 2, { 26,26,26, m_alpha });

	// border.
	render::rect(m_x, m_y, m_width, m_height, { 5, 5, 5, m_alpha });
	render::gradient(m_x, m_y + m_height - 40, m_width / 2, 1, { 0,0,0,0 }, color, true);
	render::gradient(m_x + m_width / 2, m_y + m_height - 40, m_width / 2, 1, color, { 0,0,0,0 }, true);

	Rect tabs_area = GetTabsRect();

	int icon_spacing = 5;
	std::string asuser = XOR("as ") + g_cl.m_user;
	int text_width = render::menu_shade.size(XOR("home")).m_width + render::menu_shade.size(XOR(asuser)).m_width + icon_spacing;
	int text_start_x = tabs_area.x + tabs_area.w - 13 - text_width;
	int icon_x = text_start_x + text_width - icon_spacing - render::menu_icons1.size("F").m_width;

	render::menu_shade.string(text_start_x + render::menu_shade.size(XOR("home")).m_width, tabs_area.y + 5, { 255, 255, 255, 50 }, XOR("home"), render::ALIGN_RIGHT);
	render::menu_shade.string(text_start_x + render::menu_shade.size(XOR("home")).m_width, tabs_area.y + 15, { 255, 255, 255, 50 }, XOR(asuser), render::ALIGN_RIGHT);
	render::menu_icons1.string(icon_x, tabs_area.y + 9, { 255, 255, 255, 50 }, "F");

	std::vector<Rect> tab_areas;

	// tabs background and border.
	size_t font_w = 8;
	for (size_t i{}; i < m_tabs.size(); ++i) {
		const auto& t = m_tabs[i];
		int icon_x = tabs_area.x + font_w;
		int title_x = icon_x + ICON_SPACING + render::menu_icons.size(t->m_icon).m_width;

		if (t == m_active_tab) {
			render::menu_icons.string(icon_x, tabs_area.y + 5, color, t->m_icon);
			render::menu_shade.string(title_x, tabs_area.y + 5, color, t->m_title);
		}
		else {
			render::menu_icons.string(icon_x, tabs_area.y + 7, color4, t->m_icon);
			render::menu_shade.string(title_x, tabs_area.y + 7, color4, t->m_title);
		}

		Color desc_color = (t == m_active_tab) ? color : Color(255, 255, 255, 50);
		render::menu_shade.string(title_x, tabs_area.y + 15, desc_color, t->m_desc);

		int descwidth = render::menu_shade.size(t->m_desc).m_width;
		int titlewidth = render::menu_shade.size(t->m_title).m_width;
		int iconwidth = render::menu_icons.size(t->m_icon).m_width;
		int tab_width = ((descwidth > titlewidth && t == m_active_tab) ? descwidth : titlewidth) + iconwidth + ICON_SPACING + TAB_PADDING;

		Rect tab_area = { static_cast<int>(tabs_area.x + font_w + TAB_X_OFFSET), tabs_area.y, tab_width - TAB_X_OFFSET, TAB_HEIGHT };
		tab_areas.push_back(tab_area);

		if (i < m_tabs.size() - 1) {
			render::line(tab_area.x + tab_area.w + TAB_PADDING / 2, tab_area.y, tab_area.x + tab_area.w + TAB_PADDING / 2, tab_area.y + TAB_HEIGHT +1, {60, 60, 60, m_alpha});
		}

		font_w += tab_width + TAB_PADDING;
	}

	if (g_input.GetKeyPress(VK_LBUTTON)) {
		auto previous_active_tab = m_active_tab;

		for (size_t i = 0; i < tab_areas.size(); ++i) {
			const Rect& area = tab_areas[i];
			if (g_input.m_mouse.x >= area.x && g_input.m_mouse.x <= area.x + area.w &&
				g_input.m_mouse.y >= area.y && g_input.m_mouse.y <= area.y + area.h) {
				if (m_active_tab != m_tabs[i]) {
					m_active_tab = m_tabs[i];
				}
				break;
			}
		}

		// If the active tab hasn't changed, ensure it stays the same
		if (previous_active_tab == m_active_tab) {
			m_active_tab = previous_active_tab;
		}
	}

	// this tab has elements.
	if (!m_active_tab->m_elements.empty()) {
		// elements background and border.
		Rect el = GetElementsRect();

		render::rect_filled(el.x, el.y, el.w, el.h, { 18, 18, 18, m_alpha });
		render::rect(el.x, el.y, el.w, el.h, { 5, 5, 5, m_alpha });
		render::rect(el.x + 1, el.y + 1, el.w - 2, el.h - 2, { 26, 26, 26, m_alpha });

		// iterate elements to display.
		for (const auto& e : m_active_tab->m_elements) {
			// draw the active element last.
			if (!e || (m_active_element && e == m_active_element))
				continue;

			if (!e->m_show)
				continue;

			// this element we don't draw.
			if (!(e->m_flags & ElementFlags::DRAW))
				continue;

			e->draw();
		}

		// we still have to draw one last element.
		if (m_active_element && m_active_element->m_show)
			m_active_element->draw();
	}
}
