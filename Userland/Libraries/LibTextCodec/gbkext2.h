#pragma once

#include "Types.h"
#include <AK/Forward.h>
#include <AK/Function.h>

namespace TextCodec {

int gbkext2_mbtowc(ucs4_t* pwc, char const* s, size_t n);

}
