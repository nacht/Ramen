// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ImathExt/ImathMatrixAlgo.h>

namespace ramen
{

const float rwgt = 0.212671f;
const float gwgt = 0.715160f;
const float bwgt = 0.072169f;

namespace
{

// Copied from Paul Haeberli's code...

/*
 *	matrixmult -
 *		multiply two matricies
 */
void matrixmult( float a[4][4], float b[4][4], float c[4][4])
{
    int x, y;
    float temp[4][4];

    for(y=0; y<4 ; y++)
        for(x=0 ; x<4 ; x++) {
            temp[y][x] = b[y][0] * a[0][x]
                       + b[y][1] * a[1][x]
                       + b[y][2] * a[2][x]
                       + b[y][3] * a[3][x];
        }
    for(y=0; y<4; y++)
        for(x=0; x<4; x++)
            c[y][x] = temp[y][x];
}

/*
 *	identmat -
 *		make an identity matrix
 */
void identmat( float *matrix)
{
    *matrix++ = 1.0;    /* row 1        */
    *matrix++ = 0.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;    /* row 2        */
    *matrix++ = 1.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;    /* row 3        */
    *matrix++ = 0.0;
    *matrix++ = 1.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;    /* row 4        */
    *matrix++ = 0.0;
    *matrix++ = 0.0;
    *matrix++ = 1.0;
}

/*
 *	xformpnt -
 *		transform a 3D point using a matrix
 */
void xformpnt( float matrix[4][4], float x, float y, float z, float *tx, float *ty, float *tz)
{
    *tx = x*matrix[0][0] + y*matrix[1][0] + z*matrix[2][0] + matrix[3][0];
    *ty = x*matrix[0][1] + y*matrix[1][1] + z*matrix[2][1] + matrix[3][1];
    *tz = x*matrix[0][2] + y*matrix[1][2] + z*matrix[2][2] + matrix[3][2];
}

/*
 *	xrotate -
 *		rotate about the x (red) axis
 */
void xrotatemat( float mat[4][4], float rs, float rc)
{
    float mmat[4][4];
    mmat[0][0] = 1.0;
    mmat[0][1] = 0.0;
    mmat[0][2] = 0.0;
    mmat[0][3] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = rc;
    mmat[1][2] = rs;
    mmat[1][3] = 0.0;

    mmat[2][0] = 0.0;
    mmat[2][1] = -rs;
    mmat[2][2] = rc;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat,mat,mat);
}

/*
 *	yrotate -
 *		rotate about the y (green) axis
 */
void yrotatemat( float mat[4][4], float rs, float rc)
{
    float mmat[4][4];
    mmat[0][0] = rc;
    mmat[0][1] = 0.0;
    mmat[0][2] = -rs;
    mmat[0][3] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = 1.0;
    mmat[1][2] = 0.0;
    mmat[1][3] = 0.0;

    mmat[2][0] = rs;
    mmat[2][1] = 0.0;
    mmat[2][2] = rc;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat,mat,mat);
}

/*
 *	zrotate -
 *		rotate about the z (blue) axis
 */
void zrotatemat( float mat[4][4], float rs,float rc)
{
    float mmat[4][4];
    mmat[0][0] = rc;
    mmat[0][1] = rs;
    mmat[0][2] = 0.0;
    mmat[0][3] = 0.0;

    mmat[1][0] = -rs;
    mmat[1][1] = rc;
    mmat[1][2] = 0.0;
    mmat[1][3] = 0.0;

    mmat[2][0] = 0.0;
    mmat[2][1] = 0.0;
    mmat[2][2] = 1.0;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat,mat,mat);
}

/*
 *	zshear -
 *		shear z using x and y.
 */
void zshearmat( float mat[4][4], float dx, float dy)
{
    float mmat[4][4];
    mmat[0][0] = 1.0;
    mmat[0][1] = 0.0;
    mmat[0][2] = dx;
    mmat[0][3] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = 1.0;
    mmat[1][2] = dy;
    mmat[1][3] = 0.0;

    mmat[2][0] = 0.0;
    mmat[2][1] = 0.0;
    mmat[2][2] = 1.0;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat,mat,mat);
}


/*
 *	huerotatemat -
 *		rotate the hue, while maintaining luminance.
 */
void huerotatemat( float mat[4][4], float rot)
{
    const float RLUM = 0.3086f;
    const float GLUM = 0.6094f;
    const float BLUM = 0.0820f;

    float mmat[4][4];
    float mag;
    float lx, ly, lz;
    float xrs, xrc;
    float yrs, yrc;
    float zrs, zrc;
    float zsx, zsy;

    identmat( (float *) mmat);

/* rotate the grey vector into positive Z */
    mag = sqrt(2.0);
    xrs = 1.0/mag;
    xrc = 1.0/mag;
    xrotatemat(mmat,xrs,xrc);
    mag = sqrt(3.0);
    yrs = -1.0/mag;
    yrc = sqrt(2.0)/mag;
    yrotatemat(mmat,yrs,yrc);

/* shear the space to make the luminance plane horizontal */
    xformpnt(mmat,RLUM,GLUM,BLUM,&lx,&ly,&lz);
    zsx = lx/lz;
    zsy = ly/lz;
    zshearmat(mmat,zsx,zsy);

/* rotate the hue */
    zrs = sin(rot*M_PI/180.0);
    zrc = cos(rot*M_PI/180.0);
    zrotatemat(mmat,zrs,zrc);

/* unshear the space to put the luminance plane back */
    zshearmat(mmat,-zsx,-zsy);

/* rotate the grey vector back into place */
    yrotatemat(mmat,-yrs,yrc);
    xrotatemat(mmat,-xrs,xrc);

    matrixmult(mmat,mat,mat);
}

} // detail

Imath::M44f hueRotationMatrix( float angle)
{
    float m[4][4];
    identmat( (float *) m);
    huerotatemat( m, angle);
    return Imath::M44f( m);
}

Imath::M44f saturationMatrix( float sat)
{
    Imath::M44f m;
    m[0][0] = (1.0f-sat)*rwgt + sat;
    m[0][1] = (1.0f-sat)*rwgt;
    m[0][2] = (1.0f-sat)*rwgt;
    m[0][3] = 0.0f;

    m[1][0] = (1.0f-sat)*gwgt;
    m[1][1] = (1.0f-sat)*gwgt + sat;
    m[1][2] = (1.0f-sat)*gwgt;
    m[1][3] = 0.0f;

    m[2][0] = (1.0f-sat)*bwgt;
    m[2][1] = (1.0f-sat)*bwgt;
    m[2][2] = (1.0f-sat)*bwgt + sat;
    m[2][3] = 0.0f;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
    return m;
}

Imath::M44f gainMatrix( const Imath::V3f& g)
{
    return Imath::M44f( g.x,   0, 0  , 0,
                        0  , g.y, 0  , 0,
                        0  , 0  , g.z, 0,
                        0  , 0  , 0  , 1);
}

Imath::M44f gainMatrix( float g) { return gainMatrix( Imath::V3f( g, g, g));}

} // namespace

