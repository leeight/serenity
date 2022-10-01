#include "gbk.h"

namespace TextCodec {

int gbk_mbtowc(ucs4_t* pwc, char const* s, size_t n)
{
    unsigned char c = *s;

    if (c >= 0x81 && c < 0xff) {
        if (n < 2)
            return RET_TOOFEW(0);
        if (c >= 0xa1 && c <= 0xf7) {
            unsigned char c2 = s[1];
            if (c == 0xa1) {
                if (c2 == 0xa4) {
                    *pwc = 0x00b7;
                    return 2;
                }
                if (c2 == 0xaa) {
                    *pwc = 0x2014;
                    return 2;
                }
            }
            if (c2 >= 0xa1 && c2 < 0xff) {
                unsigned char buf[2];
                int ret;
                buf[0] = c - 0x80;
                buf[1] = c2 - 0x80;
                ret = gb2312_mbtowc(pwc, (char const*)buf, 2);
                if (ret != RET_ILSEQ)
                    return ret;
                buf[0] = c;
                buf[1] = c2;
                ret = cp936ext_mbtowc(pwc, (char const*)buf, 2);
                if (ret != RET_ILSEQ)
                    return ret;
            }
        }
        if (c >= 0x81 && c <= 0xa0)
            return gbkext1_mbtowc(pwc, s, 2);
        if (c >= 0xa8 && c <= 0xfe)
            return gbkext2_mbtowc(pwc, s, 2);
        if (c == 0xa2) {
            unsigned char c2 = s[1];
            if (c2 >= 0xa1 && c2 <= 0xaa) {
                *pwc = 0x2170 + (c2 - 0xa1);
                return 2;
            }
        }
    }
    return RET_ILSEQ;
}

}
