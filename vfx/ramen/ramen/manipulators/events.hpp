// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MANIPULATORS_EVENTS_HPP
#define RAMEN_MANIPULATORS_EVENTS_HPP

#include<OpenEXR/ImathVec.h>

namespace ramen
{

struct ui_event_t
{
    enum modifiers_t
    {
        control_modifier    = 1 << 0,
        shift_modifier      = 1 << 1,
        alt_modifier        = 1 << 2
    };

    ui_event_t( int mod = 0) : modifiers( mod) {}

    int modifiers;
};

struct mouse_event_t : public ui_event_t
{
    enum mouse_button_t
    {
        left_button     = 1 << 0,
        middle_button   = 1 << 1,
        right_button    = 1 << 2
    };

    mouse_event_t( const Imath::V2f& p, int bt = 0, int mod = 0) : ui_event_t( mod)
    {
        button = bt;
        screen_pos = p;
        world_pos = p;
    }

    mouse_event_t( const Imath::V2f& p, const Imath::V2f& w, int bt = 0, int mod = 0) : ui_event_t( mod)
    {
        button = bt;
        screen_pos = p;
        world_pos = w;
    }

    int button;
    Imath::V2f screen_pos;
    Imath::V2f world_pos;
};

struct key_event_t : public ui_event_t
{
    enum key_t
    {
        enter_key,
        backspace_key,
        escape_key
        // ...
    };

    key_event_t( key_t k, int mod = 0) : ui_event_t( mod), key( k) {}

    key_t key;
};

} // namespace

#endif
