#ifndef STRUTILS_H
#define STRUTILS_H

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#define STRUTILS_INTERNAL static inline
#define STRUTILS_API extern

STRUTILS_INTERNAL uint8_t utf8_char_len(const char c) {
    // 0xxx xxxx
    if ((c & 0b10000000) == 0b00000000) {
        return 1;
    }

    // 110x xxxx
    if ((c & 0b11100000) == 0b11000000) {
        return 2;
    }

    // 1110 xxxx
    if ((c & 0b11110000) == 0b11100000) {
        return 3;
    }

    // 1111 0xxx
    if ((c & 0b11111000) == 0b11110000) {
        return 4;
    }

    // invalid
    return UINT8_MAX;
}

STRUTILS_INTERNAL wchar_t utf8_parse_char(const char* utf8_str, uint8_t char_len) {
    switch (char_len) {
        case 1:
            // 0xxx xxxx
            return (wchar_t)(*utf8_str);
        case 2:
            // 110x xxxx, 10xx xxxx
            return ((wchar_t)(*utf8_str & 0b00011111) << 6) | ((wchar_t)(*(utf8_str + 1) & 0b00111111));
        case 3:
            // 1110 xxxx, 10xx xxxx, 10xx xxxx
            return ((wchar_t)(*utf8_str & 0b00001111) << 12) | ((wchar_t)(*(utf8_str + 1) & 0b00111111) << 6) | ((wchar_t)(*(utf8_str + 2) & 0b00111111));
        case 4:
            // 1110 xxxx, 10xx xxxx, 10xx xxxx, 10xx xxxx
            return ((wchar_t)(*utf8_str & 0b00000111) << 18) | ((wchar_t)(*(utf8_str + 1) & 0b00111111) << 12) | ((wchar_t)(*(utf8_str + 2) & 0b00111111) << 6) | ((wchar_t)(*(utf8_str + 3) & 0b00111111));
        default:
            // invalid
            return WCHAR_MAX;
    }
}

STRUTILS_API size_t utf8_to_wstr(const char* utf8_str, wchar_t* wstr, size_t max_wchar_count) {
    size_t wchar_count = 0;

    while (*utf8_str != '\0' && wchar_count < max_wchar_count) {
        // byte length
        uint8_t char_len = utf8_char_len(*utf8_str);
        if (char_len == UINT8_MAX) {
            return (size_t)-1;
        }

        wchar_t wc = utf8_parse_char(utf8_str, char_len);
        if (wc == WCHAR_MAX) {
            // invalid intermediate byte
            return (size_t)-1;
        }

        wstr[wchar_count++] = wc;

        utf8_str += char_len;
    }

    return wchar_count;
}

STRUTILS_API bool utf8_validate(const char* utf8_str) {
    while (*utf8_str != '\0') {
        uint8_t char_len = utf8_char_len(*utf8_str);

        if (char_len == UINT8_MAX) {
            return false;
        }

        for (uint8_t i = 1; i < char_len; i++) {
            if ((utf8_str[i] & 0b11000000) != 0b10000000) return false;
        }

        utf8_str += char_len;
    }

    return true;
}

STRUTILS_API size_t utf8_wchar_count(const char* utf8_str) {
    size_t count = 0;

    while (*utf8_str != '\0') {
        uint8_t char_len = utf8_char_len(*utf8_str);

        if (char_len == UINT8_MAX) {
            return SIZE_MAX;
        }

        utf8_str += char_len;

        count++;
    }

    return count;
}

STRUTILS_API char* utf8_nth_char_pos(const char* utf8_str, size_t n) {
    size_t count = 0;

    while (*utf8_str != '\0') {
        if (count == n) {
            return (char*)utf8_str;
        }

        uint8_t char_len = utf8_char_len(*utf8_str);

        if (char_len == UINT8_MAX) {
            return NULL;
        }

        utf8_str += char_len;

        count++;
    }

    return NULL;
}

STRUTILS_API wchar_t utf8_nth_char(const char* utf8_str, size_t n) {
    char* pos = utf8_nth_char_pos(utf8_str, n);
    if (pos == NULL) {
        return WCHAR_MAX;
    }

    uint8_t char_len = utf8_char_len(*pos);
    if (char_len == UINT8_MAX) {
        return WCHAR_MAX;
    }

    return utf8_parse_char(pos, char_len);
}

STRUTILS_API wchar_t utf8_next_char(const char** utf8_str) {
    uint8_t char_len = utf8_char_len(**utf8_str);
    if (char_len == UINT8_MAX) {
        return WCHAR_MAX;
    }

    wchar_t wc = utf8_parse_char(*utf8_str, char_len);
    if (wc == WCHAR_MAX) {
        return WCHAR_MAX;
    }

    *utf8_str += char_len;

    return wc;
}

#endif //STRUTILS_H
