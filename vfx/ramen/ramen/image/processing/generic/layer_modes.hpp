// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_LAYER_MODES_HPP
#define RAMEN_IMAGE_GENERIC_LAYER_MODES_HPP

#include<algorithm>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

struct over_layer_mode_fun
{
    over_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct add_layer_mode_fun
{
    add_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct mul_layer_mode_fun
{
    mul_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct sub_layer_mode_fun
{
    sub_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct mix_layer_mode_fun
{
    mix_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct screen_layer_mode_fun
{
    screen_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct overlay_layer_mode_base_fun
{
protected:

    inline float overlay( float front, float back) const
    {
	const float mid_value = 0.5f;

        if( back > 1.0f)
            return front;

	if( back <= mid_value)
	    return 2.0f * front * back;
	else
        {
            float result = 1.0f - ( 2.0f * (1.0f - front) * (1.0f - back));
            result = std::max( result, back);
            return result;
        }
    }
};

struct overlay_layer_mode_fun : public overlay_layer_mode_base_fun
{
    overlay_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct zero_overlay_layer_mode_fun : public overlay_layer_mode_base_fun
{
    zero_overlay_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& p) const;

private:

    float opacity_;
};

struct diff_layer_mode_fun
{
    diff_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    inline float abs( float x) const
    {
	if( x < 0)
	    return -x;

	return x;
    }

    float opacity_;
};

struct max_layer_mode_fun
{
    max_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

struct min_layer_mode_fun
{
    min_layer_mode_fun( float opacity = 1.0f);
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;

private:

    float opacity_;
};

} // namespace
} // namespace
} // namespace

#endif
