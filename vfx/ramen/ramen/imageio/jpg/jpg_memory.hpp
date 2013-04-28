// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_JPEG_MEMORY_HPP
#define RAMEN_IMAGEIO_JPEG_MEMORY_HPP

#include<stdio.h>

#include<adobe/memory.hpp>

extern "C"
{
#include <jpeglib.h>
}

namespace adobe
{

template<>
struct delete_ptr<FILE*>
{
    void operator()( FILE *x) const
    {
        if( x)
            fclose( x);
    }
};

template<>
struct delete_ptr<jpeg_decompress_struct*>
{
    void operator()( jpeg_decompress_struct *cinfo) const
    {
        jpeg_finish_decompress( cinfo);
        jpeg_destroy_decompress( cinfo);
    }
};

template<>
struct delete_ptr<jpeg_compress_struct*>
{
    void operator()( jpeg_compress_struct *cinfo) const
    {
        jpeg_finish_compress( cinfo);
        jpeg_destroy_compress( cinfo);
    }
};

} // namespace

#endif
