// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_MEMORY_MANAGER_HPP
#define RAMEN_IMAGE_MEMORY_MANAGER_HPP

#include<memory>

#include<boost/noncopyable.hpp>
#include<boost/cstdint.hpp>

#include<boost/thread/recursive_mutex.hpp>

#include<loki/Singleton.h>

namespace ramen
{

class image_memory_manager_impl : boost::noncopyable
{
public:

    void init( boost::uint64_t size);

    boost::uint64_t max_size() const        { return max_size_;}

    unsigned char *allocate( std::size_t s, const void *hint = 0);
    void deallocate( unsigned char *p, std::size_t s);

private:

    bool free_space( std::size_t s, bool nothrow = false);

    friend struct Loki::CreateUsingNew<image_memory_manager_impl>;

    image_memory_manager_impl();
    ~image_memory_manager_impl();

    boost::uint64_t max_size_;
    boost::uint64_t allocated_, max_allocated_;

    // debugging
public:

    static bool log_mem_info;
    boost::uint64_t db_alloc, db_free;

    boost::recursive_mutex mutex_;
};

typedef Loki::SingletonHolder<image_memory_manager_impl> image_memory_manager_t;

} // namespace

#endif
