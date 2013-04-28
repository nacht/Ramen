// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/viewport.hpp>

namespace ramen
{
namespace ui
{

Imath::M33f viewport_t::world_to_screen_matrix() const
{
    Imath::M33f m;

    m = Imath::M33f().setTranslation( Imath::V2f( -world_.min.x, -world_.min.y)) *
        Imath::M33f().setScale( Imath::V2f( zoom_x(), zoom_y())) *
        Imath::M33f().setTranslation( Imath::V2f( device_.min.x, device_.min.y));

    if( y_down_)
    {
        m = m * Imath::M33f().setTranslation( Imath::V2f( 0, -device_.min.y)) *
                Imath::M33f().setScale( Imath::V2f( 1, -1)) *
                Imath::M33f().setTranslation( Imath::V2f( 0, device_.max.y));
    }

    return m;
}

Imath::M33f viewport_t::screen_to_world_matrix() const
{
    Imath::M33f m( world_to_screen_matrix());
    return m.inverse();
}

} // namespace
} // namespace
