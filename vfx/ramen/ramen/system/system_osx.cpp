// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/system/system.hpp>

#include<mach-o/dyld.h>

#include<sys/types.h>
#include<sys/sysctl.h>

#include<stdio.h>
#include<pwd.h>
#include<unistd.h>

#include<iostream>

#include<boost/scoped_array.hpp>
#include<boost/filesystem/operations.hpp>

namespace ramen
{
namespace system
{

system_impl::system_impl()
{
    struct passwd *p = getpwuid(geteuid());
    user_name_ = p->pw_name;
    home_path_ = p->pw_dir;

    // first get the size
    uint32_t size = 0;
    _NSGetExecutablePath( 0, &size);

    // then get the real path
    boost::scoped_array<char> tmp( new char[size]);
    _NSGetExecutablePath( tmp.get(), &size);
    executable_path_ = boost::filesystem::path( tmp.get());

    // cpu info

    #ifdef __BIG_ENDIAN__
	cpu_type_ = cpu_powerpc;
	simd_type_ = simd_altivec;
    #else
	cpu_type_ = cpu_i386;
	simd_type_ = simd_sse | simd_sse2; // SSE and SSE2 are required by OS X on intel

	// detect SSE3
	int hasSSE3 = 0;
	size_t length = sizeof( hasSSE3 );
	int error = sysctlbyname("hw.optional.sse3", &hasSSE3, &length, NULL, 0);

	if( error == 0 && hasSSE3)
	    simd_type_ |= simd_sse3;
    #endif

    // ram size
    int mib[2] = { CTL_HW, HW_MEMSIZE};
    uint64_t memsize;
    size_t len = sizeof( memsize);
    sysctl( mib, 2, &memsize, &len, NULL, 0);
    ram_size_ = memsize;
}

system_impl::~system_impl() {}

const std::string& system_impl::user_name() const			{ return user_name_;}
const boost::filesystem::path& system_impl::home_path() 		{ return home_path_;}
const boost::filesystem::path& system_impl::executable_path() 		{ return executable_path_;}

const boost::filesystem::path& system_impl::resource_path()
{
    if( resource_path_.empty())
	resource_path_ = executable_path_.branch_path() / boost::filesystem::path( "../Resources");

    return resource_path_;
}

const boost::filesystem::path& system_impl::preferences_path()
{
    if( preferences_path_.empty())
        preferences_path_ = home_path_ / boost::filesystem::path( "Library/Preferences/Ramen");

    if( !boost::filesystem::exists( preferences_path_))
	boost::filesystem::create_directory( preferences_path_);

    return preferences_path_;
}

std::string system_impl::system_name() const { return "Mac OS X";}

} // namespace
} // namespace
