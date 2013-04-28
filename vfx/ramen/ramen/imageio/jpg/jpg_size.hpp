/*
  Most of the code that follows was adapted from IJG's rdjpgcom.c file.

 * Copyright (C) 1994-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 *
 * This file contains a very simple stand-alone application that displays
 * the text in COM (comment) markers in a JFIF file.
 * This may be useful as an example of the minimum logic needed to parse
 * JPEG markers.
 */

#ifndef BACKEND_TOOLS_JPEG_JPEGSIZE_H
#define BACKEND_TOOLS_JPEG_JPEGSIZE_H

//returns size of a jpeg image
bool getJPEGSize( const char* filename, int& width, int& height );

#endif //BACKEND_TOOLS_JPEG_JPEGTOOLS_H
