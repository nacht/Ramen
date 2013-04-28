// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/manipulators/manipulable.hpp>

#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<QMouseEvent>

#include<ramen/ui/image_view.hpp>

#include<iostream>

namespace ramen
{

manipulable_t::manipulable_t() : parameterised_t() {}
manipulable_t::manipulable_t( const manipulable_t& other) : parameterised_t( other) {}

manipulable_t::~manipulable_t() {}

void manipulable_t::create_manipulators()
{
    do_create_manipulators();
}

void manipulable_t::do_create_manipulators() {}

void manipulable_t::draw_overlay( ui::image_view_t& view) const    { do_draw_overlay( view);}
void manipulable_t::do_draw_overlay( ui::image_view_t& view) const
{
    adobe::for_each( manipulators(), boost::bind( &manipulator_t::draw_overlay, _1, boost::ref( view)));
}

bool manipulable_t::key_press_event( ui::image_view_t& view, QKeyEvent *event)          { return do_key_press_event( view, event);}

bool manipulable_t::do_key_press_event( ui::image_view_t& view, QKeyEvent *event)
{
    for( iterator it( begin()); it != end(); ++it)
    {
        if( it->key_press_event( view, event))
            return true;
    }

    return false;
}

bool manipulable_t::key_release_event( ui::image_view_t& view, QKeyEvent *event)    { return do_key_release_event( view, event);}

bool manipulable_t::do_key_release_event( ui::image_view_t& view, QKeyEvent *event)
{
    for( iterator it( begin()); it != end(); ++it)
    {
        if( it->key_release_event( view, event))
            return true;
    }

    return false;
}

void manipulable_t::mouse_enter_event( ui::image_view_t& view, QEvent *event)           { do_mouse_enter_event( view, event);}
void manipulable_t::do_mouse_enter_event( ui::image_view_t& view, QEvent *event)
{
    adobe::for_each( manipulators(), boost::bind( &manipulator_t::mouse_enter_event, _1, boost::ref( view), event));
}

void manipulable_t::mouse_leave_event( ui::image_view_t& view, QEvent *event)           { do_mouse_leave_event( view, event);}
void manipulable_t::do_mouse_leave_event( ui::image_view_t& view, QEvent *event)
{
    adobe::for_each( manipulators(), boost::bind( &manipulator_t::mouse_leave_event, _1, boost::ref( view), event));
}

bool manipulable_t::mouse_press_event( ui::image_view_t& view, QMouseEvent *event)
{
    return do_mouse_press_event( view, event);
}

bool manipulable_t::do_mouse_press_event( ui::image_view_t& view, QMouseEvent *event)
{
    active_ = end();

    for( iterator it( begin()); it != end(); ++it)
    {
        if( it->mouse_press_event( view, event))
        {
            active_ = it;
            break;
        }
    }
    
    if( active_ != end())
    {
        return true;
    }
    
    return false;
}

void manipulable_t::mouse_move_event( ui::image_view_t& view, QMouseEvent *event) { do_mouse_move_event( view, event);}

void manipulable_t::do_mouse_move_event( ui::image_view_t& view, QMouseEvent *event)
{
    adobe::for_each( manipulators(), boost::bind( &manipulator_t::mouse_move_event, _1, boost::ref( view), event));
}

void manipulable_t::mouse_drag_event( ui::image_view_t& view, QMouseEvent *event) { do_mouse_drag_event( view, event);}

void manipulable_t::do_mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)
{
    if( active_ != end())
        active_->mouse_drag_event( view, event);
}

void manipulable_t::mouse_release_event( ui::image_view_t& view, QMouseEvent *event) { do_mouse_release_event( view, event);}

void manipulable_t::do_mouse_release_event( ui::image_view_t& view, QMouseEvent *event)
{
    if( active_ != end())
        active_->mouse_release_event( view, event);

    active_ = end();
}

void manipulable_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( parameterised_t);
}

void manipulable_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( parameterised_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::manipulable_t)
