// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MANIPULABLE_HPP
#define RAMEN_MANIPULABLE_HPP

#include<ramen/params/parameterised.hpp>

#include<memory>

#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/manipulators/manipulator.hpp>

#include<ramen/ui/image_view_fwd.hpp>

class QEvent;
class QKeyEvent;
class QMouseEvent;

namespace ramen
{

/**
 * @class manipulable_t
 * @brief an object that has a collection of manipulators,
 * can draw on screen controls and accepts user interface events
 *
 * manipulable_t is an object that has a collection of manipulators.
 */

class manipulable_t : public parameterised_t
{
public:

    manipulable_t();
    manipulable_t( const manipulable_t& other);

    virtual ~manipulable_t();

    void create_manipulators();

    template<class T>
    void add_manipulator( std::auto_ptr<T> p)
    {
        p->set_parent( this);
        manipulators_.push_back( p.release());
        active_ = manipulators_.end();
    }

    const boost::ptr_vector<manipulator_t>& manipulators() const    { return manipulators_;}
    boost::ptr_vector<manipulator_t>& manipulators()                { return manipulators_;}

    typedef boost::ptr_vector<manipulator_t>::const_iterator const_iterator;
    typedef boost::ptr_vector<manipulator_t>::iterator iterator;

    const_iterator begin() const    { return manipulators_.begin();}
    const_iterator end() const	    { return manipulators_.end();}

    iterator begin()	{ return manipulators_.begin();}
    iterator end()	{ return manipulators_.end();}

    void draw_overlay( ui::image_view_t& view) const;

    bool key_press_event( ui::image_view_t& view, QKeyEvent *event);
    bool key_release_event( ui::image_view_t& view, QKeyEvent *event);

    void mouse_enter_event( ui::image_view_t& view, QEvent *event);
    void mouse_leave_event( ui::image_view_t& view, QEvent *event);

    bool mouse_press_event( ui::image_view_t& view, QMouseEvent *event);
    void mouse_move_event( ui::image_view_t& view, QMouseEvent *event);
    void mouse_drag_event( ui::image_view_t& view, QMouseEvent *event);
    void mouse_release_event( ui::image_view_t& view, QMouseEvent *event);

protected:

    virtual void do_draw_overlay( ui::image_view_t& view) const;

    virtual bool do_key_press_event( ui::image_view_t& view, QKeyEvent *event);
    virtual bool do_key_release_event( ui::image_view_t& view, QKeyEvent *event);

    virtual void do_mouse_enter_event( ui::image_view_t& view, QEvent *event);
    virtual void do_mouse_leave_event( ui::image_view_t& view, QEvent *event);

    virtual bool do_mouse_press_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void do_mouse_move_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void do_mouse_drag_event( ui::image_view_t& view, QMouseEvent *event);
    virtual void do_mouse_release_event( ui::image_view_t& view, QMouseEvent *event);

private:

    virtual void do_create_manipulators();

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;

    boost::ptr_vector<manipulator_t> manipulators_;
    iterator active_;
};

} // namespace

#endif
