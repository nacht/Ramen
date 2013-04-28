//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_TBB_ALGO_PARALLEL_FOR_EACH_HPP
#define GIL_EXTENSION_TBB_ALGO_PARALLEL_FOR_EACH_HPP

#include<boost/gil/algorithm.hpp>

#include<tbb/parallel_for.h>
#include<tbb/blocked_range.h>

#include<gil/extension/algorithm/tbb/tbb_config.hpp>

namespace boost
{
namespace gil
{

namespace detail
{

template<class View, class Fun>
struct tbb_for_each_pixel_fun
{
    tbb_for_each_pixel_fun( const View& view, Fun f) : view_(view), f_(f) {}

    void operator()( const tbb::blocked_range<size_t>& r) const
    {
        for( std::size_t i = r.begin(); i != r.end(); ++i)
            std::for_each( view_.row_begin(i), view_.row_end(i), f_);
    }

    const View& view_;
    Fun f_;
};

template<class View, class Fun>
tbb_for_each_pixel_fun<View, Fun> make_tbb_for_each_pixel_fun( const View& view, Fun f)
{
    return tbb_for_each_pixel_fun<View, Fun>( view, f);
}

} // detail

template<class View, class Fun>
GIL_FORCEINLINE void tbb_for_each_pixel( const View& v, Fun f)
{
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, v.height()), 
                        detail::make_tbb_for_each_pixel_fun( v, f),
                        GIL_TBB_ALGORITHMS_DEFAULT_PARTITIONER());
}

template<class View, class Fun>
GIL_FORCEINLINE void tbb_for_each_pixel( const View& v, Fun f, const tbb::auto_partitioner& part)
{
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, v.height()), detail::make_tbb_for_each_pixel_fun( v, f), part);
}

template<class View, class Fun>
GIL_FORCEINLINE void tbb_for_each_pixel( const View& v, Fun f, const tbb::simple_partitioner& part)
{
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, v.height()), detail::make_tbb_for_each_pixel_fun( v, f), part);
}

template<class View, class Fun>
GIL_FORCEINLINE void tbb_for_each_pixel( const View& v, Fun f, tbb::affinity_partitioner& part)
{
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, v.height()), detail::make_tbb_for_each_pixel_fun( v, f), part);
}

} // namespace
} // namespace

#endif
