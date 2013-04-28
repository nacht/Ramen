// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FORMAT_PARAM_HPP
#define	RAMEN_FORMAT_PARAM_HPP

#include<ramen/params/static_param.hpp>
#include<ramen/util/format.hpp>

#include<QPointer>

class QrSpinBox;

namespace ramen
{

class format_param_t : public static_param_t
{
    Q_OBJECT
    
public:

    format_param_t();
    explicit format_param_t( const std::string& name);

    void set_value( format_t format);

protected:

    format_param_t( const format_param_t& other);
    void operator=( const format_param_t& other);

private:

    virtual param_t *do_clone() const;

    virtual void do_init();

    virtual std::string do_str() const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<QrSpinBox> width_input_, height_input_;

private Q_SLOTS:

    void widget_changed( int v);
};

} // namespace

// register adobe typeinfo
ADOBE_NAME_TYPE_0( "format_t:ramen", ramen::format_t);

#endif	/* _format_PARAM_HPP */
