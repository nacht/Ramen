/**
 * @file
 * @brief User interface information about a plug
 */
// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_INPUT_PLUG_INFO_HPP
#define	RAMEN_INPUT_PLUG_INFO_HPP

#include<boost/serialization/access.hpp>
#include<boost/serialization/string.hpp>
#include<boost/serialization/split_member.hpp>

#include<ramen/ImathExt/serialization/ImathColor.hpp>

#include<ramen/ui/palette.hpp>

namespace ramen
{

/**
 * @class input_plug_info_t
 * @brief User interface information about a plug
 *
 */
class input_plug_info_t
{
public:

    input_plug_info_t()
    {
	color_ = ui::palette_t::Instance()["front plug"];
    }

    input_plug_info_t( const Imath::Color3c& color) : color_(color), tooltip_( "Input")
    {
    }

    input_plug_info_t( const Imath::Color3c& color, const std::string& tooltip) : color_(color), tooltip_(tooltip)
    {
    }

    const Imath::Color3c& color() const	{ return color_;}
    void set_color( const Imath::Color3c& c)	{ color_ = c;}

    const std::string& tooltip() const	    { return tooltip_;}
    void set_tooltip( const std::string& s) { tooltip_ = s;}

private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version)
    {
	ar & boost::serialization::make_nvp( "color", color_);
	ar & boost::serialization::make_nvp( "tooltip", tooltip_);
    }

    Imath::Color3c color_;
    std::string tooltip_;
};

} // namespace

#endif
