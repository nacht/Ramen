// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/transform/corner_pin_node.hpp>

#include<cmath>

#include<OpenEXR/ImathMath.h>

#include<QMouseEvent>

#include<ramen/GL/gl.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>
#include<ramen/ImathExt/ImathMatrixAlgo.h>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/motion_blur_param.hpp>
#include<ramen/params/separator_param.hpp>
#include<ramen/params/button_param.hpp>

#include<ramen/image/processing.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/image_view.hpp>

extern "C"
{

#include<ramen/ggems/convex_opt.h>

}

namespace ramen
{

class corner_pin_manipulator_t : public manipulator_t
{
public:

    corner_pin_manipulator_t() : manipulator_t() {}

    const corner_pin_node_t *node() const
    {
        const corner_pin_node_t *n = dynamic_cast<const corner_pin_node_t*>( parent());
        assert( n && "Null corner pin node in manipulator");
        return n;
    }

    corner_pin_node_t *node()
    {
        corner_pin_node_t *n = dynamic_cast<corner_pin_node_t*>( parent());
        assert( n && "Null corner pin node in manipulator");
        return n;
    }

    virtual void draw_overlay( ui::image_view_t& view) const
    {
        boost::array<Imath::V2d, 4> dst_pts;
        node()->get_corners( dst_pts, view.subsample());

        glPointSize( 5);

        glColor3f( 1, 1, 0);
        glBegin( GL_LINE_LOOP);
            for( int i = 0; i < 4; ++i)
                glVertex2f( dst_pts[i].x, dst_pts[i].y);
        glEnd();

        glColor3f( 1, 0, 0);
        glBegin( GL_POINTS);
            for( int i = 0; i < 4; ++i)
                glVertex2f( dst_pts[i].x, dst_pts[i].y);
        glEnd();
    }

    virtual bool mouse_press_event( ui::image_view_t& view, QMouseEvent *event)
    {
        picked_corner_ = -1;

        boost::array<Imath::V2d, 4> dst_pts;
        node()->get_corners( dst_pts, view.subsample());

        for( int i = 0; i < 4; ++i)
        {
            Imath::V2i q = view.world_to_screen( dst_pts[i]);

            if( ( view.screen_current_mouse_pos - q).length2() < 25)
            {
                picked_corner_ = i;
                break;
            }
        }

        return picked_corner_ != -1;
    }

    virtual void mouse_drag_event( ui::image_view_t& view, QMouseEvent *event)
    {
        if( picked_corner_ == -1)
            return;

        if( view.first_drag)
            node()->param_set().begin_edit();

        Imath::V2f offset( view.screen_to_world_dir( Imath::V2f( view.screen_current_mouse_pos.x - view.screen_last_mouse_pos.x,
                                                                 view.screen_current_mouse_pos.y - view.screen_last_mouse_pos.y)));
        offset /= view.subsample();
        float2_param_t *p = 0;
        Imath::V2f q;

        switch( picked_corner_)
        {
        case 0: // topleft
            p = dynamic_cast<float2_param_t*>( &node()->param( "topleft"));
            q = get_absolute_value<Imath::V2f>( *p);
            q.x += offset.x;
            q.y += offset.y;
            q = p->absolute_to_relative( q);
            p->set_value(q);
            p->update_widgets();
        break;

        case 1: // topright
            p = dynamic_cast<float2_param_t*>( &node()->param( "topright"));
            q = get_absolute_value<Imath::V2f>( *p);
            q.x += offset.x;
            q.y += offset.y;
            q = p->absolute_to_relative( q);
            p->set_value(q);
            p->update_widgets();
        break;

        case 2: // botright
            p = dynamic_cast<float2_param_t*>( &node()->param( "botright"));
            q = get_absolute_value<Imath::V2f>( *p);
            q.x += offset.x;
            q.y += offset.y;
            q = p->absolute_to_relative( q);
            p->set_value(q);
            p->update_widgets();
        break;

        case 3: // botleft
            p = dynamic_cast<float2_param_t*>( &node()->param( "botleft"));
            q = get_absolute_value<Imath::V2f>( *p);
            q.x += offset.x;
            q.y += offset.y;
            q = p->absolute_to_relative( q);
            p->set_value(q);
            p->update_widgets();
        break;
        }

        ui::anim_editor_t::Instance().update();
        node()->notify();
        node()->notify_dependents();
        view.update();
    }

    virtual void mouse_release_event( ui::image_view_t& view, QMouseEvent *event)
    {
        if( view.dragged)
        {
            node()->param_set().end_edit( false);
            ui::user_interface_t::Instance().update();
        }
    }

private:

    int picked_corner_;
};

corner_pin_node_t::corner_pin_node_t() : image_xform_node_t() { set_name("corner_pin");}

void corner_pin_node_t::do_create_params()
{
    std::auto_ptr<float2_param_t> p( new float2_param_t( "Top Left"));
    p->set_id( "topleft");
    p->set_default_value( Imath::V2f( 0.0, 0.0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    p.reset( new float2_param_t( "Top Right"));
    p->set_id( "topright");
    p->set_default_value( Imath::V2f( 1.0, 0.0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    p.reset( new float2_param_t( "Bot Left"));
    p->set_id( "botleft");
    p->set_default_value( Imath::V2f( 0.0, 1.0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    p.reset( new float2_param_t( "Bot Right"));
    p->set_id( "botright");
    p->set_default_value( Imath::V2f( 1.0, 1.0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    std::auto_ptr<popup_param_t> r( new popup_param_t( "Filter"));
    r->set_id( "filter");
    r->menu_items() += "Point", "Bilinear", "Bicubic";
    r->set_default_value( 2);
    add_param( r);

    std::auto_ptr<button_param_t> b( new button_param_t( "Apply Track..."));
    b->set_id( "apply_track");
    b->set_enabled( false);
    add_param( b);

    std::auto_ptr<separator_param_t> sep( new separator_param_t());
    add_param( sep);

    std::auto_ptr<motion_blur_param_t> mb( new motion_blur_param_t( "Motion Blur"));
    mb->set_id( "motion_blur");
    add_param( mb);
}

void corner_pin_node_t::do_create_manipulators()
{
    std::auto_ptr<corner_pin_manipulator_t> m( new corner_pin_manipulator_t());
    add_manipulator( m);
}

boost::optional<Imath::M33d> corner_pin_node_t::do_calc_transform_matrix_at_time( float time, int subsample) const
{
    Imath::Box2i src_area( input()->domain());

    boost::array<Imath::V2d, 4> src_pts;
    src_pts[0].x = src_area.min.x;
    src_pts[0].y = src_area.min.y;
    src_pts[1].x = src_area.max.x;
    src_pts[1].y = src_area.min.y;
    src_pts[2].x = src_area.max.x;
    src_pts[2].y = src_area.max.y;
    src_pts[3].x = src_area.min.x;
    src_pts[3].y = src_area.max.y;

    boost::array<Imath::V2d, 4> dst_pts;
    get_corners_at_time( dst_pts, time);

    boost::optional<Imath::M33d> m;

    if( quad_is_convex( dst_pts))
        m = quadToQuadMatrix( src_pts, dst_pts);

    return m;
}

void corner_pin_node_t::get_corners( boost::array<Imath::V2d, 4>& pts, float scale) const
{
    pts[0] = get_absolute_value<Imath::V2f>( param( "topleft"))  * scale;
    pts[1] = get_absolute_value<Imath::V2f>( param( "topright")) * scale;
    pts[2] = get_absolute_value<Imath::V2f>( param( "botright")) * scale;
    pts[3] = get_absolute_value<Imath::V2f>( param( "botleft"))  * scale;
}

void corner_pin_node_t::get_corners_at_time( boost::array<Imath::V2d, 4>& pts, float time, float scale) const
{
    pts[0] = get_absolute_value_at_time<Imath::V2f>( param( "topleft"), time)  * scale;
    pts[1] = get_absolute_value_at_time<Imath::V2f>( param( "topright"), time) * scale;
    pts[2] = get_absolute_value_at_time<Imath::V2f>( param( "botright"), time) * scale;
    pts[3] = get_absolute_value_at_time<Imath::V2f>( param( "botleft"), time)  * scale;
}

bool corner_pin_node_t::quad_is_convex(const boost::array<Imath::V2d,4>& quad) const
{
    // TODO: use Graphic Gems convex test code
    float verts[4][2];

    for( int i = 0; i < 4; ++i)
    {
        verts[i][0] = quad[i].x;
        verts[i][1] = quad[i].y;
    }

    PolygonClass result = classifyPolygon2( 4, verts);
    return ( result == ConvexCW) || ( result == ConvexCCW);
}

void corner_pin_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_xform_node_t);
}

void corner_pin_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_xform_node_t);
}

// factory

node_t *create_corner_pin_node() { return new corner_pin_node_t();}

node_info_t corner_pin_node_info()
{
    node_info_t info;
    info.id = "builtin.corner_pin";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Transform";
    info.menu_item = "Corner Pin";
    info.create = &create_corner_pin_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( corner_pin_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::corner_pin_node_t)
