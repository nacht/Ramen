#ifndef RTGU_IMAGE_RESCALE_ANY_VIEW_HPP
#define RTGU_IMAGE_RESCALE_ANY_VIEW_HPP

#include <boost/gil/extension/dynamic_image/any_image_view.hpp>
#include "rescale.hpp"

//----------------------------------------------------------------------------

namespace boost { namespace gil {

  template <typename ViewTypes, typename Filter>
  inline void rescale_any_view(any_image_view<ViewTypes>& src, any_image_view<ViewTypes>& dst, const Filter& filter);

} }

//----------------------------------------------------------------------------

namespace boost { namespace gil { namespace detail
{
  template <typename Views, typename Filter>
  struct rescale_obj
  {
    typedef void result_type;        // required typedef
    
    any_image_view<Views>& _dst_view;
    const Filter& _filter;

    rescale_obj(any_image_view<Views>& dst_view, const Filter& filter) : _dst_view(dst_view), _filter(filter) {}

    template <typename SrcView> 
    void operator()(SrcView& src) const
    { 
      return rescale( src, _dst_view._dynamic_cast<SrcView>(), _filter );
    }
  };
} } }

template <typename ViewTypes, typename Filter>
inline void boost::gil::rescale_any_view(any_image_view<ViewTypes>& src, any_image_view<ViewTypes>& dst, const Filter& filter)
{
  apply_operation( src, detail::rescale_obj<ViewTypes, Filter>(dst, filter) );
}

//----------------------------------------------------------------------------

#endif
