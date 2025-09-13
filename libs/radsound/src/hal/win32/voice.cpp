//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  All rights reserved.
//=============================================================================

//========================================================================
// Include FilesalGenSources
//========================================================================

#include "pch.hpp"
#include "voice.hpp"
#include "listener.hpp"
#include "system.hpp"

//============================================================================
// Static Initialization
//============================================================================

template<> radSoundHalVoiceWin * radLinkedClass<radSoundHalVoiceWin>::s_pLinkedClassHead = NULL;
template<> radSoundHalVoiceWin * radLinkedClass<radSoundHalVoiceWin>::s_pLinkedClassTail = NULL;

//========================================================================
// radSoundHalVoiceWin::radSoundHalVoiceWin
//========================================================================

radSoundHalVoiceWin::radSoundHalVoiceWin( void )
    :
	m_Priority( 5 ),
    m_Pitch( 1.0f ),
    m_Volume( 1.0f ),
    m_MuteFactor( 1.0f ),
    m_Trim( 1.0f ),
	m_xRadSoundHalPositionalGroup( NULL )
{
  //  alGenSources( 1, &m_Source );
   // alSourcei( m_Source, AL_SOURCE_RELATIVE, AL_TRUE );
}

//========================================================================
// radSoundHalVoiceWin::~radSoundHalVoiceWin
//========================================================================

radSoundHalVoiceWin::~radSoundHalVoiceWin
(
    void
)
{
    //
    // Tell our buffer object that we are done with the voice/(buffer), our
    // buffer object manages the lifetime if its voices.
    //

    Stop( );

	if ( m_xRadSoundHalPositionalGroup != NULL )
	{
		m_xRadSoundHalPositionalGroup->RemovePositionalEntity( this );
	}

    if (m_Source)
    {
        alDeleteSources(1, &m_Source);
    }
}

void radSoundHalVoiceWin::SetPriority( unsigned int priority )
{
	m_Priority = priority;
}

unsigned int radSoundHalVoiceWin::GetPriority( void )
{
	return m_Priority;
}

//========================================================================
// radSoundHalVoiceWin::SetBuffer
//========================================================================

void radSoundHalVoiceWin::SetBuffer( IRadSoundHalBuffer * pIRadSoundHalBuffer )
{
    Stop( );

    m_xRadSoundHalBufferWin = NULL;
    ref< IRadSoundHalAudioFormat > pOldIRadSoundHalAudioFormat = m_xIRadSoundHalAudioFormat;
    m_xIRadSoundHalAudioFormat = NULL;

    if ( pIRadSoundHalBuffer != NULL )
    {
        m_xRadSoundHalBufferWin = static_cast< radSoundHalBufferWin * >( pIRadSoundHalBuffer );
        rAssert( m_xRadSoundHalBufferWin != NULL );
        m_xIRadSoundHalAudioFormat = m_xRadSoundHalBufferWin->GetFormat( );

        rAssert
        ( 
            pOldIRadSoundHalAudioFormat != NULL ?
            m_xIRadSoundHalAudioFormat->Matches( pOldIRadSoundHalAudioFormat ) :
            true
        );
    }

    if( m_xRadSoundHalPositionalGroup != NULL )
    {
        OnApplyPositionalInfo( 1.0f );
    }
}

IRadSoundHalBuffer * radSoundHalVoiceWin::GetBuffer( void )
{
    return m_xRadSoundHalBufferWin;
}

void radSoundHalVoiceWin::Play( )
{
    if (IsHardwarePlaying( ) == false)
    {
        alSourcePlay(m_Source);
        rWarningMsg(alGetError() == AL_NO_ERROR, "radSoundHalVoiceWin::Play failed");
    }
}

void radSoundHalVoiceWin::Stop( void )
{
    if (IsHardwarePlaying( ) == true)
    {
#ifdef RAD_DEBUG
        extern bool g_VoiceStoppingPlayingSilence;

        if ( g_VoiceStoppingPlayingSilence == false )
        {
            if ( ( m_Trim * m_Volume ) > 0.0f )
            {
                rDebugPrintf( "radsound: TRC Violation: Voice stopped while playing and (trim * volume) > 0.0f\n" );
            }
        }
#endif // RAD_DEBUG

        alSourceStop(m_Source);

        rWarningMsg(alGetError() == AL_NO_ERROR, "radSoundHalVoiceWin::Stop failed");
    }
}

bool radSoundHalVoiceWin::IsPlaying( void )
{
    return IsHardwarePlaying( );
}

unsigned int radSoundHalVoiceWin::GetPlaybackPositionInSamples( void )
{
 
    return 0;
}

void radSoundHalVoiceWin::SetPlaybackPositionInSamples( unsigned int positionInSamples )
{
 }

void radSoundHalVoiceWin::SetMuted( bool muted)
{
    if ( muted != GetMuted( ) )
    {
        m_MuteFactor = muted ? 0.0f : 1.0f;
        SetVolumeInternal( );
    }
}

bool radSoundHalVoiceWin::GetMuted( void )
{
    return m_MuteFactor == 0.0f ? true : false;
}

void radSoundHalVoiceWin::SetVolume( float volume )
{
	::radSoundVerifyAnalogVolume( volume );

    if ( volume != m_Volume )
    {
        ::radSoundVerifyChangeThreshold(
            IsHardwarePlaying( ), "Volume", volume, m_Volume, radSoundVolumeChangeThreshold );

		m_Volume = volume;

        SetVolumeInternal( );
    }

}

float radSoundHalVoiceWin::GetVolume( void )
{
    return m_Volume;
}

void radSoundHalVoiceWin::SetTrim( float trim )
{
	::radSoundVerifyAnalogVolume( trim );

    if ( m_Trim != trim )
    {
        ::radSoundVerifyChangeThreshold(
            IsHardwarePlaying( ), "Trim", trim, m_Trim, radSoundVolumeChangeThreshold );

        m_Trim = trim;

        SetVolumeInternal( );
    }
}
    
float radSoundHalVoiceWin::GetTrim( void )
{
    return m_Trim;
}

void radSoundHalVoiceWin::SetPitch( float pitch )
{
    ::radSoundVerifyAnalogPitch( pitch );

    if ( m_Pitch != pitch )
    {
        m_Pitch = pitch;

		SetPitchInternal( );
    }
}

float radSoundHalVoiceWin::GetPitch( void )
{
    return m_Pitch;
}

void radSoundHalVoiceWin::SetPan( float pan )
{
    ::radSoundVerifyAnalogPan( pan );

    rWarningMsg(false, "voice::SetPan not available in win32");
}

float radSoundHalVoiceWin::GetPan( void )
{
    rWarningMsg(false, "voice::GetPan not available in win32");
    return 0.0f;
}

radSoundAuxMode radSoundHalVoiceWin::GetAuxMode( unsigned int aux )
{
    rWarningMsg( false, "voice::GetAuxMode not available in win32" );
    return radSoundAuxMode_PreFader;
}

void radSoundHalVoiceWin::SetAuxMode( unsigned int aux, radSoundAuxMode  mode )
{
    rWarningMsg( false, "voice::SetAuxMode not available in win32" );
}

float radSoundHalVoiceWin::GetAuxGain( unsigned int aux )
{
    rWarningMsg( false, "voice::GetAuxGain not available in win32" );
    return 1.0f;
}

void radSoundHalVoiceWin::SetAuxGain( unsigned int aux, float gain )
{
    rWarningMsg( false, "voice::SetAuxGain not available in win32" );
}

//========================================================================
// Function radSoundHalVoiceWin::IsHardwarePlaying
//========================================================================

bool radSoundHalVoiceWin::IsHardwarePlaying( void )
{
    return false;

}

//========================================================================
// radSoundHalVoiceWin::SetVolumeInternal
//========================================================================

void radSoundHalVoiceWin::SetVolumeInternal( void )
{
    float volume = m_Trim * m_Volume * m_MuteFactor;

	alSourcef( m_Source, AL_GAIN, ::radSoundVolumeDbToHardwareWin( ::radSoundVolumeAnalogToDb( volume ) ) );

    rWarningMsg(alGetError() == AL_NO_ERROR, "radSoundHalVoiceWin::SetVolumeInternal failed!");
}

//========================================================================
// radSoundHalVoiceWin::SetPitchInternal
//========================================================================

void radSoundHalVoiceWin::SetPitchInternal( void )
{
    ::radSoundVerifyAnalogPitch(m_Pitch);

    alSourcef(m_Source, AL_PITCH, m_Pitch);

    rWarningMsg(alGetError() == AL_NO_ERROR, "radSoundHalVoiceWin::SetPitchInternal failed!");
}

//========================================================================
// radSoundHalVoiceWin::SetPositionalGroup
//========================================================================

/* virtual */ void radSoundHalVoiceWin::SetPositionalGroup
( 
	IRadSoundHalPositionalGroup * pIRadSoundHalPositionalGroup 
)
{
 
}

//========================================================================
// radSoundHalVoiceWin::GetPositionalGroup
//========================================================================

/* virtual */ IRadSoundHalPositionalGroup * radSoundHalVoiceWin::GetPositionalGroup
(	
	void 
)
{
	return m_xRadSoundHalPositionalGroup;
}


//========================================================================
// radSoundHalVoiceWin::SetPositionalGroup
//========================================================================

/* virtual */ void radSoundHalVoiceWin::OnApplyPositionalInfo( float listenerRolloffFactor )
{
	SetVolumeInternal( );

    radSoundHalPositionalGroup* p = m_xRadSoundHalPositionalGroup;
    rAssert( p );

    alSource3f(m_Source, AL_POSITION, p->m_Position.m_x, p->m_Position.m_y, -p->m_Position.m_z);
    alSource3f(m_Source, AL_VELOCITY, p->m_Velocity.m_x, p->m_Velocity.m_y, -p->m_Velocity.m_z);
    alSource3f(m_Source, AL_DIRECTION, p->m_Direction.m_x, p->m_Direction.m_y, -p->m_Direction.m_z);
    alSourcef(m_Source, AL_CONE_INNER_ANGLE, p->m_ConeOuterAngle);
    alSourcef(m_Source, AL_CONE_OUTER_ANGLE, p->m_ConeInnerAngle);
    alSourcef(m_Source, AL_CONE_OUTER_GAIN, p->m_ConeOuterGain);
    alSourcef(m_Source, AL_REFERENCE_DISTANCE, p->m_ReferenceDistance);
    alSourcef(m_Source, AL_MAX_DISTANCE, p->m_MaxDistance);
    alSourcef(m_Source, AL_ROLLOFF_FACTOR, listenerRolloffFactor);
    alSourcef(m_Source, AL_SOURCE_RELATIVE, AL_FALSE);

    rWarningMsg(alGetError() == AL_NO_ERROR, "radSoundHalVoiceWin::OnApplyPositionalInfo Failed.\n");
}

//========================================================================
// ::radSoundhalVoiceCreate
//========================================================================


IRadSoundHalVoice * radSoundHalVoiceCreate( radMemoryAllocator allocator )
{
    return new ( "radSoundHalVoiceWin", allocator ) radSoundHalVoiceWin( );
}




