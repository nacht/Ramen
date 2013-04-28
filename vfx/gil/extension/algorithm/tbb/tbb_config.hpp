//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_TBB_PARALLEL_CONFIG_HPP
#define GIL_EXTENSION_TBB_PARALLEL_CONFIG_HPP

#include<tbb/partitioner.h>

#ifndef GIL_TBB_ALGORITHMS_DEFAULT_PARTITIONER
    #define GIL_TBB_ALGORITHMS_DEFAULT_PARTITIONER tbb::auto_partitioner
#endif

#endif

