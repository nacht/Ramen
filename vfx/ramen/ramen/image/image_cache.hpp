// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_CACHE_HPP
#define	RAMEN_IMAGE_CACHE_HPP

#include<map>
#include<list>

#include<boost/noncopyable.hpp>
#include<boost/optional.hpp>

#include<loki/Singleton.h>

#include<adobe/md5.hpp>

#include<ramen/image/image_buffer.hpp>

namespace ramen
{
namespace image
{

class image_cache_impl : boost::noncopyable
{
public:

    bool empty() const;
    void clear();

    void insert( const adobe::md5_t::digest_t& key, image_buffer_t& img);
    boost::optional<image_buffer_t> find( const adobe::md5_t::digest_t& key, const Imath::Box2i& area);

    void remove_lru();

private:

    typedef std::multimap<adobe::md5_t::digest_t, image_buffer_t> map_type;
    typedef map_type::iterator map_iterator;

    friend struct Loki::CreateUsingNew<image_cache_impl>;

    image_cache_impl() {}

    void touch( map_iterator it);

    map_type items_;
    std::list<map_iterator> use_list_;

    // debugging
public:

    static bool log_image_cache_info;
};

typedef Loki::SingletonHolder<image_cache_impl> image_cache_t;

} // namespace
} // namespace

#endif


