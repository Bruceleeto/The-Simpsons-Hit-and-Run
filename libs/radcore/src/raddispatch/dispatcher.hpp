//=============================================================================
// Copyright (c) 2002 Radical Games Ltd.  
//=============================================================================

//=============================================================================
//
// File:        dispatcher.hpp
//
// Subsystem:	Foundation Technologies - Dispatcher
//
// Description:	This file is the actual class definition used to implement the
//              interface defined by the dispatcher.
//
//				For complete description of Manager, refer to the Foundation
//              Technologies technical reference manual.
//
// Revisions:   Mar 12, 2001     Creation
//              Sep 11, 2025     Modernized to std::mutex
//
//=============================================================================

#ifndef	DISPATCHER_HPP
#define DISPATCHER_HPP

//=============================================================================
// Include Files
//=============================================================================

#include <mutex>

#include <raddispatch.hpp>
#include <radobject.hpp>
#include <radmemory.hpp>

//=============================================================================
// Class Declarations
//=============================================================================

// 
// This is the Implementation of the Dispatcher. 
//
class radDispatcher : public IRadDispatcher,
                      public radObject
{
public:

    //
    // Constructor. Pass allocator and max event to queue.
    //
    radDispatcher( unsigned int maxCallbacks, radMemoryAllocator alloc );
    virtual ~radDispatcher( void );   // Only virtual to prevent warning

    //
    // Virtual members to implement.
    //
    virtual unsigned int Service( void ); 

    //
    // Use this member to queue a dispatch callback for deferred execution.
    //
    virtual void QueueCallback( IRadDispatchCallback* pDispatchCallback, void* userData );

    //
    // This member also queues an event. However, it supports queuing from
    // an interrupt context. It will not modify the state of the interrupt mask. 
    //
    virtual void QueueCallbackFromInterrupt( IRadDispatchCallback* pDispatchCallback, void* userData );

    //
    // Used for reference counting.
    //
    virtual void AddRef( void ); 
    virtual void Release( void );
    
#ifdef RAD_DEBUG
    virtual void Dump( char * pStringBuffer, unsigned int bufferSize );
#endif

private:
    
    //
    // Reference count.
    //
    unsigned int        m_ReferenceCount;

    //
    // Circular queue of dispatch objects.
    //
    struct Event
    {
        IRadDispatchCallback* m_Callback;
        void*                 m_UserData;
    };             

    Event*              m_EventQueue; 
    unsigned int        m_MaxEvents;
    unsigned int        m_EventQueueHeadIndex;
    unsigned int        m_EventQueueTailIndex;
    unsigned int        m_EventsQueued;

    std::mutex          m_Mutex;   // Modern replacement for SDL_mutex
};

#endif
