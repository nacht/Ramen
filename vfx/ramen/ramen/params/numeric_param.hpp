// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NUMERIC_PARAM_HPP
#define	RAMEN_NUMERIC_PARAM_HPP

#include<ramen/params/animated_param.hpp>

#include<cassert>
#include<utility>

#include<OpenEXR/ImathBox.h>

namespace ramen
{

class numeric_param_t : public animated_param_t
{
    Q_OBJECT

public:

    enum numeric_type_t
    {
	absolute = 0,
	relative_x,
	relative_y,
	relative_xy,
	relative_size_x,
	relative_size_y,
	relative_size_xy
    };

    numeric_param_t();
    explicit numeric_param_t( const std::string& name);

    bool round_to_int() const { return flags() & round_to_int_bit;}
    void set_round_to_int( bool r);

    numeric_type_t numeric_type() const		{ return type_;}
    void set_numeric_type( numeric_type_t type, int port = -1);

    float relative_to_absolute( float x) const;
    Imath::V2f relative_to_absolute( const Imath::V2f& x) const;
    Imath::Box2f relative_to_absolute( const Imath::Box2f& x) const;

    float absolute_to_relative( float x) const;
    Imath::V2f absolute_to_relative( const Imath::V2f& x) const;
    Imath::Box2f absolute_to_relative( const Imath::Box2f& x) const;

protected:

    numeric_param_t( const numeric_param_t& other);
    void operator=( const numeric_param_t& other);

    float absolute_min() const;
    float absolute_max() const;

    float round( float x) const;

private:

    Imath::Box2i frame_area() const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    numeric_type_t type_;
    int depends_on_port_;
};

template<class S>
S get_absolute_value( const param_t& p)
{
    const numeric_param_t *q = dynamic_cast<const numeric_param_t*>( &p);
    assert( q);

    const adobe::any_regular_t& any( p.value());
    S v = any.cast<S>();
    return q->relative_to_absolute( v);
}

/**
 * Get the value at time time of a param as an object of type S
 * @param p the param to get the value
 * @param time time
 */
template<class S>
S get_absolute_value_at_time( const param_t& p, float time)
{
    const numeric_param_t *q = dynamic_cast<const numeric_param_t*>( &p);
    assert( q);

    const adobe::any_regular_t& any( p.value_at_time( time));
    S v = any.cast<S>();
    return q->relative_to_absolute( v);
}

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::numeric_param_t)

#endif	/* RAMEN_animated_PARAM_HPP */
