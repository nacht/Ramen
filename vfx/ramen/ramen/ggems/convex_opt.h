/*
 * C code from the article
 * "Testing the Convexity of a Polygon"
 * by Peter Schorn and Frederick Fisher,
 *	(schorn@inf.ethz.ch, fred@kpc.com)
 * in "Graphics Gems IV", Academic Press, 1994
 */

#ifndef RAMEN_GGEMS_CONVEX_OPT_H
#define RAMEN_GGEMS_CONVEX_OPT_H
    
typedef enum { NotConvex, NotConvexDegenerate, ConvexDegenerate, ConvexCCW, ConvexCW } PolygonClass;

PolygonClass classifyPolygon2( int nvert, float pVert[][2]);

#endif
