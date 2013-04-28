// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/memory/image_memory_manager.hpp>

#include<iostream>

#include<boost/thread/locks.hpp>

#include<ramen/image/image_cache.hpp>

namespace ramen
{

bool image_memory_manager_impl::log_mem_info = true;

image_memory_manager_impl::image_memory_manager_impl()
{
    allocated_ = max_allocated_ = 0;
    max_size_ = 0;
    db_alloc = db_free = 0;
}

image_memory_manager_impl::~image_memory_manager_impl()
{
    std::cout << "Image memory stats\n";
    std::cout << "------------------\n";
    std::cout << "Max Size = "	<< max_size_ / 1024 / 1024 << " Mb\n";
    std::cout << "Max Allocated = " << max_allocated_ / 1024 / 1024 << " Mb\n\n";

    if( log_mem_info)
    {
        std::cout << "Image Buffers: Allocated = " << image::image_buffer_t::num_ib_allocs <<
                    ", Freed = " << image::image_buffer_t::num_ib_freed;

        if( image::image_buffer_t::num_ib_allocs != image::image_buffer_t::num_ib_freed)
            std::cout << " Warning: alloc != freed";

        std::cout << std::endl;

        std::cout << "Memory counter: Alloc = " << db_alloc / 1024 << " Kb, Free = " << db_free / 1024 << " Kb ";

        if( db_alloc != db_alloc)
            std::cout << " Warning: alloc != freed";

        std::cout << std::endl;
    }
}

void image_memory_manager_impl::init( boost::uint64_t size) { max_size_ = size;}

unsigned char *image_memory_manager_impl::allocate( std::size_t s, const void *hint)
{
    boost::lock_guard<boost::recursive_mutex> lock( mutex_);

    free_space( s);
    allocated_ += s;
    max_allocated_ = std::max( allocated_, max_allocated_);
    db_alloc += s;
    return new unsigned char[s];
}

void image_memory_manager_impl::deallocate( unsigned char *p, std::size_t s)
{
    boost::lock_guard<boost::recursive_mutex> lock( mutex_);

    allocated_ -= s;
    db_free += s;
    delete[] p;
}

bool image_memory_manager_impl::free_space( std::size_t s, bool nothrow)
{
    if( s == 0)
	return true;
    
    while( allocated_ + s > max_size_)
    {
	if( image::image_cache_t::Instance().empty())
	{
	    if( !nothrow)
		throw std::bad_alloc();

	    return false;
	}

	image::image_cache_t::Instance().remove_lru();
    }

    return true;
}

} // namespace
