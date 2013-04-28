// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BUTTON_PARAM_HPP
#define	RAMEN_BUTTON_PARAM_HPP

#include<ramen/params/param.hpp>

#include<QPointer>

class QPushButton;

namespace ramen
{

class button_param_t : public param_t
{
    Q_OBJECT
    
public:

    button_param_t();
    explicit button_param_t( const std::string& name);

protected:

    button_param_t( const button_param_t& other);
    void operator=( const button_param_t& other);

private:

    virtual param_t *do_clone() const { return new button_param_t( *this);}

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    virtual QWidget *do_create_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<QPushButton> button_;

private Q_SLOTS:

    void button_pressed();
};

} // namespace

#endif	/* _button_PARAM_HPP */
