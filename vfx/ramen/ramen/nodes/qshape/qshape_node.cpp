// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/qshape/qshape_node.hpp>

#include<cmath>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>
#include<boost/ptr_container/serialize_ptr_vector.hpp>
#include<boost/gil/algorithm.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<QMouseEvent>
#include<QKeyEvent>

#include<ramen/ui/image_view.hpp>

#include<ramen/nodes/qshape/shape.hpp>
#include<ramen/nodes/qshape/qshape_toolbar.hpp>
#include<ramen/nodes/qshape/qshape_tool.hpp>

#include<ramen/nodes/qshape/shape_renderer.hpp>

#include<ramen/params/separator_param.hpp>
#include<ramen/params/motion_blur_param.hpp>

#include<ramen/image/image_accumulator.hpp>

#include<iostream>

namespace ramen
{

qshape_node_t::qshape_node_t() : generator_node_t(), toolbar_(0), view_( this)
{
    set_name( "qshape");
    set_can_motion_blur( true);
    selected_ = 0;
    create_ = 0;
    select_ = 0;
    convert_ = 0;
    tool_ = 0;
}

qshape_node_t::qshape_node_t( const qshape_node_t& other) : generator_node_t( other), shapes_( other.shapes_), view_( this)
{
    toolbar_ = 0;
    selected_ = 0;
    create_ = 0;
    select_ = 0;
    convert_ = 0;
    tool_ = 0;
}

qshape_node_t::~qshape_node_t()
{
    delete create_;
    delete select_;
    delete convert_;
}

std::auto_ptr<qshape::shape_t> qshape_node_t::create_shape()
{
    assert( toolbar_);
    std::auto_ptr<qshape::shape_t> p( new qshape::shape_t( this));
    p->param_set().param_changed.connect( boost::bind( &qshape::qshape_toolbar_t::param_changed, toolbar_.data(), _1));
    return p;
}

void qshape_node_t::add_shape( qshape::shape_t *s) { shapes_.push_back( s);}

qshape::shape_t *qshape_node_t::pop_back_shape() { return shapes_.pop_back().release();}

int qshape_node_t::index_for_shape( qshape::shape_t *s) const
{
    for( int i = 0; i < shapes().size(); ++i)
    {
	if( s == &( shapes()[i]))
	    return i;
    }

    return -1;
}

void qshape_node_t::insert_shape( qshape::shape_t *s, int index)
{
    assert( index >= 0);

    if( index >= shapes().size())
	shapes().push_back( s);
    else
	shapes().insert( shapes().begin() + index, s);
}

qshape::shape_t *qshape_node_t::release_shape( int index)
{
    assert( index >= 0);
    assert( index < shapes().size());

    iterator it( shapes().begin() + index);
    return shapes().release( it).release();
}

void qshape_node_t::deselect_all()
{
    adobe::for_each( shapes(), boost::bind( &qshape::shape_t::select, _1, false));
    adobe::for_each( shapes(), boost::bind( &qshape::shape_t::deselect_all_points, _1));
    selected_ = 0;
    selection_changed( selected_);
}

void qshape_node_t::select( qshape::shape_t *s)
{
    assert( !selected_);
    selected_ = s;
    s->select( true);
    selection_changed( selected_);
}

void qshape_node_t::pick_select_tool()
{
    if( tool_)
	tool_->deselect();

    tool_ = select_;
}

void qshape_node_t::pick_create_tool()
{
    if( tool_)
	tool_->deselect();
    
    tool_ = create_;
}

void qshape_node_t::pick_convert_tool()
{
    if( tool_)
	tool_->deselect();

    tool_ = convert_;
}

void qshape_node_t::do_create_params()
{ 
    generator_node_t::do_create_params();

    std::auto_ptr<separator_param_t> sep( new separator_param_t());
    add_param( sep);

    std::auto_ptr<motion_blur_param_t> mb( new motion_blur_param_t( "Motion Blur"));
    mb->set_id( "motion_blur");
    add_param( mb);
}

const motion_blur_param_t *qshape_node_t::motion_blur_param() const
{
    const param_t *p = &param( "motion_blur");
    const motion_blur_param_t *mb = dynamic_cast<const motion_blur_param_t*>( p);
    assert( mb);
    return mb;
}

void qshape_node_t::do_set_frame( float f)
{
    adobe::for_each( shapes(), boost::bind( &qshape::shape_t::evaluate, _1, f));
    
    if( toolbar_)
        toolbar_->update_widgets();
}

void qshape_node_t::do_calc_bounds( const render::render_context_t& context)
{
    max_blur_ = 0;

    Imath::Box2f box;

    motion_blur_param_t::loop_data_t d( motion_blur_param()->loop_data( context.time, context.motion_blur_extra_samples,
                                                                            context.motion_blur_shutter_factor));
    float t = d.start_time;

    for( int i = 0; i < d.num_samples; ++i)
    {
        for( const_iterator it( shapes().begin()); it != shapes().end(); ++it)
        {
            if( it->active() && get_value<float>( it->param( "opacity")) != 0.0f)
            {
                box.extendBy( it->bounding_box_at_time( t));
                
                max_blur_ = std::max( max_blur_, get_value<float>( it->param( "blur")));
            }
        }

        t += d.time_step;
    }

    if( !box.isEmpty())
    {
        bounds_without_blur_ = Imath::Box2i( Imath::V2i( std::floor( box.min.x), std::floor( box.min.y)), Imath::V2i( std::ceil( box.max.x), std::ceil( box.max.y)));

        Imath::Box2i all_bounds( bounds_without_blur_);

        if( max_blur_ != 0.0f)
        {
            all_bounds.min.x -= std::ceil( max_blur_ + 1);
            all_bounds.min.y -= std::ceil( max_blur_ + 1);
            all_bounds.max.x += std::ceil( max_blur_ + 1);
            all_bounds.max.y += std::ceil( max_blur_ + 1);
        }

	set_bounds( all_bounds);
    }
    else
    {
        bounds_without_blur_ = domain();
	set_bounds( domain());
    }
}

void qshape_node_t::do_calc_defined( const render::render_context_t& context)
{
    Imath::Box2i def( intersect( bounds_without_blur_, interest()));

    if( max_blur_ != 0.0f)
    {
        def.min.x -= std::ceil( max_blur_ + 1);
        def.min.y -= std::ceil( max_blur_ + 1);
        def.max.x += std::ceil( max_blur_ + 1);
        def.max.y += std::ceil( max_blur_ + 1);
    }

    set_defined( def);
}

void qshape_node_t::do_calc_hash( const render::render_context_t& context)
{
    for( const_iterator it( shapes().begin()); it != shapes().end(); ++it)
    {
	if( it->active())
	    append_to_hash( it->str());
    }
}

void qshape_node_t::do_process( const render::render_context_t& context)
{
    if( defined().isEmpty())
	return;

    motion_blur_param_t::loop_data_t d( motion_blur_param()->loop_data( context.time, context.motion_blur_extra_samples, context.motion_blur_shutter_factor));

    if( d.num_samples == 1)
    {
        qshape::shape_renderer_t renderer( context.time, defined(), context.subsample);

        BOOST_FOREACH( qshape::shape_t& s, shapes())
        {
            if( s.active() && get_value<float>( s.param( "opacity")) != 0.0f)
                renderer.render_shape( s);
        }

        // copy the result
        boost::gil::gray32fc_view_t src( renderer.view());

        for( int j = 0; j < src.height(); ++j)
        {
            boost::gil::gray32fc_view_t::x_iterator src_it( src.row_begin( j));
            image::image_view_t::x_iterator dst_it( image_view().row_begin( j));

            for( int i = 0; i < src.width(); ++i)
            {
                *dst_it = image::pixel_t( (*src_it)[0], (*src_it)[0], (*src_it)[0], (*src_it)[0]);
                ++src_it;
                ++dst_it;
            }
        }
    }
    else
        do_process_motion_blur( context);
}

void qshape_node_t::do_process_motion_blur( const render::render_context_t& context)
{
    // TODO: implement this
    boost::gil::gray32f_image_t buffer( defined().size().x + 1, defined().size().y + 1);
    boost::gil::fill_pixels( boost::gil::view( buffer), boost::gil::gray32f_pixel_t( 0));
    image::gray_image_accumulator_t acc( boost::gil::view( buffer));

    qshape::shape_renderer_t renderer( context.time, defined(), context.subsample);

    motion_blur_param_t::loop_data_t d( motion_blur_param()->loop_data( context.time, context.motion_blur_extra_samples, context.motion_blur_shutter_factor));

    float t = d.start_time;
    float sumw = 0.0f;

    for( int i = 0; i < d.num_samples; ++i)
    {
        renderer.clear();
        renderer.set_time( t);

        BOOST_FOREACH( qshape::shape_t& s, shapes())
        {
            if( s.active() && get_value_at_time<float>( s.param( "opacity"), t) != 0.0f)
                renderer.render_shape( s);
        }

        float w = d.weight_for_time( t);
        acc.accumulate( renderer.view(), w);

        t += d.time_step;
        sumw += w;
    }

    if( sumw != 0.0f)
        acc.multiply( 1.0f / sumw);

    // copy the result
    boost::gil::gray32fc_view_t src( boost::gil::view( buffer));

    for( int j = 0; j < src.height(); ++j)
    {
        boost::gil::gray32fc_view_t::x_iterator src_it( src.row_begin( j));
        image::image_view_t::x_iterator dst_it( image_view().row_begin( j));

        for( int i = 0; i < src.width(); ++i)
        {
            *dst_it = image::pixel_t( (*src_it)[0], (*src_it)[0], (*src_it)[0], (*src_it)[0]);
            ++src_it;
            ++dst_it;
        }
    }
}

qshape::qshape_toolbar_t& qshape_node_t::toolbar() { return dynamic_cast<qshape::qshape_toolbar_t&>( *(toolbar_.data()));}

QWidget *qshape_node_t::do_create_toolbar()
{
    create_ = new qshape::qshape_create_tool_t( this);
    select_ = new qshape::qshape_select_tool_t( this);
    convert_ = new qshape::qshape_convert_tool_t( this);
    tool_ = select_;

    toolbar_ = new qshape::qshape_toolbar_t( this);
    return toolbar_;
}

void qshape_node_t::do_draw_overlay( ui::image_view_t& view) const
{
    view_.draw_overlay( view);
    tool_->draw_overlay( view);
}

bool qshape_node_t::do_key_press_event( ui::image_view_t& view, QKeyEvent *event)
{
    return tool_->key_press_event( view, event);
}

bool qshape_node_t::do_key_release_event( ui::image_view_t& view, QKeyEvent *event)
{
    return tool_->key_release_event( view, event);
}

bool qshape_node_t::do_mouse_press_event( ui::image_view_t& view, QMouseEvent *event)
{
    tool_->mouse_press_event( view, event);
    return true;
}

void qshape_node_t::do_mouse_move_event( ui::image_view_t& view, QMouseEvent *event)
{ 
    tool_->mouse_move_event( view, event);
}

void qshape_node_t::do_mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)
{
    tool_->mouse_drag_event( view, event);
}

void qshape_node_t::do_mouse_release_event( ui::image_view_t& view, QMouseEvent *event)
{
    tool_->mouse_release_event( view, event);
}

void qshape_node_t::do_create_tracks( anim::track_t *parent)
{
    node_t::do_create_tracks( parent);
    adobe::for_each( shapes(), boost::bind( &qshape::shape_t::create_tracks, _1, parent));
}

void qshape_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
    ar & boost::serialization::make_nvp( "shapes", shapes_);

    adobe::for_each( shapes_, boost::bind( &qshape::shape_t::set_parent, _1, this));
}

void qshape_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
    ar & boost::serialization::make_nvp( "shapes", shapes_);
}

// factory
node_t *create_qshape_node() { return new qshape_node_t();}

node_info_t qshape_node_info()
{
    node_info_t info;
    info.id = "builtin.qshape";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Input";
    info.menu_item = "Quick Shape";
    info.create = &create_qshape_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( qshape_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::qshape_node_t)
