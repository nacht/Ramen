// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_TRACK_HPP
#define	RAMEN_ANIM_TRACK_HPP

#include<string>

#include<memory>

#include<boost/ptr_container/ptr_vector.hpp>
#include<boost/signal.hpp>

#include<OpenEXR/ImathColor.h>

#include<ramen/anim/curve.hpp>

namespace ramen
{
namespace anim
{

class track_t
{
public:

    track_t();
    explicit track_t( const std::string& name, anim::curve_t *curve = 0, float curve_scale = 1.0f);

    track_t *parent() const	    { return parent_;}
    void set_parent( track_t *p)    { parent_ = p;}

    int num_children() const { return (int) children_.size();}

    track_t *child( int i);

    void add_child( std::auto_ptr<track_t> t);

    void clear_children();

    int row() const;

    // ranges
    const boost::ptr_vector<track_t>& children() const	{ return children_;}
    boost::ptr_vector<track_t>& children()		{ return children_;}

    const std::string& name() const	    { return name_;}
    void set_name( const std::string& name) { name_ = name;}

    bool is_leaf() const { return num_children() == 0;}

    const anim::curve_t *curve() const	{ return curve_;}
    anim::curve_t *curve()		{ return curve_;}

    const Imath::Color3c& color() { return color_;}
    void set_color( const Imath::Color3c& col);

    // observer
    boost::signal<void ( anim::curve_t *)> changed;
    boost::signal<void ( bool)> selected;

    void notify();
    void notify_selected( bool b);

private:

    std::string name_;
    track_t *parent_;
    boost::ptr_vector<track_t> children_;
    anim::curve_t *curve_;
    Imath::Color3c color_;
};

template<class Fun>
void for_each_track_depth_first( track_t *t, Fun f)
{
    f( t);

    for( int i = 0; i < t->num_children(); ++i)
        for_each_track_depth_first( t->child( i), f);
}

template<class Fun>
void for_each_leaf_track( track_t *t, Fun f)
{
    if( t->is_leaf())
	f( t);
    else
    {
	for( int i = 0; i < t->num_children(); ++i)
	    for_each_leaf_track( t->child( i), f);
    }
}

template<class Fun>
void for_each_curve( track_t *t, Fun f)
{
    if( t->is_leaf() && t->curve())
	f( *(t->curve()));
    else
    {
	for( int i = 0; i < t->num_children(); ++i)
	    for_each_curve( t->child( i), f);
    }
}

} // namespace
} // namespace

#endif	/* _TRACK_HPP */
