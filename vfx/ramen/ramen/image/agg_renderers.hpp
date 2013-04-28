
#ifndef RAMEN_AGG_RENDERERS_HPP
#define RAMEN_AGG_RENDERERS_HPP

#include<algorithm>

#include<agg/agg_basics.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{

template<class Rgba32fView>
class agg_rgba32f_renderer_t
{
public:

    // TODO: add a static assert to check Rgba32fView template param

    typedef Rgba32fView                     view_type;
    typedef typename view_type::pixel_t     color_type;

    agg_rgba32f_renderer_t() {}
    explicit agg_rgba32f_renderer_t( const view_type& view) : view_( view) {}

    void set_view( const view_type& view) { view_ = view;}

    unsigned width() const  { return view_.width();}
    unsigned height() const { return view_.height();}

    int xmin() const { return 0;}
    int ymin() const { return 0;}
    int xmax() const { return view_.width() - 1;}
    int ymax() const { return view_.height() - 1;}

    void clear( const color_type& c) { boost::gil::fill_pixels( view_, c);}

    void blend_hline( int x1, int y, int x2, const color_type& c, agg::cover_type cover)
    {
        using namespace boost::gil;

        if(x1 > x2)
        {
            int t = x2;
            x2 = x1;
            x1 = t;
        }

        if( y  > ymax() || y < ymin() || x1 > xmax() || x2 < xmin())
            return;

        x1 = std::max( x1, xmin());
        x2 = std::min( x2, xmax());

        typename view_type::x_iterator p = view_.row_begin(y) + x1;
        float alpha = cover / 255.0f * get_color( c, alpha_t());
        float inv_alpha = 1.0f - alpha;

        int len = x2 - x1 + 1;

        do
        {
            get_color( *p, red_t())    = ( get_color( *p, red_t())   * inv_alpha) + ( get_color( c, red_t())   * alpha);
            get_color( *p, green_t())  = ( get_color( *p, green_t()) * inv_alpha) + ( get_color( c, green_t()) * alpha);
            get_color( *p, blue_t())   = ( get_color( *p, blue_t())  * inv_alpha) + ( get_color( c, blue_t())  * alpha);
            get_color( *p, alpha_t()) += get_color( c, alpha_t()) * alpha;
            ++p;
        }
        while (--len);
    }

    void blend_solid_hspan( int x, int y, int len, const color_type& c, const agg::cover_type* covers)
    {
        using namespace boost::gil;

        if( y > ymax() || y < ymin())
            return;

        if( x < xmin())
        {
            len -= xmin() - x;

            if(len <= 0)
                return;

            covers += xmin() - x;
            x = xmin();
        }

        if(x + len > xmax())
        {
            len = xmax() - x + 1;

            if(len <= 0)
                return;
        }

        typename view_type::x_iterator p = view_.row_begin(y) + x;

        do
        {
            float alpha = *covers / 255.0f * get_color( c, alpha_t());
            float inv_alpha = 1.0f - alpha;
            get_color( *p, red_t())    = ( get_color( *p, red_t())   * inv_alpha) + ( get_color( c, red_t())   * alpha);
            get_color( *p, green_t())  = ( get_color( *p, green_t()) * inv_alpha) + ( get_color( c, green_t()) * alpha);
            get_color( *p, blue_t())   = ( get_color( *p, blue_t())  * inv_alpha) + ( get_color( c, blue_t())  * alpha);
            get_color( *p, alpha_t()) += get_color( c, alpha_t()) * alpha;
            ++p;
            ++covers;
        }
        while( --len);
    }

private:

    view_type view_;
};

template<class Gray32fView>
class agg_gray32f_renderer_t
{
public:

    // TODO: add a static assert to check Gray32fView template param
    typedef Gray32fView view_type;

    struct color_type
    {
        color_type() {}
        color_type( float lum, float alpha = 1.0f) : l( lum), a( alpha) {}
        
        float l, a;
    };

    agg_gray32f_renderer_t() {}
    explicit agg_gray32f_renderer_t( const view_type& view) : view_( view) {}

    void set_view( const view_type& view) { view_ = view;}

    unsigned width() const  { return view_.width();}
    unsigned height() const { return view_.height();}

    int xmin() const { return 0;}
    int ymin() const { return 0;}
    int xmax() const { return view_.width() - 1;}
    int ymax() const { return view_.height() - 1;}

    void clear( const color_type& c) { boost::gil::fill_pixels( view_, c.l);}

    void blend_hline( int x1, int y, int x2, const color_type& c, agg::cover_type cover)
    {
        if(x1 > x2)
        {
            int t = x2;
            x2 = x1;
            x1 = t;
        }

        if( ( y  > ymax()) || ( y < ymin()) || ( x1 > xmax()) || (x2 < xmin()))
            return;

        x1 = std::max( x1, xmin());
        x2 = std::min( x2, xmax());

        typename view_type::x_iterator p = view_.row_begin(y) + x1;
        float alpha = cover / 255.0f * c.a;
        float inv_alpha = 1.0f - alpha;

        int len = x2 - x1 + 1;

        do
        {
            (*p)[0] = ( (*p)[0] * inv_alpha) + ( c.l * alpha);
            ++p;
        }
        while (--len);
    }

    void blend_solid_hspan( int x, int y, int len, const color_type& c, const agg::cover_type* covers)
    {
        if( y > ymax() || y < ymin())
            return;

        if( x < xmin())
        {
            len -= xmin() - x;

            if(len <= 0)
                    return;

            covers += xmin() - x;
            x = xmin();
        }

        if(x + len > xmax())
        {
            len = xmax() - x + 1;

            if(len <= 0)
                return;
        }

        typename view_type::x_iterator p = view_.row_begin(y) + x;

        do
        {
            float alpha = *covers / 255.0f * c.a;
            float inv_alpha = 1.0f - alpha;
            (*p)[0] = ( (*p)[0] * inv_alpha) + ( c.l * alpha);
            ++p;
            ++covers;
        }
        while( --len);
    }

private:

    view_type view_;
};

} // namespace

#endif
