// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RENDERER_HPP
#define	RAMEN_RENDERER_HPP

#include<boost/optional.hpp>

#include<ramen/nodes/node_fwd.hpp>
#include<ramen/image/image_buffer.hpp>

#include<ramen/render/render_context.hpp>

namespace ramen
{
namespace render
{

class renderer_t
{
public:

    renderer_t( const render_context_t& c, bool save_state = false, bool release_image = true);
    ~renderer_t();

    void calc_areas( const Imath::Box2i& roi);
    const Imath::Box2i& domain() const;
    const Imath::Box2i& bounds() const;

    void render( const Imath::Box2i& roi);
    void render();

    image::image_buffer_t image();
    image::const_image_view_t domain_image_view() const;

private:

    render_context_t context_;
    bool restore_state_;
    bool release_image_;

    // debugging
public:

    static bool log_areas;
};

} // namespace
} // namespace

#endif
