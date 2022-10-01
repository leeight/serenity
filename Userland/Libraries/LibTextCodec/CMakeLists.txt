set(SOURCES
    Decoder.cpp
    cp936ext.cpp
    gb2312.cpp
    gbkext1.cpp
    gbkext2.cpp
    gbk.cpp
)

serenity_lib(LibTextCodec textcodec)
target_link_libraries(LibTextCodec LibC)
