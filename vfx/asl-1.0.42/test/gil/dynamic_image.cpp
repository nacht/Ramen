/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
// image_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <boost/type_traits.hpp>
#include "any_image_impl.hpp"
#include "print_types.hpp"

#ifdef NO_ASL
#include <gil/core/image_view.hpp>
#include <gil/core/planar_ref.hpp>
#include <gil/extension/dynamic_image/image_view_factory.hpp>
#include <gil/extension/dynamic_image/algorithm.hpp>
#include <gil/extension/dynamic_image/any_image.hpp>
#include <gil/extension/io/jpeg_dynamic_io.hpp>
#include <gil/extension/io/tiff_dynamic_io.hpp>
#else
#include <adobe/gil/core/image_view.hpp>
#include <adobe/gil/core/planar_ref.hpp>
#include <adobe/gil/extension/dynamic_image/image_view_factory.hpp>
#include <adobe/gil/extension/dynamic_image/algorithm.hpp>
#include <adobe/gil/extension/dynamic_image/any_image.hpp>
template <typename V> inline void jpeg_read_image(const std::string& fileName,GIL::image<V>& img) {}
template <typename C> inline void jpeg_read_image(const std::string& fileName,GIL::any_image<C>& img) {}
template <typename ImgView> inline void jpeg_write_view(const std::string& fileName, const ImgView& img_view, int quality=100) {}
template <typename V> inline void tiff_read_image(const std::string& fileName,GIL::image<V>& img) {}
template <typename C> inline void tiff_read_image(const std::string& fileName,GIL::any_image<C>& img) {}
template <typename ImgView> inline void tiff_write_view(const std::string& fileName, const ImgView& img_view, int quality=100) {}
#endif

using namespace GIL;
using namespace std;

#include <boost/mpl/vector_c.hpp>

void test_dynamic_views_jpeg(const std::string& fileName) {
    const std::string in_dir="C:\\test\\imgtest\\";
    const std::string out_dir=in_dir+"newimg_output\\generic\\";

    any_image_t anyImg;
    jpeg_read_image(in_dir+fileName,anyImg);
    std::cerr <<"File: "<<in_dir+fileName<<" height:"<<get_height(anyImg)<<" numChannels:"<<get_num_channels(anyImg)<<std::endl;

    gray8_image_t gray8(get_dimensions(view(anyImg)));
    copy_and_convert_pixels(view(anyImg),view(gray8));
    jpeg_write_view(out_dir+"gray8_converted.jpg",view(gray8));

    jpeg_write_view(out_dir+"fliplr.jpg",flipped_left_right_view(view(anyImg)));
    jpeg_write_view(out_dir+"flipud.jpg",flipped_up_down_view(view(anyImg)));
    
    any_image_t::view_t subimageView=subimage_view(view(anyImg),point2<ptrdiff_t>(0,0),point2<ptrdiff_t>(100,100));
    jpeg_write_view(out_dir+"subimage.jpg",subimageView);

    jpeg_write_view(out_dir+"subsampled180rot.jpg",subsampled_view(rotated180_view(view(anyImg)), point2<ptrdiff_t>(2,1)));
    jpeg_write_view(out_dir+"subsampled_color_converted.jpg",subsampled_view(any_color_converted_view<gray8_pixel_t>(view(anyImg)), point2<ptrdiff_t>(2,1)));
}

void test_dynamic_views_tiff(const std::string& fileName) {
    const std::string in_dir="C:\\test\\imgtest\\";
    const std::string out_dir=in_dir+"newimg_output\\generic\\";

    any_image_t anyImg;
    tiff_read_image(in_dir+fileName,anyImg);
    std::cerr <<"File: "<<in_dir+fileName<<" height:"<<get_height(anyImg)<<" numChannels:"<<get_num_channels(anyImg)<<std::endl;

    gray8_image_t gray8(get_dimensions(view(anyImg)));
    copy_and_convert_pixels(view(anyImg),view(gray8));
    tiff_write_view(out_dir+"gray8_converted.tif",view(gray8));

    tiff_write_view(out_dir+"fliplr.tif",flipped_left_right_view(view(anyImg)));
    tiff_write_view(out_dir+"flipud.tif",flipped_up_down_view(view(anyImg)));
    
    any_image_t::view_t subimageView=subimage_view(view(anyImg),point2<ptrdiff_t>(0,0),point2<ptrdiff_t>(100,100));
    tiff_write_view(out_dir+"subimage.tif",subimageView);

    tiff_write_view(out_dir+"subsampled180rot.tif",subsampled_view(rotated180_view(view(anyImg)), point2<ptrdiff_t>(2,1)));
    tiff_write_view(out_dir+"subsampled_color_converted.tif",subsampled_view(any_color_converted_view<gray8_pixel_t>(view(anyImg)), point2<ptrdiff_t>(2,1)));
}

void test_gimage() {  
    test_dynamic_views_jpeg("gray.jpg");
    test_dynamic_views_tiff("RGB.tif");
}

/*
void test_any_image() {
    // some compilers cannot handle the complexity of this in debug mode...
    any_image_t anyImg;
    cout << "__________ Any image_______"<<endl;
    print_vec(anyImg);

    typedef any_image_t::view_t any_view_t;
    any_view_t anyView=view(anyImg);
    cout <<endl<< "__________ Any view________"<<endl;
    print_vec(anyView);

    typedef nth_channel_view_type<any_view_t>::type NTH_t;
    NTH_t anyViewN=nth_channel_view(anyView,2);
    cout <<endl<< "__________ Any n-th channel view________"<<endl;
    print_vec(anyViewN);
 
    typedef color_convert_view_type<any_view_t, rgb8_pixel_t>::type CCV_t;
    CCV_t ccv=any_color_converted_view<rgb8_pixel_t>(anyView);
    cout <<endl<< "__________ Any CCV view________"<<endl;
    print_vec(ccv);

    CCV_t::dynamic_x_step_t anyFlipView=flipped_left_right_view(ccv);
    cout <<endl<< "__________ Any CCV flipped view________"<<endl;
    print_vec(anyFlipView);
}
*/


