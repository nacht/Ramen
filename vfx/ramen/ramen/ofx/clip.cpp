// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/clip.hpp>

#include<sstream>

#include"ofxImageEffect.h"

#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/util.hpp>

#include<ramen/render/renderer.hpp>
#include<ramen/app/document.hpp>
#include<ramen/app/preferences.hpp>

namespace ramen
{
namespace ofx
{

namespace
{

void *view_get_ptr_and_stride( const image::const_image_view_t& src, int& rowbytes)
{
    image::const_image_view_t::xy_locator loc( src.xy_at( 0, src.height() - 1));
    rowbytes = - (int) loc.row_size();
    return const_cast<void*>( reinterpret_cast<const void*>( &( *src.row_begin( src.height() - 1))));
}

} // unnamed

clip_t::clip_t( OFX::Host::ImageEffect::Instance* effectInstance,
	       OFX::Host::ImageEffect::ClipDescriptor& desc) : OFX::Host::ImageEffect::ClipInstance( effectInstance, desc)
{
    node_ = dynamic_cast<image_effect_t*>( effectInstance)->node();
    assert( node_);

    port_ = node_->index_for_clip_name( desc.getName());
}

const std::string &clip_t::getUnmappedBitDepth() const
{
    static const std::string sfloat( kOfxBitDepthFloat);
    static const std::string snone( kOfxBitDepthNone);

    if( node()->all_inputs_optional())
	return sfloat;

    if( getConnected())
	return sfloat;

    return snone;
}

const std::string &clip_t::getUnmappedComponents() const
{
    static const std::string srgba( kOfxImageComponentRGBA);
    static const std::string snone( kOfxImageComponentNone);

    if( node()->all_inputs_optional())
	return srgba;

    if( getConnected())
	return srgba;

    return snone;
}

const std::string &clip_t::getPremult() const
{
    static const std::string spremult( kOfxImagePreMultiplied);
    return spremult;
}

double clip_t::getAspectRatio() const { return 1.0;}

double clip_t::getFrameRate() const
{
    assert( node());

    if( node()->composition())
	return node()->composition()->frame_rate();
    else
	return 25.0;
}

void clip_t::getFrameRange(double &startFrame, double &endFrame) const
{
    assert( node());

    if( node()->composition())
    {
	startFrame = node()->composition()->start_frame();
	endFrame = node()->composition()->end_frame();
    }
    else
    {
	startFrame = 1;
	endFrame = 100;
    }
}

const std::string &clip_t::getFieldOrder() const
{
    static const std::string sfieldnone( kOfxImageFieldNone);
    return sfieldnone;
}

bool clip_t::getConnected() const
{
    assert( node());

    // is output
    if( port() == -1)
	return true;
    else
    {
	if( node()->num_inputs() == 0)
	    return false;

	if( node()->input( port()) != 0)
	    return true;
	else
	    return false;
    }
}

double clip_t::getUnmappedFrameRate() const
{
    assert( node());

    if( node()->composition())
	return node()->composition()->frame_rate();
    else
	return 25.0;
}

void clip_t::getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame) const
{
    assert( node());

    if( node()->composition())
    {
	unmappedStartFrame = node()->composition()->start_frame();
	unmappedEndFrame = node()->composition()->end_frame();
    }
    else
    {
	unmappedStartFrame = 1;
	unmappedEndFrame = 100;
    }
}

bool clip_t::getContinuousSamples() const { return false;}


OfxRectD clip_t::getRegionOfDefinition(OfxTime time) const
{
    assert( node());
    assert( port_ != -1 && "clip_t: getRoD for output clip called\n");

    const node_t *in = node()->input( port_);

    OfxRectD v;

    if( in)
    {
        Imath::Box2i b = node()->vertical_flip_box( in->bounds());
        v.x1 = b.min.x;
        v.y1 = b.min.y;
        v.x2 = b.max.x + 1;
        v.y2 = b.max.y + 1;
    }
    else
    {
	if( node()->composition())
	{
	    v.x1 = v.y1 = 0;
	    v.x2 = node()->composition()->default_format().width;
	    v.y2 = node()->composition()->default_format().height;
	}
	else
	{
	    v.x1 = v.y1 = 0;
	    v.x2 = preferences_t::Instance().default_format().width;
	    v.y2 = preferences_t::Instance().default_format().height;
	}
    }

    return v;
}

OFX::Host::ImageEffect::Image* clip_t::getImage( OfxTime time, OfxRectD *optionalBounds)
{
    if( getName() == "Output")
	return get_output_image( time, optionalBounds);
    else
	return get_input_image( time, optionalBounds);
}

OFX::Host::ImageEffect::Image* clip_t::get_input_image( OfxTime time, OfxRectD *optionalBounds)
{
    assert( node());
    assert( node()->composition());

    node_t *in = node()->input( port());

    if( !in)
	return 0;

    render::render_context_t context = node()->render_context();

    context.composition = node()->composition();
    context.result_node = in;
    context.time = time;

    Imath::Box2i area;

    if( optionalBounds)
    {
        // TODO: is this correct if the effect does not support tiles?
	area = Imath::Box2i( Imath::V2i( optionalBounds->x1, optionalBounds->y1), Imath::V2i( optionalBounds->x2 - 1, optionalBounds->y2 - 1));
	area = node()->vertical_flip_box( area);
    }
    else
        area = scale( node()->defined(), context.subsample);

    render::renderer_t r( context, true, false);
    r.render( area);

    if( optionalBounds)
    {
	area.min.x = optionalBounds->x1;
	area.min.y = optionalBounds->y1;
	area.max.x = optionalBounds->x2 - 1;
	area.max.y = optionalBounds->y2 - 1;
	area = node()->vertical_flip_box( area);
	area = intersect( in->defined(), area);
    }
    else
	area = in->defined();

    if( area.isEmpty())
	return 0;

    image::image_buffer_t pixels = in->image();
    image::const_image_view_t view( in->const_subimage_view( area));

    int rowbytes;
    void *ptr = view_get_ptr_and_stride( view, rowbytes);

    area = node()->vertical_flip_box( area);

    OfxRectI bounds;
    bounds.x1 = area.min.x;
    bounds.y1 = area.min.y;
    bounds.x2 = area.max.x + 1;
    bounds.y2 = area.max.y + 1;

    std::stringstream s;
    for( int i = 0; i < 16; ++i)
	s << (int) in->digest()[i];

    input_image_t *result = new input_image_t( *this, pixels, 1.0 / node()->render_context().subsample, ptr, bounds, bounds, rowbytes, s.str());
    in->release_image();
    return result;
}

OFX::Host::ImageEffect::Image* clip_t::get_output_image( OfxTime time, OfxRectD *optionalBounds)
{
    assert( node());

    Imath::Box2i area;

    if( optionalBounds)
    {
	area = Imath::Box2i( Imath::V2i( optionalBounds->x1, optionalBounds->y1), Imath::V2i( optionalBounds->x2 - 1, optionalBounds->y2 - 1));
	area = scale( area, 1.0f / node()->render_context().subsample);
	area = node()->vertical_flip_box( area);
    }
    else
	area = node()->defined();

    assert( !node()->image_empty());

    image::image_view_t view( node()->subimage_view( area));

    int rowbytes;
    void *ptr = view_get_ptr_and_stride( view, rowbytes);

    // convert to OFX coordinate sys
    area = node()->vertical_flip_box( area);

    OfxRectI bounds;
    bounds.x1 = area.min.x;
    bounds.y1 = area.min.y;
    bounds.x2 = area.max.x + 1;
    bounds.y2 = area.max.y + 1;

    return new output_image_t( *this, 1.0 / node()->render_context().subsample, ptr, bounds, bounds, rowbytes, std::string( ""));
}

} // namespace
} // namespace
