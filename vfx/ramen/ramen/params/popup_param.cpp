// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/popup_param.hpp>

#include<boost/serialization/string.hpp>
#include<boost/serialization/vector.hpp>

#include<QComboBox>
#include<ramen/Qr/QrLabel.hpp>

#include<ramen/params/param_set.hpp>
#include<ramen/nodes/node.hpp>

namespace ramen
{

popup_param_t::popup_param_t() : static_param_t()
{
    set_default_value( 0);
}

popup_param_t::popup_param_t( const std::string& name) : static_param_t( name)
{
    set_default_value( 0);
}

popup_param_t::popup_param_t( const popup_param_t& other) : static_param_t( other), menu_items_( other.menu_items_)
{
    menu_ = 0;
}

void popup_param_t::set_default_value( int x)
{
    value().assign( x);
    default_value().assign( x);
}

void popup_param_t::set_value( int x)
{
    if( can_undo())
	param_set()->add_command( this);

    value().assign( x);
    param_set()->param_changed( this);
}

void popup_param_t::add_menu_item( const std::string& item) { menu_items_.push_back( item);}

QWidget *popup_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QrLabel *label = new QrLabel( top);
    menu_ = new QComboBox( top);
    menu_->setFocusPolicy( Qt::NoFocus);

    for( int i = 0; i < menu_items_.size(); ++i)
        menu_->addItem( menu_items_[i].c_str());

    QSize s = menu_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    menu_->move( ui::inspector_t::Instance().left_margin(), 0);
    menu_->resize( s.width(), s.height());
    menu_->setCurrentIndex( get_value<int>( *this));
    menu_->setEnabled( enabled());
    connect( menu_, SIGNAL( currentIndexChanged( int)), this, SLOT( item_picked( int)));

    top->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    top->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void popup_param_t::do_update_widgets()
{
    if( menu_)
    {
        menu_->blockSignals( true);
        menu_->setCurrentIndex( get_value<int>( *this));
        menu_->blockSignals( false);
    }
}

void popup_param_t::do_enable_widgets( bool e)
{
    if( menu_)
        menu_->setEnabled( e);
}

void popup_param_t::item_picked( int index)
{
    param_set()->begin_edit();
    set_value( index);
    param_set()->end_edit();
}

void popup_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
    ar & boost::serialization::make_nvp( "menu_items", menu_items_);
}

void popup_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( static_param_t);
    ar & boost::serialization::make_nvp( "menu_items", menu_items_);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::popup_param_t)
