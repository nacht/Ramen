// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SYSTEM_HPP
#define RAMEN_SYSTEM_HPP

#include<string>

#include<boost/noncopyable.hpp>
#include<boost/filesystem/path.hpp>
#include<boost/cstdint.hpp>

#include<loki/Singleton.h>

namespace ramen
{
namespace system
{

enum cpu_family_type
{
    cpu_unknown = 0,
    cpu_i386,
    cpu_powerpc,
    cpu_arm
};

enum simd_instruction_set
{
    simd_none = 0,

    // intel
    simd_sse	= 1 << 0,
    simd_sse2	= 1 << 1,
    simd_sse3	= 1 << 2,

    // powerpc
    simd_altivec = 1 << 3
};

class system_impl : boost::noncopyable
{
public:

    const std::string& user_name() const;

    const boost::filesystem::path& home_path();
    const boost::filesystem::path& executable_path();
    const boost::filesystem::path& resource_path();
    const boost::filesystem::path& preferences_path();

    std::string system_name() const;

    // cpu
    cpu_family_type cpu_type() const	    { return cpu_type_;}
    int simd_type() const  { return simd_type_;}

    // ram
    boost::uint64_t ram_size() const { return ram_size_;}

private:

    system_impl();
    ~system_impl();

    friend struct Loki::CreateUsingNew<system_impl>;

    std::string user_name_;
    boost::filesystem::path home_path_;
    boost::filesystem::path executable_path_;
    boost::filesystem::path resource_path_;
    boost::filesystem::path preferences_path_;

    cpu_family_type cpu_type_;
    int simd_type_;

    boost::uint64_t ram_size_;
};

typedef Loki::SingletonHolder<system_impl> system_t;

} // namespace
} // namespace

#endif
