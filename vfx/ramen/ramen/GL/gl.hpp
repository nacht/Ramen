// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_GL_HPP
#define	RAMEN_GL_HPP

#include<GL/glew.h>

#ifdef __APPLE__
    #include<OpenGL/gl.h>
#else
    #if defined(WIN32) || defined(WIN64)
        #include<windows.h>
    #endif

    #include<GL/gl.h>
#endif

#endif
