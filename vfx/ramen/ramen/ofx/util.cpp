// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/util.hpp>

namespace ramen
{
namespace ofx
{

std::string ofx_status_to_string( OfxStatus s)
{
    switch( s)
    {
    case kOfxStatOK:
	return std::string( "kOfxStatOK");

    case kOfxStatFailed:
	return std::string( "kOfxStatFailed");

    case kOfxStatErrFatal:
	return std::string( "kOfxStatErrFatal");

    case kOfxStatErrUnknown:
	return std::string( "kOfxStatErrUnknown");

    case kOfxStatErrMissingHostFeature:
	return std::string( "kOfxStatErrMissingHostFeature");

    case kOfxStatErrUnsupported:
	return std::string( "kOfxStatErrUnsupported");

    case kOfxStatErrExists:
	return std::string( "kOfxStatErrExists");

    case kOfxStatErrFormat:
	return std::string( "kOfxStatErrFormat");

    case kOfxStatErrMemory:
	return std::string( "kOfxStatErrMemory");

    case kOfxStatErrBadHandle:
	return std::string( "kOfxStatErrBadHandle");

    case kOfxStatErrBadIndex:
	return std::string( "kOfxStatErrBadIndex");

    case kOfxStatErrValue:
	return std::string( "kOfxStatErrValue");

    case kOfxStatReplyYes:
	return std::string( "kOfxStatReplyYes");

    case kOfxStatReplyNo:
	return std::string( "kOfxStatReplyNo");

    case kOfxStatReplyDefault:
	return std::string( "kOfxStatReplyDefault");
    }
}

} // namespace
} // namespace

std::ostream& operator << ( std::ostream& os, const OfxPointI& p)
{
    os << "[ " << p.x << ", " << p.y << "]";
    return os;
}

std::ostream& operator << ( std::ostream& os, const OfxPointD& p)
{
    os << "[ " << p.x << ", " << p.y << "]";
    return os;
}

std::ostream& operator << ( std::ostream& os, const OfxRectI& rect)
{
    os << "[ " << rect.x1 << ", " << rect.y1 << ", " << rect.x2 << ", " << rect.y2 << "]";
    return os;
}

std::ostream& operator << ( std::ostream& os, const OfxRectD& rect)
{
    os << "[ " << rect.x1 << ", " << rect.y1 << ", " << rect.x2 << ", " << rect.y2 << "]";
    return os;
}
