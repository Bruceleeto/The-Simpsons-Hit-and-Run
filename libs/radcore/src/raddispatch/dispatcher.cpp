//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.
//=============================================================================

//=============================================================================
//
// File:        dispatcher.cpp
//
// Subsystem:	Foundation Technologies - Dispatcher
//
// Description:	This file contains the implementation of the Foundation 
//              Technologies Dispatcher. The dispatcher is responsible
//              for controlling the flow of low level events within the system.
//
// Date:    	Mar 12, 2001
// Revisions:  Sep 11, 2025     Modernized to std::mutex
//
//=============================================================================

//=============================================================================
// Include Files
//=============================================================================

#include "pch.hpp"

#include <raddispatch.hpp>
#include <radobject.hpp>
#include <radmemory.hpp>
#include <raddebug.hpp>
#include <radmemorymonitor.hpp>

#include "dispatcher.hpp"

//=============================================================================
// Public Member Functions
//=============================================================================

//=============================================================================
// Function:    radDispatchCreate
//=============================================================================

void radDispatchCreate
( 
    IRadDispatcher**   pIRadDispatcher, 
    unsigned int       maxCallbacks,
    radMemoryAllocator alloc
)
{
    *pIRadDispatcher = new( alloc ) radDispatcher( maxCallbacks, alloc );
}

//=============================================================================
// Function:    radDispatcher::radDispatcher
//=============================================================================

radDispatcher::radDispatcher
( 
    unsigned int maxCallbacks,
    radMemoryAllocator alloc    
)
    :
    m_ReferenceCount( 1 ),
    m_MaxEvents( maxCallbacks ),
    m_EventQueueHeadIndex( 0 ),
    m_EventQueueTailIndex( 0 ),
    m_EventsQueued( 0 )
{
    radMemoryMonitorIdentifyAllocation( this, g_nameFTech, "radDispatcher" );

    // Allocate memory to use for queuing events.
    m_EventQueue = (Event*) radMemoryAlloc( alloc, sizeof(Event) * m_MaxEvents );
}

//=============================================================================
// Function:    radDispatcher::~radDispatcher
//=============================================================================

radDispatcher::~radDispatcher( void )
{
    // If this asserts the caller did not call purge.
    rAssert( m_EventsQueued == 0 );

    // Free up the memory
    radMemoryFree( m_EventQueue );
}

//=============================================================================
// Function:    radDispatcher::AddRef
//=============================================================================

void radDispatcher::AddRef( void )
{
    m_ReferenceCount++;
}

//=============================================================================
// Function:    radDispatcher::Release
//=============================================================================

void radDispatcher::Release( void )
{
    m_ReferenceCount--;
    
    if( m_ReferenceCount == 0 )
    {
       delete this;
    }
}

//=============================================================================
// Function:    radDispatcher::Dump
//=============================================================================

#ifdef RAD_DEBUG
void radDispatcher::Dump( char * pStringBuffer, unsigned int bufferSize )
{
    sprintf( pStringBuffer, "Object: [radDispatcher] At Memory Location:[%p]\n", this );
}
#endif

//=============================================================================
// Function:    radDispatcher::QueueCallback
//=============================================================================

void radDispatcher::QueueCallback
( 
    IRadDispatchCallback* pDispatchCallback,
    void*                 userData 
)
{
    pDispatchCallback->AddRef();

    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        // Assert that we have not exceeded the maximum number of events in the queue.
        rAssert( m_EventsQueued != m_MaxEvents );                         

        // Add it to the queue at the head.
        m_EventQueue[m_EventQueueHeadIndex].m_Callback = pDispatchCallback;
        m_EventQueue[m_EventQueueHeadIndex].m_UserData = userData;
        m_EventQueueHeadIndex++;
        if( m_EventQueueHeadIndex == m_MaxEvents )
        {
            m_EventQueueHeadIndex = 0;
        }        
        m_EventsQueued++;
    }
}

//=============================================================================
// Function:    radDispatcher::QueueCallbackFromInterrupt
//=============================================================================

void radDispatcher::QueueCallbackFromInterrupt
( 
    IRadDispatchCallback* pDispatchCallback,
    void*                 userData 
)
{
#if defined ( RAD_WIN32 ) || defined( RAD_XBOX )
    (void) pDispatchCallback;
    (void) userData;
    rAssert( false );
#endif

#if defined( RAD_PS2 ) || defined( RAD_GAMECUBE )
    pDispatchCallback->AddRef();

    rAssert( m_EventsQueued != m_MaxEvents );                         

    m_EventQueue[m_EventQueueHeadIndex].m_Callback = pDispatchCallback;
    m_EventQueue[m_EventQueueHeadIndex].m_UserData = userData;
    m_EventQueueHeadIndex++;
    if( m_EventQueueHeadIndex == m_MaxEvents )
    {
        m_EventQueueHeadIndex = 0;
    }        
    m_EventsQueued++;
#endif
}

//=============================================================================
// Function:    radDispatcher::Service
//=============================================================================

unsigned int radDispatcher::Service( void )
{
    unsigned int eventsToDispatch = m_EventsQueued;

#ifdef RAD_PS2
    ThreadParam threadInfo;
    ReferThreadStatus( GetThreadId( ), &threadInfo );
#endif

    while ((m_EventsQueued != 0) && (eventsToDispatch != 0))
    {
        Event event;

        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            if (m_EventsQueued == 0)
            {
                break;
            }

            event = m_EventQueue[m_EventQueueTailIndex];
            m_EventQueueTailIndex++;
            if (m_EventQueueTailIndex == m_MaxEvents)
            {
                m_EventQueueTailIndex = 0;
            }        
            m_EventsQueued--;
            eventsToDispatch--;
        }

        // Invoke the event handler outside the lock
        event.m_Callback->OnDispatchCallack(event.m_UserData);

        // Release our reference
        event.m_Callback->Release();   // don't call radRelease() to report to memory monitor

#ifdef RAD_PS2
        RotateThreadReadyQueue( threadInfo.currentPriority );
#endif
    }

    return m_EventsQueued;
}
