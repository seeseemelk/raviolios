/**@file
 * @brief
 */
#ifndef _MATH_HPP_
#define _MATH_HPP_

#include "defs.hpp"

namespace Math {
	size_t ceildiv(size_t a, size_t b);
	size_t ceilg(size_t n, size_t granularity);
	size_t floorg(size_t n, size_t granularity);

	template<typename T>
	T max(T a, T b)
	{
		return a > b ? a : b;
	}
}

#endif /* _MATH_HPP_ */
