// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/color_param.hpp>

#include<limits>
#include<cmath>

#include<boost/bind.hpp>

#include<ramen/ImathExt/serialization/ImathColor.hpp>

#include<ramen/nodes/node.hpp>
#include<ramen/app/document.hpp>

#include<ramen/Qr/QrLabel.hpp>
#include<ramen/Qr/QrDoubleSpinBox.hpp>
#include<ramen/Qr/QrColorButton.hpp>
#include<ramen/Qr/ColorPicker/QrColorPicker.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>

namespace ramen
{

color_param_t::color_param_t() : animated_param_t() { private_init();}
color_param_t::color_param_t( const std::string& name) : animated_param_t( name) { private_init();}

color_param_t::color_param_t( const color_param_t& other) : animated_param_t( other)
{
    input0_ = input1_ = input2_ = input3_ = 0;
    is_rgba_ = other.is_rgba_;
}

void color_param_t::private_init()
{
    set_is_rgba( true);
    add_curves( 4);
    set_default_value( Imath::Color4f( 0, 0, 0, 0));
    set_step( 0.05f);
}

void color_param_t::set_default_value( const Imath::Color4f& x)
{
    value().assign( x);
    default_value().assign( x);
}

void color_param_t::set_value( const Imath::Color4f& x)
{
    float time;

    if( composition())
	time = composition()->frame();
    else
	time = 1.0f;

    set_value_at_time( x, time);
}

void color_param_t::set_value_at_time( const Imath::Color4f& x, float time)
{
    if( can_undo())
	param_set()->add_command( this);

    value().assign( x);

    if( !is_static() && ( param_set()->autokey() || !curves()[0].empty()))
	curves()[0].insert( time, x.r);

    if( !is_static() && ( param_set()->autokey() || !curves()[1].empty()))
	curves()[1].insert( time, x.g);

    if( !is_static() && ( param_set()->autokey() || !curves()[2].empty()))
	curves()[2].insert( time, x.b);

    if( is_rgba() && !is_static() && ( param_set()->autokey() || !curves()[3].empty()))
	curves()[3].insert( time, x.a);

    evaluate( time);
    param_set()->param_changed( this);
}

void color_param_t::do_create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( name()));

    std::auto_ptr<anim::track_t> tr( new anim::track_t( "R", &( curves()[0])));
    tr->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tr);

    std::auto_ptr<anim::track_t> tg( new anim::track_t( "G", &( curves()[1])));
    tg->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tg);

    std::auto_ptr<anim::track_t> tb( new anim::track_t( "B", &( curves()[2])));
    tb->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tb);

    if( is_rgba())
    {
        std::auto_ptr<anim::track_t> ta( new anim::track_t( "A", &( curves()[3])));
	ta->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
	t->add_child( ta);
    }

    parent->add_child( t);
}

void color_param_t::do_evaluate( float time)
{
    value() = do_get_value_at_time( time);
}

adobe::any_regular_t color_param_t::do_get_value_at_time( float time) const
{
    Imath::Color4f v( get_value<Imath::Color4f>( *this));

    if( !curves()[0].empty())
	v.r = curves()[0].evaluate( time);

    if( !curves()[1].empty())
	v.g = curves()[1].evaluate( time);

    if( !curves()[2].empty())
	v.b = curves()[2].evaluate( time);

    if( is_rgba() && !curves()[3].empty())
	v.a = curves()[3].evaluate( time);
    else
	v.a = 1.0f;

    return adobe::any_regular_t( v);
}

QWidget *color_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QrLabel *label = new QrLabel( top);
    button_ = new QrColorButton( top);

    input0_ = new QrDoubleSpinBox( top);
    input1_ = new QrDoubleSpinBox( top);
    input2_ = new QrDoubleSpinBox( top);

    if( is_rgba())
        input3_ = new QrDoubleSpinBox( top);

    QSize s = input0_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    Imath::Color4f col = get_value<Imath::Color4f>( *this);

    int xpos = ui::inspector_t::Instance().left_margin();

    button_->move( xpos, 0);
    button_->resize( s.height(), s.height());
    button_->setValue( QrColor( std::pow( (double) col.r, 1.0 / 2.2),
                                std::pow( (double) col.g, 1.0 / 2.2),
                                std::pow( (double) col.b, 1.0 / 2.2)));

    button_->setEnabled( enabled());
    connect( button_, SIGNAL( pressed()), this, SLOT( color_button_pressed()));
    xpos += s.height();

    eyedropper_ = new ui::eyedropper_button_t( top);
    eyedropper_->move( xpos, 0);
    eyedropper_->resize( s.height(), s.height());
    eyedropper_->setEnabled( enabled());
    connect( eyedropper_, SIGNAL( color_picked( const QrColor&)), this, SLOT( eyedropper_color_picked( const QrColor&)));
    xpos += s.height() + 3;

    // make spinboxes a bit smaller
    if( is_rgba())
        s.setWidth( s.width() - 20);
    else
        s.setWidth( s.width() - 10);

    input0_->move( xpos, 0);
    input0_->resize( s.width(), s.height());
    input0_->setValue( col.r);
    input0_->setStep( 0.05f);
    input0_->setEnabled( enabled());
    connect( input0_, SIGNAL( valueChanged(double)), button_, SLOT( setRed(double)));
    connect( input0_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input0_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input0_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input0_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += s.width() + 3;

    input1_->move( xpos, 0);
    input1_->resize( s.width(), s.height());
    input1_->setValue( col.g);
    input1_->setStep( 0.05f);
    input1_->setEnabled( enabled());
    connect( input1_, SIGNAL( valueChanged(double)), button_, SLOT( setGreen(double)));
    connect( input1_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input1_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input1_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input1_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += s.width() + 3;

    input2_->move( xpos, 0);
    input2_->resize( s.width(), s.height());
    input2_->setValue( col.b);
    input2_->setStep( 0.05f);
    input2_->setEnabled( enabled());
    connect( input2_, SIGNAL( valueChanged(double)), button_, SLOT( setBlue(double)));
    connect( input2_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
    connect( input2_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input2_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
    connect( input2_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));

    if( is_rgba())
    {
        xpos += s.width() + 3;
        input3_->move( xpos, 0);
        input3_->resize( s.width(), s.height());
        input3_->setValue( col.a);
        input3_->setRange( 0, 1);
        input3_->setStep( 0.05f);
        input3_->setEnabled( enabled());
        connect( input3_, SIGNAL( valueChanged(double)), button_, SLOT( setBlue(double)));
        connect( input3_, SIGNAL( calculatorValueChanged( double)), this, SLOT( value_changed( double)));
        connect( input3_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
        connect( input3_, SIGNAL( spinBoxMoved( double)), this, SLOT( spinbox_moved( double)));
        connect( input3_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    }

    top->setMinimumSize( ui::inspector_t::Instance().width(), s.height());
    top->setMaximumSize( ui::inspector_t::Instance().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void color_param_t::do_update_widgets()
{
    if( input0_)
    {
        input0_->blockSignals( true);
        input1_->blockSignals( true);
        input2_->blockSignals( true);

        if( input3_)
            input3_->blockSignals( true);

        button_->blockSignals( true);

        Imath::Color4f col = get_value<Imath::Color4f>( *this);
        input0_->setValue( col.r);
        input1_->setValue( col.g);
        input2_->setValue( col.b);

        if( input3_)
            input3_->setValue( col.a);

        button_->setValue( QrColor( std::pow( (double) col.r, 1.0 / 2.2),
                                    std::pow( (double) col.g, 1.0 / 2.2),
                                    std::pow( (double) col.b, 1.0 / 2.2)));

        input0_->blockSignals( false);
        input1_->blockSignals( false);
        input2_->blockSignals( false);

        if( input3_)
            input3_->blockSignals( false);

        button_->blockSignals( false);
    }
}

void color_param_t::do_enable_widgets( bool e)
{
    if( input0_)
    {
        input0_->setEnabled( e);
        input1_->setEnabled( e);
        input2_->setEnabled( e);

        if( input3_)
            input3_->setEnabled( e);

        button_->setEnabled( e);
        eyedropper_->setEnabled( e);
    }
}

void color_param_t::value_changed( double value)
{
    param_set()->begin_edit();

    if( is_rgba())
        set_value( Imath::Color4f( input0_->value(), input1_->value(), input2_->value(), input3_->value()));
    else
        set_value( Imath::Color4f( input0_->value(), input1_->value(), input2_->value(), 1));

    param_set()->end_edit();
}

void color_param_t::spinbox_pressed() { param_set()->begin_edit();}

void color_param_t::spinbox_moved( double value)
{
    if( is_rgba())
        set_value( Imath::Color4f( input0_->value(), input1_->value(), input2_->value(), input3_->value()));
    else
        set_value( Imath::Color4f( input0_->value(), input1_->value(), input2_->value(), 1));

    if( track_mouse())
        param_set()->notify_parent();

    ui::anim_editor_t::Instance().update();
}

void color_param_t::spinbox_released() { param_set()->end_edit( !track_mouse());}

void color_param_t::eyedropper_color_picked( const QrColor& c)
{
    param_set()->begin_edit();

    if( is_rgba())
        set_value( Imath::Color4f( c.red(), c.green(), c.blue(), c.alpha()));
    else
        set_value( Imath::Color4f( c.red(), c.green(), c.blue(), 1));

    update_widgets();
    param_set()->end_edit();
}

void color_param_t::color_button_pressed()
{
    Imath::Color4f col = get_value<Imath::Color4f>( *this);
    QrColor c( col.r, col.g, col.b);
    QrColorPicker *picker = new QrColorPicker( ui::user_interface_t::Instance().main_window(), c);

    if( picker->exec_dialog() == QDialog::Accepted)
    {
        c = picker->color();

        param_set()->begin_edit();

        if( is_rgba())
            set_value( Imath::Color4f( c.red(), c.green(), c.blue(), col.a));
        else
            set_value( Imath::Color4f( c.red(), c.green(), c.blue(), 1));

        update_widgets();
        param_set()->end_edit();
    }

    delete picker;
}

void color_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( animated_param_t);
    ar & boost::serialization::make_nvp( "is_rgba", is_rgba_);
}

void color_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( animated_param_t);
    ar & boost::serialization::make_nvp( "is_rgba", is_rgba_);
}

static bool registered = any_regular_serialization_registry_t::Instance().register_type<Imath::Color4f>();

} // namespace

BOOST_CLASS_EXPORT( ramen::color_param_t)
