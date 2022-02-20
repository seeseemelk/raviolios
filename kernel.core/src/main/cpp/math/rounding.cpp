#include "math.hpp"

using namespace Math;

size_t Math::ceildiv(size_t a, size_t b)
{
	return (a == 0) ? 0 : ((a - 1) / b + 1);
}

size_t Math::ceilg(size_t n, size_t granularity)
{
	return ((n - 1) / granularity + 1) * granularity;
}

size_t Math::floorg(size_t n, size_t granularity)
{
	return (n / granularity) * granularity;
}
