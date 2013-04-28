// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_CURVE_HPP
#define	RAMEN_ANIM_CURVE_HPP

#include<ramen/anim/curve_fwd.hpp>

#include<boost/serialization/split_member.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/anim/keyframe.hpp>
#include<ramen/anim/keyframe_vector.hpp>

#include<ramen/serialization/copy_on_write.hpp>

namespace ramen
{
namespace anim
{

class curve_t
{
public:

    curve_t();

    bool empty() const          { return keys().empty();}
    std::size_t size() const    { return keys().size();}

    void clear() { keys().clear();}

    void copy( const curve_t& other, float offset = 0.0f);
    void copy( const curve_t& other, float offset, float start, float end);

    auto_tangent_method default_auto_tangents() const	    { return default_auto_tan_;}
    void set_default_auto_tangents( auto_tangent_method m)  { default_auto_tan_ = m;}

    float get_min() const { return min_;}
    float get_max() const { return max_;}

    void set_min( float x) { min_ = x;}
    void set_max( float x) { max_ = x;}

    void set_range( float lo, float hi)
    {
	assert( lo <= hi);
	min_ = lo;
	max_ = hi;
    }

    const keyframe_vector_t<keyframe_t>& keys() const	{ return keys_.read();}
    keyframe_vector_t<keyframe_t>& keys()		{ return keys_.write();}

    const keyframe_t& operator[]( int x) const
    {
        assert( x >= 0);
        assert( x < size());
        return keys()[x];
    }

    keyframe_t& operator[]( int x)
    {
        assert( x >= 0);
        assert( x < size());
        return keys()[x];
    }

    typedef keyframe_vector_t<keyframe_t>::const_iterator   const_iterator;
    typedef keyframe_vector_t<keyframe_t>::iterator         iterator;

    const_iterator begin() const    { return keys_.read().begin();}
    const_iterator end() const      { return keys_.read().end();}

    iterator begin()    { return keys_.write().begin();}
    iterator end()      { return keys_.write().end();}

    typedef keyframe_vector_t<keyframe_t>::const_reverse_iterator   const_reverse_iterator;
    typedef keyframe_vector_t<keyframe_t>::reverse_iterator         reverse_iterator;

    const_reverse_iterator rbegin() const    { return keys_.read().rbegin();}
    const_reverse_iterator rend() const      { return keys_.read().rend();}

    reverse_iterator rbegin()    { return keys_.write().rbegin();}
    reverse_iterator rend()      { return keys_.write().rend();}

    iterator insert( float time, float value, bool recalc = true);
    iterator insert( const keyframe_t& k, bool recalc = true);
    iterator insert( float time, bool recalc = true);

    void erase( float time);
    void erase_selected_keyframes();

    iterator iterator_from_keyframe_ptr( keyframe_t *k);

    // extrapolation
    extrapolation_method extrapolation() const          { return extrap_;}
    void set_extrapolation( extrapolation_method m)	{ extrap_ = m;}

    // tangents and coefficients
    void recalc_coefficients();

    void recalc_tangents_and_coefficients( iterator it);
    void recalc_tangents_and_coefficients();

    // eval
    float evaluate( float time) const;
    float derive( float time) const;
    float integrate( float time1, float time2) const;

    // selection
    void select_all_keyframes( bool b) const;
    bool any_keyframe_selected() const;

    Imath::Box2f bounds() const;
    Imath::Box2f selection_bounds() const;

    float start_time() const;
    float end_time() const;

    std::string str() const;

private:

    friend void swap( curve_t& a, curve_t& b);

    float do_evaluate( float time) const;

    friend class boost::serialization::access;

    template<class Archive>
    void save( Archive& ar, const unsigned int version) const
    {
	ar & boost::serialization::make_nvp( "keys", keys_);
	ar & boost::serialization::make_nvp( "default_auto_tan", default_auto_tan_);
	ar & boost::serialization::make_nvp( "min", min_);
	ar & boost::serialization::make_nvp( "max", max_);
	ar & boost::serialization::make_nvp( "extrap", extrap_);
    }

    template<class Archive>
    void load( Archive& ar, const unsigned int version)
    {
	ar & boost::serialization::make_nvp( "keys", keys_);
	ar & boost::serialization::make_nvp( "default_auto_tan", default_auto_tan_);
	ar & boost::serialization::make_nvp( "min", min_);
	ar & boost::serialization::make_nvp( "max", max_);
	ar & boost::serialization::make_nvp( "extrap", extrap_);
        recalc_coefficients();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();

    adobe::copy_on_write<keyframe_vector_t<keyframe_t> > keys_;
    auto_tangent_method default_auto_tan_;
    float min_, max_;
    extrapolation_method extrap_;
};

} // namespace
} // namespace

#endif	/* _FLOAT_ANIM_CURVE_HPP */
