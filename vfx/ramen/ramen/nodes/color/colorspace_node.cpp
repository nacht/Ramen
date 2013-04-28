// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/colorspace_node.hpp>

#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{

namespace
{

enum
{
    rgb2yuv = 0,
    rgb2hsv,
    rgb2lab,
    yuv2rgb,
    hsv2rgb,
    lab2rgb
};

} // unnamed

const char *colorspace_node_t::help_string() const
{
    return "Converts between RGB and other color spaces.";
}

colorspace_node_t::colorspace_node_t() : pointop_node_t()
{
    set_name("colorspace");
}

void colorspace_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Convert"));
    p->set_id( "convert");
    p->menu_items() += "RGB to YUV", "RGB to HSV", "RGB to Lab",
			"YUV to RGB", "HSV to RGB", "Lab to RGB";
    add_param( p);
}

void colorspace_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    int convert = get_value<int>( param( "convert"));

    switch( convert)
    {
    case rgb2yuv:
	image::convert_rgb_to_yuv( src, dst);
    break;

    case rgb2hsv:
	image::convert_rgb_to_hsv( src, dst);
    break;

    case rgb2lab:
	image::convert_rgb_to_lab( src, dst);
    break;

    case yuv2rgb:
	image::convert_yuv_to_rgb( src, dst);
    break;

    case hsv2rgb:
	image::convert_hsv_to_rgb( src, dst);
    break;

    case lab2rgb:
	image::convert_lab_to_rgb( src, dst);
    break;
    }
}

void colorspace_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void colorspace_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_colorspace_node() { return new colorspace_node_t();}

node_info_t colorspace_node_info()
{
    node_info_t info;
    info.id = "builtin.colorspace";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "Colorspace";
    info.create = &create_colorspace_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( colorspace_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::colorspace_node_t)
