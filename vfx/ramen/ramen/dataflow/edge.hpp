// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DATAFLOW_EDGE_HPP
#define RAMEN_DATAFLOW_EDGE_HPP

#include<boost/serialization/serialization.hpp>
#include<boost/serialization/nvp.hpp>

namespace ramen
{
namespace dataflow
{

template<class Node>
struct edge_t
{
    edge_t( Node *s=0, Node *d=0, int p=-1) : src(s), dst(d), port(p) {}

    bool operator==( const edge_t& other) const { return src == other.src && dst == other.dst && port == other.port;}
    bool operator!=( const edge_t& other) const { return !(src == other.src && dst == other.dst && port == other.port);}

    Node *src;
    Node *dst;
    int port;

private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int version)
    {
	ar & boost::serialization::make_nvp( "source", src);
	ar & boost::serialization::make_nvp( "dest", dst);
	ar & boost::serialization::make_nvp( "port", port);
    }
};

} // namespace
} // namespace

#endif
