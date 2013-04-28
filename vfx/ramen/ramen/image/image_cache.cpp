// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/image_cache.hpp>

#include<iostream>

#include<ramen/ImathExt/ImathBoxAlgo.h>

namespace ramen
{
namespace image
{

bool image_cache_impl::log_image_cache_info = true;

void image_cache_impl::clear()
{
    items_.clear();
    use_list_.clear();
}

bool image_cache_impl::empty() const { return items_.empty();}

void image_cache_impl::insert( const adobe::md5_t::digest_t& key, image_buffer_t& img)
{
    if( items_.find( key) != items_.end())
    {
	// check to see if the area this image buffer represents
	// is already included in the cache
	std::pair<map_iterator, map_iterator> range = items_.equal_range( key);
	for( map_iterator it( range.first); it != range.second; ++it)
	{
	    if( isInside( it->second.bounds(), img.bounds()))
	    {
		// in this case, just move the buffer to the front
		// of the use list
		touch( it);
		return;
	    }
	}
    }

    // insert the image buffer in the cache
    img.set_cached( true);
    map_iterator result( items_.insert( map_type::value_type( key, img)));
    use_list_.push_front( result);

    // remove image buffers included inside this one
    std::pair<map_iterator, map_iterator> range = items_.equal_range( key);
    for( map_iterator it( range.first); it != range.second; )
    {
	if( it != result)
	{
	    if( isInside( img.bounds(), it->second.bounds()))
	    {
		use_list_.remove( it);
		items_.erase( it++);
	    }
	    else
		++it;
	}
	else
	    ++it;
    }
}

boost::optional<image_buffer_t> image_cache_impl::find( const adobe::md5_t::digest_t& key, const Imath::Box2i& area)
{
    if( items_.find( key) != items_.end())
    {
	std::pair<map_iterator, map_iterator> range = items_.equal_range( key);
	for( map_iterator it( range.first); it != range.second; ++it)
	{
	    if( isInside( it->second.bounds(), area))
	    {
		touch( it);
		return it->second;
	    }
	}
    }

    return boost::optional<image_buffer_t>();
}

void image_cache_impl::touch( map_iterator it)
{
    use_list_.remove( it);
    use_list_.push_front( it);
}

void image_cache_impl::remove_lru()
{
    map_iterator it( use_list_.back());
    use_list_.pop_back();
    items_.erase( it);
}

} // namespace
} // namespace
