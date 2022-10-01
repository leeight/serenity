/*
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Vector.h>
#include <LibTest/TestCase.h>
#include <LibTextCodec/Decoder.h>

TEST_CASE(test_utf8_decode)
{
    auto decoder = TextCodec::UTF8Decoder();
    // Bytes for U+1F600 GRINNING FACE
    auto test_string = "\xf0\x9f\x98\x80"sv;

    Vector<u32> processed_code_points;
    decoder.process(test_string, [&](u32 code_point) {
        processed_code_points.append(code_point);
    });
    EXPECT(processed_code_points.size() == 1);
    EXPECT(processed_code_points[0] == 0x1F600);

    EXPECT(decoder.to_utf8(test_string) == test_string);
}

TEST_CASE(test_gbk_decode)
{
    auto* decoder = TextCodec::decoder_for("gbk");
    auto test_string = "\xc4\xe3\xba\xc3\xca\xc0\xbd\xe7"sv;

    Vector<u32> processed_code_points;
    decoder->process(test_string, [&](u32 code_point) {
        processed_code_points.append(code_point);
    });
    EXPECT(processed_code_points.size() == 4);
    EXPECT(processed_code_points[0] == 0x4F60);
    EXPECT(processed_code_points[1] == 0x597D);
    EXPECT(processed_code_points[2] == 0x4E16);
    EXPECT(processed_code_points[3] == 0x754C);

    // Hello World
    EXPECT(decoder->to_utf8(test_string) == "你好世界"sv);
}
