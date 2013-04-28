// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PATH_SEQUENCE_HPP
#define	RAMEN_PATH_SEQUENCE_HPP

#include<boost/tuple/tuple.hpp>

#include<ramen/serialization/path.hpp>

namespace ramen
{

class path_sequence_t
{
public:

    path_sequence_t();
    path_sequence_t( const boost::filesystem::path& p, bool sequence = true);

    bool valid() const { return !( !is_sequence() && base_.empty());}

    boost::filesystem::path operator()( int n) const;

    std::string format_string() const;

    bool add_path( const boost::filesystem::path& p);

    bool is_sequence() const { return is_seq_;}

    int start() const   { return start_;}
    int end() const     { return end_;}

    bool operator==( const path_sequence_t& other) const;
    bool operator!=( const path_sequence_t& other) const;

private:

    std::string get_extension( const std::string& s) const;
    std::string get_frame_number( const std::string& s) const;
    int get_pad( const std::string& str) const;
    boost::tuple<std::string, std::string, std::string> decompose_filename( const std::string& filename) const;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::make_nvp( "dir", dir_);
        ar & boost::serialization::make_nvp( "base", base_);
        ar & boost::serialization::make_nvp( "start", start_);
        ar & boost::serialization::make_nvp( "end", end_);
        ar & boost::serialization::make_nvp( "pad", pad_);
        ar & boost::serialization::make_nvp( "ext", ext_);
        ar & boost::serialization::make_nvp( "is_seq", is_seq_);
    }

    friend std::ostream& operator << ( std::ostream& os, const path_sequence_t& p);

    boost::filesystem::path dir_;
    std::string base_;
    int start_, end_;
    int pad_;
    std::string ext_;
    bool is_seq_;
};

std::ostream& operator << ( std::ostream& os, const path_sequence_t& p);

} // namespace

#endif	/* _PATH_SEQUENCE_H */
