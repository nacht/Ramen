// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/render/render_flipbook.hpp>

#include<iostream>

#include<OpenEXR/Iex.h>

#include<ramen/flipbook/flipbook.hpp>
#include<ramen/render/renderer.hpp>
#include<ramen/app/document.hpp>
#include<ramen/app/preferences.hpp>

#include<ramen/imageio/imf_memory_stream.hpp>
#include<ramen/imageio/write_exr.hpp>

#include<ramen/util/signal.hpp>

namespace ramen
{
namespace render
{

namespace
{

bool render_cancelled;

void cancel_render( flipbook::flipbook_t *f) { render_cancelled = true;}

} // namespace

bool render_flipbook( flipbook::flipbook_t *flip, node_t *n, int start, int end, bool mblur, int subsample)
{
    render_cancelled = false;
    signal_connection_t connection = flip->closed.connect( &cancel_render);
    std::size_t mem_size = 0;

    render_context_t new_context = document_t::Instance().composition().current_context();
    new_context.mode = flipbook_render;
    new_context.result_node = n;
    new_context.subsample = subsample;
    new_context.motion_blur_extra_samples = 0;
    new_context.motion_blur_shutter_factor = mblur ? 1 : 0;

    // recalc the domain, in case this is our first render
    n->recursive_calc_domain( new_context);

    image::image_t img( n->domain().size().x + 1, n->domain().size().y + 1);

    for( int i = start; i <= end; ++i)
    {
	try
	{
	    new_context.time = i;

	    renderer_t renderer( new_context, true);
	    renderer.render();

            // find the amount of memory required to store a frame.
            // not the most efficient way of doing it, but it's done only once.
            if( mem_size == 0)
            {
                // copy the renderer result to our buffer
                boost::gil::fill_pixels( boost::gil::view( img), image::pixel_t( 0, 0, 0, 0));
                Imath::Box2i area( intersect( n->domain(), n->defined()));

                if( !area.isEmpty())
                {
                    boost::gil::copy_pixels( n->const_subimage_view( area),
                                            boost::gil::subimage_view( boost::gil::view( img),
                                                                     area.min.x - n->domain().min.x,
                                                                     area.min.y - n->domain().min.y,
                                                                     area.size().x + 1,
                                                                     area.size().y + 1));
                }

                Imf::Header header = Imf::Header( renderer.domain().size().x + 1, renderer.domain().size().y + 1);
                header.compression() = Imf::B44_COMPRESSION;

                imageio::imf_null_ostream os;
                imageio::write_half_rgb_exr( os, header, boost::gil::const_view( img));
                mem_size = os.size();
                std::cout << "Flipbook: frame size = " << mem_size / 1024 << " kb \n";
            }

	    std::auto_ptr<imageio::imf_memory_ostream> os( new imageio::imf_memory_ostream( mem_size));

	    if( !os.get())
	    {
		// out of memory
		connection.disconnect();
		return true;
	    }

	    // copy the renderer result to our buffer
	    boost::gil::fill_pixels( boost::gil::view( img), image::pixel_t( 0, 0, 0, 0));
	    Imath::Box2i area( intersect( n->domain(), n->defined()));

	    if( !area.isEmpty())
	    {
		boost::gil::copy_pixels( n->const_subimage_view( area),
					boost::gil::subimage_view( boost::gil::view( img),
								 area.min.x - n->domain().min.x,
								 area.min.y - n->domain().min.y,
								 area.size().x + 1,
								 area.size().y + 1));
	    }

	    Imf::Header header = Imf::Header( renderer.domain().size().x + 1, renderer.domain().size().y + 1);
	    header.compression() = Imf::B44_COMPRESSION;

	    imageio::write_half_rgb_exr( *os, header, boost::gil::const_view( img));

	    if( render_cancelled)
            {
		connection.disconnect();
                return false;
            }

	    flip->add_frame( os);
	}
	catch( ...)
	{
	    // TODO: report out of memory here
	    // if we have any frame, then play
            connection.disconnect();
	    return !flip->empty();
	}
    }

    connection.disconnect();
    return true;
}

} // namespace
} // namespace
