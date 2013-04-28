// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/filter/dv_chroma_blur_node.hpp>

#include<tbb/parallel_for.h>
#include<tbb/blocked_range.h>

#include<ramen/image/processing.hpp>

namespace ramen
{

namespace
{

struct apply_chroma_joint_bilateral
{
    apply_chroma_joint_bilateral( const image::const_image_view_t& src, const image::image_view_t& dst,
                                    const image::const_gray_image_view_t& filter, float csigma = 0.3f) : src_( src), dst_( dst), filter_( filter)
    {
        color_sigma_ = csigma;
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
        // convolve
        int xoff = 2;
        int yoff = 2;

        float color_sigma_mult = 0.5f / ( color_sigma_ * color_sigma_);

        for (int y = r.begin(); y < r.end(); y++)
        {
            for (int x = 0; x < src_.width(); x++)
            {
                float total_weight = 0;

                for (int dy = -yoff; dy <= yoff; dy++)
                {
                    int imy = y + dy;

                    if (imy < 0)                continue;
                    if (imy >= src_.height())   break;

                    int filtery = dy + yoff;
                    
                    for (int dx = -xoff; dx <= xoff; dx++)
                    {
                        int imx = x + dx;

                        if (imx < 0)                continue;
                        if (imx >= src_.width())    break;

                        int filterx = dx + xoff;
                        float weight = filter_( filterx, filtery)[0];

                        float diff = src_( imx, imy)[0] - src_( x, y)[0];
                        float lum_distance = diff * diff * color_sigma_mult;

                        weight *= std::exp( (double) -lum_distance);
                        total_weight += weight;

                        dst_( x, y)[1] += weight * src_( imx, imy)[1];
                        dst_( x, y)[2] += weight * src_( imx, imy)[2];
                    }
                }

                dst_( x, y)[0] = src_( x, y)[0];
                dst_( x, y)[1] /= total_weight;
                dst_( x, y)[2] /= total_weight;
                dst_( x, y)[3] = src_( x, y)[3];
            }
        }
    }

private:

    const image::const_image_view_t& src_;
    const image::image_view_t& dst_;
    const image::const_gray_image_view_t& filter_;
    float color_sigma_;
};

} // unnamed

const char *dv_chroma_blur_node_t::help_string() const
{
    return "Blurs the chrominance channels of the input image "
            "using a joint bilateral filter. It's useful to "
            "reduce chroma subsampling and compression artifacts in DV video.";
}

dv_chroma_blur_node_t::dv_chroma_blur_node_t() : areaop_node_t()
{
    set_name( "dv_cblur");
}

void dv_chroma_blur_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    hradius = vradius = 2;
}

void dv_chroma_blur_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i area( intersect( input()->defined(), defined()));

    if( area.isEmpty())
	return;

    image::const_image_view_t src( input()->const_subimage_view( area));
    image::image_view_t dst( subimage_view( area));

    image::image_t tmp( src.width(), src.height());
    image::convert_rgb_to_yuv( src, boost::gil::view( tmp));

    // create the kernel
    const float filterWidth  = 4.0f / 6.0f;
    const float filterHeight = 4.0f / 6.0f;

    image::gray_image_t filter_img( 5, 5);
    image::gray_image_view_t filter( boost::gil::view( filter_img));

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 5; x++)
        {
            float dx = (x - 5 / 2) / filterWidth;
            float dy = (y - 5 / 2) / filterHeight;
            float d2 = dx * dx + dy * dy;
            filter(x, y)[0] = std::exp( (double) - d2 / 2);
        }
    }

    tbb::parallel_for( tbb::blocked_range<int>( 0, src.height()),
                        apply_chroma_joint_bilateral( boost::gil::const_view( tmp), dst, filter),
                        tbb::auto_partitioner());

    image::convert_yuv_to_rgb( dst, dst);
}

void dv_chroma_blur_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

void dv_chroma_blur_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

// factory
node_t *create_dv_chroma_blur_node() { return new dv_chroma_blur_node_t();}

node_info_t dv_chroma_blur_node_info()
{
	node_info_t info;
	info.id = "builtin.dv_chroma_blur";
	info.major_version = 1;
	info.minor_version = 0;
	info.submenu = "Filter";
	info.menu_item = "DV Chroma Blur";
	info.create = &create_dv_chroma_blur_node;
	return info;
}

static bool registered = node_factory_t::Instance().register_node( dv_chroma_blur_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::dv_chroma_blur_node_t)
