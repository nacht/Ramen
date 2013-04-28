// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/render/render_sequence.hpp>

#include<boost/foreach.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/render/renderer.hpp>
#include<ramen/nodes/base/image_output_node.hpp>

#include<ramen/app/application.hpp>

namespace ramen
{
namespace render
{

bool default_render_callback( int x) { return false;}

int total_frames_to_render( composition_t& comp, int start, int end, bool selected_only)
{
    if( end < start)
	return 0;

    int num_outputs = 0;

    BOOST_FOREACH( node_t& n, comp.nodes())
    {
	image_output_node_t *out = dynamic_cast<image_output_node_t*>( &n);

	if( !out || ( selected_only && !out->selected()) || out->ignored())
	    continue;

	++num_outputs;
    }

    return num_outputs * (end - start + 1);
}

void render_sequence( composition_t& comp, int start, int end, int subsample, int mb_extra_samples, float mb_shutter_factor,
		     bool selected_only, const progress_callback_t& callback)
{
    if( end < start)
	return;

    render_context_t new_context = comp.current_context();

    new_context.mode = process_render;
    new_context.subsample = subsample;
    new_context.motion_blur_extra_samples = mb_extra_samples;
    new_context.motion_blur_shutter_factor = mb_shutter_factor;

    bool stop = false;
    int rendered_frames = 0;

    if( callback)
	stop = callback( rendered_frames);

    if( stop)
	return;

    for( int i = start; i <= end; ++i, ++rendered_frames)
    {
	new_context.time = i;

	BOOST_FOREACH( node_t& n, comp.nodes())
	{
	    image_output_node_t *out = dynamic_cast<image_output_node_t*>( &n);

	    if( !out || ( selected_only && !out->selected()) || out->ignored())
		continue;

	    // render one frame
	    {
		new_context.result_node = out;
		renderer_t renderer( new_context, true);
		renderer.render();

		try
		{
		    out->write_image( new_context);
		}
		catch( std::exception& e)
		{
                    application_t::Instance().error( std::string( "exception thrown during render sequence. what = ") + e.what());
		}

		if( callback)
		    stop = callback( rendered_frames);

		if( stop)
		    return;
	    }
	}
    }
}

} // namespace
} // namespace
