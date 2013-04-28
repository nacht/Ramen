// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UTIL_MD5_HASH_GENERATOR_HPP
#define	RAMEN_UTIL_MD5_HASH_GENERATOR_HPP

#include<sstream>

#include<boost/optional.hpp>

#include<adobe/md5.hpp>

namespace ramen
{

class md5_hash_generator_t
{
public:

    typedef adobe::md5_t::digest_t digest_type;

    md5_hash_generator_t() {}

    void reset();

    std::stringstream& sstream() { return ss_;}

    std::string str() const;

    bool finalized() const { return digest_;}

    const digest_type& digest() const;
    std::string digest_as_string() const;
    
private:

    //friend md5_hash_generator_t& operator<<( md5_hash_generator_t& hash_gen, const std::string& str);

    std::stringstream ss_;
    mutable boost::optional<digest_type> digest_;
};

template<class T>
md5_hash_generator_t& operator<<( md5_hash_generator_t& hash_gen, const T& x)
{
    assert( !hash_gen.finalized());
    hash_gen.sstream() << x;
    return hash_gen;
}


//md5_hash_generator_t& operator<<( md5_hash_generator_t& hash_gen, const std::string& str);

} // namespace

#endif

