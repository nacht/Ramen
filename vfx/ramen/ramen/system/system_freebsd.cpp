// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/system/system.hpp>

#include<sys/types.h>
#include<sys/resource.h>
#include<sys/sysctl.h>
#include<sys/wait.h>

#include<stdio.h>
#include<pwd.h>
#include<unistd.h>

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

    // cpu info
    #if ( defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)))

	#define cpuid(func,ax,bx,cx,dx)\
	    __asm__ __volatile__ ("cpuid": "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));

	cpu_type_ = cpu_i386;
	simd_type_ = simd_none;

	int eax, ebx, ecx, edx;
	cpuid( 1, eax, ebx, ecx, edx);
	
	if( edx & 1 << 25)  simd_type_ |= simd_sse;
	if( edx & 1 << 26)  simd_type_ |= simd_sse2;
	if( ecx & 1)	    simd_type_ |= simd_sse3;
    #else
	cpu_type_ = cpu_unknown;
	simd_type_ = simd_none;
    #endif

    uint64_t totalmem;
    size_t sizelong = sizeof( uint64_t);
    sysctlbyname( "hw.physmem", &totalmem, &sizelong, NULL, 0); //total memory (in bytes)
    ram_size_ = totalmem;
}

system_impl::~system_impl() {}

const std::string& system_impl::user_name() const	{ return user_name_;}
const boost::filesystem::path& system_impl::home_path() { return home_path_;}

const boost::filesystem::path& system_impl::executable_path()
{
    if( executable_path_.empty())
    {
	char linkname[128];

	pid_t pid = getpid();

	if( snprintf( linkname, sizeof( linkname), "/proc/%i/exe", pid) < 0)
		assert( false);

	char buf[256];

	int ret = readlink( linkname, buf, sizeof( buf));

	if( ret == -1)
		assert( false);

	if( ret >= sizeof( buf))
		assert( false);

	buf[ret] = 0;
	executable_path_ = boost::filesystem::path( buf);
    }

    return executable_path_;
}

const boost::filesystem::path& system_impl::resource_path()
{
    if( resource_path_.empty())
	    resource_path_ = boost::filesystem::path( "/usr/local/share/ramen/");

    return resource_path_;
}

const boost::filesystem::path& system_impl::preferences_path()
{
    if( preferences_path_.empty())
	preferences_path_ = home_path_ / ".ramen/";

    if( !boost::filesystem::exists( preferences_path_))
	boost::filesystem::create_directory( preferences_path_);

    return preferences_path_;
}

std::string system_impl::system_name() const { return "FreeBSD";}

} // namespace
} // namespace
