// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PROPORTIONAL_PARAM_HPP
#define	RAMEN_PROPORTIONAL_PARAM_HPP

#include<ramen/params/numeric_param.hpp>

#include<OpenEXR/ImathVec.h>

#include<QPointer>

#include<ramen/Qr/QrSmallToolButton.hpp>

namespace ramen
{

class proportional_param_t : public numeric_param_t
{
    Q_OBJECT

public:

    proportional_param_t();
    explicit proportional_param_t( const std::string& name);

    bool proportional() const	    { return flags() & proportional_bit;}
    void set_proportional( bool p);

    bool proportional_checked() const;

protected:

    proportional_param_t( const proportional_param_t& other);
    void operator=( const proportional_param_t& other);

protected:

    void create_proportional_button( QWidget *parent = 0);

    QPointer<QrSmallToolButton> prop_button_;

    static Imath::V3f proportional_factor;

private:

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::proportional_param_t)

#endif
