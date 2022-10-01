#pragma once

#include "cp936ext.h"
#include "gb2312.h"
#include "gbkext1.h"
#include "gbkext2.h"
#include <AK/Forward.h>
#include <AK/Function.h>

namespace TextCodec {

/*
 * GBK, as described in Ken Lunde's book, is an extension of GB 2312-1980
 * (shifted by adding 0x8080 to the range 0xA1A1..0xFEFE, as used in EUC-CN).
 * It adds the following ranges:
 *
 * (part of GBK/1)  0xA2A1-0xA2AA  Small Roman numerals
 * GBK/3   0x{81-A0}{40-7E,80-FE}  6080 new characters, all in Unicode
 * GBK/4   0x{AA-FE}{40-7E,80-A0}  8160 new characters, 8080 in Unicode
 * GBK/5   0x{A8-A9}{40-7E,80-A0}  166 new characters, 153 in Unicode
 *
 * Furthermore, all four tables I have looked at
 *   - the CP936 table by Microsoft, found on ftp.unicode.org in 1999,
 *   - the GBK table by Sun, investigated on a Solaris 2.7 machine,
 *   - the GBK tables by CWEX, found in the Big5+ package,
 *   - the GB18030 standard (second printing),
 * agree in the following extensions. (Ken Lunde must have overlooked these
 * differences between GB2312 and GBK. Also, the CWEX tables have additional
 * differences.)
 *
 * 1. Some characters in the GB2312 range are defined differently:
 *
 *     code    GB2312                         GBK
 *    0xA1A4   0x30FB # KATAKANA MIDDLE DOT   0x00B7 # MIDDLE DOT
 *    0xA1AA   0x2015 # HORIZONTAL BAR        0x2014 # EM DASH
 *
 * 2. 19 characters added in the range 0xA6E0-0xA6F5.
 *
 * 3. 4 characters added in the range 0xA8BB-0xA8C0.
 *
 * CP936 as of 1999 was identical to GBK. However, since 1999, Microsoft has
 * added new mappings to CP936...
 */
int gbk_mbtowc(ucs4_t* pwc, char const* s, size_t n);

}
