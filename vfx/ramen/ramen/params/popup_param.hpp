// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_POPUP_PARAM_HPP
#define	RAMEN_POPUP_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<string>
#include<vector>

#include<boost/assign/std/vector.hpp>
using namespace boost::assign;

#include<QPointer>

class QComboBox;

namespace ramen
{

class popup_param_t : public static_param_t
{
    Q_OBJECT
    
public:

    popup_param_t();
    explicit popup_param_t( const std::string& name);

    void set_default_value( int x);

    void set_value( int x);

    void add_menu_item( const std::string& item);

    std::vector<std::string>& menu_items() { return menu_items_;}

protected:

    popup_param_t( const popup_param_t& other);
    void operator=( const popup_param_t& other);

private:

    virtual param_t *do_clone() const { return new popup_param_t( *this);}

    virtual void do_evaluate( float time) {}

    virtual std::string do_str() const { return string_for_type<int>();}

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    std::vector<std::string> menu_items_;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<QComboBox> menu_;

private Q_SLOTS:

    void item_picked( int index);
};

} // namespace

#endif	/* _POPUP_PARAM_HPP */
