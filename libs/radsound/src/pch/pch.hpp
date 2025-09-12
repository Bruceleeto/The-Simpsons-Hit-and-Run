//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================


#ifndef RADSOUND_PCH_HPP
#define RADSOUND_PCH_HPP

//
// only win32 and xbox take advantage of the pch at this moment.
//
#if defined RAD_WIN32 || defined RAD_XBOX
//
// Microsoft header files
//
#ifdef RAD_WIN32
#define WINVER 0x0501
#define _WIN32_WINNT 0x400
#endif

#ifdef RAD_XBOX
#include <xtl.h>
#include <dsound.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

//
// Standard C header files
//

#include <stdio.h>

#endif // defined RAD_WIN32 || defined RAD_XBOX

#ifndef AL_EFFECT_TYPE

/* Effect object definitions */
#define AL_EFFECT_TYPE                           0x8001
#define AL_EFFECT_NULL                           0x0000
#define AL_EFFECT_REVERB                         0x0001

/* Auxiliary Slot definitions */
#define AL_EFFECTSLOT_NULL                       0x0000
#define AL_EFFECTSLOT_EFFECT                     0x0001
#define AL_EFFECTSLOT_GAIN                       0x0002
#define AL_EFFECTSLOT_AUXILIARY_SEND_AUTO        0x0003

/* Reverb effect parameters */
#define AL_REVERB_DENSITY                        0x0001
#define AL_REVERB_DIFFUSION                      0x0002
#define AL_REVERB_GAIN                           0x0003
#define AL_REVERB_GAINHF                         0x0004
#define AL_REVERB_DECAY_TIME                     0x0005
#define AL_REVERB_DECAY_HFRATIO                  0x0006
#define AL_REVERB_REFLECTIONS_GAIN               0x0007
#define AL_REVERB_REFLECTIONS_DELAY              0x0008
#define AL_REVERB_LATE_REVERB_GAIN               0x0009
#define AL_REVERB_LATE_REVERB_DELAY              0x000A
#define AL_REVERB_AIR_ABSORPTION_GAINHF          0x000B
#define AL_REVERB_ROOM_ROLLOFF_FACTOR            0x000C

/* Source properties */
#define AL_AUXILIARY_SEND_FILTER                 0x20006
#define AL_METERS_PER_UNIT                       0x20004

/* Context properties */
#define ALC_MAX_AUXILIARY_SENDS                  0x20003

/* Function pointer types for EFX - stub versions */
typedef void (*LPALGENAUXILIARYEFFECTSLOTS)(ALsizei, ALuint*);
typedef void (*LPALDELETEAUXILIARYEFFECTSLOTS)(ALsizei, const ALuint*);
typedef void (*LPALAUXILIARYEFFECTSLOTI)(ALuint, ALenum, ALint);
typedef void (*LPALAUXILIARYEFFECTSLOTF)(ALuint, ALenum, ALfloat);
typedef void (*LPALGETAUXILIARYEFFECTSLOTF)(ALuint, ALenum, ALfloat*);

typedef void (*LPALGENEFFECTS)(ALsizei, ALuint*);
typedef void (*LPALDELETEEFFECTS)(ALsizei, const ALuint*);
typedef void (*LPALEFFECTI)(ALuint, ALenum, ALint);
typedef void (*LPALEFFECTF)(ALuint, ALenum, ALfloat);
typedef void (*LPALGETEFFECTI)(ALuint, ALenum, ALint*);
typedef void (*LPALGETEFFECTF)(ALuint, ALenum, ALfloat*);

#endif // AL_EFFECT_TYPE

#endif // RADSOUND_PCH_HPP