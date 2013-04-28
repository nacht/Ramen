/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
// image_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#ifdef NO_ASL
#include <gil/core/gil_all.hpp>
#include <gil/extension/io/jpeg_io.hpp>
#else
#include <adobe/gil/core/gil_all.hpp>
template <typename V> inline void jpeg_read_image(const std::string& fileName,GIL::image<V>& img) {}
template <typename V> inline void jpeg_read_and_convert_image(const std::string& fileName,GIL::image<V>& img) {}
template <typename ImgView> inline void jpeg_write_view(const std::string& fileName, const ImgView& img_view, int quality=100) {}
static void io_error_if(bool expr, const char* descr="") { }
#endif

using namespace GIL;
using namespace std;
using namespace boost;

extern const std::string in_dir;
extern const std::string out_dir;
extern const std::string ref_dir;

#ifdef LBOURDEV
const std::string in_dir="C:/test/imgtest/";  // directory of source images
#else
const std::string in_dir="";  // directory of source images
#endif

const std::string out_dir=in_dir+"scratch/";    // directory where to write output
const std::string ref_dir=in_dir+"reference/";  // reference directory to compare written with actual output



// Enable checking. 
#define PERFORM_CHECK

template <typename ImgView>
void check_image(const ImgView& img_view, const string& imgName) {
    jpeg_write_view(out_dir+imgName,img_view);
    rgb8_image_t img1, img2;
    jpeg_read_and_convert_image(out_dir+imgName,img1);
        std::cerr << "writing "<<imgName<<"\n";
#ifdef PERFORM_CHECK
    jpeg_read_and_convert_image(ref_dir+imgName,img2);
    if (img1!=img2) {
        std::cerr << "Error with "<<imgName<<"\n";
        io_error_if(1,"image mismatch");
    }
#endif
}

// testing image iterators, clone, fill, locators, color convert
template <typename Img>
void test_image_basic(const std::string& prefix) {
    typedef typename Img::view_t View;

    // make a 20x20 image
    Img img(typename View::point_t(20,20));
    const View& img_view=view(img);

    // fill it with red
    rgb8_pixel_t red8(255,0,0), green8(0,255,0), blue8(0,0,255), white8(255,255,255);
    typename View::value_type red,green,blue,white;
    color_convert(red8,red);
    default_color_converter()(red8,red);
    red=color_convert_deref_fn<rgb8_ref_t,typename Img::view_t::value_type>()(red8);

    color_convert(green8,green);
    color_convert(blue8,blue);
    color_convert(white8,white);
    fill(img_view.begin(),img_view.end(),red);

    color_convert(red8,img_view[0]);

    // pointer to first pixel of second row
    typename View::reference rt=img_view.at(0,0)[img_view.width()];
    typename View::x_iterator ptr=&rt; ignore_unused_variable_warning(ptr);
    typename View::reference rt2=*(img_view.at(0,0)+img_view.width());
    typename View::x_iterator ptr2=&rt2; ignore_unused_variable_warning(ptr2);
    assert(ptr==ptr2);
    assert(img_view.x_at(0,0)+10==10+img_view.x_at(0,0));

    // draw a blue line along the diagonal
    typename View::xy_locator loc=img_view.xy_at(0,img_view.height()-1);
    for (int y=0; y<img_view.height(); ++y) {
        *loc=blue;
        ++loc.x();
        loc.y()--;
    }

    // draw a green dotted line along the main diagonal with step of 3
    loc=img_view.xy_at(img_view.width()-1,img_view.height()-1);
    while (loc.x()>=img_view.x_at(0,0)) {
        *loc=green;
        loc-=typename View::point_t(3,3);
    }

    // Clone and make every red pixel white
    Img imgWhite(img);
    for (typename View::iterator it=view(imgWhite).end(); (it-1)!=view(imgWhite).begin(); --it) {
        if (*(it-1)==red)
            *(it-1)=white;
    }
    // make sure the const iterators can be constructed from non-const ones.
    typename Img::const_iterator tst=imgWhite.begin();

    check_image(img_view,prefix+"red_x.jpg");
    check_image(view(imgWhite),prefix+"white_x.jpg");
}

template <typename GrayView, typename R>
void gray_image_hist(const GrayView& img_view, R& hist) {
//    for_each_pixel(img_view,++lambda::var(hist)[lambda::_1]);
    for (typename GrayView::iterator it=img_view.begin(); it!=img_view.end(); ++it)
        ++hist[*it];
}

template <typename V, typename R>
void get_hist(const V& img_view, R& hist) {
    gray_image_hist(color_converted_view<gray8_pixel_t>(img_view), hist);
}

template <typename View>
void test_histogram(const View& img_view) {
//  std::vector<int> histogram(255,0);
//  get_hist(cropped,histogram.begin());
    int histogram[256];
    fill(histogram,histogram+256,0);
    get_hist(img_view,histogram);

    std::cout<<endl;
    std::for_each(histogram, histogram+255, std::cout << lambda::_1 << ' ');
}

// testing custom color conversion
template <typename T1, typename C1, typename T2, typename C2>
struct my_color_converter_impl : public GIL::color_converter_default_impl<T1,C1,T2,C2> {
};
template <typename T1, typename C1, typename T2>
struct my_color_converter_impl<T1,C1,T2,gray_t> {
    template <typename P1, typename P2>
    void operator()(const P1& src, P2& dst) const {
        GIL::color_converter_default_impl<T1,C1,T2,gray_t>()(src,dst);
        dst.gray=channel_invert(dst.gray);
    }
};

struct my_color_converter {
    template <typename SrcP,typename DstP>
    void operator()(const SrcP& src,DstP& dst) const { 
        typedef typename pixel_traits<SrcP>::channel_t src_channel_t;
        typedef typename pixel_traits<DstP>::channel_t dst_channel_t;
        typedef typename pixel_traits<SrcP>::color_space_t::base src_cs_t;
        typedef typename pixel_traits<DstP>::color_space_t::base dst_cs_t;
        my_color_converter_impl<src_channel_t,src_cs_t,dst_channel_t,dst_cs_t>()(src,dst);
    }
};

template <typename View>
void test_image_views(const View& img_view, const string& prefix) {
    check_image(img_view,prefix+"original.jpg");

    check_image(subimage_view(img_view, iround(img_view.dimensions()/4), iround(img_view.dimensions()/2)),prefix+"cropped.jpg");
    check_image(color_converted_view<gray8_pixel_t>(img_view),prefix+"gray8.jpg");
    check_image(color_converted_view<gray8_pixel_t>(img_view,my_color_converter()),prefix+"my_gray8.jpg");
    check_image(transposed_view(img_view),prefix+"transpose.jpg");
    check_image(rotated180_view(img_view),prefix+"rot180.jpg");
    check_image(rotated90cw_view(img_view),prefix+"90cw.jpg");
    check_image(rotated90ccw_view(img_view),prefix+"90ccw.jpg");
    check_image(flipped_up_down_view(img_view),prefix+"flipped_ud.jpg");
    check_image(flipped_left_right_view(img_view),prefix+"flipped_lr.jpg");
    check_image(subsampled_view(img_view,typename View::point_t(2,1)),prefix+"subsampled.jpg");
    check_image(nth_channel_view(img_view,0),prefix+"0th_channel.jpg");

    // some iterator math
    ptrdiff_t dst=(img_view.end()-(img_view.begin()+4500))+4500; ignore_unused_variable_warning(dst);
    ptrdiff_t dst2=img_view.end()-img_view.begin(); ignore_unused_variable_warning(dst2);
    ptrdiff_t sz=img_view.size(); ignore_unused_variable_warning(sz);
    io_error_if(sz!=dst2);
}

template <typename Img>
void test_image_all(const string& prefix) {
    test_image_basic<Img>(prefix+"basic_");

    Img img;
    jpeg_read_and_convert_image(in_dir+"monkey.jpg",img);
    test_image_views(view(img), prefix+"views_");

    // histogram
    test_histogram(const_view(img));
}

// Models a Unary Function
template <typename P>   // Models PixelValueConcept
struct mandelbrot_fn {
    typedef point2<ptrdiff_t>    point_t;

    typedef mandelbrot_fn        const_t;
    typedef P                    value_type;
    typedef value_type           reference;
    typedef value_type           const_reference;
    typedef point_t              argument_type;
    typedef reference            result_type;
    BOOST_STATIC_CONSTANT(bool, is_mutable=false);

    value_type                    _in_color,_out_color;
    point_t                       _img_size;
    static const int MAX_ITER=100;        // max number of iterations

    mandelbrot_fn() {}
    mandelbrot_fn(const point_t& sz, const value_type& in_color, const value_type& out_color) : _in_color(in_color), _out_color(out_color), _img_size(sz) {}

    result_type operator()(const point_t& p) const {
        // normalize the coords to (-2..1, -1.5..1.5)
        // (actually make y -1.0..2 so it is asymmetric, so we can verify some view factory methods)
        double t=get_num_iter(point2<double>(p.x/(double)_img_size.x*3-2, p.y/(double)_img_size.y*3-1.0f));//1.5f));
        t=pow(t,0.2);

        value_type ret;
        for (int k=0; k<P::num_channels; ++k)
            ret[k]=(typename pixel_traits<value_type>::channel_t)(_in_color[k]*t + _out_color[k]*(1-t));
        return ret;
    }

private:
    double get_num_iter(const point2<double>& p) const {
        point2<double> Z(0,0);
        for (int i=0; i<MAX_ITER; ++i) {
            Z = point2<double>(Z.x*Z.x - Z.y*Z.y + p.x, 2*Z.x*Z.y + p.y);
            if (Z.x*Z.x + Z.y*Z.y > 4)
                return i/(double)MAX_ITER;
        }
        return 0;
    }
};

template <typename T>
void x_gradient(const T& src, const gray8s_view_t& dst) {
    for (int y=0; y<src.height(); ++y) {
        typename T::x_iterator src_it = src.row_begin(y);
        gray8s_view_t::x_iterator dst_it = dst.row_begin(y);

        for (int x=1; x<src.width()-1; ++x)
            dst_it[x] = (src_it[x+1] - src_it[x-1]) / 2;
    }
}

void test_synthetic_image_view() {
    typedef mandelbrot_fn<rgb8_pixel_t> deref_t;
    typedef deref_t::point_t            point_t;
    typedef virtual_2d_locator<deref_t,false> locator_t;
    typedef image_view<locator_t> my_virt_view_t;

    boost::function_requires<PixelLocatorConcept<locator_t> >();
    gil_function_requires<StepIteratorConcept<locator_t::x_iterator> >();

    point_t dims(200,200);
    my_virt_view_t mandel(dims, locator_t(point_t(0,0), point_t(1,1), deref_t(dims, rgb8_pixel_t(255,0,255), rgb8_pixel_t(0,255,0))));

    gray8s_image_t img(dims);
    fill_pixels(view(img),0);   // our x_gradient algorithm doesn't change the first & last column, so make sure they are 0
    x_gradient(color_converted_view<gray8_pixel_t>(mandel), view(img));
    check_image(color_converted_view<gray8_pixel_t>(const_view(img)), "mandelLuminosityGradient.jpg");

    test_image_views(mandel,"virtual_");

    test_histogram(mandel);
}

void test_image() {
#ifdef NO_ASL
#ifdef MSVC
    CreateDirectory(in_dir.c_str(), NULL);
    CreateDirectory(out_dir.c_str(), NULL);
    CreateDirectory(ref_dir.c_str(), NULL);
#endif
#endif

    test_image_all<bgr8_image_t>("bgr8_");
    test_image_all<rgb8_image_t>("rgb8_");
    test_image_all<rgb8_planar_image_t>("planarrgb8_");
    test_image_all<gray8_image_t>("gray8_");


    rgb8_planar_image_t img;
    jpeg_read_image(in_dir+"monkey.jpg",img);
    test_image_views(subsampled_view(view(img),point2<ptrdiff_t>(1,2)),"subsampled_");
    test_image_views(color_converted_view<gray8_pixel_t>(view(img)),"color_converted_");

    // Test reverse iterators
    rgb8_image_t img2(view(img).dimensions());
    copy(img.rbegin(),img.rend(), img2.begin());
    check_image(view(img2), "reversed_monkey.jpg");

    // Test synthetic image views
    test_synthetic_image_view();

    gil_function_requires<ImageConcept<rgb8_image_t> >();

    BOOST_STATIC_ASSERT(view_is_basic<rgb8_step_view_t>::value);
    BOOST_STATIC_ASSERT(view_is_basic<cmyk8c_planar_step_view_t>::value);
    BOOST_STATIC_ASSERT(view_is_basic<rgb8_planar_view_t>::value);

    BOOST_STATIC_ASSERT(view_is_step_in_x<rgb8_step_view_t>::value);
    BOOST_STATIC_ASSERT(view_is_step_in_x<cmyk8c_planar_step_view_t>::value);
    BOOST_STATIC_ASSERT(!view_is_step_in_x<rgb8_planar_view_t>::value);

    BOOST_STATIC_ASSERT(!view_is_planar<rgb8_step_view_t>::value);
    BOOST_STATIC_ASSERT(view_is_planar<cmyk8c_planar_step_view_t>::value);
    BOOST_STATIC_ASSERT(view_is_planar<rgb8_planar_view_t>::value);

    BOOST_STATIC_ASSERT(view_is_mutable<rgb8_step_view_t>::value);
    BOOST_STATIC_ASSERT(!view_is_mutable<cmyk8c_planar_step_view_t>::value);
    BOOST_STATIC_ASSERT(view_is_mutable<rgb8_planar_view_t>::value);

    BOOST_STATIC_ASSERT((boost::is_same<derived_view_type<cmyk8c_planar_step_view_t>::type, cmyk8c_planar_step_view_t>::value));
    BOOST_STATIC_ASSERT((boost::is_same<derived_view_type<cmyk8c_planar_step_view_t, bits16, rgb_t>::type,  rgb16c_planar_step_view_t>::value));
    BOOST_STATIC_ASSERT((boost::is_same<derived_view_type<cmyk8c_planar_step_view_t, GIL::use_default, rgb_t, mpl::false_>::type,  rgb8c_step_view_t>::value));
}
