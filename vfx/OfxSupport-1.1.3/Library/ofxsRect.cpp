/*
OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
Copyright (C) 2004-2005 The Open Effects Association Ltd
Author Bruno Nicoletti bruno@thefoundry.co.uk

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
* Neither the name The Open Effects Association Ltd, nor the names of its 
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The Open Effects Association Ltd
1 Wardour St
London W1D 6PA
England
*/

/*
 * This file is not part of the original OfxSupport library.
 * Added by Esteban Tovagliari.
 */

#include "ofxsRect.h"

#include<limits>
#include<algorithm>

namespace OFX
{

void makeEmpty( OfxRectI& r)
{
    r.x1 = std::numeric_limits<int>::max();
    r.y1 = std::numeric_limits<int>::max();
    r.x2 = - r.x1;
    r.y2 = - r.y1;
}

void makeEmpty( OfxRectD& r)
{
    r.x1 = std::numeric_limits<double>::max();
    r.y1 = std::numeric_limits<double>::max();
    r.x2 = - r.x1;
    r.y2 = - r.y1;
}

bool isEmpty( const OfxRectI& r) { return ( r.x1 >= r.x2) || ( r.y1 >= r.y2);}
bool isEmpty( const OfxRectD& r) { return ( r.x1 >= r.x2) || ( r.y1 >= r.y2);}

OfxRectD intersect( const OfxRectD& a, const OfxRectD& b)
{
    OfxRectD result;
    makeEmpty( result);

    if( !( a.x1 > b.x2 || a.x2 < b.x1 || a.y1 > b.y2 || a.y2 < b.y1))
    {
        result.x1 = std::max( a.x1, b.x1);
        result.y1 = std::max( a.y1, b.y1);
        result.x2 = std::min( a.x2, b.x2);
        result.y2 = std::min( a.y2, b.y2);
    }

    return result;
}

OfxRectI intersect( const OfxRectI& a, const OfxRectI& b)
{
    OfxRectI result;
    makeEmpty( result);

    if( !( a.x1 > b.x2 || a.x2 < b.x1 || a.y1 > b.y2 || a.y2 < b.y1))
    {
        result.x1 = std::max( a.x1, b.x1);
        result.y1 = std::max( a.y1, b.y1);
        result.x2 = std::min( a.x2, b.x2);
        result.y2 = std::min( a.y2, b.y2);
    }

    return result;
}

OfxRectD join( const OfxRectD& a, const OfxRectD& b)
{
    OfxRectD result;
    result.x1 = std::min( a.x1, b.x1);
    result.y1 = std::min( a.y1, b.y1);
    result.x2 = std::max( a.x2, b.x2);
    result.y2 = std::max( a.y2, b.y2);
    return result;
}

OfxRectI join( const OfxRectI& a, const OfxRectI& b)
{
    OfxRectI result;
    result.x1 = std::min( a.x1, b.x1);
    result.y1 = std::min( a.y1, b.y1);
    result.x2 = std::max( a.x2, b.x2);
    result.y2 = std::max( a.y2, b.y2);
    return result;
}

} // namespace OFX
