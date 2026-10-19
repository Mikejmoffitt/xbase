//
// XBase 16.16 Fixed point types and utility macros.
//

#pragma once
// Fixed point types

#ifndef __ASSEMBLER__
#include <stdint.h>
typedef int32_t fix32_t;
typedef uint32_t ufix32_t;
#endif

#define FIX_BITS 16

// Precision configuration
#define FIX_COEF (1 << FIX_BITS)

// Fixed point conversions
#define INTTOFIX(x) ((x) * FIX_COEF)
#define FIXTOINT(x) ((int)((x) / FIX_COEF))

// Fixed point multiplication and division
#define FIXMUL(x, y) (((x) * (y)) >> FIX_BITS)
#define FIXDIV(x, y) (((x) << XB_FIX_BITS) / (y))
