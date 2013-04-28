// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_ALGORITHM_HPP
#define	RAMEN_IMAGEIO_ALGORITHM_HPP

namespace ramen
{
namespace imageio
{

template<class SrcIter, class DstIter>
void copy_subsampled( SrcIter src_begin, SrcIter src_end, DstIter dst_begin, DstIter dst_end, int subsample = 1)
{
    DstIter dst_it( dst_begin);

    for( SrcIter src_it = src_begin; src_it < src_end; src_it += subsample)
    {
	if( dst_it == dst_end)
	    return;

	*dst_it++ = *src_it;
    }

    // fill remaining space if needed
    for( ; dst_it != dst_end; ++dst_it)
	*dst_it = *(src_end - 1);
}

template<class SrcRange, class DstRange>
void copy_subsampled( const SrcRange& src, DstRange& dst, int subsample = 1)
{
    copy_subsampled( boost::begin( src), boost::end( src), boost::begin( dst), boost::end( dst), subsample);
}


template<class SrcIter, class DstIter, class Fun>
void transform_subsampled( SrcIter src_begin, SrcIter src_end, DstIter dst_begin, DstIter dst_end,
			    Fun f, int subsample = 1)
{
    DstIter dst_it( dst_begin);

    for( SrcIter src_it = src_begin; src_it < src_end; src_it += subsample)
    {
	if( dst_it == dst_end)
	    return;

	*dst_it++ = f( *src_it);
    }

    // fill remaining space if needed
    for( ; dst_it != dst_end; ++dst_it)
	*dst_it = f( *(src_end - 1));
}

template<class SrcRange, class DstRange, class Fun>
void transform_subsampled( const SrcRange& src, DstRange& dst, Fun f, int subsample = 1)
{
    transform_subsampled( boost::begin( src), boost::end( src), boost::begin( dst), boost::end( dst), f, subsample);
}

} // namespace
} // namespace

#endif
