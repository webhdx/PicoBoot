#pragma once

// https://codereview.stackexchange.com/a/151070

inline uint32_t Reverse32(uint32_t value) 
{
    return (((value & 0x000000FF) << 24) |
            ((value & 0x0000FF00) <<  8) |
            ((value & 0x00FF0000) >>  8) |
            ((value & 0xFF000000) >> 24));
}

inline uint32_t BigEndian32(uint32_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return Reverse32(value);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return value;
#else
#    error unsupported endianness
#endif
}
