//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_PARALLEL_COPY_HPP
#define GIL_EXTENSION_PARALLEL_COPY_HPP

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

template<class View1, class View2>
struct tbb_copy_pixels_fun
{
    tbb_copy_pixels_fun( const View1& src, const View2& dst) : src_(src), dst_(dst) {}

    void operator()( const tbb::blocked_range<size_t>& r) const
    {
	for( std::size_t i = r.begin(); i != r.end(); ++i)
	    std::copy( src_.row_begin(i), src_.row_end(i), dst_.row_begin(i));
    }

    const View1& src_;
    const View2& dst_;
};

template<class View1, class View2>
tbb_copy_pixels_fun<View1, View2> make_tbb_copy_pixels_fun( const View1& src, const View2& dst)
{
    return tbb_copy_pixels_fun<View1, View2>( src, dst);
}

template <typename CC>
class tbb_copy_and_convert_pixels_fn : public binary_operation_obj<tbb_copy_and_convert_pixels_fn<CC> >
{
private:
    CC _cc;
public:

    typedef typename binary_operation_obj<tbb_copy_and_convert_pixels_fn<CC> >::result_type result_type;
    tbb_copy_and_convert_pixels_fn() {}

    tbb_copy_and_convert_pixels_fn(CC cc_in) : _cc(cc_in) {}
   // when the two color spaces are incompatible, a color conversion is performed
    template <typename V1, typename V2>
    result_type apply_incompatible( const V1& src, const V2& dst) const
    {
        //copy_pixels( color_converted_view<typename V2::value_type>( src, _cc),dst);
	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
			    detail::make_tbb_copy_pixels_fun( color_converted_view<typename V2::value_type>( src, _cc), dst),
			    tbb::auto_partitioner());
    }

    // If the two color spaces are compatible, copy_and_convert is just copy
    template <typename V1, typename V2>
    result_type apply_compatible( const V1& src, const V2& dst) const
    {
	tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
			    detail::make_tbb_copy_pixels_fun( src, dst),
			    tbb::auto_partitioner());
    }
};

} // detail

template<class View1, class View2>
GIL_FORCEINLINE void tbb_copy_pixels( const View1& src, const View2& dst)
{ 
    assert(src.dimensions()==dst.dimensions());
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
			detail::make_tbb_copy_pixels_fun( src, dst),
			tbb::auto_partitioner());
}

template <typename V1, typename V2,typename CC>
GIL_FORCEINLINE void tbb_copy_and_convert_pixels(const V1& src, const V2& dst,CC cc)
{
    detail::tbb_copy_and_convert_pixels_fn<CC> ccp(cc);
    ccp( src, dst);
}

template <typename View1, typename View2>
GIL_FORCEINLINE void tbb_copy_and_convert_pixels(const View1& src, const View2& dst)
{
    detail::tbb_copy_and_convert_pixels_fn<default_color_converter> ccp;
    ccp( src, dst);
}

} // namespace gil
} // namespace boost

#endif

