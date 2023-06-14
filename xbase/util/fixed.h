#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>

// Fixed point types
typedef int32_t fix32_t;
typedef int16_t fix16_t;
typedef int8_t fix8_t;
typedef uint32_t ufix32_t;
typedef uint16_t ufix16_t;
typedef uint8_t ufix8_t;

#ifndef XB_FIXED_BITS
#define XB_FIXED_BITS 4
#endif  // XB_FIXED_BITS

// Precision configuration
#define FIX16_COEF (1 << XB_FIXED_BITS)

// Fixed point conversions
#define INTTOFIX(x) ((fix_t)((x) * FIX32_COEF))
#define FIXTOINT(x) ((int)((x) / FIX_COEF))

// Fixed point multiplication and division
#define FIXMUL(x, y) (((x) * (y)) >> XB_FIXED_BITS)
#define FIXDIV(x, y) (((x) << XB_FIXED_BITS) / (y))

#endif // FIXED_H
