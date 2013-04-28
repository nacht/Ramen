/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_DANCING_LINKS_HPP
#define ADOBE_DANCING_LINKS_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <adobe/implementation/toroid.hpp>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

#ifndef ADOBE_NO_DOCUMENTATION

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

struct do_nothing_callback_t
{
    inline void operator () (std::size_t, bool) const
    { }
};

/****************************************************************************************************/

struct select_right_heuristic_t
{
    template <std::size_t Rows, std::size_t Cols, std::size_t NodeBlockSize>
    inline toroid_header_t* operator () (binary_toroid<Rows, Cols, NodeBlockSize>& toroid) const
        { return toroid.right_of(&toroid.header_m); }
};

/****************************************************************************************************/

struct select_most_constrained_heuristic_t
{
    template <std::size_t Rows, std::size_t Cols, std::size_t NodeBlockSize>
    inline toroid_header_t* operator () (binary_toroid<Rows, Cols, NodeBlockSize>& toroid) const
    {
        toroid_header_t* c(toroid.right_of(&toroid.header_m));
        std::size_t      sz(c->size_m);

        for (   toroid_header_t* p(toroid.right_of(c));
                p != &toroid.header_m; p = toroid.right_of(p))
        {
            if (p->size_m < sz)
            {
                c = p;
                sz = p->size_m;
            }

            if (sz == 1) break;
        }

        return c;
    }
};

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

#endif

/****************************************************************************************************/

template <std::size_t Rows, std::size_t Cols>
class dancing_links : boost::noncopyable
{
public:
#ifndef ADOBE_NO_DOCUMENTATION
    dancing_links() :
        solutions_m(0)
    { }
#endif

    inline void set(std::size_t row, std::size_t col)
        { toroid_m.set(row, col); }

    inline void set_secondary_column(std::size_t col)
        { toroid_m.set_secondary_column(col); }

    template <typename ResultCallback>
    inline std::size_t search(std::size_t max_solutions, ResultCallback callback)
    {
        max_solutions_m = max_solutions;

        toroid_m.finalize();

#if 1
        do_search(0, callback, implementation::select_most_constrained_heuristic_t());
#else
        do_search(0, callback, implementation::select_right_heuristic_t());     
#endif

        return solutions_m;
    }

    inline std::size_t search(std::size_t max_solutions)
        { return search(max_solutions, implementation::do_nothing_callback_t()); }

private:
    template <typename ResultCallback, typename SearchHeuristic>
    void do_search(std::size_t k, ResultCallback callback, SearchHeuristic heuristic)
    {
        if (toroid_m.right_of(&toroid_m.header_m) == &toroid_m.header_m)
        {
            ++solutions_m;

            for (std::size_t i(0); i < k; ++i)
                callback(toroid_m.row_index_of(output_m[i]), i + 1 == k);

            return;
        }

        std::size_t next_k(k + 1);

        toroid_header_t* c(heuristic(toroid_m));

        toroid_m.cover_column(c);

        for (toroid_node_t* r(toroid_m.down_of(c));
            r != c; r = toroid_m.down_of(r))
        {
            output_m[k] = r;

            for (toroid_node_t* j1(toroid_m.right_of(r));
                j1 != r; j1 = toroid_m.right_of(j1))
                { toroid_m.cover_column(toroid_m.column_of(j1)); }

            do_search(next_k, callback, heuristic);

            if (solutions_m >= max_solutions_m) return;

            r = output_m[k];

            c = toroid_m.column_of(r);

            for (toroid_node_t* j(toroid_m.left_of(r));
                j != r; j = toroid_m.left_of(j))
                { toroid_m.uncover_column(toroid_m.column_of(j)); }
        }

        toroid_m.uncover_column(c);
    };

    binary_toroid<Rows, Cols> toroid_m;
    toroid_node_t*            output_m[Cols];
    std::size_t               solutions_m;
    std::size_t               max_solutions_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
