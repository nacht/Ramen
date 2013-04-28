#ifndef BOOST_GIL_FILTER_WEIGHT_TABLE_HPP
#define BOOST_GIL_FILTER_WEIGHT_TABLE_HPP

#include <vector>
#include <math.h>
#include <boost/utility.hpp>

// adapted from FreeImage source code (see FreeImage/Source/FreeImageToolkit/Resize.h)

namespace boost { namespace gil {

namespace detail
{
  //----------------------------------------------------------------------------
  /**
    Filter weights table.<br>
    This class stores contribution information for an entire line (row or column).
  */
  class weight_table : boost::noncopyable
  {
    /** 
      Sampled filter weight table.<br>
      Contribution information for a single pixel
    */
    struct Contribution {
      /// Normalized weights of neighboring pixels (not owned, pointer to elements of _weights_storage)
      float* weights;
	    /// Bounds of source pixels window
	    int left, right;   
    };

	  /// Row (or column) of contribution weights 
    std::vector<Contribution> _contribution_table;      // TODO add allocator template parameter or replace std::vector use entirely
    std::vector<float> _weights_storage;

	  /// Filter window size (of affecting source pixels) 
	  unsigned _window_size;

  public:

    typedef std::vector<Contribution>::const_iterator const_iterator;

    weight_table() {};

	  /** 
	  Allocate and compute the weights table
	  @param filter Filter used for upsampling or downsampling
	  @param src_size Length (in pixels) of the source line
	  @param dst_size Length (in pixels) of the destination line
	  */
    template <typename Filter>
    void reset(Filter const& filter, unsigned src_size, unsigned dst_size);

    unsigned window_size() const { return _window_size; };

    const_iterator begin() const { return _contribution_table.begin(); };
    const_iterator end() const   { return _contribution_table.end(); };
  };

  //----------------------------------------------------------------------------

} // namespace detail

} }   // namespace boost::gil

template <typename Filter>
void boost::gil::detail::weight_table::reset(Filter const& filter, unsigned src_size, unsigned dst_size)
{
  float const filter_width = static_cast<float>( filter.width() );

  // scale factor from source to destination
  float const scale_factor = static_cast<float>(dst_size) / static_cast<float>(src_size);

  float sample_width, filter_scale_factor;

  if (scale_factor < 1.0)
  {
    // minification
    sample_width  = filter_width / scale_factor; 
    filter_scale_factor = scale_factor; 
  }
  else
  {
    // magnification
    sample_width  = filter_width;
    filter_scale_factor = 1.0f;
  }

  // window size is the number of sampled source pixels per destination pixel
  _window_size = 2 * static_cast<int>( ceil(sample_width) ) + 1; 

  // allocate list of contributions 
  _contribution_table.resize( dst_size );

  // allocate vector for every needed weight factor
  _weights_storage.resize( dst_size * _window_size );

  // offset for discrete to continuous coordinate conversion
  float const scale_offset = (0.5f / scale_factor) - 0.5f;

  // scan through line of contributions

  for (unsigned u = 0; u < dst_size; u++)
  {
    float const sample_center = static_cast<float>(u) / scale_factor + scale_offset;   // reverse mapping to source coordinates

    // find the significant edge points that affect the pixel

    int sample_begin = static_cast<int>( floor(sample_center - sample_width) );
    int sample_end   = static_cast<int>(  ceil(sample_center + sample_width) );

    // clamp edge points to borders
    // FIXME could handle wrapping of image here

    sample_begin = std::max( sample_begin, 0 );
    sample_end   = std::min( sample_end, static_cast<int>(src_size) - 1 );
    
    // cut edge points to fit in filter window in case of spill-off due to rounding

    if ((sample_end - sample_begin + 1) > static_cast<int>(_window_size))
    {
      if (sample_begin < ((int(src_size) - 1) / 2)) // FIXME missing () ?
      {    
        sample_begin++;
      }
      else
      {
        sample_end--; 
      }
    }

    // calculate weights

    float* const weight_base_pointer = &_weights_storage[ u * _window_size ];
    float* weight_storage = weight_base_pointer;

    float total_weight = 0.f;  // zero sum of weights

    for (int srcx = sample_begin; srcx <= sample_end; srcx++)
    {
      float x = filter_scale_factor * (sample_center - static_cast<float>(srcx));
      float filter_value = filter.filter(x);
      float weight = filter_scale_factor * filter_value;
      
      total_weight += weight;

      *weight_storage++ = weight;
    }
    
    // normalize weight of neighbouring points

    if ((total_weight > 0) && (total_weight != 1))
    {
      float* weight_storage = weight_base_pointer;

      for (int srcx = sample_begin; srcx <= sample_end; srcx++)
      {
        *weight_storage++ /= total_weight; 
      }
      
      // simplify the filter, discarding null weights at the right

      int weight_index = sample_end - sample_begin;
      while (weight_index > 0 && weight_base_pointer[weight_index] == 0.f)
      {
        sample_end--;
        weight_index--;
      }
    }

    // store contribution for current pixel

    _contribution_table[u].left    = sample_begin; 
    _contribution_table[u].right   = sample_end;
    _contribution_table[u].weights = weight_base_pointer;
  } 
}

#endif
