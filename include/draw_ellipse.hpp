/**
 * @file       draw_ellipse.hpp
 * @brief
 * @date       2020-02-05
 * @author     Peter
 * @copyright
 *      Peter of [ThinkSpirit Laboratory](http://thinkspirit.org/)
 *   of [Nanjing University of Information Science & Technology](http://www.nuist.edu.cn/)
 *   all rights reserved
 */

#ifndef BOUNCINGBALL_DRAW_ELLIPSE_HPP
#define BOUNCINGBALL_DRAW_ELLIPSE_HPP

#include <nana/paint/graphics.hpp>

inline void ellipse(nana::paint::graphics& graph, const nana::point& point, float a, float b, const nana::color& color)
{
	for (int i = -b; i < b; ++i) {
		int width = (float)(a) * std::sqrt(1 - std::pow(i / b, 2));
		graph.line({point.x - width, point.y + i}, {point.x + width, point.y + i}, color);
	}
}

#endif //BOUNCINGBALL_DRAW_ELLIPSE_HPP
