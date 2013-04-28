// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_FORMAT_HPP
#define	RAMEN_IMAGEIO_FORMAT_HPP

#include<string>
#include<vector>
#include<memory>

#include<boost/cstdint.hpp>

#include<ramen/imageio/reader.hpp>
#include<ramen/imageio/writer.hpp>

namespace ramen
{
namespace imageio
{

struct format_t
{
    enum flag_bits
    {
	input_bit  = 1 << 0,
	output_bit = 1 << 1
    };

    format_t();
    virtual ~format_t();

    bool input() const;
    void set_input( bool b);

    bool output() const;
    void set_output( bool b);

    virtual std::string tag() const = 0;

    virtual bool check_extension( const std::string& str) const;

    virtual std::size_t detect_size() const;
    virtual bool detect( const char *p) const;

    virtual void add_extensions( std::vector<std::string>& ext) const = 0;

    virtual bool is_multichannel() const;

    virtual std::auto_ptr<reader_t> reader( const boost::filesystem::path& p) const;
    virtual std::auto_ptr<writer_t> writer() const;

protected:

    boost::uint32_t flags_;
};

} // namespace
} // namespace

#endif	/* FORMAT_HPP */
