// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/render/render_context.hpp>
#include<sstream>

namespace
{

void default_progress( float x) {}
bool default_cancel() { return false;}

} // namespace

namespace ramen
{
namespace render
{

render_context_t::render_context_t() : mode( interface_render), time( 1), subsample( 1)
{
    progress = default_progress;
    cancel = default_cancel;
    composition = 0;
    result_node = 0;
    motion_blur_extra_samples = 0;
    motion_blur_shutter_factor = 1;
}

std::string render_context_t::str() const
{
    std::stringstream s;
    s << (*this);
    return s.str();
}

std::ostream& operator << ( std::ostream& os, const render_context_t& c)
{
    os << c.mode << "," << c.time << "," << c.subsample << "," << c.default_format <<
            c.motion_blur_extra_samples << ", " << c.motion_blur_shutter_factor;
    return os;
}

} // namespace
} // namespace
