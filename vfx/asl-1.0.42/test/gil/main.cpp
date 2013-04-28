/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
// image_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void test_integral_image();
int test_convolve();
void test_resample();

int main(int argc, unsigned char* argv[])
{
#ifdef NO_ASL
    //test_integral_image();
    //performance_test();
#endif

    test_resample();
    test_convolve();
    test_pixel();
 //   test_pixel_iterator();
//  test_image_io();
    test_image();
//  test_image_tiler();
    return 0;
}
