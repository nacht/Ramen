/**
 * @file
 * @brief This struct contains the current rendering state
 */

// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RENDER_RENDER_CONTEXT_HPP
#define RAMEN_RENDER_RENDER_CONTEXT_HPP

#include<ramen/util/format.hpp>

#include<boost/function.hpp>

#include<ramen/app/composition_fwd.hpp>
#include<ramen/nodes/node_fwd.hpp>

#include<iostream>

namespace ramen
{
namespace render
{

enum render_mode
{
	interface_render = 1,
	flipbook_render,
	process_render
};

/**
 * @class render_context_t
 * @brief This struct contains the current rendering state
 *
 */

struct render_context_t
{
    render_context_t();

    std::string str() const;

    /// pointer to composition
    composition_t *composition;

    /// rendering mode
    render_mode mode;

    /// node to render
    node_t *result_node;
    Imath::Box2i interest;

    float time;
    int subsample;

    format_t default_format;

    int motion_blur_extra_samples;
    float motion_blur_shutter_factor;

    boost::function<void (float)> progress;
    boost::function<bool ()> cancel;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version)
    {
	ar & BOOST_SERIALIZATION_NVP( mode);
	ar & BOOST_SERIALIZATION_NVP( time);
	ar & BOOST_SERIALIZATION_NVP( subsample);
	ar & BOOST_SERIALIZATION_NVP( default_format);
        ar & BOOST_SERIALIZATION_NVP( motion_blur_extra_samples);
        ar & BOOST_SERIALIZATION_NVP( motion_blur_shutter_factor);
    }
};

std::ostream& operator << ( std::ostream& os, const render_context_t& c);

} // namespace
} // namespace

#endif
