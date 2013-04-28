// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/input/mandelbrot_node.hpp>

#include<ramen/nodes/node_factory.hpp>

namespace ramen
{

namespace
{

template <typename P>
struct mandelbrot_fn
{
    typedef boost::gil::point2<ptrdiff_t>   point_t;
    typedef mandelbrot_fn			const_t;
    typedef P				value_type;
    typedef value_type			reference;
    typedef value_type			const_reference;
    typedef point_t				argument_type;
    typedef reference			result_type;

    BOOST_STATIC_CONSTANT(bool, is_mutable=false);

    value_type       _in_color,_out_color;
    point_t          _img_size;
    static const int MAX_ITER = 35;

    mandelbrot_fn() {}
    mandelbrot_fn( const point_t& sz) : _img_size(sz) {}

    result_type operator()( const point_t& p) const
    {
	double t=get_num_iter(boost::gil::point2<double>(p.x/(double)_img_size.x*3-2, p.y/(double)_img_size.y*3-1.5f));
	t=pow(t,0.2);
	return value_type( t, t, t, 1.0f);
    }

private:

    double get_num_iter(const boost::gil::point2<double>& p) const
    {
	boost::gil::point2<double> Z(0,0);
		
	for( int i=0; i<MAX_ITER; ++i)
	{
	    Z = boost::gil::point2<double>(Z.x*Z.x - Z.y*Z.y + p.x, 2*Z.x*Z.y + p.y);

	    if (Z.x*Z.x + Z.y*Z.y > 4)
		return i/(double)MAX_ITER;
        }

	return 0;
    }
};

} // namespace

mandelbrot_node_t::mandelbrot_node_t() : generator_node_t() { set_name( "mandelbrot");}

void mandelbrot_node_t::do_create_params()
{
    generator_node_t::do_create_params();
}

void mandelbrot_node_t::do_process( const render::render_context_t& context)
{
    typedef mandelbrot_fn<boost::gil::rgba32f_pixel_t> deref_t;
    typedef deref_t::point_t            point_t;
    typedef boost::gil::virtual_2d_locator<deref_t,false> locator_t;
    typedef boost::gil::image_view<locator_t> my_virt_view_t;

    point_t dims( domain().size().x+1, domain().size().y+1);
    my_virt_view_t mandel( dims, locator_t(point_t(0,0), point_t(1,1), deref_t( dims)));

    render_virtual_view( mandel);
}

void mandelbrot_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
}

void mandelbrot_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
}

// factory
node_t *create_mandelbrot_node() { return new mandelbrot_node_t();}

node_info_t mandelbrot_node_info()
{
    node_info_t info;
    info.id = "builtin.mandelbrot";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Input";
    info.menu_item = "Mandelbrot";
    info.create = &create_mandelbrot_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( mandelbrot_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::mandelbrot_node_t)
