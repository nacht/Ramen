// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COMBO_GROUP_PARAM_HPP
#define	RAMEN_COMBO_GROUP_PARAM_HPP

#include<ramen/params/composite_param.hpp>

#include<QPointer>

class QComboBox;
class QStackedWidget;

namespace ramen
{

class combo_group_param_t : public composite_param_t
{
    Q_OBJECT

public:

    combo_group_param_t();
    explicit combo_group_param_t( const std::string& name);

    void set_default_value( int x);

    void set_value( int x);

    std::auto_ptr<undo::command_t> create_command();

protected:

    combo_group_param_t( const combo_group_param_t& other);
    void operator=( const combo_group_param_t& other);

private:

    virtual param_t *do_clone() const { return new combo_group_param_t( *this);}

    virtual std::string do_str() const;

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<QComboBox> menu_;
    QPointer<QStackedWidget> stack_;

private Q_SLOTS:

    void item_picked( int index);
};

} // namespace

#endif	/* _combo_group_param_HPP */
