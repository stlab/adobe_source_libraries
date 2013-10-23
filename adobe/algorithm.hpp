/*
    Copyright 2005 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_HPP
#define ADOBE_ALGORITHM_HPP

#include <adobe/config.hpp>

#include <adobe/algorithm/binary_search.hpp>
#include <adobe/algorithm/copy.hpp>
#include <adobe/algorithm/count.hpp>
#include <adobe/algorithm/clamp.hpp>
#include <adobe/algorithm/equal.hpp>
#include <adobe/algorithm/equal_range.hpp>
#include <adobe/algorithm/erase_if.hpp>
#include <adobe/algorithm/exists.hpp>
#include <adobe/algorithm/fill.hpp>
#include <adobe/algorithm/filter.hpp>
#include <adobe/algorithm/find.hpp>
#include <adobe/algorithm/find_match.hpp>
#include <adobe/algorithm/for_each.hpp>
#include <adobe/algorithm/for_each_position.hpp>
#include <adobe/algorithm/generate.hpp>
#include <adobe/algorithm/heap.hpp>
#include <adobe/algorithm/identity_element.hpp>
#include <adobe/algorithm/iota.hpp>
#include <adobe/algorithm/lexicographical_compare.hpp>
#include <adobe/algorithm/lower_bound.hpp>
#include <adobe/algorithm/median.hpp>
#include <adobe/algorithm/merge.hpp>
#include <adobe/algorithm/minmax.hpp>
#include <adobe/algorithm/mismatch.hpp>
#include <adobe/algorithm/other_of.hpp>
#include <adobe/algorithm/partition.hpp>
#include <adobe/algorithm/permutation.hpp>
#if !ADOBE_IS_DEPRECATED_ERROR(100039)
#include <adobe/algorithm/pin.hpp>
#endif
#include <adobe/algorithm/random_shuffle.hpp>
#include <adobe/algorithm/reduce.hpp>
#include <adobe/algorithm/remove.hpp>
#include <adobe/algorithm/replace.hpp>
#include <adobe/algorithm/reverse.hpp>
#include <adobe/algorithm/rotate.hpp>
#include <adobe/algorithm/search.hpp>
#include <adobe/algorithm/select.hpp>
#include <adobe/algorithm/selection_algorithms.hpp>
#include <adobe/algorithm/set.hpp>
#include <adobe/algorithm/sort.hpp>
#include <adobe/algorithm/swap_ranges.hpp>
#include <adobe/algorithm/transform.hpp>
#include <adobe/algorithm/unique.hpp>
#include <adobe/algorithm/upper_bound.hpp>

/**************************************************************************************************/

/*!
\defgroup algorithm Algorithms
\ingroup asl_libraries

All algorithms have been grouped by algorithm "families" in the <code>adobe/algorithm/</code>
folder. The header file <code>adobe/algorithm.hpp</code> is a top-level inclusion of all the header
files in the \c algorithm folder.

To improve build times and minimize global dependency issues, it is recommended that you include
only the algorithm family(ies) that you need for a given source/header file.

STL algorithms which have been adapted to use models of \ref concept_convertible_to_range and
\ref concept_convertible_to_function for convenience.

We have also added several new variants of algorithms. Currently there is not suppor for all
variants for all algorithms but it is our intention to provide them. Specifically:

    - <code><em>algorithm</em>_n</code> - range is specified with an iterator, i, and an integral
type, n, representing the range [i, i + n).
    - <code><em>algorithm</em>_bounded</code> - rather than an OutputIterator a mutable ForwardRange
specifies the destination and the algorithm ends when either the source or destination are exhausted.
The returned value of such algorithms is a pair of the last source and destination iterators.
    - Algorithms which take a comparison function can also talke a unary function which is a
projection to a key value.

\note The following algorithms differ from their STL counterparts. Read the documentation before
using.

    - \ref binary_search
    - \ref for_each
    - \ref minmax
    - \ref rotate

\sa \ref stldoc_index
    
\defgroup mutating_algorithm Mutating Algorithms
\ingroup algorithm

\defgroup non_mutating_algorithm Non-Mutating Algorithms
\ingroup algorithm

\defgroup container_algorithm Container Algorithms
\ingroup algorithm

\defgroup sorting Sorting Algorithms
\ingroup algorithm

\defgroup node_algorithm Node Algorithms
\ingroup algorithm


*/

/**************************************************************************************************/

#endif

/**************************************************************************************************/
