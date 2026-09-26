#ifndef UTF8_UTIL_H
#define UTF8_UTIL_H

#include <stddef.h>
#include <string.h>
#include <stdint.h>

static inline size_t utf8_val(int* codepoint, const char* inp) {
    unsigned char c = (unsigned char)*inp++;
    int val;
    if (c < 0x80) {
        *codepoint = c;
        return 1u;
    }
    else if (c > 0xC1 && c < 0xF5) {
        int cont_byte_count = (c >= 0xF0) ? 3 : (c >= 0xE0) ? 2 : 1;
        val = (c & ((1 << (6 - cont_byte_count)) - 1));
        for (int i = 0; i < cont_byte_count; i++) {
            unsigned char next = (unsigned char)*inp++;
            if ((next & 0xC0) != 0x80) {
                *codepoint = -1;
                return 0u;
            }
            val = (val << 6) | (next & 0x3F);
        }
        if ((*inp & 0xC0) == 0x80 ||
            val > 0x10FFFF ||
            (cont_byte_count == 3 && val < 0x10000) ||
            (cont_byte_count == 2 && val < 0x800) ||
            (cont_byte_count == 1 && val < 0x80) ||
            (val >= 0xD800 && val <= 0xDFFF)) {
            *codepoint = -1;
            return 0u;
        }
        *codepoint = val;
        return 1u + cont_byte_count;
    }
    *codepoint = -1;
    return 0u;
}

static inline bool is_valid_utf8(const char* inp, size_t inp_len) {
    for (size_t i = 0; i < inp_len;) {
        if (i + 8 <= inp_len) {
            uint64_t buf;
            memcpy(&buf, inp + i, sizeof(buf));
            if ((buf & 0x8080808080808080ull) == 0) {
                i += 8;
                continue;
            }
        }
        int codepoint;
        size_t adv = utf8_val(&codepoint, inp + i);
        if (adv == 0 || codepoint == -1)
            return false;
        i += adv;
    }
    return true;
}

#endif // UTF8_UTIL_H