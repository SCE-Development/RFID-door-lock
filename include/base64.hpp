#pragma once

#include <Arduino.h>

namespace base64url {

    /**
     * Convert an array of bytes into a base64url string.
     * This does not check the length of the output buffer.
     * 
     * @returns The length of the encoded string (without the null terminator).
    */
    size_t from_bytes(uint8_t * bytes, size_t n_bytes, char * base64);
    /**
     * Convert a base64url string into an array of bytes.
     * This does not check the length of the output buffer.
     * 
     * @returns The number of bytes of the resulting base64url string.
     */
    size_t to_bytes(const char * base64, size_t length, uint8_t * bytes);


    namespace internal {
        char character_map[] = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
            'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
            'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
            'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
            's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
            '3', '4', '5', '6', '7', '8', '9', '-', '_'
        };    

        uint8_t convert_base64_char_to_byte(const char c);
    };
};


uint8_t base64url::internal::convert_base64_char_to_byte(const char c) {
    if ('A' <= c && c <= 'Z')
        return c - 'A';
    if ('a' <= c && c <= 'z')
        return c - 'a' + 26;
    if ('0' <= c && c <= '9')
        return c - '0' + 52;
    if (c == '-')
        return 62;
    if (c == '_')
        return 63;
    return -1;
}

size_t base64url::from_bytes(uint8_t * bytes, size_t n_bytes, char * base64) {
    size_t j = 0;
    for (size_t i = 0; i < n_bytes; i += 3) {
        uint8_t a = 0, b = 0, c = 0;

        if (i + 0 < n_bytes)
            a = bytes[i];
        if (i + 1 < n_bytes)
            b = bytes[i + 1];
        if (i + 2 < n_bytes)
            c = bytes[i + 2];

        base64[j] = base64url::internal::character_map[a >> 2];
        j++;
        base64[j] = base64url::internal::character_map[((a & 0b00000011)) << 4 | (b >> 4)];
        j++;
        base64[j] = base64url::internal::character_map[((b & 0b00001111)) << 2 | (c >> 6)];
        j++;
        base64[j] = base64url::internal::character_map[(c & 0b00111111)];
        j++;
    }

    base64[j] = '\0';
    return j;
}
size_t base64url::to_bytes(const char * base64, size_t length, uint8_t * bytes) {
    size_t j = 0;
    for (size_t i = 0; i < length; i += 4) {
        uint8_t a = 0, b = 0, c = 0, d = 0;
        if(i < length)
            a = base64url::internal::convert_base64_char_to_byte(base64[i]);
        if(i+1 < length)
            b = base64url::internal::convert_base64_char_to_byte(base64[i + 1]);
        if(i+2 < length)
            c = base64url::internal::convert_base64_char_to_byte(base64[i + 2]);
        if(i+3 < length)
            d = base64url::internal::convert_base64_char_to_byte(base64[i + 3]);

        bytes[j] = a << 2 | b >> 4;
        j++;
        bytes[j] = b << 4 | c >> 2;
        j++;
        bytes[j] = c << 6 | d;
        j++;
    }
    return j;
}
