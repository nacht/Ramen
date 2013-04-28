// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/curve_param.hpp>

#include<ramen/ImathExt/serialization/ImathColor.hpp>

#include<ramen/anim/track.hpp>

#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

curve_param_t::curve_param_t() : animated_param_t() { private_init();}
curve_param_t::curve_param_t( const std::string& name) : animated_param_t( name) { private_init();}

curve_param_t::curve_param_t( const curve_param_t& other) : animated_param_t( other) {}

void curve_param_t::private_init()
{
    add_curve();
    color_ = Imath::Color3c( 255, 255, 255);
}

const anim::curve_t& curve_param_t::curve() const   { return curves()[0];}
anim::curve_t& curve_param_t::curve()		    { return curves()[0];}

void curve_param_t::do_create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( name(), &( curves()[0])));
    t->set_color( color_);
    t->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    parent->add_child( t);
}

std::string curve_param_t::do_str() const { return curves()[0].str();}

void curve_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( animated_param_t);
    ar & boost::serialization::make_nvp( "color", color_);
}

void curve_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( animated_param_t);
    ar & boost::serialization::make_nvp( "color", color_);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::curve_param_t)
