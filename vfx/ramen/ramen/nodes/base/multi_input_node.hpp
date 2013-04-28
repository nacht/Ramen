// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MULTI_INPUT_NODE_HPP
#define	RAMEN_MULTI_INPUT_NODE_HPP

#include<ramen/nodes/node.hpp>

namespace ramen
{

class multi_input_node_t : public node_t
{
public:

    multi_input_node_t();

protected:

    multi_input_node_t( const multi_input_node_t& other) : node_t( other) {}
    void operator=( const multi_input_node_t&);

    void reconnect_node();

private:

    friend class boost::serialization::access;

    void serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version);
    void serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const;
};

} // namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT( ramen::multi_input_node_t)

#endif
        