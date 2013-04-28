/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_TOROID_HPP
#define ADOBE_TOROID_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/array.hpp>
#include <boost/noncopyable.hpp>

#include <list>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct toroid_header_t;

/****************************************************************************************************/

struct toroid_node_t
{
    toroid_node_t*      left_m;     // node to the left
    toroid_node_t*      right_m;    // node to the right
    toroid_node_t*      up_m;       // node to the up
    toroid_node_t*      down_m;     // node to the down
    toroid_header_t*    column_m;   // header at head of this column
    toroid_header_t*    row_m;      // header at head of this row
};

/****************************************************************************************************/

struct toroid_header_t : public toroid_node_t
{
    std::size_t size_m; // number of 1s in the row/column
};

/****************************************************************************************************/

template <std::size_t Rows, std::size_t Cols, std::size_t NodeBlockSize = 512>
class binary_toroid : boost::noncopyable
{
private:
    enum
    {
        rows_k = Rows,
        cols_k = Cols,
        block_size_k = NodeBlockSize
    };

    typedef boost::array<toroid_node_t, block_size_k>    node_block_t;
    typedef std::list<node_block_t>                             data_set_t;
    typedef boost::array<toroid_header_t, rows_k>        row_header_set_t;
    typedef boost::array<toroid_header_t, cols_k>        col_header_set_t;

public:
    binary_toroid() :
        node_block_count_m(0)
#ifndef NDEBUG
        , finalized_m(false)
#endif
    {
        data_set_m.push_back(node_block_t());

        for (   typename row_header_set_t::iterator first(row_header_set_m.begin()),
                last(row_header_set_m.end()); first != last; ++first)
        {
            toroid_header_t* this_row(&(*first));

            first->left_m = this_row;
            first->right_m = this_row;
        }

        typename col_header_set_t::iterator front_i(col_header_set_m.begin());
        typename col_header_set_t::iterator back_i(col_header_set_m.end() - 1);

        for (   typename col_header_set_t::iterator first(col_header_set_m.begin()),
                last(col_header_set_m.end()); first != last; ++first)
        {
            toroid_header_t* this_col(&(*first));

            first->up_m = this_col;
            first->down_m = this_col;

            if (first != front_i)
                first->left_m = &(*(first - 1));

            if (first != back_i)
                first->right_m = &(*(first + 1));

            first->size_m = 0;
        }

        front_i->left_m = &header_m;
        back_i->right_m = &header_m;

        header_m.down_m = &(row_header_set_m.front());
        header_m.up_m = &(row_header_set_m.back());
        header_m.right_m = &(col_header_set_m.front());
        header_m.left_m = &(col_header_set_m.back());
    }

    void set(std::size_t row, std::size_t col)
    {
        // prerequisite: no other node can be set to the right or below this one.
        // i.e., you must start with the top-left node and work to the right, then
        // down. i.e., this must be the right-bottom-most node you have set.

        assert (row < rows_k && col < cols_k);
        assert (!finalized_m);

        toroid_header_t& row_h(row_header_set_m[row]);
        toroid_header_t& col_h(col_header_set_m[col]);

        if (node_block_count_m == block_size_k)
        {
            data_set_m.push_back(node_block_t());
            node_block_count_m = 0;
        }

        toroid_node_t& node(data_set_m.back()[node_block_count_m]);

        ++node_block_count_m;

        node.left_m = row_h.left_m;
        node.right_m = &row_h;
        row_h.left_m->right_m = &node;
        row_h.left_m = &node;

        node.up_m = col_h.up_m;
        node.down_m = &col_h;
        col_h.up_m->down_m = &node;
        col_h.up_m = &node;

        node.column_m = &col_h;
        node.row_m = &row_h;

        //++(row_h.size_m); // not used
        ++(col_h.size_m);
    }

    void set_secondary_column(std::size_t col)
    {
        assert (col < cols_k);

        // Secondary columns take dancing links to the next step,
        // in that they allow for a column to be optionally used
        // zero or one times. This esentially allows for a
        // condition that can be met once, but no more than once.
        // 
        // According to Knuth, the header for a secondary column
        // should have left and right fields that simply point
        // to itself, so we unlink it from the doubly-linked
        // header column list.

        toroid_header_t& column(col_header_set_m[col]);

        column.left_m->right_m = column.right_m;
        column.right_m->left_m = column.left_m;
        column.right_m = &column;
        column.left_m = &column;
    }

    void finalize()
    {
        assert (!finalized_m);

        // This "covers" the row headers in the toroid, leaving a data
        // structure as described by Knuth's Dancing Links Algorithm.

        for (   typename row_header_set_t::iterator first(row_header_set_m.begin()),
                last(row_header_set_m.end()); first != last; ++first)
        {
            first->left_m->right_m = first->right_m;
            first->right_m->left_m = first->left_m;
        }

#ifndef NDEBUG
        finalized_m = true;
#endif
    }

    void cover_column(toroid_header_t* c)
    {
        assert (finalized_m);

        right_of(c)->left_m = left_of(c);
        left_of(c)->right_m = right_of(c);

        for (toroid_node_t* i(down_of(c)); i != c; i = down_of(i))
        {
            for (toroid_node_t* j(right_of(i)); j != i; j = right_of(j))
            {
                down_of(j)->up_m = up_of(j);

                up_of(j)->down_m = down_of(j);

                --(column_of(j)->size_m);
            }
        }
    }

    void uncover_column(toroid_header_t* c)
    {
        assert (finalized_m);

        for (toroid_node_t* i(up_of(c)); i != c; i = up_of(i))
        {
            for (toroid_node_t* j(left_of(i)); j != i; j = left_of(j))
            {
                ++(column_of(j)->size_m);

                down_of(j)->up_m = j;

                up_of(j)->down_m = j;
            }
        }

        right_of(c)->left_m = c;
        left_of(c)->right_m = c;
    }

    // This leverages the row headers used to initialize the data structure by
    // taking the distance from the start of the row header list to the row
    // header referenced by this node.

    std::size_t row_index_of(toroid_node_t* node)
        { return static_cast<std::size_t>(std::distance(&(*(row_header_set_m.begin())), node->row_m)); }

    template <typename ToroidDataPointer>
    static inline ToroidDataPointer left_of(ToroidDataPointer node)
        { assert(node->left_m != 0); return reinterpret_cast<ToroidDataPointer>(node->left_m); }

    template <typename ToroidDataPointer>
    static inline ToroidDataPointer right_of(ToroidDataPointer node)
        { assert(node->right_m != 0); return reinterpret_cast<ToroidDataPointer>(node->right_m); }

    static inline toroid_node_t* up_of(toroid_node_t* node)
        { assert(node->up_m != 0); return node->up_m; }

    static inline toroid_node_t* down_of(toroid_node_t* node)
        { assert(node->down_m != 0); return node->down_m; }

    static inline toroid_header_t* column_of(toroid_node_t* node)
        { assert(node->column_m != 0); return node->column_m; }

    toroid_header_t header_m;

private:
    data_set_t          data_set_m;
    row_header_set_t    row_header_set_m;
    col_header_set_t    col_header_set_m;
    std::size_t         node_block_count_m;
#ifndef NDEBUG
    bool                finalized_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
