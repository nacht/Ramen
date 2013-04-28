// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/input/gradient_node.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/nodes/node_factory.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/color_param.hpp>

#include<ramen/manipulators/point2_manipulator.hpp>

namespace ramen
{

namespace
{

template <typename P>
struct gradient_fn
{
    typedef boost::gil::point2<ptrdiff_t>   point_t;
    typedef gradient_fn			    const_t;
    typedef P				    value_type;
    typedef value_type			    reference;
    typedef value_type			    const_reference;
    typedef point_t			    argument_type;
    typedef reference			    result_type;

    BOOST_STATIC_CONSTANT(bool, is_mutable=false);

    gradient_fn( const Imath::V2f& p1, const Imath::V2f& p2,
		const Imath::Color4f& c1, const Imath::Color4f& c2) : p1_( p1), p2_( p2), c1_( c1), c2_( c2)
    {
	length2_ = ( p2_ - p1_).length2();
    }

    result_type operator()( const point_t& p) const
    {
	float t = ( ( p.x - p1_.x) * ( p2_.x - p1_.x)) + ( ( p.y - p1_.y) * ( p2_.y - p1_.y));
	t = adobe::clamp( t / length2_, 0.0f, 1.0f);

	return value_type(  c1_.r * (1.0f - t) + c2_.r * t,
			    c1_.g * (1.0f - t) + c2_.g * t,
			    c1_.b * (1.0f - t) + c2_.b * t,
			    c1_.a * (1.0f - t) + c2_.a * t);

	return value_type( t, t, t, t);
    }

private:

    Imath::V2f p1_, p2_;
    float length2_;
    Imath::Color4f c1_, c2_;
};

} // namespace

gradient_node_t::gradient_node_t() : generator_node_t() { set_name( "gradient");}

void gradient_node_t::do_create_params()
{
    generator_node_t::do_create_params();

    std::auto_ptr<float2_param_t> p( new float2_param_t( "Start"));
    p->set_id( "start");
    p->set_default_value( Imath::V2f( 0.5, 0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    std::auto_ptr<color_param_t> q( new color_param_t( "Start Color"));
    q->set_id( "startcol");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 0, 0, 0, 0));
    add_param( q);

    p.reset( new float2_param_t( "End"));
    p->set_id( "end");
    p->set_default_value( Imath::V2f( 0.5, 1));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);
    
    q.reset( new color_param_t( "End Color"));
    q->set_id( "endcol");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 1, 1, 1, 1));
    add_param( q);
}

void gradient_node_t::do_create_manipulators()
{
    float2_param_t *p = dynamic_cast<float2_param_t*>( &param( "start"));
    std::auto_ptr<point2_manipulator_t> m( new point2_manipulator_t( p));
    add_manipulator( m);

    p = dynamic_cast<float2_param_t*>( &param( "end"));
    m.reset( new point2_manipulator_t( p));
    add_manipulator( m);
}

void gradient_node_t::do_process( const render::render_context_t& context)
{
    typedef gradient_fn<boost::gil::rgba32f_pixel_t>	    deref_t;
    typedef deref_t::point_t				    point_t;
    typedef boost::gil::virtual_2d_locator<deref_t,false> locator_t;
    typedef boost::gil::image_view<locator_t>		    my_virt_view_t;

    point_t dims( domain().size().x+1, domain().size().y+1);
    my_virt_view_t grad( dims, locator_t(point_t(0,0), point_t(1,1), deref_t( get_absolute_value<Imath::V2f>( param( "start")),
										get_absolute_value<Imath::V2f>( param( "end")),
										get_value<Imath::Color4f>( param( "startcol")),
										get_value<Imath::Color4f>( param( "endcol")))));
    render_virtual_view( grad);
}

void gradient_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
}

void gradient_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
}

// factory
node_t *create_gradient_node() { return new gradient_node_t();}

node_info_t gradient_node_info()
{
    node_info_t info;
    info.id = "builtin.gradient";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Input";
    info.menu_item = "Gradient";
    info.create = &create_gradient_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( gradient_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::gradient_node_t)
