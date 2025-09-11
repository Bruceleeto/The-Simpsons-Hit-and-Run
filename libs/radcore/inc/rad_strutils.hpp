#ifndef RAD_STRUTILS_HPP
#define RAD_STRUTILS_HPP

#include <cctype>

// uppercase
inline char* strupr(char* s)
{
    for (char* p = s; *p; ++p)
    {
        *p = static_cast<char>(std::toupper(static_cast<unsigned char>(*p)));
    }
    return s;
}

// lowercase
inline char* strlwr(char* s)
{
    for (char* p = s; *p; ++p)
    {
        *p = static_cast<char>(std::tolower(static_cast<unsigned char>(*p)));
    }
    return s;
}

#endif
