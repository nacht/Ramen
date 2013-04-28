// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMF_MEMORY_STREAM_HPP
#define RAMEN_IMF_MEMORY_STREAM_HPP

#include<OpenEXR/ImfIO.h>

namespace ramen
{
namespace imageio
{

class imf_memory_istream : public Imf::IStream
{
public:

    imf_memory_istream( const char *data, std::size_t size);
    imf_memory_istream( const imf_memory_istream& other);

    virtual bool isMemoryMapped() const { return false;}
    virtual bool read( char c[/*n*/], int n);

    virtual Imf::Int64 tellg();
    virtual void seekg( Imf::Int64 pos);

private:

    const char *data_;
    std::size_t size_;
    Imf::Int64 pos_;
};

class imf_memory_ostream : public Imf::OStream
{
public:

    imf_memory_ostream( std::size_t mem_size);
    ~imf_memory_ostream();

    virtual void write (const char c[], int n);

    virtual Imf::Int64 tellp();
    virtual void seekp( Imf::Int64 pos);

    std::size_t size() const { return mem_size_;}

    char *data() const { return buffer_;}

private:

    std::size_t mem_size_;
    char *buffer_;
    Imf::Int64 pos_;
};

// null ostream
class imf_null_ostream : public Imf::OStream
{
public:

    imf_null_ostream();

    virtual void write (const char c[], int n);

    virtual Imf::Int64 tellp();
    virtual void seekp( Imf::Int64 pos);

    std::size_t size() const { return ( std::size_t) size_ + 1;}

private:

    Imf::Int64 size_;
    Imf::Int64 pos_;
};

} // namespace
} // namespace

#endif
