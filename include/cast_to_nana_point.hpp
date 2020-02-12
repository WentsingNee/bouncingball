/**
 * @file       cast_to_nana_point.hpp
 * @brief
 * @date       2020-02-10
 * @author     Peter
 * @copyright
 *      Peter of [ThinkSpirit Laboratory](http://thinkspirit.org/)
 *   of [Nanjing University of Information Science & Technology](http://www.nuist.edu.cn/)
 *   all rights reserved
 */

#ifndef BOUNCINGBALL_CAST_TO_NANA_POINT_HPP
#define BOUNCINGBALL_CAST_TO_NANA_POINT_HPP

template <typename Tp, typename Up>
nana::basic_point<Tp>
cast_to_nana_point(const std::complex<Up>& pos)
{
	return nana::basic_point<Tp>(pos.real(), pos.imag());
}

#endif //BOUNCINGBALL_CAST_TO_NANA_POINT_HPP
