// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/separator_param.hpp>

#include<QFrame>

namespace ramen
{

separator_param_t::separator_param_t() : param_t() { set_static( true);}

separator_param_t::separator_param_t( const std::string& name) : param_t( name) { set_static( true);}

separator_param_t::separator_param_t( const separator_param_t& other) : param_t( other) {}

QWidget *separator_param_t::do_create_widgets()
{
    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::HLine | QFrame::Raised);
    separator->setLineWidth( 1);
    QSize s = separator->sizeHint();

    separator->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    separator->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    separator->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return separator;
}

void separator_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
}

void separator_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::separator_param_t)
