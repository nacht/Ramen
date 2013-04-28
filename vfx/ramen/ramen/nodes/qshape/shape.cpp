// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/shape.hpp>

#include<sstream>
#include<algorithm>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<agg/agg_path_storage.h>
#include<agg/agg_conv_curve.h>
#include<agg/agg_conv_contour.h>
#include<agg/agg_bounding_rect.h>

#include<ramen/nodes/qshape/qshape_node.hpp>

#include<ramen/params/float_param.hpp>

#include<ramen/anim/track.hpp>

namespace ramen
{
namespace qshape
{

shape_t::shape_t( qshape_node_t *p) : parameterised_t(), closed_(false), selected_(false)
{
    set_parent( p);
    display_color_ = Imath::Color3f( 1, 1, 0);
    active_ = true;
    orientation_ = undefined;
}

void shape_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Color"));
    p->set_id( "color");
    p->set_range( 0, 1);
    p->set_default_value( 1);
    add_param( p);

    p.reset( new float_param_t( "Opacity"));
    p->set_id( "opacity");
    p->set_range( 0, 1);
    p->set_default_value( 1);
    add_param( p);

    p.reset( new float_param_t( "Grow"));
    p->set_id( "grow");
    p->set_default_value( 0);
    p->set_min( 0);
    add_param( p);

    p.reset( new float_param_t( "Blur"));
    p->set_id( "blur");
    p->set_default_value( 0);
    p->set_min( 0);
    add_param( p);
}

void shape_t::close()
{
    // calc orientation here

    // find the lower left vertex
    const_iterator lr_it( begin());

    for( const_iterator it( begin()); it != end(); ++it)
    {
	if( ( it->p1().y > lr_it->p1().y) || (( it->p1().y == lr_it->p1().y) && ( it->p1().x > lr_it->p1().x)))
	    lr_it = it;
    }

    const_iterator it0, it2;

    if( lr_it != begin())
	it0 = lr_it - 1;
    else
	it0 = end() - 1;

    if( lr_it != end() - 1)
	it2 = lr_it + 1;
    else
	it2 = begin();

    // this can fail if the points are colinear
    float area2 = it0->p1().x   * lr_it->p1().y - it0->p1().y * lr_it->p1().x +
		  it0->p1().y   * it2->p1().x   - it0->p1().x * it2->p1().y +
		  lr_it->p1().x * it2->p1().y   - it2->p1().x * lr_it->p1().y;

    if( area2 > 0)
        orientation_ = cw;
    else
        orientation_ = ccw;

    closed_ = true;
}

shape_t::iterator shape_t::insert( int span)
{
    shape_t::iterator it = begin() + span + 1;

    if( it >= end())
    {
	push_back( triple_t());
	return end() - 1;
    }
    else
	return triples().insert( it, triple_t());

    return end();
}

bool shape_t::valid() const { return triples().size() >= 3;}

std::string shape_t::str() const
{
    std::stringstream s;

    s << param_set().str();

    float coords[6];

    for( const_iterator it( begin()); it != end(); ++it)
    {
	coords[0] = it->p0().x;
	coords[1] = it->p0().y;
	coords[2] = it->p1().x;
	coords[3] = it->p1().y;
	coords[4] = it->p2().x;
	coords[5] = it->p2().y;
	s.write( reinterpret_cast<char *>( coords), sizeof( float) * 6);
    }

    s.write( reinterpret_cast<const char *>( &active_)	, sizeof( bool));
    return s.str();
}

Imath::Box2f shape_t::control_points_bounding_box( int subsample) const
{
    Imath::Box2f box;

    for( const_iterator it( begin()); it != end(); ++it)
    {
        box.extendBy( it->p0());
        box.extendBy( it->p1());
        box.extendBy( it->p2());
    }

    box.min /= subsample;
    box.max /= subsample;
    return box;
}

Imath::Box2f shape_t::bounding_box_at_time( float time, int subsample) const
{
    // this is potentially expensive.
    // caching the bbox could be interesting

    Imath::Box2f box;
    agg::path_storage path;

    convert_to_path_storage( path, time);
    agg::conv_curve<agg::path_storage> cpath( path);

    float grow = get_value_at_time<float>( param( "grow"), time) / subsample;

    if( grow != 0.0f)
    {
        agg::conv_contour<agg::conv_curve<agg::path_storage> > contour( cpath);
        set_conv_contour_options( contour);
        contour.width( -grow);
        agg::bounding_rect_single( contour, 0, &(box.min.x), &(box.min.y), &(box.max.x), &(box.max.y));
    }
    else
        agg::bounding_rect_single( cpath, 0, &(box.min.x), &(box.min.y), &(box.max.x), &(box.max.y));

    // blur is handled outside of this method

    // add some small margin for antialiasing
    box.min.x -= 2 * subsample;
    box.min.y -= 2 * subsample;
    box.max.x += 2 * subsample;
    box.max.y += 2 * subsample;

    // scale
    box.min /= subsample;
    box.max /= subsample;
    return box;
}

void shape_t::deselect_all_points()
{
    adobe::for_each( triples(), boost::bind( &triple_t::select, _1, false));
}

bool shape_t::any_point_selected() const
{
    for( const_iterator it( begin()); it != end(); ++it)
    {
	if( it->selected())
	    return true;
    }

    return false;
}

int shape_t::num_selected() const
{
    int count = 0;

    for( const_iterator it( begin()); it != end(); ++it)
    {
	if( it->selected())
	    ++count;
    }

    return count;
}

void shape_t::delete_selected_points()
{
    triples().erase( std::remove_if( begin(), end(), boost::bind( &triple_t::selected, _1)), end());
}

void shape_t::move( const Imath::V2f& off)
{
    adobe::for_each( triples(), boost::bind( &triple_t::move, _1, off));
}

void shape_t::move_selection( const Imath::V2f& off)
{
    BOOST_FOREACH( triple_t& t, triples())
    {
        if( t.selected())
            t.move( off);
    }
}

void shape_t::create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( "Shape"));
    adobe::for_each( param_set(), boost::bind( &param_t::create_tracks, _1, t.get()));

    std::auto_ptr<anim::track_t> geom( new anim::track_t( "Points"));
    adobe::for_each( triples(), boost::bind( &triple_t::create_tracks, _1, geom.get()));

    t->add_child( geom);

    parent->add_child( t);
}

void shape_t::evaluate( float time)
{
    parameterised_t::evaluate( time);
    adobe::for_each( triples(), boost::bind( &triple_t::evaluate, _1, time));
}

void shape_t::set_shape_keyframe( float time)
{
    adobe::for_each( triples(), boost::bind( &triple_t::set_keyframe, _1, time));
}

void shape_t::convert_to_corner_point( float time, iterator it)
{
    it->convert_to_corner();
    it->set_keyframe( time);
}

void shape_t::convert_to_curve_point( float time, iterator it)
{
    iterator prev, next;

    if( it == begin())
	prev = end() - 1;
    else
	prev = it - 1;

    if( it == end()-1)
	next = begin();
    else
	next = it + 1;

    Imath::V2f tn( next->p1() - prev->p1());
    tn *= 0.25f; // ????

    it->set_p0( it->p1() - tn);
    it->set_p2( it->p1() + tn);
    it->convert_to_curve();
    it->set_keyframe( time);
}

void shape_t::toggle_corner_curve_point( float time, iterator it)
{
    if( it->corner())
	convert_to_curve_point( time, it);
    else
	convert_to_corner_point( time, it);
}

void shape_t::toggle_corner_curve_selected_points( float time)
{
    for( iterator it( begin()); it != end(); ++it)
    {
	if( it->selected())
	    toggle_corner_curve_point( time, it);
    }
}

void shape_t::set_color_at_time( float color, float t)
{
    float_param_t *p = dynamic_cast<float_param_t*>( &param( "color"));
    assert( p);
    p->set_value_at_time( color, t);
}

void shape_t::set_opacity_at_time( float opacity, float t)
{
    float_param_t *p = dynamic_cast<float_param_t*>( &param( "opacity"));
    assert( p);
    p->set_value_at_time( opacity, t);
}

void shape_t::set_grow_at_time( float grow, float t)
{
    float_param_t *p = dynamic_cast<float_param_t*>( &param( "grow"));
    assert( p);
    p->set_value_at_time( grow, t);
}

void shape_t::set_blur_at_time( float blur, float t)
{
    float_param_t *p = dynamic_cast<float_param_t*>( &param( "blur"));
    assert( p);
    p->set_value_at_time( blur, t);
}

void shape_t::param_edit_finished()
{
    parent()->param_edit_finished();
}

void shape_t::convert_to_path_storage( agg::path_storage& path, float time, float subsample, const Imath::V2i offset) const
{
    Imath::V2f p( begin()->p1_at_time( time));
    p /= subsample; p -= offset;
    path.move_to( p.x, p.y);

    for( const_iterator it( begin()); it != end()-1; ++it)
    {
        Imath::V2f b( it->p2_at_time( time));
        Imath::V2f c( (it+1)->p0_at_time( time));
        Imath::V2f d( (it+1)->p1_at_time( time));

        b /= subsample; b -= offset;
        c /= subsample; c -= offset;
        d /= subsample; d -= offset;
        path.curve4( b.x, b.y, c.x, c.y, d.x, d.y);
    }

    Imath::V2f b( (end() - 1)->p2_at_time( time));
    Imath::V2f c( begin()->p0_at_time( time));
    Imath::V2f d( begin()->p1_at_time( time));

    b /= subsample; b -= offset;
    c /= subsample; c -= offset;
    d /= subsample; d -= offset;
    path.curve4( b.x, b.y, c.x, c.y, d.x, d.y);

    if( orientation() == cw)
        path.close_polygon( agg::path_flags_cw);
    else
        path.close_polygon( agg::path_flags_ccw);
}

void shape_t::set_conv_contour_options( agg::conv_contour<agg::conv_curve<agg::path_storage> >& conv) const
{
//    conv.line_join( agg::miter_join);
//    conv.inner_join( agg::inner_jag);
//    conv.inner_miter_limit( 2);
//    conv.miter_limit_theta( 90)
    conv.auto_detect_orientation( false);
}

void shape_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( parameterised_t);
    ar & boost::serialization::make_nvp( "active", active_);
    ar & boost::serialization::make_nvp( "display_color", display_color_);
    ar & boost::serialization::make_nvp( "closed", closed_);
    ar & boost::serialization::make_nvp( "orientation", orientation_);
    ar & boost::serialization::make_nvp( "triples", triples_);
}

void shape_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( parameterised_t);
    ar & boost::serialization::make_nvp( "active", active_);
    ar & boost::serialization::make_nvp( "display_color", display_color_);
    ar & boost::serialization::make_nvp( "closed", closed_);
    ar & boost::serialization::make_nvp( "orientation", orientation_);
    ar & boost::serialization::make_nvp( "triples", triples_);
}

} // namespace
} // namespace
