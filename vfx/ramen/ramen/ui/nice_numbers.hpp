// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_NICE_NUMBERS_HPP
#define	RAMEN_UI_NICE_NUMBERS_HPP

namespace ramen
{
namespace ui
{

class nice_numbers_t
{
public:

    nice_numbers_t( double imin, double imax, int num_ticks);

    struct iterator
    {
	iterator( double x, double imax, double d)
	{
	    x_ = x;
	    imax_ = imax;
	    d_ = d;
	}

	iterator( double imax, double d)
	{
	    x_ = imax;
	    imax_ = imax;
	    d_ = d;
	}

	iterator& operator++()
	{
	    x_ += d_;
	    return *this;
	}

	iterator operator++( int)
	{
	    iterator tmp( *this);
	    x_ += d_;
	    return tmp;
	}

	double operator*() const { return x_;}

	bool operator==( const iterator& other) const
	{
	    return x_ == other.x_;
	}

	bool operator!=( const iterator& other) const
	{
	    return x_ != other.x_;
	}

	bool operator<( const iterator& other) const
	{
	    return x_ < other.x_;
	}

	double x_;
	double imax_;
	double d_;
    };

    iterator begin() const
    {
	return iterator( graphmin_, graphmax_ + 0.5 * d_, d_);
    }

    iterator end() const
    {
	return iterator( graphmax_ + 0.5 * d_, d_);
    }

private:

    double nice_num( double x, bool round) const;

    int nticks_;
    double range_;
    double graphmin_, graphmax_;
    double d_;
    double nfrac_;
};

} // namespace
} // namespace

#endif	/* _NICE_NUMBERS_HPP */
