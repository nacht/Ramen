/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
// image_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#ifdef NO_ASL
#include <gil/core/planar_ref.hpp>
#include <gil/core/rgb.hpp>
#include <gil/core/pixel_iterator.hpp>
#include <gil/core/typedefs.hpp>
#include <gil/core/color_convert.hpp>
#include <gil/core/typedefs.hpp>
#include <gil/core/image_view_factory.hpp>
#else
#include <adobe/gil/core/planar_ref.hpp>
#include <adobe/gil/core/rgb.hpp>
#include <adobe/gil/core/pixel_iterator.hpp>
#include <adobe/gil/core/typedefs.hpp>
#include <adobe/gil/core/color_convert.hpp>
#include <adobe/gil/core/typedefs.hpp>
#include <adobe/gil/core/image_view_factory.hpp>
#endif


using namespace GIL;
using namespace std;
//using namespace boost;

template <typename T> inline void ignore_unused_variable_warning(const T&){}
void test_pixel_iterator() {
    boost::function_requires<Point2DConcept<point2<int> > >();

    boost::function_requires<MutablePixelIteratorConcept<bgr8_ptr_t> >();
    boost::function_requires<MutablePixelIteratorConcept<cmyk8_planar_ptr_t> >();
    boost::function_requires<PixelIteratorConcept<rgb8c_planar_step_ptr_t> >();
    boost::function_requires<MutableStepIteratorConcept<rgb8_step_ptr_t> >();

    boost::function_requires<MutablePixelLocatorConcept<rgb8_step_loc_t> >();
    boost::function_requires<PixelLocatorConcept<rgb8c_planar_step_loc_t> >();

    boost::function_requires<MutableStepIteratorConcept<cmyk8_planar_step_ptr_t> >();
    boost::function_requires<StepIteratorConcept<gray8c_step_ptr_t> >();

    boost::function_requires<MutablePixelLocatorConcept<membased_2d_locator<rgb8_step_ptr_t> > >();

    rgb8_pixel_t rgb8(1,2,3);
    rgba8_pixel_t rgba8;

    rgb8_ptr_t ptr1=&rgb8;
    byte_advance(ptr1, 3);
    const rgb8_ptr_t ptr2=byte_advanced(ptr1,10);

    byte_distance(ptr1,ptr2);
    const rgb8_pixel_t& ref=byte_advanced_ref(ptr1,10); ignore_unused_variable_warning(ref);

    rgb8_planar_ptr_t planarPtr1(&rgb8);
    rgb8_planar_ptr_t planarPtr2(&rgb8);
    byte_advance(planarPtr1,10);
    byte_distance(planarPtr1,planarPtr2);
    rgb8_planar_ptr_t planarPtr3=byte_advanced(planarPtr1,10);

//    planarPtr2=&rgba8;

    planar_ref<bits8&,rgb_t> pxl=*(planarPtr1+5);
  rgb8_pixel_t pv2=pxl;
  rgb8_pixel_t pv3=*(planarPtr1+5);
     rgb8_pixel_t pv=planarPtr1[5];

    assert(*(planarPtr1+5)==planarPtr1[5]);

    rgb8_planar_ref_t planarRef=byte_advanced_ref(planarPtr1,10);

    rgb8_step_ptr_t stepIt(&rgb8,5);
    stepIt++;
    rgb8_step_ptr_t stepIt2=stepIt+10;
    stepIt2=stepIt;
    
    rgb8_step_ptr_t stepIt3(&rgb8,5);

    rgb8_pixel_t& ref1=stepIt3[5];
//  bool v=boost::is_POD<iterator_traits<pixel_step_iterator<rgb8_ptr_t> >::value_type>::value;
//  v=boost::is_POD<rgb8_pixel_t>::value;
//  v=boost::is_POD<int>::value;

    rgb8_step_ptr_t rgb8StepIt(ptr1, 10);
    rgb8_step_ptr_t rgb8StepIt2=rgb8StepIt;
    rgb8StepIt=rgb8StepIt2;
    ++rgb8StepIt;
    rgb8_ref_t reff=*rgb8StepIt; ignore_unused_variable_warning(reff);
    rgb8StepIt+=10;
    ptrdiff_t dst=rgb8StepIt2-rgb8StepIt; ignore_unused_variable_warning(dst);


    rgb8_pixel_t val1=ref1;
    rgb8_ptr_t ptr=&ref1;

    rgb8_pixel_t& val21=stepIt3[5];
    rgb8_pixel_t val22=val21;

    rgb8_pixel_t val2=stepIt3[5];
    rgb8_ptr_t ptr11=&(stepIt3[5]); ignore_unused_variable_warning(ptr11);
    rgb8_ptr_t ptr3=&*(stepIt3+5); ignore_unused_variable_warning(ptr3);

    rgb8_step_ptr_t stepIt4(ptr,5);
    ++stepIt4;

    rgb8_step_ptr_t stepIt5;
    if (stepIt4==stepIt5) {
        int st=0;ignore_unused_variable_warning(st);
    }

    pixel_image_iterator<rgb8_loc_t> pix_img_it(rgb8_loc_t(ptr, 20), 5);
    ++pix_img_it;
    pix_img_it+=10;
    rgb8_pixel_t& refr=*pix_img_it;
    refr=rgb8_pixel_t(1,2,3);
    *pix_img_it=rgb8_pixel_t(1,2,3);
    pix_img_it[3]=rgb8_pixel_t(1,2,3);
    *(pix_img_it+3)=rgb8_pixel_t(1,2,3);

    pixel_image_iterator<rgb8c_loc_t> pix_img_it_c(rgb8c_loc_t(rgb8c_ptr_t(ptr),20), 5);
    ++pix_img_it_c;
    pix_img_it_c+=10;
    //  *pix_img_it_c=rgb8_pixel_t(1,2,3);        // error: assigning though const iterator
    typedef pixel_image_iterator<rgb8_loc_t>::difference_type dif_t;
    dif_t dt=0;
    ptrdiff_t tdt=dt; ignore_unused_variable_warning(tdt);



    //  pixel_step_iterator<rgb8_pixel_t> stepIt3Err=stepIt+10;       // error: non-const from const iterator

    membased_2d_locator<rgb8_step_ptr_t> xy_locator(ptr,27);

    xy_locator.x()++;
//  pixel_step_iterator<rgb8_pixel_t>& yit=xy_locator.y();
    xy_locator.y()++;
    xy_locator+=point2<std::ptrdiff_t>(3,4);
    //*xy_locator=(xy_locator(-1,0)+xy_locator(0,1))/2;
    rgb8_pixel_t& rf=*xy_locator; ignore_unused_variable_warning(rf);

    make_step_iterator(rgb8_ptr_t(),3);
    make_step_iterator(rgb8_planar_ptr_t(),3);
    make_step_iterator(rgb8_planar_step_ptr_t(),3);

    // Test operator-> on planar ptrs
    {
    rgb8c_planar_ptr_t cp(&rgb8);
    rgb8_planar_ptr_t p(&rgb8);
    p->red = cp->green;

    }
//  xy_locator.y()++;

// TEST dynamic_step_t
    BOOST_STATIC_ASSERT(( boost::is_same<cmyk16_step_ptr_t,pixel_iterator_traits<cmyk16_step_ptr_t>::dynamic_step_t>::value )); 
    BOOST_STATIC_ASSERT(( boost::is_same<cmyk16_planar_step_ptr_t,pixel_iterator_traits<cmyk16_planar_ptr_t>::dynamic_step_t>::value )); 

    BOOST_STATIC_ASSERT(( boost::is_same<iterator_type<bits8,gray_t,false,false,false>::type,gray8c_ptr_t>::value ));

// TEST iterator_is_step
    BOOST_STATIC_ASSERT(iterator_is_step< cmyk16_step_ptr_t >::value);
    BOOST_STATIC_ASSERT(iterator_is_step< cmyk16_planar_step_ptr_t >::value);
    BOOST_STATIC_ASSERT(!iterator_is_step< cmyk16_planar_ptr_t >::value);

    typedef color_convert_deref_fn<rgb8c_ref_t, gray8_pixel_t> ccv_rgb_g_fn;
    typedef color_convert_deref_fn<gray8c_ref_t, rgb8_pixel_t> ccv_g_rgb_fn;
    gil_function_requires<PixelDereferenceAdaptorConcept<ccv_rgb_g_fn> >();
    gil_function_requires<PixelDereferenceAdaptorConcept<deref_compose<ccv_rgb_g_fn,ccv_g_rgb_fn> > >();
}
