// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/util/md5_hash_generator.hpp>

#include<iomanip>

namespace ramen
{

void md5_hash_generator_t::reset()
{
    digest_.reset();
    ss_.str( std::string());
}

std::string md5_hash_generator_t::str() const { return ss_.str();}

const md5_hash_generator_t::digest_type& md5_hash_generator_t::digest() const
{
    if( !digest_)
    {
	adobe::md5_t md5;
	md5.update( reinterpret_cast<void*>( const_cast<char*>( ss_.str().c_str())), ss_.str().size());
	digest_ = md5.final();
    }

    return digest_.get();
}

std::string md5_hash_generator_t::digest_as_string() const
{
    std::stringstream s;

    s << std::hex;

    for( int i = 0; i < sizeof( digest_type); ++i)
	s << digest()[i];

    return s.str();
}

//md5_hash_generator_t& operator<<( md5_hash_generator_t& hash_gen, const std::string& str)
//{
//    assert( !hash_gen.finalized());
//    hash_gen.sstream() << str;
//    return hash_gen;
//}

} // namespace
