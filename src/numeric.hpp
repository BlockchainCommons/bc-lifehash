#ifndef LIFEHASH_NUMERIC_HPP
#define LIFEHASH_NUMERIC_HPP

#include <math.h>
#include "stdint.h"

namespace LifeHash
{

// Interpolate `t` from [0..1] to [a..b].
inline double lerp_to(double toA, double toB, double t) { return t * (toB - toA) + toA; }

// Interpolate `t` from [a..b] to [0..1].
inline double lerp_from(double fromA, double fromB, double t) { return (fromA - t) / (fromA - fromB); }

// Interpolate `t` from [a..b] to [c..d].
inline double lerp(double fromA, double fromB, double toC, double toD, double t) { return lerp_to(toC, toD, lerp_from(fromA, fromB, t)); }

// Return the minimum of `a` and `b`.
inline double min(double a, double b) { return a < b ? a : b; }

// Return the maximum of `a` and `b`.
inline double max(double a, double b) { return a > b ? a : b; }

// Return the minimum of `a`, `b`, and `c`.
inline double min(double a, double b, double c) { return min(min(a, b), c); }

// Return the maximum of `a`, `b`, and `c`.
inline double max(double a, double b, double c) { return max(max(a, b), c); }

// Return `n` clamped to the range [0..1].
inline double clamped(double n) { return max(min(n, 1), 0); }

// Return `dividend` MODULO `divisor` where `dividend` can be negative,
// but the result is always non-negative.
inline double modulo(double dividend, double divisor) { return fmodf(fmodf(dividend, divisor) + divisor, divisor); }

} // namespace LifeHash

#endif
