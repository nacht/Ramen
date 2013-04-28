// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/apply_gamma.hpp>

#include<algorithm>

#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<half/halfExpLog.h>

namespace ramen
{
namespace image
{
namespace generic
{

namespace
{

struct apply_gamma_fun
{
    apply_gamma_fun( float f) : g_( f) {}

    pixel_t operator()( const pixel_t& src) const
    {
	pixel_t result;
	half h;

	h = std::max( (float) boost::gil::get_color( src, boost::gil::red_t()), 0.0f);
	h = pow_h( h, g_);
	boost::gil::get_color( result, boost::gil::red_t()) = h;

	h = std::max( (float) boost::gil::get_color( src, boost::gil::green_t()), 0.0f);
	h = pow_h( h, g_);
	boost::gil::get_color( result, boost::gil::green_t()) = h;

	h = std::max( (float) boost::gil::get_color( src, boost::gil::blue_t()), 0.0f);
	h = pow_h( h, g_);
	boost::gil::get_color( result, boost::gil::blue_t()) = h;

	boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
	return result;
    }

private:

    float g_;
};

} // unnamed

void apply_gamma( const const_image_view_t& src, const image_view_t& dst, float gamma)
{
    boost::gil::tbb_transform_pixels( src, dst, apply_gamma_fun( gamma));
}

} // namespace
} // namespace
} // namespace
