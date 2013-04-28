/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef TEST_STDAFX_H
#define TEST_STDAFX_H

#include <string>

#ifndef NO_ASL
#include <adobe/config.hpp>
#endif

#ifdef _MSC_VER
    #pragma inline_depth(255)

    #define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

    #ifndef WINVER  
    #define WINVER 0x0501
    #endif

    #pragma warning(disable: 4244)    // disable warning conversion from __w64 int to int, possible loss of data
#endif

/*
#include <afxwin.h>         // MFC core and standard components

// multibyte and ascii to wide
inline const std::wstring& mbToWide(const std::string& mb) {
    static std::wstring buffer;
    buffer.resize(mb.size());
    mbstowcs(const_cast<wchar_t*>(buffer.c_str()),mb.c_str(),mb.size());
    return buffer;
}

#ifdef UNICODE
    typedef std::wstring _string;
    typedef unsigned short _char;
    #define _strcmp wcscmp
    #define _stricmp _wcsicmp
    #define _fopen _wfopen
    inline const _string& mbToTEXT(const std::string& mb) { return mbToWide(mb); }
    inline const char* toASCII(const _string& str) {
        static char buffer[MAX_PATH];
        wcstombs(buffer, str.c_str(), MAX_PATH);
        return buffer;
    }
    inline const std::wstring& TEXTToWide(const _string& mb) { return text; }
#else
    typedef std::string _string;
    typedef char _char;
    #define _strcmp std::strcmp
    #define _stricmp stricmp
    #define _fopen fopen
    inline const _string& mbToTEXT(const std::string& mb) {return mb; }
    inline const char* toASCII(const _string& str) { return str.c_str(); }
    inline const std::wstring& TEXTToWide(const _string& mb) { return mbToWide(mb); }
#endif
*/

void test_pixel();
void test_image_tiler();
void test_pixel_iterator();
void test_image();
void test_gimage();
void test_image_io();
void performance_test();

//#include "any_image_impl.hpp"

#endif
