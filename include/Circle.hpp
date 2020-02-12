/**
 * @file       Circle.hpp
 * @brief
 * @date       2020-02-10
 * @author     Peter
 * @copyright
 *      Peter of [ThinkSpirit Laboratory](http://thinkspirit.org/)
 *   of [Nanjing University of Information Science & Technology](http://www.nuist.edu.cn/)
 *   all rights reserved
 */

#ifndef BOUNCINGBALL_CIRCLE_HPP
#define BOUNCINGBALL_CIRCLE_HPP

#include "cast_to_nana_point.hpp"
#include "draw_ellipse.hpp"

struct Circle
{
		typedef std::complex<float> Vec;

		Vec position;
		int radius;
		nana::color color;

		Circle(const Vec& position, int radius, const nana::color& color) : position(position), radius(radius),
		                                                                    color(color)
		{
		}

		virtual void draw(nana::paint::graphics& graph) const
		{
			ellipse(graph, cast_to_nana_point<int>(position), radius, radius, color);
		}
};

#endif //BOUNCINGBALL_CIRCLE_HPP
