#pragma once

#define XB_BITVAL(x) (1 << x)

#define XB_NUM_IS_POW2(x) ((x & (x - 1)) == 0)

#ifndef XB_ARRAYSIZE
#define XB_ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))
#endif  // ARRAYSIZE

#ifndef XB_MAX
#define XB_MAX(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b; })
#endif  // XB_MAX

#ifndef XB_MIN
#define XB_MIN(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b; })
#endif  // XB_MIN
