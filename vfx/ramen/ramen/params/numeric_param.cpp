// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/numeric_param.hpp>

#include<ramen/nodes/node.hpp>
#include<ramen/app/composition.hpp>

namespace ramen
{

numeric_param_t::numeric_param_t() : animated_param_t(), type_( absolute) {}

numeric_param_t::numeric_param_t( const std::string& name) : animated_param_t( name), type_( absolute) {}

numeric_param_t::numeric_param_t( const numeric_param_t& other) : animated_param_t( other), type_( other.type_)
{
    depends_on_port_ = other.depends_on_port_;
}

void numeric_param_t::set_round_to_int( bool r)
{
    if( r)
	set_flags( flags() | round_to_int_bit);
    else
	set_flags( flags() & ~round_to_int_bit);
}

float numeric_param_t::round( float x) const
{
    if( round_to_int())
        return (int) x;

    return x;
}

void numeric_param_t::set_numeric_type( numeric_type_t type, int port)
{
    type_ = type;
    depends_on_port_ = port;
}

Imath::Box2i numeric_param_t::frame_area() const
{
    const node_t *n = dynamic_cast<const node_t*>( parameterised());
    assert( n && "numeric relative params can only be used inside nodes");

    if( depends_on_port_ == -1)
	return n->domain();

    assert( depends_on_port_ < n->num_inputs());

    if( n->input( depends_on_port_))
	return n->input( depends_on_port_)->domain();

    // fallback, return default area
    Imath::Box2i area( composition()->default_format().area());
    --area.max.x;
    --area.max.y;
    return area;
}

float numeric_param_t::relative_to_absolute( float x) const
{
    switch( type_)
    {
    case relative_x:
	return ( x * frame_area().size().x) + frame_area().min.x;

    case relative_y:
	return ( x * frame_area().size().y) + frame_area().min.y;

    case relative_size_x:
	return x * frame_area().size().x;

    case relative_size_y:
	return x * frame_area().size().y;

    default:
	return x;
    }
}

Imath::V2f numeric_param_t::relative_to_absolute( const Imath::V2f& x) const
{
    switch( type_)
    {
    case relative_xy:
	return Imath::V2f( ( x.x  * frame_area().size().x) + frame_area().min.x,
                            ( x.y * frame_area().size().y) + frame_area().min.y);

    case relative_size_xy:
	return Imath::V2f( x.x  * frame_area().size().x,
                            x.y * frame_area().size().y);

    default:
	return x;
    }
}

Imath::Box2f numeric_param_t::relative_to_absolute( const Imath::Box2f& x) const
{
    switch( type_)
    {
    case relative_xy:
	return Imath::Box2f( Imath::V2f( x.min.x * frame_area().size().x + frame_area().min.x, x.min.y * frame_area().size().y + frame_area().min.y),
                             Imath::V2f( x.max.x * frame_area().size().x + frame_area().min.x, x.max.y * frame_area().size().y + frame_area().min.y));

    case relative_size_xy:
	return Imath::Box2f( Imath::V2f( x.min.x * frame_area().size().x, x.min.y * frame_area().size().y),
                             Imath::V2f( x.max.x * frame_area().size().x, x.max.y * frame_area().size().y));

    default:
	return x;
    }
}

float numeric_param_t::absolute_to_relative( float x) const
{
    switch( type_)
    {
    case relative_x:
	return ( x - frame_area().min.x) / frame_area().size().x;

    case relative_y:
	return ( x - frame_area().min.y) / frame_area().size().y;

    case relative_size_x:
	return x / frame_area().size().x;

    case relative_size_y:
	return x / frame_area().size().y;

    default:
	return x;
    }
}

Imath::V2f numeric_param_t::absolute_to_relative( const Imath::V2f& x) const
{
    switch( type_)
    {
    case relative_xy:
	return Imath::V2f( ( x.x - frame_area().min.x) / frame_area().size().x, ( x.y - frame_area().min.y) / frame_area().size().y);

    case relative_size_xy:
	return Imath::V2f( x.x / frame_area().size().x, x.y / frame_area().size().y);
    default:
	return x;
    }
}

Imath::Box2f numeric_param_t::absolute_to_relative( const Imath::Box2f& x) const
{
    switch( type_)
    {
    case relative_xy:
	return Imath::Box2f( Imath::V2f( ( x.min.x - frame_area().min.x) / frame_area().size().x, ( x.min.y - frame_area().min.y) / frame_area().size().y),
                             Imath::V2f( ( x.max.x - frame_area().min.x) / frame_area().size().x, ( x.max.y - frame_area().min.y) / frame_area().size().y));

    case relative_size_xy:
	return Imath::Box2f( Imath::V2f( x.min.x / frame_area().size().x, x.min.y / frame_area().size().y),
                             Imath::V2f( x.max.x / frame_area().size().x, x.max.y / frame_area().size().y));
    default:
	return x;
    }
}

float numeric_param_t::absolute_min() const
{
    if( get_min() == -std::numeric_limits<float>::max())
	return get_min();

    switch( type_)
    {
    case relative_x:
	return ( get_min() * frame_area().size().x) + frame_area().min.x;

    case relative_y:
	return ( get_min() * frame_area().size().y) + frame_area().min.y;

    case relative_xy:
	return ( get_min() * frame_area().size().x) + frame_area().min.x;

    case relative_size_x:
	return get_min() * frame_area().size().x;

    case relative_size_y:
	return get_min() * frame_area().size().y;

    case relative_size_xy:
	return get_min() * frame_area().size().x;

    default:
	return get_min();
    }
}

float numeric_param_t::absolute_max() const
{
    if( get_max() == std::numeric_limits<float>::max())
	return get_max();

    switch( type_)
    {
    case relative_x:
	return ( get_max() * frame_area().size().x) + frame_area().min.x;

    case relative_y:
	return ( get_max() * frame_area().size().y) + frame_area().min.y;

    case relative_xy:
	return ( get_max() * frame_area().size().x) + frame_area().min.x;

    case relative_size_x:
	return get_max() * frame_area().size().x;

    case relative_size_y:
	return get_max() * frame_area().size().y;

    case relative_size_xy:
	return get_max() * frame_area().size().x;

    default:
	return get_max();
    }
}

void numeric_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( animated_param_t);
    ar & boost::serialization::make_nvp( "numeric_type", type_);
    ar & boost::serialization::make_nvp( "depends_on", depends_on_port_);
}

void numeric_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( animated_param_t);
    ar & boost::serialization::make_nvp( "numeric_type", type_);
    ar & boost::serialization::make_nvp( "depends_on", depends_on_port_);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::numeric_param_t)
