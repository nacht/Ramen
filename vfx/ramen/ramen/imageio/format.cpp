// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/format.hpp>

namespace ramen
{
namespace imageio
{

format_t::format_t()
{
    set_input( true);
    set_output( true);
}

format_t::~format_t() {}

bool format_t::input() const { return flags_ & input_bit;}

void format_t::set_input( bool b)
{
    if( b)
	flags_ |= input_bit;
    else
	flags_ &= ~input_bit;
}

bool format_t::output() const { return flags_ & output_bit;}

void format_t::set_output( bool b)
{
    if( b)
	flags_ |= output_bit;
    else
	flags_ &= ~output_bit;
}

bool format_t::check_extension( const std::string& str) const { return false;}

std::size_t format_t::detect_size() const     { return 4;}
bool format_t::detect( const char *p) const   { return false;}

bool format_t::is_multichannel() const { return false;}

std::auto_ptr<reader_t> format_t::reader( const boost::filesystem::path& p) const   { return std::auto_ptr<reader_t>();}
std::auto_ptr<writer_t> format_t::writer() const				    { return std::auto_ptr<writer_t>();}

} // namespace
} // namespace
