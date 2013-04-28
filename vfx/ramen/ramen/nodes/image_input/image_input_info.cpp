// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image_input/image_input_info.hpp>

#include<adobe/algorithm/clamp.hpp>

namespace ramen
{

image_input_info_t::image_input_info_t()
{
    slip_ = 0;
    lock_ = false;
    channels_ = tuple4i_t( 0, 1, 2, 3);
}

image_input_info_t::image_input_info_t( const path_sequence_t& seq, int slip, bool lock) : seq_( seq)
{
    slip_ = slip;
    lock_ = lock;
    channels_ = tuple4i_t( 0, 1, 2, 3);
}

image_input_info_t::image_input_info_t( const path_sequence_t& seq, const std::vector<std::string>& channel_list,
					    int slip, bool lock) : seq_( seq), channel_list_( channel_list)
{
    slip_ = slip;
    lock_ = lock;
    channels_ = tuple4i_t( 0, 1, 2, 3);
}

boost::filesystem::path image_input_info_t::operator()( int n) const
{
    int fnum;

    if( lock_)
	fnum = seq_.start() + slip();
    else
	fnum = n + slip();

    fnum = adobe::clamp( fnum, seq_.start(), seq_.end());
    return seq_( fnum);
}

bool image_input_info_t::operator==( const image_input_info_t& other) const
{
    return seq_ == other.seq_ && slip_ == other.slip_ && lock_ == other.lock_
	    && channels_ == other.channels_;
}

bool image_input_info_t::operator!=( const image_input_info_t& other) const
{
    return seq_ != other.seq_ || slip_ != other.slip_ || lock_ != other.lock_
	    || channels_ != other.channels_;
}

} // namespace
