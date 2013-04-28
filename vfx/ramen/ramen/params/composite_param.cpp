// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/composite_param.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>
#include<boost/ptr_container/serialize_ptr_vector.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<QWidget>
#include<QVBoxLayout>
#include<QFrame>

#include<ramen/params/param_set.hpp>
#include<ramen/anim/track.hpp>

namespace ramen
{

composite_param_t::composite_param_t() : param_t() {}
composite_param_t::composite_param_t( const std::string& name) : param_t( name) {}

composite_param_t::composite_param_t( const composite_param_t& other) : param_t( other), params_( other.params_)
{
}

    // this is for OFX
void composite_param_t::copy( const param_t& other, float offset)
{
    assert( 0 && "Copy called for a composite param");
}

void composite_param_t::copy( const param_t& other, float offset, float start, float end)
{
    assert( 0 && "Copy called for a composite param");
}

void composite_param_t::set_param_set( param_set_t *parent)
{
    param_t::set_param_set( parent);
    adobe::for_each( params(), boost::bind( &param_t::set_param_set, _1, parent));
}

void composite_param_t::do_init()
{
    adobe::for_each( params(), boost::bind( &param_t::init, _1));
}

void composite_param_t::do_add_param( param_t *p)
{
    p->set_param_set( param_set());

    if( param_set())
	param_set()->add_param_to_map( *p);

    params().push_back( p);
}

std::auto_ptr<undo::command_t> composite_param_t::create_reset_command()
{
    std::auto_ptr<undo::composite_command_t> c( new undo::composite_command_t( "Reset"));

    BOOST_FOREACH( param_t& p, params())
    {
        std::auto_ptr<undo::command_t> q( p.create_reset_command());

        if( q.get())
            c->push_back( q);
    }

    if( c->empty())
        return std::auto_ptr<undo::command_t>();

    return std::auto_ptr<undo::command_t>( c.release());
}

void composite_param_t::do_create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( name()));
    adobe::for_each( params(), boost::bind( &param_t::create_tracks, _1, t.get()));

    if( t->num_children() != 0)
	parent->add_child( t);
}

void composite_param_t::do_evaluate( float time)
{
    adobe::for_each( params(), boost::bind( &param_t::evaluate, _1, time));
}

std::string composite_param_t::do_str() const
{
    std::string str;
    BOOST_FOREACH( const param_t& p, params()) { str.append( p.str());}
    return str;
}

QWidget *composite_param_t::do_create_widgets()
{
    QWidget *widget = new QWidget();
    create_widgets_inside_widget( widget);
    return widget;
}

void composite_param_t::create_widgets_inside_widget( QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);
    layout->setSpacing( 5);
    layout->setSizeConstraint( QLayout::SetFixedSize);

    BOOST_FOREACH( param_t& p, params())
    {
        if( !p.secret())
        {
            QWidget *w = p.create_widgets();

            if( w)
                layout->addWidget( w);
        }
    }

    layout->addStretch();
    parent->setLayout( layout);
}

void composite_param_t::do_update_widgets()
{
    adobe::for_each( params_, boost::bind( &param_t::update_widgets, _1));
}

void composite_param_t::do_enable_widgets( bool e)
{
    adobe::for_each( params_, boost::bind( &param_t::enable_widgets, _1, e));
}

void composite_param_t::do_domain_changed( const Imath::Box2i& new_domain)
{
    adobe::for_each( params_, boost::bind( &param_t::domain_changed, _1, new_domain));
}

void composite_param_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
    ar & boost::serialization::make_nvp( "params", params_);
}

void composite_param_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( param_t);
    ar & boost::serialization::make_nvp( "params", params_);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::composite_param_t)
