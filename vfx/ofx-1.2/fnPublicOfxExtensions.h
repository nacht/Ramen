#ifndef _fnPublicOfxExtensions_h_
#define _fnPublicOfxExtensions_h_

/*
Software License :

Copyright (c) 2007, The Foundry Visionmongers Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Foundry Visionmongers Ltd, nor the names of its 
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
*/

#include "ofxImageEffect.h"

/** @brief Whether the initial state of a group is open or closed in a hierarchical layout. 

    - Type - int X 1
    - Property Set - plugin parameter descriptor (read/write) and instance (read only)
    - Default - 1
    - Valid Values - 0 or 1

This is a property on parameters of type ::kOfxParamTypeGroup, and tells the group whether it should be open or closed by default.

*/
#define kOfxParamPropGroupOpen "OfxParamPropGroupOpen"

/** @brief The suggested colour of an overlay colour in an interact.

    - Type - double X 3
    - Property Set - plugin parameter descriptor (read/write) and instance (read only)
    - Default - 1.0
    - Valid Values - greater than or equal to 0.0

This is a property of an overlay interact instance. 
*/
#define kOfxPropOverlayColour "OfxPropOverlayColour"

/** @brief Description of the plug-in to a user.

    - Type - string X 1
    - Property Set - plugin descriptor (read/write) and instance (read only)
    - Default - ""
    - Valid Values - UTF8 string

*/
#define kOfxPropPluginDescription "OfxPropPluginDescription"
 
#endif
