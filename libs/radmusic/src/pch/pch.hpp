#ifndef RADMUSIC_PCH_HPP
#define RADMUSIC_PCH_HPP

//
// only win32 and xbox take advantage of the pch at this moment.
//
#if defined WIN32 || defined RAD_XBOX
//
// Microsoft header files
//
#ifdef RAD_WIN32
#define WINVER 0x0501
#define _WIN32_WINNT 0x400
#include <windows.h>
#endif

#ifdef RAD_XBOX
#include <xtl.h>
#endif

//
// Standard C header files
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <radoptions.hpp>
#include <radfile.hpp>
#include <radmemory.hpp>
#include <radsound.hpp>

#endif // defined RAD_WIN32 || defined RAD_XBOX

#endif // RADMUSIC_PCH_HPP
