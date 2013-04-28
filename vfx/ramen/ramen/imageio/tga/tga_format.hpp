// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_TGA_FORMAT_HPP
#define	RAMEN_TGA_FORMAT_HPP

#include<ramen/imageio/format.hpp>

namespace ramen
{
namespace imageio
{

class tga_format_t : public format_t
{
public:

    tga_format_t();
    ~tga_format_t();

    virtual std::string tag() const;

    virtual bool check_extension( const std::string& str) const;

    virtual std::size_t detect_size() const;
    virtual bool detect( const char *p) const;

    virtual void add_extensions( std::vector<std::string>& ext) const;

    virtual bool is_multichannel() const { return false;}

    virtual std::auto_ptr<reader_t> reader( const boost::filesystem::path& p) const;
    virtual std::auto_ptr<writer_t> writer() const;
};

} // namespace
} // namespace

#endif	/* _EXR_IMAGE_FORMAT_HPP */
