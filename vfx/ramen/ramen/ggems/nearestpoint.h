/*
Solving the Nearest Point-on-Curve Problem
and
A Bezier Curve-Based Root-Finder
by Philip J. Schneider
from "Graphics Gems", Academic Press, 1990
*/

#ifndef RAMEN_GGEMS_NEARESTPOINT_H
#define	RAMEN_GGEMS_NEARESTPOINT_H

#include<ramen/ggems/graphicsgems.h>

extern "C"
{

Point2 NearestPointOnCurve( Point2 P, Point2 *V, double *out_t);

}

#endif
