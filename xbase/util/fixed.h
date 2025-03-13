#pragma once
// Fixed point types

#ifndef __ASSEMBLER__
#include <stdint.h>
typedef int32_t fix32_t;
typedef int16_t fix16_t;
typedef uint32_t ufix32_t;
typedef uint16_t ufix16_t;
#endif

#ifndef XB_FIXED_BITS
#define XB_FIXED_BITS 7
#endif  // XB_FIXED_BITS

// Precision configuration
#define FIX_COEF (1 << XB_FIXED_BITS)

// Fixed point conversions
#define INTTOFIX(x) ((x) * FIX_COEF)
#define FIXTOINT(x) ((int)((x) / FIX_COEF))

// Fixed point multiplication and division
#define FIXMUL(x, y) (((x) * (y)) >> XB_FIXED_BITS)
#define FIXDIV(x, y) (((x) << XB_FIXED_BITS) / (y))
