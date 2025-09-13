#ifndef RAD_STRUTILS_HPP
#define RAD_STRUTILS_HPP

#include <cctype>

// uppercase
inline char* rad_strupr(char* s)
{
    for (char* p = s; *p; ++p)
    {
        *p = static_cast<char>(std::toupper(static_cast<unsigned char>(*p)));
    }
    return s;
}

// lowercase
inline char* rad_strlwr(char* s)
{
    for (char* p = s; *p; ++p)
    {
        *p = static_cast<char>(std::tolower(static_cast<unsigned char>(*p)));
    }
    return s;
}

#endif // RAD_STRUTILS_HPP
