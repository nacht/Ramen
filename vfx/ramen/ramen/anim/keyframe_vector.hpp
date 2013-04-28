// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_KEYFRAME_VECTOR_HPP
#define RAMEN_ANIM_KEYFRAME_VECTOR_HPP

#include<vector>
#include<iostream>

#include<boost/optional.hpp>
#include<boost/serialization/vector.hpp>

namespace ramen
{
namespace anim
{

namespace
{

struct keyframe_less_t
{
    typedef bool result_type;

    template<class K>
    bool operator()( const K& a, const K& b) const  { return a.time() < b.time();}

    template<class K>
    bool operator()( const K& a, float b) const     { return a.time() < b;}

    // MSVC debug builds requires the next overloads
    template<class K>
    bool operator()( float a, const K& b) const     { return a < b.time();}

    bool operator()( float a, float b) const        { return a < b;}
};

} // namespace

template<class K>
class keyframe_vector_t
{
public:

    typedef K key_type;
    typedef typename K::value_type value_type;

    keyframe_vector_t() {}

    bool empty() const { return keys().empty();}
    std::size_t size() const { return keys().size();}

    void clear() { keys().clear();}

    std::vector<K>& keys()		{ return keys_;}
    const std::vector<K>& keys() const	{ return keys_;}

    K& operator[]( std::size_t i)
    {
	assert( i >= 0 && i < size());
	return keys()[i];
    }

    const K& operator[]( std::size_t i) const
    {
	assert( i >= 0 && i < size());
	return keys()[i];
    }

    typedef typename std::vector<K>::iterator       iterator;
    typedef typename std::vector<K>::const_iterator const_iterator;

    iterator begin()	{ return keys().begin();}
    iterator end()	{ return keys().end();}

    const_iterator begin() const	{ return keys().begin();}
    const_iterator end() const		{ return keys().end();}

    typedef typename std::vector<K>::reverse_iterator       reverse_iterator;
    typedef typename std::vector<K>::const_reverse_iterator const_reverse_iterator;

    reverse_iterator rbegin()	{ return keys().rbegin();}
    reverse_iterator rend()	{ return keys().rend();}

    const_reverse_iterator rbegin() const	{ return keys().rbegin();}
    const_reverse_iterator rend() const		{ return keys().rend();}

    const key_type& front() const	{ return keys().front();}
    key_type& front()			{ return keys().front();}

    const key_type& back() const	{ return keys().back();}
    key_type& back()			{ return keys().back();}

    iterator insert( const K& key)
    {
	if( empty())
	{
	    keys().push_back( key);
	    return begin();
	}

	if( key.time() < front().time())
	{
	    keys().insert( begin(), key);
	    return begin();
	}

	if( key.time() > back().time())
	{
	    keys().push_back( key);
	    return end() - 1;
	}

	iterator it( std::lower_bound( begin(), end(), key.time(), keyframe_less_t()));

	if( it->time() == key.time())
        {
	    *it = key;
            return it;
        }
	else
	    return keys().insert( it, key);
    }

    iterator erase( float time)
    {
	iterator it( std::lower_bound( begin(), end(), time, keyframe_less_t()));

	if( it != end())
        {
            if( time == it->time())
                return keys().erase( it);
        }
        
        return end();
    }

    iterator erase( iterator first, iterator last)
    {
        return keys().erase( first, last);
    }

    const_iterator lower_bound( float time) const
    {
	return std::lower_bound( begin(), end(), time, keyframe_less_t());
    }

    iterator lower_bound( float time)
    {
	return std::lower_bound( begin(), end(), time, keyframe_less_t());
    }

    const_iterator upper_bound( float time) const
    {
	return std::upper_bound( begin(), end(), time, keyframe_less_t());
    }

    iterator upper_bound( float time)
    {
	return std::upper_bound( begin(), end(), time, keyframe_less_t());
    }

private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version)
    {
	ar & boost::serialization::make_nvp( "keys", keys_);
    }

    std::vector<K> keys_;
};

} // namespace
} // namespace

#endif
