// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_INPUT_INFO_HPP
#define	RAMEN_IMAGE_INPUT_INFO_HPP

#include<vector>

#include<boost/tuple/tuple.hpp>
#include<boost/tuple/tuple_io.hpp>
#include<boost/tuple/tuple_comparison.hpp>

#include<adobe/typeinfo.hpp>

#include<ramen/serialization/tuple.hpp>

#include<ramen/util/path_sequence.hpp>

namespace ramen
{

class image_input_info_t
{
public:

    typedef boost::tuple<int,int,int,int> tuple4i_t;

    image_input_info_t();
    image_input_info_t( const path_sequence_t& seq, int slip = 0, bool lock = false);
    image_input_info_t( const path_sequence_t& seq, const std::vector<std::string>& channel_list,
			int slip = 0, bool lock = false);

    bool valid() const { return seq_.valid();}

    boost::filesystem::path operator()( int n) const;

    const path_sequence_t& sequence() const		{ return seq_;}
    void set_path_sequence( const path_sequence_t& seq) { seq_ = seq;}

    int sequence_start() const	{ return seq_.start();}
    int sequence_end() const	{ return seq_.end();}
    std::string format_string() const { return seq_.format_string();}

    int slip() const	    { return slip_;}
    void set_slip( int s)   { slip_ = s;}

    bool locked() const		{ return lock_;}
    void set_locked( bool b)	{ lock_ = b;}

    const tuple4i_t& channels() const		    { return channels_;}
    void set_channels( const tuple4i_t& c)	    { channels_ = c;}
    void set_channels( int r, int g, int b, int a)  { channels_ = tuple4i_t( r, g, b, a);}
    void reset_channels()			    { channels_ = tuple4i_t( 0, 1, 2, 3);}

    const std::vector<std::string>& channel_list() const	{ return channel_list_;}
    void set_channel_list( const std::vector<std::string>& c)	{ channel_list_ = c;}
    void clear_channel_list()					{ channel_list_.clear();}

    bool operator==( const image_input_info_t& other) const;
    bool operator!=( const image_input_info_t& other) const;

private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version)
    {
	ar & boost::serialization::make_nvp( "sequence", seq_);
	ar & boost::serialization::make_nvp( "slip", slip_);
	ar & boost::serialization::make_nvp( "lock", lock_);
	ar & boost::serialization::make_nvp( "channel_list", channel_list_);
	ar & boost::serialization::make_nvp( "channels", channels_);
    }

    path_sequence_t seq_;
    int slip_;
    bool lock_;
    tuple4i_t channels_;
    std::vector<std::string> channel_list_;
};

} // namespace

// register adobe typeinfo
ADOBE_NAME_TYPE_0( "image_input_info_t:ramen", ramen::image_input_info_t);

#endif	/* _IMAGE_INPUT_INFO_HPP */
