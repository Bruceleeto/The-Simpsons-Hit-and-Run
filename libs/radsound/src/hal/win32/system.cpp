//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================

#include "pch.hpp"
#include "system.hpp"
#include "listener.hpp"
#include "buffer.hpp"
#include "voice.hpp"
#include "../common/banner.hpp"
#include "../common/memoryregion.hpp"
#include <radplatform.hpp>
#include "radinprogext.h"

LPALBUFFERSTORAGESOFT radBufferStorageSOFT;
LPALMAPBUFFERSOFT radMapBufferSOFT;
LPALUNMAPBUFFERSOFT radUnmapBufferSOFT;

//================================================================================
// Static Members
//================================================================================

radSoundHalSystem * radSoundHalSystem::s_pRsdSystem = NULL;
static int g_RadSoundInitializeCount = 0;

//============================================================================
// radSoundHalSystem::radSoundHalSystem
//============================================================================

radSoundHalSystem::radSoundHalSystem( radMemoryAllocator allocator )
    :
    m_NumAuxSends( 0 ),
    m_pSoundMemory( 0 ),
    m_LastServiceTime( ::radTimeGetMilliseconds( ) )
{
    s_pRsdSystem = this;

    for( unsigned int i = 0; i < RSD_SYSTEM_MAX_AUX_SENDS; i++ )
    {
        m_refIRadSoundHalEffect[ i ] = NULL;
    }
    
	::radSoundPrintBanner( );
}

//============================================================================
// radSoundHalSystem::~radSoundHalSystem
//============================================================================

radSoundHalSystem::~radSoundHalSystem( void )
{
	radSoundHalListener::Terminate( );

    if (m_NumAuxSends > 0)
        alDeleteAuxiliaryEffectSlots(m_NumAuxSends, m_AuxSlots);

    alcMakeContextCurrent(NULL);
    if (m_pContext)
        alcDestroyContext(m_pContext);
    m_pContext = NULL;

    if (m_pDevice)
        alcCloseDevice(m_pDevice);
    m_pDevice = NULL;

	radSoundHalMemoryRegion::Terminate( );
    ::radMemoryFreeAligned( GetThisAllocator( ), m_pSoundMemory );

    s_pRsdSystem = NULL;
}

typedef void (AL_APIENTRY*ALDEBUGPROCEXT)(ALenum source, ALenum type, ALuint id, ALenum severity, ALsizei length, const ALchar *message, void *userParam);

typedef void (AL_APIENTRY*LPALDEBUGMESSAGECALLBACKEXT)(ALDEBUGPROCEXT callback, void *userParam);

void AL_APIENTRY PrintOpenALErrors(ALenum source, ALenum type, ALuint id, ALenum severity, ALsizei length, const ALchar *message, void *userParam)
{
    (void)length;
    (void)userParam;
    fprintf(stderr, "OpenAL says: source=%u type=%u id=%u severity=%u '%s'\n", source, type, id, severity, message);
}

//============================================================================
// radSoundHalSystem::Initialize
//============================================================================

void radSoundHalSystem::Initialize( const SystemDescription & systemDescription )
{
    rAssertMsg( systemDescription.m_SamplingRate != 0, 
        "ERROR radsound: system sampling rate must be set"
        "to the highest sampling rate required by your program (probably 48000Hz)" );

    m_NumAuxSends = 0;
    m_pDevice = NULL;
    m_pContext = NULL;
    
    printf("[Sound] OpenAL initialization stubbed out\n");

    // radSoundHalListener::Initialize(GetThisAllocator(), m_pContext);
    printf("[Sound] Skipping listener initialization\n");

    // Still allocate memory for sound system
    m_pSoundMemory = ::radMemoryAllocAligned( 
        GetThisAllocator(),
        systemDescription.m_ReservedSoundMemory, 
        radSoundHalDataSourceReadAlignmentGet() );

    radSoundHalMemoryRegion::Initialize( 
        m_pSoundMemory, 
        systemDescription.m_ReservedSoundMemory, 
        systemDescription.m_MaxRootAllocations,
        radSoundHalDataSourceReadAlignmentGet(), 
        radMemorySpace_Local, GetThisAllocator() );
}

//============================================================================
// radSoundHalSystem::GetRootMemoryRegion
//============================================================================

IRadSoundHalMemoryRegion * radSoundHalSystem::GetRootMemoryRegion( void )
{
	return radSoundHalMemoryRegion::GetRootRegion( );
}

//============================================================================
// radSoundHalSystem::GetNumAuxSends
//============================================================================

unsigned int radSoundHalSystem::GetNumAuxSends( )
{
    return m_NumAuxSends;
}

//============================================================================
// radSoundHalSystem::SetOutputMode
//============================================================================

void radSoundHalSystem::SetOutputMode( radSoundOutputMode mode )
{
	rDebugString( "radSoundHalSystem: SetOutputMode() not supported on Win32/XBox use DashBoard\n" );
}

//============================================================================
// radSoundHalSystem::GetOutputMode
//============================================================================

radSoundOutputMode radSoundHalSystem::GetOutputMode( void )
{
	return radSoundOutputMode_Stereo;
}

//============================================================================
// radSoundHalSystem::Service
//============================================================================

void radSoundHalSystem::Service( void )
{
    unsigned int now = ::radTimeGetMilliseconds( );

    radSoundUpdatableObject::UpdateAll( now - m_LastServiceTime );

    m_LastServiceTime = now;
}

//============================================================================
// radSoundHalSystem::ServiceOncePerFrame
//============================================================================

void radSoundHalSystem::ServiceOncePerFrame( void )
{
	radSoundHalListener::GetInstance( )->UpdatePositionalSettings( );
}

//============================================================================
// radSoundHalSystem::GetStats
//============================================================================
    
void radSoundHalSystem::GetStats( IRadSoundHalSystem::Stats * pStats )
{
    rAssert( pStats );

    ::memset( pStats, 0, sizeof( IRadSoundHalSystem::Stats ) );

	//
	// Get voice info
	//

	radSoundHalVoiceWin * pVoiceSearch = radSoundHalVoiceWin::GetLinkedClassHead( );
		
    while ( pVoiceSearch != NULL )
    {
		if ( pVoiceSearch->GetPositionalGroup( ) != NULL )
		{
			pStats->m_NumPosVoices++;

			if ( pVoiceSearch->IsPlaying( ) )
			{
				pStats->m_NumPosVoicesPlaying++;
			}				
		}
		else
		{
			pStats->m_NumVoices++;

			if ( pVoiceSearch->IsPlaying( ) )
			{
				pStats->m_NumVoicesPlaying++;
			}
		}

        pVoiceSearch = pVoiceSearch->GetLinkedClassNext( );
    }

	//
	// GetBuffer info
	//
	
	radSoundHalBufferWin * pBufferSearch = radSoundHalBufferWin::GetLinkedClassHead( );

	while ( pBufferSearch != NULL )
	{
		pStats->m_NumBuffers ++;
		pStats->m_BufferMemoryUsed += pBufferSearch->GetSizeInBytes( );
		
		pBufferSearch = pBufferSearch->GetLinkedClassNext( );
	}
	
	// Effects Memory is always zero it is in the hardware.

	pStats->m_EffectsMemoryUsed = 0;
									
	radSoundHalMemoryRegion::GetRootRegion( )->GetStats( & pStats->m_TotalFreeSoundMemory, NULL, NULL, true );
}

//============================================================================
// radSoundHalSystem::SetAuxEffect
//============================================================================

void radSoundHalSystem::SetAuxEffect( unsigned int auxNumber, IRadSoundHalEffect * pIRadSoundHalEffect )
{
    rAssert( auxNumber < m_NumAuxSends );

    if( m_refIRadSoundHalEffect[ auxNumber ] != NULL )
    {
        m_refIRadSoundHalEffect[ auxNumber ]->Detach( );
    }

    m_refIRadSoundHalEffect[ auxNumber ] = pIRadSoundHalEffect;

    if( m_refIRadSoundHalEffect[ auxNumber ] != NULL )
    {
        m_refIRadSoundHalEffect[ auxNumber ]->Attach( auxNumber );
    }
}

//============================================================================
// radSoundHalSystem::GetAuxEffect
//============================================================================

IRadSoundHalEffect * radSoundHalSystem::GetAuxEffect( unsigned int auxNumber )
{
    rAssert( auxNumber < m_NumAuxSends );
    return m_refIRadSoundHalEffect[ auxNumber ];
}

//============================================================================
// radSoundHalSystem::SetAuxGain
//============================================================================

void radSoundHalSystem::SetAuxGain( unsigned int aux, float gain )
{
    rAssert(aux < m_NumAuxSends);
    alAuxiliaryEffectSlotf(m_AuxSlots[aux], AL_EFFECTSLOT_GAIN, gain);
    rAssert(alGetError() == AL_NO_ERROR);
}

//============================================================================
// radSoundHalSystem::GetAuxGain
//============================================================================

float radSoundHalSystem::GetAuxGain( unsigned int aux )
{
    rAssert(aux < m_NumAuxSends);
    rWarningMsg( false, "system::GetAuxGain not supported on PC" );
    ALfloat gain;
    alGetAuxiliaryEffectSlotf(m_AuxSlots[aux], AL_EFFECTSLOT_GAIN, &gain);
    rAssert(alGetError() == AL_NO_ERROR);
    return gain;
}

//============================================================================
// radSoundHalSystem::GetOpenALDevice
//============================================================================

ALCdevice * radSoundHalSystem::GetOpenALDevice( void )
{
    return m_pDevice;
}

//============================================================================
// radSoundHalSystem::GetOpenALContext
//============================================================================

ALCcontext * radSoundHalSystem::GetOpenALContext( void )
{
    return m_pContext;
}

//============================================================================
// radSoundHalSystem::GetContext
//============================================================================

ALuint radSoundHalSystem::GetOpenALAuxSlot( unsigned int aux )
{
    rAssert(aux < m_NumAuxSends);

    return m_AuxSlots[aux];
}

//============================================================================
// radSoundHalSystem::GetInstance
//============================================================================

radSoundHalSystem * radSoundHalSystem::GetInstance( void )
{
    return s_pRsdSystem;
}

//================================================================================
// ::rsdGetSystem
//================================================================================

IRadSoundHalSystem* radSoundHalSystemGet( void )
{
    return NULL;
}

//================================================================================
// ::radSoundIntialize
//================================================================================

void radSoundHalSystemInitialize( radMemoryAllocator allocator )
{
    return;
}
//================================================================================
// ::radSoundIntialize
//================================================================================
        
void radSoundHalSystemTerminate( void )
{
    rAssert( radSoundHalSystem::s_pRsdSystem != NULL );

    radSoundHalSystem::s_pRsdSystem->Release( );
}











   
