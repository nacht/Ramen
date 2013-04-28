// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/channel_mixer_node.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

#include<ramen/params/float_param.hpp>

namespace ramen
{

namespace
{

struct channel_mixer_fun
{
    channel_mixer_fun( float mr, float mg, float mb)
    {
	mr_ = mr / (mr + mg + mb);
	mg_ = mg / (mr + mg + mb);
	mb_ = mb / (mr + mg + mb);
    }

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	float val = (src[0] * mr_) + (src[1] * mg_) + (src[2] * mb_);
	return image::pixel_t( val, val, val, src[3]);
    }

private:

    float mr_, mg_, mb_;
};

}

channel_mixer_node_t::channel_mixer_node_t() : pointop_node_t()
{
    set_name("ch mix");
}

void channel_mixer_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Red"));
    p->set_id( "red");
    p->set_default_value( 0.212671f);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Green"));
    p->set_id( "green");
    p->set_default_value( 0.715160f);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Blue"));
    p->set_id( "blue");
    p->set_default_value( 0.072169f);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);
}

void channel_mixer_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    boost::gil::tbb_transform_pixels( src, dst, 
					  channel_mixer_fun( get_value<float>( param( "red")),
							     get_value<float>( param( "green")),
							     get_value<float>( param( "blue"))));
}

void channel_mixer_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void channel_mixer_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_channel_mixer_node() { return new channel_mixer_node_t();}

node_info_t channel_mixer_node_info()
{
    node_info_t info;
    info.id = "builtin.channel_mixer";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "Channel Mixer";
    info.create = &create_channel_mixer_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( channel_mixer_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::channel_mixer_node_t)
