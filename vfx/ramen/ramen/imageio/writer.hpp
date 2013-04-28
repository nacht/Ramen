// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_WRITER_HPP
#define	RAMEN_WRITER_HPP

#include<vector>

#include<boost/filesystem/path.hpp>
#include<boost/cstdint.hpp>

#include<adobe/dictionary.hpp>
#include<adobe/algorithm/clamp.hpp>

#include<IECore/FastFloat.h>

#include<half/halfExpLog.h>

#include<ramen/image/typedefs.hpp>

#include<ramen/imageio/exceptions.hpp>
#include<ramen/imageio/enums.hpp>

namespace ramen
{
namespace imageio
{

class writer_t
{
public:

    writer_t() {}
    virtual ~writer_t() {}

    void write_image( const boost::filesystem::path& p,
			const image::const_image_view_t& view,
			const adobe::dictionary_t& params) const
    {
        do_write_image( p, view, params);
    }

private:

    virtual void do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const = 0;

protected:

    boost::uint8_t convert8( float x) const
    {
       return IECore::fastFloat2Int( adobe::clamp( x, 0.0f, 1.0f) * 255.0);
    }

    boost::uint16_t convert16( float x) const
    {
       return IECore::fastFloat2Int( adobe::clamp( x, 0.0f, 1.0f) * 65535.0);
    }

    float apply_gamma( float x, float g) const
    {
	half h = x;
	h = pow_h( h, g);
        return h;
    }

    template<class PixelIter>
    void apply_gamma( float gamma, PixelIter start, PixelIter end) const
    {
        for( PixelIter it( start); it != end; ++it)
        {
            boost::gil::get_color( *it, boost::gil::red_t())   = apply_gamma( boost::gil::get_color( *it, boost::gil::red_t()), gamma);
            boost::gil::get_color( *it, boost::gil::green_t()) = apply_gamma( boost::gil::get_color( *it, boost::gil::green_t()), gamma);
            boost::gil::get_color( *it, boost::gil::blue_t())  = apply_gamma( boost::gil::get_color( *it, boost::gil::blue_t()), gamma);
        }
    }

    template<class PixelIter>
    void clamp( PixelIter start, PixelIter end) const
    {
        for( PixelIter it( start); it != end; ++it)
        {
            boost::gil::get_color( *it, boost::gil::red_t())   = adobe::clamp( (float) boost::gil::get_color( *it, boost::gil::red_t()), 0.0f, 1.0f);
            boost::gil::get_color( *it, boost::gil::green_t()) = adobe::clamp( (float) boost::gil::get_color( *it, boost::gil::green_t()), 0.0f, 1.0f);
            boost::gil::get_color( *it, boost::gil::blue_t())  = adobe::clamp( (float) boost::gil::get_color( *it, boost::gil::blue_t()), 0.0f, 1.0f);
            boost::gil::get_color( *it, boost::gil::alpha_t()) = adobe::clamp( (float) boost::gil::get_color( *it, boost::gil::alpha_t()), 0.0f, 1.0f);
        }
    }
};

} // namespace
} // namespace

#endif
