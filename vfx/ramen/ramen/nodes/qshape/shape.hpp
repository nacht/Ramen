// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_SHAPE_HPP
#define	RAMEN_QSHAPE_SHAPE_HPP

#include<ramen/params/parameterised.hpp>

#include<boost/serialization/vector.hpp>

#include<OpenEXR/ImathBox.h>

#include<agg/agg_path_storage.h>
#include<agg/agg_conv_curve.h>
#include<agg/agg_conv_contour.h>

#include<ramen/ImathExt/serialization/ImathColor.hpp>

#include<ramen/nodes/qshape/qshape_node_fwd.hpp>
#include<ramen/nodes/qshape/triple.hpp>

#include<ramen/anim/track_fwd.hpp>

namespace ramen
{
namespace qshape
{

class shape_t : public parameterised_t
{
public:

    enum orientation_t
    {
        cw = 0,
        ccw,
        undefined
    };

    shape_t( qshape_node_t *p = 0);

    const qshape_node_t *parent() const { return parent_;}
    qshape_node_t *parent()             { return parent_;}
    void set_parent( qshape_node_t *p)  { parent_ = p;}

    bool valid() const;

    bool closed() const { return closed_;}
    void close();

    bool selected() const	    { return selected_;}
    void select( bool b) const	    { selected_ = b;}

    const Imath::Color3f& display_color() const	    { return display_color_;}
    void set_display_color( const Imath::Color3f& c) const { display_color_ = c;}

    bool active() const		{ return active_;}
    void set_active( bool b)	{ active_ = b;}

    bool empty() const { return triples_.empty();}
    std::size_t size() const { return triples_.size();}

    triple_t& front()		    { return triples_.front();}
    const triple_t& front() const   { return triples_.front();}

    triple_t& back()		    { return triples_.back();}
    const triple_t& back() const    { return triples_.back();}

    typedef std::vector<triple_t>::iterator iterator;

    iterator begin()	{ return triples_.begin();}
    iterator end()	{ return triples_.end();}

    typedef std::vector<triple_t>::const_iterator const_iterator;

    const_iterator begin() const    { return triples_.begin();}
    const_iterator end() const	    { return triples_.end();}

    std::vector<triple_t>& triples()		    { return triples_;}
    const std::vector<triple_t>& triples() const    { return triples_;}

    void push_back( const triple_t& t) { triples_.push_back( t);}
    iterator insert( int span);

    std::string str() const;

    Imath::Box2f control_points_bounding_box( int subsample = 1) const;
    Imath::Box2f bounding_box_at_time( float time, int subsample = 1) const;

    orientation_t orientation() const { return orientation_;}

    // point selection
    void deselect_all_points();
    bool any_point_selected() const;
    int num_selected() const;

    // drag
    void move( const Imath::V2f& off);
    void move_selection( const Imath::V2f& off);

    // insert / remove
    void delete_selected_points();

    // corner / curve
    void convert_to_corner_point( float time, iterator it);
    void convert_to_curve_point( float time, iterator it);

    void toggle_corner_curve_point( float time, iterator it);

    void toggle_corner_curve_selected_points( float time);

    // anim
    void create_tracks( anim::track_t *parent);
    void evaluate( float time);
    void set_shape_keyframe( float time);

    // params
    void set_color_at_time( float color, float t);
    void set_opacity_at_time( float opacity, float t);
    void set_grow_at_time( float grow, float t);
    void set_blur_at_time( float blur, float t);

    virtual void param_edit_finished();

    // agg utility functions
    void convert_to_path_storage( agg::path_storage& path, float time, float subsample = 1.0f, const Imath::V2i offset = Imath::V2i( 0, 0)) const;
    void set_conv_contour_options( agg::conv_contour<agg::conv_curve<agg::path_storage> >& conv) const;

private:

    virtual void do_create_params();
    
    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    qshape_node_t *parent_;

    std::vector<triple_t> triples_;

    bool closed_;
    bool active_;

    mutable bool selected_;

    mutable Imath::Color3f display_color_;

    orientation_t orientation_;
};

} // namespace
} // namespace

#endif
