// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_UTIL_HPP
#define	RAMEN_OFX_UTIL_HPP

#include<string>
#include<ostream>

#include"ofxCore.h"

namespace ramen
{
namespace ofx
{

std::string ofx_status_to_string( OfxStatus s);

} // namespace
} // namespace

std::ostream& operator << ( std::ostream& os, const OfxPointI& p);
std::ostream& operator << ( std::ostream& os, const OfxPointD& p);

std::ostream& operator << ( std::ostream& os, const OfxRectI& rect);
std::ostream& operator << ( std::ostream& os, const OfxRectD& rect);

#endif
