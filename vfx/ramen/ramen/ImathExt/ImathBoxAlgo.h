// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMATHEXT_BOX_ALGO_HPP
#define RAMEN_IMATHEXT_BOX_ALGO_HPP

#include<algorithm>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMatrix.h>

namespace ramen
{

template<class T>
inline std::ostream& operator << ( std::ostream& os, const Imath::Box<T>& b)
{
    os << "[ " << b.min << "; " << b.max << "]";
    return os;
}

template<class T>
bool isInside( const Imath::Box<Imath::Vec2<T> >& bounds, const Imath::Box<Imath::Vec2<T> >& box)
{
    return bounds.intersects( box.min) && bounds.intersects( box.max);
}

template<class T>
Imath::Box<Imath::Vec2<T> > intersect( const Imath::Box<Imath::Vec2<T> >& a, const Imath::Box<Imath::Vec2<T> >& b)
{
    if( !( a.min.x > b.max.x || a.max.x < b.min.x || a.min.y > b.max.y || a.max.y < b.min.y))
    {
        return Imath::Box<Imath::Vec2<T> >( Imath::Vec2<T>( std::max( a.min.x, b.min.x), std::max( a.min.y, b.min.y)),
                                            Imath::Vec2<T>( std::min( a.max.x, b.max.x), std::min( a.max.y, b.max.y)));
    }
    else
        return Imath::Box<Imath::Vec2<T> >();
}

template<class T>
Imath::Box<T> offsetBy( const Imath::Box<T>& box, const T& d)
{
    Imath::Box<T> newbox( box);
    newbox.min += d;
    newbox.max += d;
    return newbox;
}

template <class T>
Imath::Box< Imath::Vec2<T> > transform( const Imath::Box< Imath::Vec2<T> >& box, const Imath::Matrix33<T>& m)
{
    if (box.isEmpty())
	return box;

    if (m[0][2] == 0 && m[1][2] == 0 && m[2][2] == 1)
    {
	Imath::Box< Imath::Vec2<T> > newBox;

	for (int i = 0; i < 2; i++)
        {
	    newBox.min[i] = newBox.max[i] = m[2][i];

	    for (int j = 0; j < 2; j++)
            {
		float a, b;

		a = m[j][i] * box.min[j];
		b = m[j][i] * box.max[j];

		if (a < b)
                {
		    newBox.min[i] += a;
		    newBox.max[i] += b;
		}
		else
                {
		    newBox.min[i] += b;
		    newBox.max[i] += a;
		}
	    }
	}

	return newBox;
    }

    Imath::Vec2<T> points[4];

    points[0].x = box.min.x;
    points[0].y = box.min.y;

    points[1].x = box.max.x;
    points[1].y = box.min.y;

    points[2].x = box.max.x;
    points[2].y = box.max.y;

    points[3].x = box.min.x;
    points[3].y = box.max.y;

    Imath::Box< Imath::Vec2<T> > newBox;

    for (int i = 0; i < 4; i++)
	newBox.extendBy (points[i] * m);

    return newBox;
}

Imath::Box2i transform( const Imath::Box2i& box, const Imath::M33f& m, bool round_up = false);
Imath::Box2i transform( const Imath::Box2i& box, const Imath::M33d& m, bool round_up = false);

Imath::Box2i scale( const Imath::Box2i& b, float s);
Imath::Box2i scale( const Imath::Box2i& b, float sx, float sy);
Imath::Box2i scale( const Imath::Box2i& b, float sx, float sy, int xcenter, int ycenter);

} // namespace

#endif
