#ifndef SRC_FORMATTER_H
#define SRC_FORMATTER_H


#include <string>

template<typename T>
inline std::string operator%(std::string format, T t) {
    auto ind = format.find('?');
    if (ind == 0 || format[ind - 1] != '\\') {
        format.replace(ind, 1, std::to_string(t));
    }
    return format;
}

template<>
inline std::string operator%(std::string format, std::string s) {
    auto ind = format.find('?');
    if (ind == 0 || format[ind - 1] != '\\') {
        format.replace(ind, 1u, s);
    }
    return format;
}

template<>
inline std::string operator%(std::string format, const char* s) {
    auto ind = format.find('?');
    if (ind == 0 || format[ind - 1] != '\\') {
        format.replace(ind, 1u, s);
    }
    return format;
}

inline std::string operator""_f(const char* str, size_t size) {
    return std::string(str);
}

#endif //SRC_FORMATTER_H
