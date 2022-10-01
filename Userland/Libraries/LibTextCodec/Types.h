#pragma once

#include <AK/Forward.h>
#include <AK/Function.h>

#define RET_SHIFT_ILSEQ(n) (-1 - 2 * (n))
#define RET_ILSEQ RET_SHIFT_ILSEQ(0)
#define RET_TOOFEW(n) (-2 - 2 * (n))
#define RET_ILUNI -1
#define RET_TOOSMALL -2

typedef u32 ucs4_t;
typedef struct {
    unsigned short indx; /* index into big table */
    unsigned short used; /* bitmask of used entries */
} Summary16;
