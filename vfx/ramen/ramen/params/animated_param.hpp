// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIMATED_PARAM_HPP
#define	RAMEN_ANIMATED_PARAM_HPP

#include<ramen/params/param.hpp>

#include<vector>

#include<adobe/empty.hpp>

#include<loki/AssocVector.h>

#include<ramen/anim/curve.hpp>

namespace ramen
{

class animated_param_t : public param_t
{
    Q_OBJECT

public:

    animated_param_t();
    explicit animated_param_t( const std::string& name);

    // this is for OFX
    virtual void copy( const param_t& other, float offset = 0.0f);
    virtual void copy( const param_t& other, float offset, float start, float end);

    void add_curve();
    void add_curves( int num);

    const std::vector<anim::curve_t>& curves() const	{ return curves_;}
    std::vector<anim::curve_t>& curves()		{ return curves_;}

    float get_min() const { return curves()[0].get_min();}
    void set_min( float lo);

    float get_max() const { return curves()[0].get_max();}
    void set_max( float hi);

    void set_range( float lo, float hi);

    float step() const	    { return step_;}
    void set_step( float s) { step_ = s;}

    void anim_curve_changed( anim::curve_t *c);

    virtual std::auto_ptr<undo::command_t> create_command();

    // for OFX
    int get_num_keys() const;
    float get_key_time( int nth) const;
    void get_key_index( float time, int direction, int& index) const;
    void delete_key( float time);
    void delete_all_keys();

protected:

    animated_param_t( const animated_param_t& other);
    void operator=( const animated_param_t& other);

    float clamp( float x) const;

    virtual void do_anim_curve_changed( anim::curve_t *c);

private:

    void fill_key_times_assoc_vector( Loki::AssocVector<float, adobe::empty_t>& v) const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    std::vector<anim::curve_t> curves_;
    float step_;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::animated_param_t)

#endif	/* RAMEN_animated_PARAM_HPP */
