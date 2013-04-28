// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<OpenEXR/ImathMath.h>

namespace ramen
{

Imath::Box2i transform( const Imath::Box2i& box, const Imath::M33f& m, bool round_up)
{
    Imath::Box2f b( box.min, box.max);
    b = transform( b, m);

    if( round_up)
    {
        return Imath::Box2i( Imath::V2i( Imath::Math<float>::floor( b.min.x), Imath::Math<float>::floor( b.min.y)),
                             Imath::V2i( Imath::Math<float>::ceil( b.max.x) , Imath::Math<float>::ceil( b.max.y)));
    }
    else
    {
        return Imath::Box2i( Imath::V2i( Imath::Math<float>::ceil( b.min.x) , Imath::Math<float>::ceil( b.min.y)),
                             Imath::V2i( Imath::Math<float>::floor( b.max.x), Imath::Math<float>::floor( b.max.y)));
    }
}

Imath::Box2i transform( const Imath::Box2i& box, const Imath::M33d& m, bool round_up)
{
    Imath::Box2d b( box.min, box.max);
    b = transform( b, m);

    if( round_up)
    {
        return Imath::Box2i( Imath::V2i( Imath::Math<double>::floor( b.min.x), Imath::Math<double>::floor( b.min.y)),
                             Imath::V2i( Imath::Math<double>::ceil( b.max.x) , Imath::Math<double>::ceil( b.max.y)));
    }
    else
    {
        return Imath::Box2i( Imath::V2i( Imath::Math<double>::ceil( b.min.x) , Imath::Math<double>::ceil( b.min.y)),
                             Imath::V2i( Imath::Math<double>::floor( b.max.x), Imath::Math<double>::floor( b.max.y)));
    }
}

Imath::Box2i scale( const Imath::Box2i& b, float s) { return scale( b, s, s);}

Imath::Box2i scale( const Imath::Box2i& b, float sx, float sy)
{
    return Imath::Box2i( Imath::V2i( Imath::Math<float>::floor( b.min.x * sx), Imath::Math<float>::floor( b.min.y * sy)),
			 Imath::V2i( Imath::Math<float>::ceil(  b.max.x * sx), Imath::Math<float>::ceil(  b.max.y * sy)));
}

Imath::Box2i scale( const Imath::Box2i& b, float sx, float sy, int xcenter, int ycenter)
{
    Imath::Box2i box = offsetBy( b, Imath::V2i( -xcenter, -ycenter));
    box = scale( box, sx, sy);
    box = offsetBy( box, Imath::V2i( xcenter, ycenter));
    return box;
}

} // namespace
