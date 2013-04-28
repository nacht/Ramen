#ifndef RTGU_IMAGE_RESCALE_HPP
#define RTGU_IMAGE_RESCALE_HPP

#include <gil/extension/numeric/algorithm.hpp>
#include <gil/extension/numeric/pixel_numeric_operations.hpp>
#include "filter_weight_table.hpp"

//----------------------------------------------------------------------------

namespace boost { namespace gil {

  // horizontal rescale only, rescale src view into dst view, matching dst view width
  template <typename SrcView, typename DstView, typename Filter>
  inline void rescale_cols(const SrcView& src, const DstView& dst, const Filter& filter);

  // vertical rescale only, rescale src view into dst view, matching dst view height
  template <typename SrcView, typename DstView, typename Filter>
  inline void rescale_rows(const SrcView& src, const DstView& dst, const Filter& filter);

  // rescale src view into dst view, matching dst view dimensions
  template <typename SrcView, typename DstView, typename Filter>
  inline void rescale(const SrcView& src, const DstView& dst, const Filter& filter);

} }

//----------------------------------------------------------------------------

namespace boost { namespace gil {

  namespace detail {

    //----------------------------------------------------------------------------

    struct clamp_channel_fn 
    {
        template <typename SrcChannel, typename DstChannel>
        void operator()(const SrcChannel& src, DstChannel& dst)
        {
            typedef typename channel_traits<DstChannel>::value_type dst_value_t;
            typedef typename channel_traits<SrcChannel>::value_type src_value_t;
            dst = dst_value_t( 
                    std::min(
                        src_value_t(channel_traits<DstChannel>::max_value())
                      , std::max(
                            src_value_t(channel_traits<DstChannel>::min_value())
                          , src)
                      )
                    );
        }

        template <typename SrcChannel, typename T, typename S, typename U>
        void operator()(const SrcChannel& src, scoped_channel_value<T,S,U>& dst)
        {
            typedef typename channel_traits<scoped_channel_value<T,S,U> >::value_type dst_value_t;
            typedef typename channel_traits<SrcChannel>::value_type src_value_t;
            dst = dst_value_t( src);
        }

    };

    template <typename SrcPixel, typename DstPixel>
    void copy_clamp_pixel(const SrcPixel& src, DstPixel& dst)
    {
        static_for_each(src,dst,clamp_channel_fn());
    }

    //----------------------------------------------------------------------------

    //template <typename PixelType> struct create_accum_pixel_type : public create_accum_pixel_type<PixelType> {};

    template <typename PixelType> struct create_accum_pixel_type {};

    template <typename Layout>
    struct create_accum_pixel_type<pixel<bits8,Layout> >
    {
      typedef pixel<float, Layout> type;
    };

    template <typename Layout>
    struct create_accum_pixel_type<pixel<bits16,Layout> >
    {
      typedef pixel<float, Layout> type;
    };

    template <typename Layout>
    struct create_accum_pixel_type<pixel<bits32,Layout> >
    {
      typedef pixel<double, Layout> type;
    };

    template <typename Layout>
    struct create_accum_pixel_type<pixel<bits8s,Layout> >
    {
      typedef pixel<float, Layout> type;
    };

    template <typename Layout>
    struct create_accum_pixel_type<pixel<bits16s,Layout> >
    {
      typedef pixel<float, Layout> type;
    };

    template <typename Layout>
    struct create_accum_pixel_type<pixel<bits32s,Layout> >
    {
      typedef pixel<double, Layout> type;
    };

    template <typename Layout>
    struct create_accum_pixel_type<pixel<bits32f,Layout> >
    {
      typedef pixel<float, Layout> type;
    };

    //----------------------------------------------------------------------------

    /// \brief 1D un-guarded correlation with a variable-size kernel
    /// This function is much like 'correlate_pixels_n' in boost/gil/extension/numeric/algorithm.hpp
    /// except it clamps the result to min/max channel values (because of filters potential clipping artifacts
    template <typename AccumulatorPixelType, typename SrcIterator, typename DstIterator, typename ContribIterator>
    void rescale_line(SrcIterator const src_begin, DstIterator const dst_begin, ContribIterator const contrib_begin, ContribIterator const contrib_end)
    {
      typedef typename pixel_proxy<typename std::iterator_traits<SrcIterator>::value_type>::type src_pixel_t;

      typedef pixel_plus_t<AccumulatorPixelType, AccumulatorPixelType, AccumulatorPixelType> pixel_adder_t;
      typedef pixel_multiplies_scalar_t<src_pixel_t, float, AccumulatorPixelType> pixel_mul_t;                  // NB, no color conversion occuring

      AccumulatorPixelType zero_pixel;
      zero_channels(zero_pixel);
      
      ContribIterator contrib_it = contrib_begin;
      DstIterator dst_it = dst_begin;

      // iterate over contributions, one contribution for every destination pixel
      while (contrib_it != contrib_end)
      {
        // iterator range on source pixels contributing to current destination pixel
        SrcIterator const src_pixels_begin = src_begin + contrib_it->left;
        SrcIterator const src_pixels_end   = src_begin + (contrib_it->right+1);
        
        // for every weight: accum += src_pixel * weight
        AccumulatorPixelType accum = std::inner_product(
            src_pixels_begin, src_pixels_end
          , contrib_it->weights
          , zero_pixel
          , pixel_adder_t()
          , pixel_mul_t()
          );

        copy_clamp_pixel(accum, *dst_it);

        ++contrib_it;
        ++dst_it;
      }
    }

  }

  //----------------------------------------------------------------------------

} }

//----------------------------------------------------------------------------

template <typename SrcView, typename DstView, typename Filter>
inline void boost::gil::rescale_cols(const SrcView& src, const DstView& dst, const Filter& filter)
{
  typedef typename SrcView::value_type src_pixel_t;
  typedef typename gil::detail::create_accum_pixel_type<src_pixel_t>::type accum_pixel_t;

  if (src.height() != dst.height())
  {
    throw std::runtime_error("rescale_cols: height of source and destination views must match");
  }

  detail::weight_table weights;
  weights.reset(filter, src.width(), dst.width());

  detail::weight_table::const_iterator const table_begin = weights.begin();
  detail::weight_table::const_iterator const table_end   = weights.end();

  typedef typename DstView::coord_t coord_t;
  coord_t const height = dst.height();

  for (coord_t y=0; y<height; y++)
  {
    detail::rescale_line<accum_pixel_t>(
        src.row_begin(y)
      , dst.row_begin(y)
      , table_begin
      , table_end
      );
  }
}

//----------------------------------------------------------------------------

template <typename SrcView, typename DstView, typename Filter>
inline void boost::gil::rescale_rows(const SrcView& src, const DstView& dst, const Filter& filter)
{
  typedef typename SrcView::value_type src_pixel_t;
  typedef typename gil::detail::create_accum_pixel_type<src_pixel_t>::type accum_pixel_t;

  if (src.width() != dst.width())
  {
    throw std::runtime_error("rescale_rows: width of source and destination views must match");
  }

  detail::weight_table weights;
  weights.reset(filter, src.height(), dst.height());

  detail::weight_table::const_iterator const table_begin = weights.begin();
  detail::weight_table::const_iterator const table_end   = weights.end();

  typedef typename DstView::coord_t coord_t;
  coord_t const width = dst.width();

  for (coord_t x=0; x<width; x++)
  {
    detail::rescale_line<accum_pixel_t>(
        src.col_begin(x)
      , dst.col_begin(x)
      , table_begin
      , table_end
      );
  }
}

//----------------------------------------------------------------------------

template <typename SrcView, typename DstView, typename Filter>
inline void boost::gil::rescale(const SrcView& src, const DstView& dst, const Filter& filter)
{
  typedef typename SrcView::value_type src_pixel_t;
  typedef typename gil::detail::create_accum_pixel_type<src_pixel_t>::type accum_pixel_t;

  // construct weights tables

  detail::weight_table horizontal_weights;
  horizontal_weights.reset(filter, src.width(), dst.width());

  detail::weight_table vertical_weights;
  vertical_weights.reset(filter, src.height(), dst.height());

  // allocate intermediary pixels row

  std::vector<accum_pixel_t> intermediate_row( src.width() );

  typedef typename DstView::coord_t coord_t;
  coord_t const src_width  = src.width();
  coord_t const dst_height = dst.height();

  for (coord_t y=0; y<dst_height; y++)
  {
    // create the intermediate row by vertically sampling the source image pixels

    detail::weight_table::const_iterator const vtable_begin = vertical_weights.begin() + y;
    detail::weight_table::const_iterator const vtable_end   = vtable_begin + 1;

    for (coord_t x=0; x<src_width; x++)
    {
      detail::rescale_line<accum_pixel_t>(
          src.col_begin(x)
        , intermediate_row.begin() + x
        , vtable_begin
        , vtable_end
        );
    }

    // scale horizontally the intermediate row into the destination row

    detail::weight_table::const_iterator const htable_begin = horizontal_weights.begin();
    detail::weight_table::const_iterator const htable_end   = horizontal_weights.end();

    detail::rescale_line<accum_pixel_t>(
        intermediate_row.begin()
      , dst.row_begin(y)
      , htable_begin
      , htable_end
      );
  }
}

//----------------------------------------------------------------------------

#endif
