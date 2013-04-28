// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/imf_memory_stream.hpp>

#include<stdio.h>
#include<string.h>
#include<errno.h>

#include<algorithm>

#include<OpenEXR/Iex.h>

#include<ramen/memory/image_memory_manager.hpp>

namespace ramen
{
namespace imageio
{

imf_memory_istream::imf_memory_istream( const char *data, std::size_t size) : Imf::IStream("")
{
    data_ = data;
    size_ = size;
    pos_ = 0;
}

imf_memory_istream::imf_memory_istream( const imf_memory_istream& other) : Imf::IStream("")
{
    data_ = other.data_;
    size_ = other.size_;
    pos_ = other.pos_;
}

Imf::Int64 imf_memory_istream::tellg()		{ return pos_;}
void imf_memory_istream::seekg( Imf::Int64 pos) { pos_ = pos;}

bool imf_memory_istream::read (char c[/*n*/], int n)
{
    if( pos_ + n < size_)
    {
	memcpy( (void *) c, (void *) &(data_[pos_]), n);
	pos_ += n;
	return true;
    }

    return false;
}

// ostream

imf_memory_ostream::imf_memory_ostream( std::size_t mem_size) : Imf::OStream(""), buffer_( 0)
{
    assert( mem_size != 0 && "imf_memory_ostream: zero mem_size");
    mem_size_ = mem_size;
    pos_ = 0;

    try
    {
        buffer_ = reinterpret_cast<char *>( image_memory_manager_t::Instance().allocate( mem_size));
    }
    catch( std::bad_alloc& e)
    {
	errno = ENOMEM;
	Iex::throwErrnoExc();
    }
}

imf_memory_ostream::~imf_memory_ostream()
{
    if( buffer_)
        image_memory_manager_t::Instance().deallocate( reinterpret_cast<unsigned char *>( buffer_), mem_size_);
}

Imf::Int64 imf_memory_ostream::tellp() { return pos_;}

void imf_memory_ostream::seekp( Imf::Int64 pos) { pos_ = pos;}

void imf_memory_ostream::write (const char c[], int n)
{
    assert( pos_ + n < mem_size_);
    memcpy( (void *) &(buffer_[pos_]), (void *) c, n);
    pos_ += n;
}

// null ostream
imf_null_ostream::imf_null_ostream() : Imf::OStream(""), size_(0), pos_(0) {}

Imf::Int64 imf_null_ostream::tellp() { return pos_;}

void imf_null_ostream::seekp( Imf::Int64 pos) { pos_ = pos;}

void imf_null_ostream::write (const char c[], int n)
{
    size_ = std::max( size_, pos_ + n);
    pos_ += n;
}

} // namespace
} // namespace
