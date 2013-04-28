/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
#include <adobe/config.hpp>
/*************************************************************************************************/

#ifndef GIL_TEST_CONFIG
#define GIL_TEST_CONFIG

//#define GIL_IO        // use this if GIL's i/o extension is available
//#define GIL_RESAMPLE  // use this if GIL's resample extension is available


#ifdef GIL_IO
    #include <adobe/gil/extension/io/image_io.hpp>
#else
    // stub to use when i/o extension is unavailable
    #include <adobe/gil/core/image.hpp>
    #include <adobe/gil/core/variant.hpp>
    #include <adobe/gil/core/typedefs.hpp>
    #include <string>

    inline void io_error_if(bool, const char*){}
    template <typename View> void save_view(const View& v,const std::string& fileName) {}
    template <typename Img>  void load_image(Img& img, const std::string& fileName) {
        Img tmp(100,100);
        GIL::swap(tmp,img);
    }
    template <typename ICS>  void load_image(GIL::variant<ICS>& img, const std::string& fileName) {
        GIL::gray8_image rgb8img(100,100);
        img.move_in(rgb8img);
    }
#endif



#endif
