#ifndef BOOST_GIL_RESCALE_VIRTUAL_VIEW_HPP
#define BOOST_GIL_RESCALE_VIRTUAL_VIEW_HPP

#include <boost/gil/virtual_locator.hpp>

//----------------------------------------------------------------------------

namespace boost { namespace gil {

  // view types factories

  template <typename SourceView> struct rescale_x_view_type;
  template <typename SourceView> struct rescale_y_view_type;
  template <typename SourceView> struct rescale_view_type;

  // view factories

  template <typename SourceView, typename Filter>
  typename rescale_x_view_type<SourceView>::view_t
    rescale_x_view(SourceView const& view, typename SourceView::coord_t dst_width, const Filter& filter);

  template <typename SourceView, typename Filter>
  typename rescale_y_view_type<SourceView>::view_t
    rescale_y_view(SourceView const& view, typename SourceView::coord_t dst_height, const Filter& filter);

  template <typename SourceView, typename Filter>
  typename rescale_view_type<SourceView>::view_t
    rescale_view(SourceView const& view, typename SourceView::coord_t dst_width, typename SourceView::coord_t dst_height, const Filter& filter);

} }

//----------------------------------------------------------------------------

namespace boost { namespace gil {

  namespace detail {

    // models PixelDereferenceAdaptorConcept
    template <typename SourceView>
    struct rescale_x_fn
    {
      typedef SourceView          view_t;

      typedef typename SourceView::point_t    point_t;
      typedef typename SourceView::value_type value_type;

      typedef rescale_x_fn        const_t;
      typedef value_type          reference;
      typedef value_type          const_reference;
      typedef point_t             argument_type;
      typedef reference           result_type;
      BOOST_STATIC_CONSTANT(bool, is_mutable=false);

      SourceView   _srcview;
      boost::shared_ptr<weight_table> _weight_table;

      template <typename Filter>
      rescale_x_fn(const SourceView& view, typename SourceView::coord_t dst_width, const Filter& filter)
        : _srcview(view)
      {
        _weight_table.reset( new weight_table );
        _weight_table->reset( filter, view.width(), dst_width );
      }

      __forceinline result_type operator()(const point_t& p) const
      {
        typedef typename SourceView::value_type src_pixel_t;
        typedef gil::detail::create_accum_pixel_type<src_pixel_t>::type accum_pixel_t;

        value_type result;

        detail::weight_table::const_iterator const weights_begin = _weight_table->begin() + p.x;
        detail::weight_table::const_iterator const weights_end   = weights_begin + 1;

        detail::rescale_line<accum_pixel_t>(
          _srcview.row_begin(p.y)
          , &result
          , weights_begin
          , weights_end
          );

        return result;
      }
    };

  }       // namespace detail
} }

template <typename SourceView>
struct boost::gil::rescale_x_view_type
{
  GIL_CLASS_REQUIRE(SourceView, boost::gil, ImageViewConcept);

  typedef typename detail::rescale_x_fn<SourceView> function_t;
  typedef typename virtual_2d_locator<function_t, false> locator_t;
  typedef typename image_view<locator_t> view_t;

  template <typename Filter>
  static view_t make(SourceView const& view, typename SourceView::coord_t dst_width, const Filter& filter)
  {
    typedef function_t::point_t point_t;

    point_t dims(dst_width, view.height());

    // Construct a view with a locator, taking top-left corner (0,0) and step (1,1)
    view_t result(dims, locator_t(point_t(0,0), point_t(1,1), function_t(view, dst_width, filter)));

    return result;
  }
};

template <typename SourceView, typename Filter>
typename boost::gil::rescale_x_view_type<SourceView>::view_t
  boost::gil::rescale_x_view(SourceView const& view, typename SourceView::coord_t dst_width, const Filter& filter)
{
  typedef rescale_x_view_type<SourceView> view_factory_t;
  return view_factory_t::make(view, dst_width, filter);
}

//----------------------------------------------------------------------------

namespace boost { namespace gil {

  namespace detail {

    // models PixelDereferenceAdaptorConcept
    template <typename SourceView>
    struct rescale_y_fn
    {
      typedef SourceView          view_t;

      typedef typename SourceView::point_t    point_t;
      typedef typename SourceView::value_type value_type;

      typedef rescale_y_fn        const_t;
      typedef value_type          reference;
      typedef value_type          const_reference;
      typedef point_t             argument_type;
      typedef reference           result_type;
      BOOST_STATIC_CONSTANT(bool, is_mutable=false);

      SourceView   _srcview;
      boost::shared_ptr<weight_table> _weight_table;

      template <typename Filter>
      rescale_y_fn(const SourceView& view, typename SourceView::coord_t dst_height, const Filter& filter)
        : _srcview(view)
      {
        _weight_table.reset( new weight_table );
        _weight_table->reset( filter, view.height(), dst_height );
      }

      __forceinline result_type operator()(const point_t& p) const
      {
        typedef typename SourceView::value_type src_pixel_t;
        typedef gil::detail::create_accum_pixel_type<src_pixel_t>::type accum_pixel_t;

        value_type result;

        detail::weight_table::const_iterator const weights_begin = _weight_table->begin() + p.y;
        detail::weight_table::const_iterator const weights_end   = weights_begin + 1;

        detail::rescale_line<accum_pixel_t>(
          _srcview.col_begin(p.x)
          , &result
          , weights_begin
          , weights_end
          );

        return result;
      }
    };

  }       // namespace detail
} }

template <typename SourceView>
struct boost::gil::rescale_y_view_type
{
  GIL_CLASS_REQUIRE(SourceView, boost::gil, ImageViewConcept);

  typedef typename detail::rescale_y_fn<SourceView> function_t;
  typedef typename virtual_2d_locator<function_t, false> locator_t;
  typedef typename image_view<locator_t> view_t;

  template <typename Filter>
  static view_t make(SourceView const& view, typename SourceView::coord_t dst_height, const Filter& filter)
  {
    typedef function_t::point_t point_t;

    point_t dims(view.width(), dst_height);

    // Construct a view with a locator, taking top-left corner (0,0) and step (1,1)
    view_t result(dims, locator_t(point_t(0,0), point_t(1,1), function_t(view, dst_height, filter)));

    return result;
  }
};

template <typename SourceView, typename Filter>
typename boost::gil::rescale_y_view_type<SourceView>::view_t
  boost::gil::rescale_y_view(SourceView const& view, typename SourceView::coord_t dst_height, const Filter& filter)
{
  typedef rescale_y_view_type<SourceView> view_factory_t;
  return view_factory_t::make(view, dst_height, filter);
}

//----------------------------------------------------------------------------

template <typename SourceView>
struct boost::gil::rescale_view_type
{
  GIL_CLASS_REQUIRE(SourceView, boost::gil, ImageViewConcept);
  
  typedef rescale_x_view_type<SourceView> composition1_t;
  typedef rescale_y_view_type<typename composition1_t::view_t> composition2_t;
  
  typedef typename composition2_t::function_t function_t;
  typedef typename virtual_2d_locator< function_t,false > locator_t;
  typedef typename image_view<locator_t> view_t;
};

template <typename SourceView, typename Filter>
typename boost::gil::rescale_view_type<SourceView>::view_t
  boost::gil::rescale_view(SourceView const& view, typename SourceView::coord_t dst_width, typename SourceView::coord_t dst_height, const Filter& filter)
{
  typedef rescale_view_type<SourceView> view_factory_t;
  
  typedef view_factory_t::composition1_t composition1_t;
  typedef view_factory_t::composition2_t composition2_t;

  return composition2_t::make( composition1_t::make(view, dst_width, filter), dst_height, filter );
}

//----------------------------------------------------------------------------

#endif
