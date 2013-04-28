/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
// image_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#include <iterator>
#include <boost/type_traits.hpp>
#ifdef NO_ASL
#include <gil/core/planar_ref.hpp>
#include <gil/core/rgb.hpp>
#include <gil/core/gray.hpp>
#include <gil/core/rgba.hpp>
#include <gil/core/cmyk.hpp>
#include <gil/core/pixel.hpp>
#include <gil/core/pixel_iterator.hpp>
#include <gil/core/color_convert.hpp>
#include <gil/core/typedefs.hpp>
#include <gil/core/gil_concept.hpp>
#else
#include <adobe/gil/core/planar_ref.hpp>
#include <adobe/gil/core/rgb.hpp>
#include <adobe/gil/core/gray.hpp>
#include <adobe/gil/core/rgba.hpp>
#include <adobe/gil/core/cmyk.hpp>
#include <adobe/gil/core/pixel.hpp>
#include <adobe/gil/core/pixel_iterator.hpp>
#include <adobe/gil/core/color_convert.hpp>
#include <adobe/gil/core/typedefs.hpp>
#include <adobe/gil/core/gil_concept.hpp>
#endif

using namespace GIL;
using namespace std;

template <typename T>
struct clear_fn {
    void operator()(T& v) { v=0; }
};
template <typename T> inline void ignore_unused_variable_warning(const T&){}
void test_pixel() {
    gil_function_requires<MutableChannelConcept<bits8> >();
    gil_function_requires<MutableChannelConcept<bits8&> >();
    gil_function_requires<ChannelConcept<const bits8&> >();

    gil_function_requires<MutableChannelConcept<bits16> >();
    gil_function_requires<MutableChannelConcept<bits32f> >();


    gil_function_requires<MutablePixelConcept<bgr32f_pixel_t> >();
    gil_function_requires<MutablePixelConcept<rgb16_planar_ref_t> >();
    gil_function_requires<PixelConcept<rgb8c_pixel_t> >();

    gil_function_requires<PixelConvertibleConcept<rgb8_pixel_t,cmyk16_pixel_t> >();
    gil_function_requires<PixelConvertibleConcept<rgb8_pixel_t,cmyk16_planar_ref_t> >();
    gil_function_requires<PixelConvertibleConcept<gray8c_pixel_t,cmyk16_planar_ref_t> >();

    BOOST_STATIC_ASSERT(rgb_t::num_channels==3);
    BOOST_STATIC_ASSERT(bgr_t::num_channels==3);

    // dimensions to explore
    //
    // values, references, pointers
    // color spaces (rgb,cmyk,gray)
    // channel ordering (bgr vs rgb) 
    // planar vs interleaved    
// Pixel VALUES
// Default constructors and construct with values
    rgb8_pixel_t rgb8(10,100,200);
    bgr8_pixel_t bgr8(10,100,200);
    rgb32f_pixel_t rgb32(0.1f, 0.5f, 0.7f);

    transform_channels(rgb8,rgb8,std::bind2nd(std::plus<bits8>(),3));

    cmyk8_pixel_t cmyk8;
    rgba8_pixel_t rgba8;
    gray8_pixel_t gray8(11);
    rgb16_pixel_t rgb16(0,0,0);
    cmyk16_pixel_t cmyk16;
    rgba16_pixel_t rgba16;

    bits8 mv=max_channel(rgb8);
    mv=min_channel(rgb8);

// Copy construct from compatible pixel
    rgb8_pixel_t rgb8_1(rgb8);

    cmyk16_pixel_t cmyk16_1(cmyk16);

    bgr8_pixel_t bgr8_1(rgb8);        // with different channel ordering
    assert(bgr8_1==rgb8);

    //  RGB8 fail1(cmyk8);      // error: different color space
    //  RGB8 fail2(rgb16);      // error: different channel depth

// Assignment to compatible pixel
    rgb8_1=rgb8;

    bgr8_1=rgb8_pixel_t(1,2,3);
    rgb8_1=bgr8_1;
    assert(rgb8_1.red==1);
    
    //  rgb8=rgba8;             // error different color space
    //  rgb8=rgb16;             // error different channel depth

//  rgb8_pixel_t result=bgr8_1+rgb8;


// Pixel REFERENCES

    //  rgb8ref rgb8ref_err;    // error: references have no default constructors


// Assign/copy-construct reference from value
    rgb8_ref_t rgb8ref(rgb8);
    rgb8_planar_ref_t rgb8planarref(rgb8);
    rgb8planarref=rgb8_pixel_t(11,22,32);
    unsigned char rr=rgb8planarref.channel<2>(); ignore_unused_variable_warning(rr);
    assert(rgb8planarref.channel<2>()==32);
    assert(rgb8.channel<2>()==32);

    rgb32f_pixel_t r32=rgb32f_pixel_t(1,2,3);
    rgb32f_planar_ref_t rgb32pr(r32);
    bits32f xty=rgb32pr.channel<2>(); ignore_unused_variable_warning(xty);
    assert(xty==3);
    assert(r32.channel<2>()==3);


    const rgb8_pixel_t& CR(rgb8);
    rgb8ref=CR;

    rgb8c_pixel_t rgb8c(1,2,3);
    //BOOST_STATIC_ASSERT((boost::is_same<bits8,REMOVE_CONST_AND_REFERENCE(const bits8&)>::value));
    rgb8c_planar_ref_t rgb8c_ref_t=rgb8c;

//    rgb8==cmyk8;
//  rgb8=rgb8c_pixel_t(1,2,3);

    assert(rgb8ref.blue==rgb8.blue);
    assert(rgb8==rgb8ref);
    assert(rgb8planarref==rgb8ref);

    boost::add_const<std::iterator_traits<rgb8_ptr_t>::reference>::type rgb8_const_ref(rgb8);
//      rgb8_const_ref=rgb8_pixel_t(3,5,7);       // error: setting value of const reference
    rgb8planarref.blue=33;
    assert(rgb8_const_ref.blue==33);
    bgr8_ref_t bgr8_ref_t(bgr8);

    gray8_ref_t gray8_ref(gray8);

// copy-construct reference from reference
    rgb8_ref_t rgb8ref_1(rgb8ref);                
    rgb8_planar_ref_t rgb8planarref_1(rgb8ref);       // planar from non-planar reference is ok
//  rgb8_ref_t rgb8ref_1_err(rgb8planarref);      // error: non-planar from planar reference

    cmyk8_planar_ref_t cpr(cmyk8);

    int a=5, b=6;

    const int& cr=a;
    const rgb8_planar_ref_t cr_(rgb8);

    int& nr=b;
    rgb8_planar_ref_t nr_(rgb8);

    const int& cr2(cr); ignore_unused_variable_warning(cr2);
    const rgb8_planar_ref_t cr2_(cr_);

    const int& cr3(nr); ignore_unused_variable_warning(cr3);
    const rgb8_planar_ref_t cr3_(nr_);

    // SHOULD NOT COMPILE BUT IT DOES!! Bizzare because reference class' copy constructor takes argument by non-const reference, so it should not match "const rgb8_planar_ref_t"
//  int& nr2(cr);
    rgb8_planar_ref_t _nr2(cr_);      // ERROR: reference from const reference

    int& nr3(nr); ignore_unused_variable_warning(nr3);
    rgb8_planar_ref_t _nr3(nr_);

//  cr=nr;
//  cr_=nr_;                        // ERROR: assigning to const reference

    nr=cr;
    nr_=cr_;

//  cr=cr;
//  cr_=cr_;

    nr=nr;
    nr_=nr_;

    {
     rgb8_pixel_t rgb8p;
    planar_ref<bits8& ,rgb_t> r8r(rgb8p);
planar_ref<bits8& ,rgb_t> r8r2=rgb8p;
r8r2=r8r;
    }

// Assign value or reference to reference
    rgb8ref=rgb8_pixel_t(3,5,7);
    rgb8ref=rgb8ref_1;
//      rgb8_const_ref=rgb8ref;     // error: const from non-const
    rgb8ref=rgb8_const_ref;         // non-const from const

// Pixel POINTERS
//  typedef const iterator_traits<rgb8_ptr_t>::pointer  RGB8ConstPtr;
    typedef const rgb8_ptr_t  RGB8ConstPtr;
    typedef const rgb8_planar_ptr_t  RGB8ConstPlanarPtr;
//  typedef const iterator_traits<rgb8_planar_ptr_t>::pointer RGB8ConstPlanarPtr;

// constructing from values, references and other pointers
    RGB8ConstPtr rgb8_const_ptr=NULL; ignore_unused_variable_warning(rgb8_const_ptr);
    rgb8_ptr_t rgb8ptr=&rgb8;


    rgb8=bgr8_pixel_t(30,20,10);
    rgb8_planar_ptr_t rgb8_pptr=&rgb8;
    ++rgb8_pptr;
    rgb8_pptr--;
    rgb8_pptr[0]=rgb8;
    RGB8ConstPlanarPtr rgb8_const_planar_ptr=&rgb8;

    rgb8c_planar_ptr_t r8c=&rgb8;
    r8c=&rgb8;

    rgb8_pptr=&rgb8;


    //  rgb8_const_planar_ptr=&rgb16p;                  // error: incompatible bit depth

    //  iterator_traits<CMYK8>::pointer cmyk8_ptr_t=&rgb8;    // error: incompatible pointer type

    RGB8ConstPtr rgb8_const_ptr_err=rgb8ptr;        // const pointer from non-regular pointer
ignore_unused_variable_warning(rgb8_const_ptr_err);
// dereferencing pointers to obtain references
    rgb8_ref_t rgb8ref_2=*rgb8ptr; ignore_unused_variable_warning(rgb8ref_2);
    assert(rgb8ref_2==rgb8);
    //  RGB8Ref rgb8ref_2_err=*rgb8_const_planar_ptr;   // error: non-const reference from const pointer
    
    rgb8_planar_ref_t rgb8planarref_3=*rgb8_pptr; // planar reference from planar pointer
    assert(rgb8planarref_3==rgb8);
    //  RGB8Ref rgb8ref_3=*rgb8_planar_ptr_t; // error: non-planar reference from planar pointer


    const rgb8_pixel_t crgb8=rgb8;
    *rgb8_pptr=rgb8;
    *rgb8_pptr=crgb8;

    byte_advance(rgb8_pptr,3);
    byte_advance(rgb8_pptr,-3);

// CHANNEL OPERATIONS
    
    // on values
    bgr8.blue=3;
    rgb8.green=bgr8.blue;
    rgb8.semantic_channel<0>();
    bgr8.semantic_channel<0>();
    assert(rgb8.green==3);

    // on references
    rgb8ref_1.green=bgr8_ref_t.red;
    bgr8_ref_t.blue=3;
    //  rgb8_const_ref.blue=3;     // error: modifying channel of const reference

// OPERATOR==
    //  rgb8==cmyk8;        // error: incompatible color spaces
    rgb8==bgr8;         // classes with different channel ordering are OK to compare

    rgb8_planar_ref_t rf=*rgb8_pptr;
// COLOR CONVERSION

    color_convert(rgb16, gray8);


//  typedef iterator_traits<iterator_traits<rgb8_planar_ptr_t>::reference*>::color_space_t cst;
//  BOOST_STATIC_ASSERT((boost::is_same<rgb_t,cst>::value));
    color_convert(rgb16, rf);
    color_convert(rgb16, rf=*rgb8_pptr);
    color_convert(*rgb8_const_planar_ptr, gray8_ref);
    color_convert(rgb8ref, gray8_ref);
    //  color_convert(rgb8ref, *rgb8_const_ptr);        // error: Destination is const

// CHANNEL-LEVEL OPERATIONS
    //bgr8=rf*2;

    unsigned char* xxx=0;
    xxx+=2;
    rgb8_pptr.semantic_channel<0>()=xxx;
    fill_channels(rgb8_pptr,xxx);
}

