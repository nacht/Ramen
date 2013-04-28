// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RENDER_CMD_LINE_HPP
#define	RAMEN_RENDER_CMD_LINE_HPP

#include<ramen/app/composition_fwd.hpp>

#include<boost/function.hpp>

namespace ramen
{
namespace render
{

typedef boost::function<bool ( int)> progress_callback_t;

bool default_render_callback( int x);

int total_frames_to_render( composition_t& comp, int start, int end, bool selected_only = false);

void render_sequence( composition_t& comp, int start, int end, int subsample, int mb_extra_samples, float mb_shutter_factor, 
                        bool selected_only = false, const progress_callback_t& callback = default_render_callback);

} // namespace
} // namespace

#endif
