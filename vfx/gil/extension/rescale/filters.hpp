#ifndef BOOST_GIL_FILTERS_HPP
#define BOOST_GIL_FILTERS_HPP

// Filters adapted from FreeImage source code (see FreeImage/Source/FreeImageToolkit/Filters.h)

#include<cmath>

namespace boost { namespace gil {

struct bilinear_filter
{
  float width() const { return 1.0f; }

  float filter(float x) const {
		x = fabs(x); 
		return (x < width() ? width() - x : 0); 
  }
};

/**
 Mitchell & Netravali's two-param cubic filter<br>

 The parameters b and c can be used to adjust the properties of the cubic. 
 They are sometimes referred to as "blurring" and "ringing" respectively. 
 The default is b = 1/3 and c = 1/3, which were the values recommended by 
 Mitchell and Netravali as yielding the most visually pleasing results in subjective tests of human beings. 
 Larger values of b and c can produce interesting op-art effects--for example, try b = 0 and c = -5. <br><br>

 <b>Reference</b> : <br>
 Don P. Mitchell and Arun N. Netravali, Reconstruction filters in computer graphics. 
 In John Dill, editor, Computer Graphics (SIGGRAPH '88 Proceedings), Vol. 22, No. 4, August 1988, pp. 221-228.
*/
struct bicubic_filter
{
protected:
	// data for parameterized Mitchell filter
  float p0, p2, p3;
  float q0, q1, q2, q3;

public:

  float width() const { return 2.0f; }

  /**
	Constructor<br>
	Default fixed width = 2
	@param b Filter parameter (default value is 1/3)
	@param c Filter parameter (default value is 1/3)
	*/
  bicubic_filter (float b = 1.0f / 3.0f, float c = 1.0f / 3.0f)
  {
    p0 = (6 - 2*b) / 6;
    p2 = (-18 + 12*b + 6*c) / 6;
    p3 = (12 - 9*b - 6*c) / 6;
    q0 = (8*b + 24*c) / 6;
    q1 = (-12*b - 48*c) / 6;
    q2 = (6*b + 30*c) / 6;
    q3 = (-b - 6*c) / 6;
  }

  float filter(float x) const
  { 
	  x = fabs(x);

	  if(x < 1.0f)
		  return (p0 + x*x*(p2 + x*p3));

	  if(x < 2.0f)
		  return (q0 + x*(q1 + x*(q2 + x*q3)));
	  
	  return 0;
  }
};

/**
 Catmull-Rom spline, Overhauser spline<br>

 <b>References</b> : <br>
 <ul>
 <li>Mitchell Don P., Netravali Arun N., Reconstruction filters in computer graphics. 
 In John Dill, editor, Computer Graphics (SIGGRAPH '88 Proceedings), Vol. 22, No. 4, August 1988, pp. 221-228.
 <li>Keys R.G., Cubic Convolution Interpolation for Digital Image Processing. 
 IEEE Trans. Acoustics, Speech, and Signal Processing, vol. 29, no. 6, pp. 1153-1160, Dec. 1981.
 </ul>
*/
class catmull_rom_filter
{
public:

  float width() const { return 2.0f; }

  float filter(float x) const
  { 
    if (x < -2) return 0.0f;
    if (x < -1) return 0.5f * (4 + x*(8 + x*(5 + x)));
    if (x < 0)  return 0.5f * (2 + x*x*(-5 - 3*x));
    if (x < 1)  return 0.5f * (2 + x*x*(-5 + 3*x));
    if (x < 2)  return 0.5f * (4 + x*(-8 + x*(5 - x)));
    return 0.0f;
  }
};

class lanczos_filter
{
public:

    float width() const { return 2.0f;}

    float filter( float x) const { return sinc1d( x * 0.5f);}

private:

    float sinc1d( float x) const
    {
	if( x < 0)
	    x = -x;

	if( x < 1e-5f)
	    return 1.0f;

	if( x > 1.0f)
	    return 0.0f;

	x *= 3.1415926535897f;
	float sinc = std::sin( 3.0f * x) / ( 3.0f * x);
	float lanczos = std::sin(x) / x;
	return sinc * lanczos;
    }
};

} }   // namespace boost::gil

#endif
