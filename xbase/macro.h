#pragma once

#define XB_BITVAL(n) (1 << (n))
#define XB_BTST(x, n) (!!((x) & XB_BITVAL(n)))
#define XB_BSET(x, n) x = ((x) | (XB_BITVAL(n)))
#define XB_BCLR(x, n) x =((x) & (~XB_BITVAL(n)))

#define XB_NUM_IS_POW2(x) (((x) & ((x) - 1)) == 0)

#define XB_ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

#define XB_MAX(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b; })

#define XB_MIN(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b; })
