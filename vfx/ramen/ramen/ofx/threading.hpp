// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_THREADING_HPP
#define	RAMEN_OFX_THREADING_HPP

#include"ofxCore.h"
#include"ofxMultiThread.h"

namespace ramen
{
namespace ofx
{

OfxStatus multiThread( OfxThreadFunctionV1 func, unsigned int nThreads, void *customArg);

OfxStatus multiThreadNumCPUs( unsigned int *nCPUs);
OfxStatus multiThreadIndex( unsigned int *threadIndex);
int multiThreadIsSpawnedThread(void);

OfxStatus mutexCreate( const OfxMutexHandle *mutex, int lockCount);
OfxStatus mutexDestroy( const OfxMutexHandle mutex);
OfxStatus mutexLock( const OfxMutexHandle mutex);
OfxStatus mutexUnLock( const OfxMutexHandle mutex);
OfxStatus mutexTryLock( const OfxMutexHandle mutex);

} // namespace
} // namespace

#endif	/* _THREADING_HPP */
