/**
 * @file
 * @brief imageio factory class
 */

// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_FACTORY_HPP
#define	RAMEN_IMAGEIO_FACTORY_HPP

#include<vector>
#include<string>

#include<boost/noncopyable.hpp>
#include<boost/filesystem/path.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

#include<loki/Singleton.h>

#include<ramen/imageio/exceptions.hpp>
#include<ramen/imageio/format.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace imageio
{

/**
 * @class factory_impl
 * @brief imageio factory class
 *
 * A composition_t contains a graph of nodes, a time range and
 * a stack of rendering options.
 *
 */

class factory_impl : boost::noncopyable
{
public:

    /**
     * Returns a list of formats registered in the factory
     */
    const boost::ptr_vector<format_t>& formats() const { return formats_;}

    typedef boost::ptr_vector<format_t>::const_iterator const_iterator;

    const_iterator begin() const    { return formats_.begin();}
    const_iterator end() const	    { return formats_.end();}

    /**
     * Register a new imageio format in the factory
     * @param format the format to add
     */
    bool register_image_format( std::auto_ptr<format_t> format);

    /**
     * Returns a list of file extensions registered
     */
    const std::vector<std::string>& extensions() const  { return extensions_;}

    /**
     * Returns an imageio reader that can read the file specified.
     * @param p the image file to read
     */
    std::auto_ptr<reader_t> reader_for_image( const boost::filesystem::path& p) const;

    /**
     * Returns an imageio writer that can write the specified format.
     * @param tag the image format to write
     */
    std::auto_ptr<writer_t> writer_for_tag( const std::string& tag) const;

private:

    factory_impl();
    ~factory_impl();

    const_iterator format_for_tag( const std::string& tag) const;
    const_iterator format_for_extension( const boost::filesystem::path& p) const;
    const_iterator format_for_file_contents( const boost::filesystem::path& p) const;

    friend struct Loki::CreateUsingNew<factory_impl>;

    boost::ptr_vector<format_t> formats_;
    std::vector<std::string> extensions_;

    std::size_t detect_size_;
    mutable char *detect_buffer_;
};

typedef Loki::SingletonHolder<factory_impl> factory_t;

} // namespace
} // namespace

#endif
