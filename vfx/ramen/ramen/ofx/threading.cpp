// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/threading.hpp>

#include<boost/bind.hpp>

#include<boost/thread/thread.hpp>
#include<boost/thread/recursive_mutex.hpp>
#include<boost/thread/tss.hpp>

#include<ramen/app/application.hpp>

namespace ramen
{
namespace ofx
{

namespace
{

struct thread_specific_data
{
    thread_specific_data( int num) : thread_num( num) {}
    int thread_num;
};

boost::thread_specific_ptr<thread_specific_data> ptr;

void run_in_thread( OfxThreadFunctionV1 f, unsigned int threadIndex, unsigned int threadMax, void *customArg)
{
    ptr.reset( new thread_specific_data( threadIndex));
    f( threadIndex, threadMax, customArg);
}

} // unnamed

OfxStatus multiThread( OfxThreadFunctionV1 func, unsigned int nThreads, void *customArg)
{
    if( nThreads == 0 )
	return kOfxStatErrValue;

    if( nThreads == 1)
	func( 0, 1, customArg);
    else
    {
	boost::thread_group g;

	for( int i = 0; i < nThreads; ++i)
	    g.create_thread( boost::bind( run_in_thread, func, i, nThreads, customArg));

	g.join_all();
    }

    return kOfxStatOK;
}

OfxStatus multiThreadNumCPUs( unsigned int *nCPUs)
{
    //*nCPUs = 1;
    *nCPUs = ramen::application_t::Instance().max_threads();
    return kOfxStatOK;
}

OfxStatus multiThreadIndex( unsigned int *threadIndex)
{
    if( ptr.get())
    {
	*threadIndex = ptr->thread_num;
	return kOfxStatOK;
    }
    else
    {
	*threadIndex = 0;
	return kOfxStatFailed;
    }
}

int multiThreadIsSpawnedThread(void) { return ptr.get() != 0;}

OfxStatus mutexCreate( const OfxMutexHandle *mutex, int lockCount)
{
    boost::recursive_mutex *m = new boost::recursive_mutex();

    if( m == 0)
	return kOfxStatFailed;

    for( int i = 0; i < lockCount; ++i)
	m->lock();

    OfxMutexHandle *mutex_ = const_cast<OfxMutexHandle*>( mutex);
    *mutex_ = reinterpret_cast<OfxMutexHandle>( m);
    return kOfxStatOK;
}

OfxStatus mutexDestroy( const OfxMutexHandle mutex)
{
    if( !mutex)
	return kOfxStatErrBadHandle;

    OfxMutexHandle mutex_ = const_cast<OfxMutexHandle>( mutex);
    delete reinterpret_cast<boost::recursive_mutex*>( mutex_);
    return kOfxStatOK;
}

OfxStatus mutexLock( const OfxMutexHandle mutex)
{
    if( !mutex)
	return kOfxStatErrBadHandle;

    boost::recursive_mutex *m = reinterpret_cast<boost::recursive_mutex*>( mutex);
    m->lock();
    return kOfxStatOK;
}

OfxStatus mutexUnLock( const OfxMutexHandle mutex)
{
    if( !mutex)
	return kOfxStatErrBadHandle;

    boost::recursive_mutex *m = reinterpret_cast<boost::recursive_mutex*>( mutex);
    m->unlock();
    return kOfxStatOK;
}

OfxStatus mutexTryLock( const OfxMutexHandle mutex)
{
    if( !mutex)
	return kOfxStatErrBadHandle;

    boost::recursive_mutex *m = reinterpret_cast<boost::recursive_mutex*>( mutex);

    if( m->try_lock())
	return kOfxStatOK;
    else
	return kOfxStatFailed;
}

} // namespace
} // namespace
