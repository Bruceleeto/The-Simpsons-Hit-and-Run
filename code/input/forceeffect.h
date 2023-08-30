//=============================================================================
// Copyright (C) 2002 Radical Entertainment Ltd.  All rights reserved.
//
// File:        forceeffect.h
//
// Description: Blahblahblah
//
// History:     6/25/2003 + Created -- Cary Brisebois
//
//=============================================================================

#ifndef FORCEEFFECT_H
#define FORCEEFFECT_H

//========================================
// Nested Includes
//========================================
#include <SDL.h>
#include <radcontroller.hpp>

#ifdef WIN32
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#endif

#if defined(RAD_PS2) || defined(RAD_WIN32)
typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
#endif

//========================================
// Forward References
//========================================

//=============================================================================
//
// Synopsis:    Blahblahblah
//
//=============================================================================

class ForceEffect
{
public:
    ForceEffect();
    virtual ~ForceEffect();

#ifdef WIN32
    void SetForceID( u8 forceID ) { m_index = forceID; }
    u8   GetForceID() const { return m_index; }
#endif

    void Init( IRadControllerOutputPoint* outputPoint );
    bool IsInit() const { return ( mOutputPoint != NULL ); };

    void Start();
    void Stop();

#ifdef WIN32
    virtual void Update(unsigned timeins = 0);
    void ShutDownEffects();
    void SetResetTime( DWORD dwMilliSeconds );
#else
    void Update();
#endif

protected:
    IRadControllerOutputPoint* mOutputPoint;
#ifdef WIN32
    DIEFFECT      mForceEffect;
    DWORD         m_rgdwAxes[2];
    LONG          m_rglDirection[2];
    u8            m_index;         // the index ID of this force.
    DWORD         m_currentTime;
    DWORD         m_effectTime;
#else
    SDL_HapticEffect mForceEffect;
#endif
    bool mEffectDirty;

    virtual void OnInit() = 0;

    //Prevent wasteful constructor creation.
    ForceEffect( const ForceEffect& forceeffect );
    ForceEffect& operator=( const ForceEffect& forceeffect );
};

//*****************************************************************************
//
//Inline Public Member Functions
//
//*****************************************************************************

#endif //FORCEEFFECT_H
