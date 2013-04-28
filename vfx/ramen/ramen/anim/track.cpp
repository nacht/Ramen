// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/track.hpp>

namespace ramen
{
namespace anim
{

track_t::track_t() : parent_(0), color_( 255, 255, 255)
{
}

track_t::track_t( const std::string& name, anim::curve_t *curve, float curve_scale) : name_(name), curve_(curve), parent_(0), color_( 255, 255, 255)
{
}

track_t *track_t::child( int i)
{
    assert( i >= 0);
    assert( i < num_children());
    return &children_[i];
}

void track_t::add_child( std::auto_ptr<track_t> t)
{
    t->set_parent( this);
    children_.push_back( t);
}

void track_t::clear_children() { children_.clear();}

int track_t::row() const
{
    for( int i = 0; i < parent()->num_children(); ++i)
    {
	if( &( parent()->children()[i]) == this)
	    return i;
    }

    return 0;
}

void track_t::set_color( const Imath::Color3c& col) { color_ = col;}

void track_t::notify()
{
    changed( curve());

    track_t *p = parent();

    while( p)
    {
	p->notify();
	p = p->parent();
    }
}

void track_t::notify_selected( bool b)
{
    selected( b);
    //for_each_track_depth_first( this, boost::bind( &track_t::selected, _1, b));
}

} // namespace
} // namespace
