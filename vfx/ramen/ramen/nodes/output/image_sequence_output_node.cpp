// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/output/image_sequence_output_node.hpp>

#include<boost/bind.hpp>
#include<boost/format.hpp>
#include<boost/serialization/map.hpp>
#include<boost/serialization/version.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/file_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/combo_group_param.hpp>
#include<ramen/params/group_param.hpp>

#include<ramen/imageio/factory.hpp>
#include<ramen/imageio/enums.hpp>

#include<ramen/serialization/any_regular.hpp>

namespace
{

enum output_format
{
    exr_format = 0,
    jpg_format,
    cin_format,
    dpx_format,
    hdr_format,
    tiff_format,
    tga_format,
    png_format
};

} // namespace

namespace ramen
{

image_sequence_output_node_t::image_sequence_output_node_t() : image_output_node_t()
{
    set_name( "image_out");
    param_set().param_changed.connect( boost::bind( &image_sequence_output_node_t::param_changed, this, _1));
    set_dont_persist_params( true);
}

image_sequence_output_node_t::image_sequence_output_node_t( const image_sequence_output_node_t& other) : image_output_node_t( other)
{
    param_set().param_changed.connect( boost::bind( &image_sequence_output_node_t::param_changed, this, _1));
}

void image_sequence_output_node_t::do_create_params()
{
    std::auto_ptr<file_param_t> out( new file_param_t( "Output"));
    out->set_id( "output");
    out->set_is_input( false);
    add_param( out);

    std::auto_ptr<combo_group_param_t> top( new combo_group_param_t( "Format"));
    top->set_id( "format");

    // exr
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "EXR"));
	group->set_id( "exr");

	std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
	p->set_id( "exr_channels");
	p->menu_items() += "RGBA", "RGB", "Alpha";
	group->add_param( p);

	p.reset( new popup_param_t( "Type"));
	p->set_id( "exr_type");
	p->menu_items() += "Half", "Float";
	group->add_param( std::auto_ptr<param_t>( p));

	p.reset( new popup_param_t( "Compress"));
	p->set_id( "exr_compress");
	p->menu_items() += "None", "RLE", "ZIPS", "ZIP", "PIZ", "PXR24", "B44", "B44A";
	p->set_default_value( (int) 4);
	group->add_param( p);

	std::auto_ptr<bool_param_t> b( new bool_param_t( "Multilevel"));
	b->set_id( "multilevel");
	b->set_default_value( false);
	b->set_enabled( false);
	group->add_param( b);

	top->add_param( group);
    }

   // jpg
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Jpg"));
	group->set_id( "jpg");

	std::auto_ptr<float_param_t> p( new float_param_t( "Quality"));
	p->set_id( "jpg_quality");
	p->set_static( true);
	p->set_range( 10, 100);
	p->set_default_value( 90);
	group->add_param( std::auto_ptr<param_t>( p));

	top->add_param( group);
    }

    // cin
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Cineon"));
	group->set_id( "cineon");
	top->add_param( group);
    }

    // dpx
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Dpx"));
	group->set_id( "dpx");
	top->add_param( group);
    }

    // hdr
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "HDR"));
	group->set_id( "hdr");
	top->add_param( group);
    }

    // tiff
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Tiff"));
	group->set_id( "tiff");

	std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
	p->set_id( "tiff_channels");
	p->menu_items() += "RGBA", "RGB";
	group->add_param( p);

	p.reset( new popup_param_t( "Type"));
	p->set_id( "tiff_type");
	p->menu_items() += "8 Bits", "16 Bits", "Float";
	group->add_param( std::auto_ptr<param_t>( p));

	p.reset( new popup_param_t( "Compress"));
	p->set_id( "tiff_compress");
	p->menu_items() += "None", "LZW", "ZIP";
	group->add_param( p);

	top->add_param( group);
    }

    // tga
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Tga"));
	group->set_id( "tga");

	std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
	p->set_id( "tga_channels");
	p->menu_items() += "RGBA", "RGB";
	group->add_param( p);

	p.reset( new popup_param_t( "Compress"));
	p->set_id( "tga_compress");
	p->menu_items() += "None", "RLE";
	group->add_param( p);

	top->add_param( group);
    }

    // png
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Png"));
	group->set_id( "png");

	std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
	p->set_id( "png_channels");
	p->menu_items() += "RGBA", "RGB", "Alpha";
	group->add_param( p);

	top->add_param( group);
    }

    add_param( top);
}

void image_sequence_output_node_t::param_changed( param_t *p)
{
    file_param_t *out		= dynamic_cast<file_param_t*>( &param( "output"));
    combo_group_param_t *format = dynamic_cast<combo_group_param_t*>( &param( "format"));
    int selected_format = get_value<int>( *format);

    if( p == out)
    {
	if( !out->empty())
	{
	    std::string ext = out->extension();

	    if( ext.empty())
	    {
		out->set_extension( extension_for_format( selected_format));
		out->update_widgets();
	    }
	    else
		set_format_for_extension( ext);
	}
    }
    else
    {
	if( p == format)
	{
	    if( !out->empty())
	    {
		out->set_extension( extension_for_format( selected_format));
		out->update_widgets();
	    }
	}
    }
}

void image_sequence_output_node_t::do_calc_defined( const render::render_context_t& context)
{
    set_defined( input()->domain());
}

void image_sequence_output_node_t::do_process( const render::render_context_t& context)
{
    // just in case
    boost::gil::fill_pixels( image_view(), image::pixel_t( 0, 0, 0, 0));

    input_defined_ = input()->defined();

    Imath::Box2i area( intersect( input_defined_, defined()));

    if( !area.isEmpty())
	boost::gil::copy_pixels( input()->const_subimage_view( area), subimage_view( area));
}

void image_sequence_output_node_t::write_image( const render::render_context_t& context)
{
    boost::filesystem::path p( get_value<boost::filesystem::path>( param( "output")));
    std::string in( p.string());
    boost::format formater( in);
    formater.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit));
    formater % ((int) context.time);
    p = formater.str();

    if( p.empty())
	throw( std::runtime_error( "Image output empty path"));

    std::string tag;
    adobe::dictionary_t params;

    params[adobe::name_t( "domain")]	= adobe::any_regular_t( domain());
    params[adobe::name_t( "defined")]	= adobe::any_regular_t( input_defined_);

    switch( get_value<int>( param( "format")))
    {
    case exr_format:
	tag = "exr";
	params[adobe::name_t( "channels")]	= adobe::any_regular_t( get_value<int>( param( "exr_channels")));
	params[adobe::name_t( "type")]		= adobe::any_regular_t( get_value<int>( param( "exr_type")));
	params[adobe::name_t( "compress")]	= adobe::any_regular_t( get_value<int>( param( "exr_compress")));
	params[adobe::name_t( "multilevel")]	= adobe::any_regular_t( get_value<bool>( param( "multilevel")));
    break;

    case jpg_format:
	tag = "jpg";
	params[adobe::name_t( "quality")]  = adobe::any_regular_t( get_value<float>( param( "jpg_quality")));
    break;

    case cin_format:
	tag = "cin";
    break;

    case dpx_format:
        tag = "dpx";
    break;

    case hdr_format:
	tag = "hdr";
    break;

    case tiff_format:
	tag = "tiff";
	params[adobe::name_t( "channels")]  = adobe::any_regular_t( get_value<int>( param( "tiff_channels")));
	params[adobe::name_t( "type")]	    = adobe::any_regular_t( get_value<int>( param( "tiff_type")) + 1);
	params[adobe::name_t( "compress")]  = adobe::any_regular_t( get_value<int>( param( "tiff_compress")));
    break;

    case tga_format:
	tag = "tga";
	params[adobe::name_t( "channels")]  = adobe::any_regular_t( get_value<int>( param( "tga_channels")));
	params[adobe::name_t( "compress")]  = adobe::any_regular_t( get_value<int>( param( "tga_compress")));
    break;

    case png_format:
	tag = "png";
	params[adobe::name_t( "channels")]  = adobe::any_regular_t( get_value<int>( param( "png_channels")));
    break;
    }

    std::auto_ptr<imageio::writer_t> writer( imageio::factory_t::Instance().writer_for_tag( tag));

    if( writer.get())
	writer->write_image( p, image_view(), params);
}

std::string image_sequence_output_node_t::extension_for_format( int format) const
{
    switch( format)
    {
    case exr_format:
	return std::string( ".exr");
    break;

    case jpg_format:
	return std::string( ".jpg");
    break;

    case cin_format:
	return std::string( ".cin");
    break;

    case dpx_format:
	return std::string( ".dpx");
    break;

    case hdr_format:
	return std::string( ".hdr");
    break;

    case tiff_format:
	return std::string( ".tif");
    break;

    case tga_format:
	return std::string( ".tga");
    break;

    case png_format:
	return std::string( ".png");
    break;

    default:
        return std::string(); // to keep MSVC happy
    }
}

void image_sequence_output_node_t::set_format_for_extension( const std::string& ext)
{
    combo_group_param_t *format = dynamic_cast<combo_group_param_t*>( &param( "format"));
    int selected_format = get_value<int>( *format);

    if( ext == ".exr" || ext == ".EXR")
    {
	if( selected_format != exr_format)
	    format->set_value( (int) exr_format);

	format->update_widgets();
	return;
    }

    if( ext == ".jpg" || ext == ".JPG" || ext == ".jpeg" || ext == ".JPEG")
    {
	if( selected_format != jpg_format)
	    format->set_value( (int) jpg_format);

	format->update_widgets();
	return;
    }

    if( ext == ".cin" || ext == ".CIN")
    {
	if( selected_format != cin_format)
	    format->set_value( (int) cin_format);

	format->update_widgets();
	return;
    }

    if( ext == ".dpx" || ext == ".DPX")
    {
	if( selected_format != dpx_format)
	    format->set_value( (int) dpx_format);

	format->update_widgets();
	return;
    }

    if( ext == ".hdr" || ext == ".HDR" || ext == ".rgbe" || ext == ".RGBE")
    {
	if( selected_format != hdr_format)
	    format->set_value( (int) hdr_format);

	format->update_widgets();
	return;
    }

    if( ext == ".tif" || ext == ".TIF" || ext == ".tiff" || ext == ".TIFF")
    {
	if( selected_format != tiff_format)
	    format->set_value( (int) tiff_format);

	format->update_widgets();
	return;
    }

    if( ext == ".tga" || ext == ".TGA")
    {
	if( selected_format != tga_format)
	    format->set_value( (int) tga_format);

	format->update_widgets();
	return;
    }

    if( ext == ".png" || ext == ".PNG")
    {
	if( selected_format != png_format)
	    format->set_value( (int) png_format);

	format->update_widgets();
	return;
    }
}

void image_sequence_output_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_output_node_t);

    std::map<std::string, adobe::any_regular_t> param_map;
    ar & BOOST_SERIALIZATION_NVP( param_map);

    param_set().set_param_values( param_map);
}

void image_sequence_output_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_output_node_t);

    std::map<std::string, adobe::any_regular_t> param_map;
    param_set().param_values_map( param_map);
    ar & BOOST_SERIALIZATION_NVP( param_map);
}

// factory
node_t *create_image_sequence_output_node() { return new image_sequence_output_node_t();}

node_info_t image_output_node_info()
{
    node_info_t info;
    info.id = "builtin.image_seq_output";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Output";
    info.menu_item = "Image";
    info.create = &create_image_sequence_output_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( image_output_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::image_sequence_output_node_t)
