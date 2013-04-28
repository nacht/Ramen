// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IECORE_LINE_SEGMENT_ALGO_HPP
#define RAMEN_IECORE_LINE_SEGMENT_ALGO_HPP

#include<OpenEXR/ImathBox.h>

#include<IECore/LineSegment.h>

#include<ramen/IECoreExt/detail/clipLiangBarsky.hpp>

namespace ramen
{

template<class T>
bool clipLineSegment( IECore::LineSegment<Imath::Vec2<T> >& line, const Imath::Box<Imath::Vec2<T> >& box)
{
    unsigned result = detail::clipLineSegment( &(line.p0.x), &(line.p0.y), &(line.p1.x), &(line.p1.y), box);
    return !(result >= 4);
}

template<class T>
bool intersects( const IECore::LineSegment<Imath::Vec2<T> >& line, const Imath::Box<Imath::Vec2<T> >& box)
{
    IECore::LineSegment<Imath::Vec2<T> > l( line);
    return clipLineSegment( l, box);
}

} // namespace

#endif
