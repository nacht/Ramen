// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CURVE_PARAM_HPP
#define	RAMEN_CURVE_PARAM_HPP

#include<ramen/params/animated_param.hpp>

#include<OpenEXR/ImathColor.h>

namespace ramen
{

class curve_param_t : public animated_param_t
{
    Q_OBJECT

public:

    curve_param_t();
    explicit curve_param_t( const std::string& name);

    const anim::curve_t& curve() const;
    anim::curve_t& curve();

    const Imath::Color3c& color() const       { return color_;}
    void set_color( const Imath::Color3c& c)  { color_ = c;}

protected:

    curve_param_t( const curve_param_t& other);
    void operator=( const curve_param_t& other);

private:

    void private_init();

    virtual param_t *do_clone() const { return new curve_param_t( *this);}

    virtual void do_create_tracks( anim::track_t *parent);

    virtual std::string do_str() const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    Imath::Color3c color_;
};

} // namespace

#endif
