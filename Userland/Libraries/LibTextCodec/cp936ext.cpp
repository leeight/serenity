#include "cp936ext.h"

namespace TextCodec {
/*
 * CP936 extensions
 */

static unsigned short const cp936ext_2uni_pagea6[181 - 159] = {
    /* 0xa6 */
    0xfe35,
    0xfe36,
    0xfe39,
    0xfe3a,
    0xfe3f,
    0xfe40,
    0xfe3d,
    0xfe3e,
    0xfe41,
    0xfe42,
    0xfe43,
    0xfe44,
    0xfffd,
    0xfffd,
    0xfe3b,
    0xfe3c,
    0xfe37,
    0xfe38,
    0xfe31,
    0xfffd,
    0xfe33,
    0xfe34,
};
static unsigned short const cp936ext_2uni_pagea8[128 - 122] = {
    /* 0xa8 */
    0x0251,
    0xfffd,
    0x0144,
    0x0148,
    0xfffd,
    0x0261,
};

int cp936ext_mbtowc(ucs4_t* pwc, char const* s, size_t n)
{
    unsigned char c1 = s[0];
    if ((c1 == 0xa6) || (c1 == 0xa8)) {
        if (n >= 2) {
            unsigned char c2 = s[1];
            if ((c2 >= 0x40 && c2 < 0x7f) || (c2 >= 0x80 && c2 < 0xff)) {
                unsigned int i = 190 * (c1 - 0x81) + (c2 - (c2 >= 0x80 ? 0x41 : 0x40));
                unsigned short wc = 0xfffd;
                if (i < 7410) {
                    if (i >= 7189 && i < 7211)
                        wc = cp936ext_2uni_pagea6[i - 7189];
                } else {
                    if (i >= 7532 && i < 7538)
                        wc = cp936ext_2uni_pagea8[i - 7532];
                }
                if (wc != 0xfffd) {
                    *pwc = (ucs4_t)wc;
                    return 2;
                }
            }
            return RET_ILSEQ;
        }
        return RET_TOOFEW(0);
    }
    return RET_ILSEQ;
}

}
