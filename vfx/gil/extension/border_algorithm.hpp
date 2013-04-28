//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_BORDER_ALGORITHM_HPP
#define GIL_EXTENSION_BORDER_ALGORITHM_HPP

#include<boost/gil/algorithm.hpp>

namespace boost
{
namespace gil
{

// fill pixels around border
template <typename View, typename Pixel>
void fill_border_pixels(const View& view, int border_x0, int border_y0, int border_x1, int border_y1, const Pixel& val)
{
    assert( border_x0 >= 0);
    assert( border_y0 >= 0);
    assert( border_x1 >= 0);
    assert( border_y1 >= 0);
    assert( border_x0 < view.width());
    assert( border_y0 < view.height());
    assert( border_x1 < view.width());
    assert( border_y1 < view.height());

    // top
    for( int j=0;j<border_y0;++j)
    {
        typename View::x_iterator top_it( view.row_begin( j));

        for( std::ptrdiff_t x=0; x<view.width(); ++x)
            top_it[x] = val;
    }

    // bottom
    for( int j=0;j<border_y1;++j)
    {
        typename View::x_iterator bot_it( view.row_begin( view.height() - j - 1));

        for( std::ptrdiff_t x=0; x<view.width(); ++x)
            bot_it[x] = val;
    }

    // left
    for( int j=0;j<border_x0;++j)
    {
        typename View::y_iterator left_it(  view.col_begin( j));

        for( std::ptrdiff_t y=0; y<view.height(); ++y)
            left_it[y] = val;
    }

    // right
    for( int j=0;j<border_x1;++j)
    {
        typename View::y_iterator right_it( view.col_begin( view.width() - j - 1));

        for( std::ptrdiff_t y=0; y<view.height(); ++y)
            right_it[y] = val;
    }
}

// reflect pixels around border
template <typename View>
void reflect_border_pixels(const View& view, int border_x0, int border_y0, int border_x1, int border_y1)
{
    assert( border_x0 >= 0);
    assert( border_y0 >= 0);
    assert( border_x1 >= 0);
    assert( border_y1 >= 0);
    assert( border_x0 < view.width());
    assert( border_y0 < view.height());
    assert( border_x1 < view.width());
    assert( border_y1 < view.height());

    // top
    for( int j=0;j<border_y0;++j)
    {
        typename View::x_iterator dst_it( view.row_begin( j));
        typename View::x_iterator src_it( view.row_begin( 2 * border_y0 - j));

        for( std::ptrdiff_t x=0; x<view.width(); ++x)
            dst_it[x] = src_it[x];
    }

    // bottom
    for( int j=0;j<border_y1;++j)
    {
        typename View::x_iterator dst_it( view.row_begin( view.height() -j -1));
        typename View::x_iterator src_it( view.row_begin( view.height() - (2 * border_y1) +j -1));

        for( std::ptrdiff_t x=0; x<view.width(); ++x)
            dst_it[x] = src_it[x];
    }

    // left
    for( int j=0;j<border_x0;++j)
    {
        typename View::y_iterator dst_it(  view.col_begin( j));
        typename View::y_iterator src_it( view.col_begin( 2*border_x0 - j));

        for( std::ptrdiff_t y=0; y<view.height(); ++y)
            dst_it[y] = src_it[y];
    }

    // right
    for( int j=0;j<border_x1;++j)
    {
        typename View::y_iterator dst_it(  view.col_begin( view.width() - j - 1));
        typename View::y_iterator src_it( view.col_begin( view.width() - (2 * border_x1) +j -1));

        for( std::ptrdiff_t y=0; y<view.height(); ++y)
            dst_it[y] = src_it[y];
    }
}

// repeat pixels around border
template <typename View>
void repeat_border_pixels(const View& view, int border_x0, int border_y0, int border_x1, int border_y1)
{
    assert( border_x0 >= 0);
    assert( border_y0 >= 0);
    assert( border_x1 >= 0);
    assert( border_y1 >= 0);
    assert( border_x0 < view.width());
    assert( border_y0 < view.height());
    assert( border_x1 < view.width());
    assert( border_y1 < view.height());

    // top
    typename View::x_iterator top_src_it( view.row_begin( border_y0));

    for( int j=0;j<border_y0;++j)
    {
        typename View::x_iterator top_dst_it( view.row_begin( j));

        for( std::ptrdiff_t x=0; x<view.width(); ++x)
            top_dst_it[x] = top_src_it[x];
    }

    // bottom
    typename View::x_iterator bot_src_it( view.row_begin( view.height() - border_y1 -1));

    for( int j=0;j<border_y1;++j)
    {
        typename View::x_iterator bot_dst_it( view.row_begin( view.height() -j -1));

        for( std::ptrdiff_t x=0; x<view.width(); ++x)
            bot_dst_it[x] = bot_src_it[x];
    }

    // left
    typename View::y_iterator lef_src_it( view.col_begin( border_x0));

    for( int j=0;j<border_x0;++j)
    {
        typename View::y_iterator lef_dst_it( view.col_begin( j));
		
        for( std::ptrdiff_t y=0; y<view.height(); ++y)
            lef_dst_it[y] = lef_src_it[y];
    }

    // right
    typename View::y_iterator rig_src_it( view.col_begin( view.width() - border_x1 -1));

    for( int j=0;j<border_x1;++j)
    {
        typename View::y_iterator rig_dst_it( view.col_begin( view.width() -j -1));
		
        for( std::ptrdiff_t y=0; y<view.height(); ++y)
            rig_dst_it[y] = rig_src_it[y];
    }
}

} // namespace
} // namespace

#endif
