// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QSHAPE_COMMANDS_HPP
#define	RAMEN_QSHAPE_COMMANDS_HPP

#include<ramen/undo/command.hpp>

#include<ramen/nodes/qshape/shape.hpp>

namespace ramen
{

class qshape_node_t;

namespace undo
{

class add_qshape_command_t : public command_t
{
public:

    add_qshape_command_t( qshape_node_t *node, qshape::shape_t *shape);
    virtual ~add_qshape_command_t();

    virtual void undo();
    virtual void redo();

private:

    qshape_node_t *node_;
    qshape::shape_t *shape_;
};

class modify_qshape_command_t : public command_t
{
public:

    modify_qshape_command_t( qshape_node_t *node, bool recreate_curves = false);

    virtual void undo();
    virtual void redo();

private:

    void swap_control_points();

    qshape_node_t *node_;
    qshape::shape_t *shape_;

    std::vector<qshape::triple_t> old_;
    std::vector<qshape::triple_t> *new_;

    bool recreate_curves_;
};

class qshape_toggle_active_command_t : public command_t
{
public:

    qshape_toggle_active_command_t( qshape_node_t *node);

    virtual void undo();
    virtual void redo();

private:

    qshape_node_t *node_;
    qshape::shape_t *shape_;
    bool prev_state_;
};

class qshape_order_command_t : public command_t
{
public:

    qshape_order_command_t( qshape_node_t *node, bool up);

    virtual void undo();
    virtual void redo();

private:

    qshape_node_t *node_;
    int index_;
    bool move_up_;
};

class qshape_delete_command_t : public command_t
{
public:

    qshape_delete_command_t( qshape_node_t *node, qshape::shape_t *shape);
    ~qshape_delete_command_t();

    virtual void undo();
    virtual void redo();

private:

    qshape_node_t *node_;
    qshape::shape_t *shape_;
    int index_;
};

} // namespace
} // namespace

#endif
