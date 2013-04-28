// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BOOL_PARAM_HPP
#define	RAMEN_BOOL_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<QPointer>
#include<QCheckBox>

namespace ramen
{

class bool_param_t : public static_param_t
{
    Q_OBJECT

public:

    bool_param_t();
    explicit bool_param_t( const std::string& name);

    void set_default_value( bool x);

    void set_value( bool x);

protected:

    bool_param_t( const bool_param_t& other);
    void operator=( const bool_param_t& other);

private:

    virtual param_t *do_clone() const;

    virtual std::string do_str() const { return string_for_type<bool>();}

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<QCheckBox> button_;

private Q_SLOTS:

    void button_checked( int state);
};

} // namespace

#endif	/* _BOOL_PARAM_HPP */
