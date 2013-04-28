// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/system/system.hpp>

#include<windows.h>
#include<intrin.h>
#include"Userenv.h"

#include<boost/filesystem/operations.hpp>

namespace ramen
{
namespace system
{

system_impl::system_impl()
{
    // TODO: test this

    // Get the user name
    char acUserName[100];
    DWORD nUserName = sizeof(acUserName);
    if( GetUserName( acUserName, &nUserName))
	user_name_ = acUserName;

    cpu_type_ = cpu_i386;

    // TODO: correctly detect this
    // for now, we assume on windows, we have a pentium 4 or better
    // update: 64 bits Windows requires SSE2...
    simd_type_ = simd_sse | simd_sse2;

   // Get the ram size
    MEMORYSTATUS theStatus;
    ZeroMemory(&theStatus,sizeof(theStatus));
    theStatus.dwLength = sizeof(theStatus);
    GlobalMemoryStatus(&theStatus);
    ram_size_ = theStatus.dwTotalPhys;
}

system_impl::~system_impl() {}

const std::string& system_impl::user_name() const { return user_name_;}

const boost::filesystem::path& system_impl::home_path()
{
    if( home_path_.empty())
    {
	TCHAR szHomeDirBuf[MAX_PATH] = { 0 };

	HANDLE hToken = 0;
	OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken );

	DWORD BufSize = MAX_PATH;
	GetUserProfileDirectory( hToken, szHomeDirBuf, &BufSize);
	CloseHandle( hToken );
	home_path_ = boost::filesystem::path( szHomeDirBuf, boost::filesystem::native);
    }

    return home_path_;
}

const boost::filesystem::path& system_impl::executable_path()
{
    // TODO: absolutely untested
    if( executable_path_.empty())
    {
	char path[MAX_PATH + 1];
	GetModuleFileName( NULL, path, MAX_PATH + 1 );
	executable_path_ = boost::filesystem::path( path);
    }

    return executable_path_;
}

const boost::filesystem::path& system_impl::resource_path()
{
    // TODO: implement this
    if( resource_path_.empty())
    {
	// TODO:
	// resource_path_ = path where application data files shared by all users are
    }

    return resource_path_;
}

const boost::filesystem::path& system_impl::preferences_path()
{
    if( preferences_path_.empty())
    {
	//preferences_path_ = boost::filesystem::path( "c:\\Documents and Settings\\", boost::filesystem::native);
	//preferences_path_ /= user_name_;
	//preferences_path_ /= boost::filesystem::path( "\\Local Settings\\Application Data\\Ramen\\", boost::filesystem::native);
        preferences_path_ = home_path() / "Ramen";

        if( !boost::filesystem::exists( preferences_path_))
            boost::filesystem::create_directory( preferences_path_);
    }

    return preferences_path_;
}

std::string system_impl::system_name() const { return "Windows";}

} // namespace
} // namespace
