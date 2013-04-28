//  Copyright Esteban Tovagliari 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXT_TBB_PARALLEL_RESCALE_HPP
#define	GIL_EXT_TBB_PARALLEL_RESCALE_HPP

#include"rescale.hpp"

#include<tbb/parallel_for.h>
#include<tbb/blocked_range.h>

namespace boost
{
namespace gil
{

namespace detail
{

template<class SrcView, class DstView, class Filter>
struct tbb_rescale_rows_fun
{
    tbb_rescale_rows_fun( const SrcView& src, const DstView& dst, const Filter& filter,
                        const detail::weight_table& weights) : src_(src), dst_(dst), filter_(filter), weights_(weights)
    {
    }

    void operator()( const tbb::blocked_range<size_t>& r) const
    {
        typedef typename SrcView::value_type src_pixel_t;
        typedef src_pixel_t accum_pixel_t;

        detail::weight_table::const_iterator const table_begin = weights_.begin();
        detail::weight_table::const_iterator const table_end   = weights_.end();

        for( std::size_t i = r.begin(); i != r.end(); ++i)
            detail::rescale_line<accum_pixel_t>( src_.col_begin(i), dst_.col_begin(i), table_begin, table_end);
    }

    const SrcView& src_;
    const DstView& dst_;
    const Filter& filter_;
    const detail::weight_table& weights_;
};

template<class SrcView, class DstView, class Filter>
struct tbb_rescale_cols_fun
{
    tbb_rescale_cols_fun( const SrcView& src, const DstView& dst, const Filter& filter,
                        const detail::weight_table& weights) : src_(src), dst_(dst), filter_(filter), weights_(weights)
    {
    }

    void operator()( const tbb::blocked_range<size_t>& r) const
    {
        typedef typename SrcView::value_type src_pixel_t;
        typedef src_pixel_t accum_pixel_t;

        detail::weight_table::const_iterator const table_begin = weights_.begin();
        detail::weight_table::const_iterator const table_end   = weights_.end();

        for( std::size_t i = r.begin(); i != r.end(); ++i)
            detail::rescale_line<accum_pixel_t>( src_.row_begin(i), dst_.row_begin(i), table_begin, table_end);
    }

    const SrcView& src_;
    const DstView& dst_;
    const Filter& filter_;
    const detail::weight_table& weights_;
};

} // namespace

template <typename SrcView, typename DstView, typename Filter>
inline void tbb_rescale_rows(const SrcView& src, const DstView& dst, const Filter& filter)
{
    assert(src.width() == dst.width());

    detail::weight_table weights;
    weights.reset( filter, src.height(), dst.height());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.width()),
                        detail::tbb_rescale_rows_fun<SrcView, DstView, Filter>( src, dst, filter, weights),
                        tbb::auto_partitioner());
}

template <typename SrcView, typename DstView, typename Filter>
inline void tbb_rescale_cols(const SrcView& src, const DstView& dst, const Filter& filter)
{
    assert(src.height() == dst.height());

    detail::weight_table weights;
    weights.reset( filter, src.width(), dst.width());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
                        detail::tbb_rescale_cols_fun<SrcView, DstView, Filter>( src, dst, filter, weights),
                        tbb::auto_partitioner());
}

template <typename SrcView, typename DstView, typename Filter>
inline void tbb_rescale(const SrcView& src, const DstView& dst, const Filter& filter)
{
    typedef typename SrcView::value_type src_pixel_t;
    typedef typename gil::detail::create_accum_pixel_type<src_pixel_t>::type accum_pixel_t;

    image<accum_pixel_t,false> buffer( dst.width(), src.height());

    tbb_rescale_cols( src, boost::gil::view( buffer), filter);
    tbb_rescale_rows( boost::gil::const_view( buffer), dst, filter);
}

} // namespace
} // namespace

#endif	/* TBB_RESCALE_HPP */
