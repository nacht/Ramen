// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/combo_group_param.hpp>

#include<algorithm>

#include<boost/foreach.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/nodes/node.hpp>
#include<ramen/params/static_param_command.hpp>

#include<QStackedWidget>
#include<QComboBox>
#include<QVBoxLayout>

#include<ramen/Qr/QrLabel.hpp>

namespace ramen
{

combo_group_param_t::combo_group_param_t() : composite_param_t()
{
    set_default_value( 0);
}

combo_group_param_t::combo_group_param_t( const std::string& name) : composite_param_t( name)
{
    set_default_value( 0);
}

combo_group_param_t::combo_group_param_t( const combo_group_param_t& other) : composite_param_t( other)
{
    menu_ = 0;
    stack_ = 0;
}

void combo_group_param_t::set_default_value( int x)
{
    value().assign( x);
    default_value() = value();
}

void combo_group_param_t::set_value( int x)
{
    if( can_undo())
	param_set()->add_command( this);

    value().assign( x);
    param_set()->param_changed( this);
}

std::auto_ptr<undo::command_t> combo_group_param_t::create_command()
{
    return std::auto_ptr<undo::command_t>( new static_param_command_t( this));
}

std::string combo_group_param_t::do_str() const
{
    std::string str;
    str.append( string_for_type<int>());
    str.append( params()[get_value<int>( *this)].str());
    return str;
}

QWidget *combo_group_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);
    layout->setSpacing( 5);
    layout->setSizeConstraint( QLayout::SetFixedSize);

    QWidget *selector = new QWidget();
    QrLabel *label = new QrLabel( selector);

    menu_ = new QComboBox( selector);
    menu_->setFocusPolicy( Qt::NoFocus);

    stack_ = new QStackedWidget();

    BOOST_FOREACH( param_t& p, params())
    {
        QWidget *w = p.create_widgets();

        if( w)
        {
            stack_->addWidget( w);
            menu_->addItem( p.name().c_str());
        }
    }

    QSize s = menu_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    menu_->move( ui::inspector_t::Instance().left_margin(), 0);
    menu_->resize( s.width(), s.height());
    menu_->setEnabled( enabled());

    menu_->setCurrentIndex( get_value<int>( *this));
    stack_->setCurrentIndex( get_value<int>( *this));
    stack_->setEnabled( enabled());
    connect( menu_, SIGNAL( currentIndexChanged( int)), this, SLOT( item_picked( int)));

    selector->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    selector->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    selector->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);

    layout->addWidget( selector);
    layout->addWidget( stack_);

    top->setLayout( layout);
    return top;
}

void combo_group_param_t::do_update_widgets()
{
    if( menu_)
    {
        menu_->blockSignals( true);
        menu_->setCurrentIndex( get_value<int>( *this));
        stack_->setCurrentIndex( get_value<int>( *this));
        menu_->blockSignals( false);
        adobe::for_each( params(), boost::bind( &param_t::update_widgets, _1));
    }
}

void combo_group_param_t::do_enable_widgets( bool e)
{
    if( menu_)
    {
        menu_->setEnabled( e);
        stack_->setEnabled( e);
    }
}

void combo_group_param_t::item_picked( int index)
{
    param_set()->begin_edit();
    set_value( index);
    param_set()->end_edit();
}

void combo_group_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( composite_param_t);
}

void combo_group_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( composite_param_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::combo_group_param_t)
