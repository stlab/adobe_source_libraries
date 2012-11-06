/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/dancing_links.hpp>
#include <adobe/timer.hpp>

#include <iostream>
#include <limits>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

template <std::size_t N>
std::size_t dancing_queens()
{
    enum
    {
        diag_count = N * 2 - 1,
        max_d_index = 2 * N - 2,
        cols_k = N * 2 + diag_count * 2,
        rows_k = N * N,
        file_base = N,
        diagonals_base = file_base + N,
        rdiagonals_base = diagonals_base + diag_count
    };

    adobe::dancing_links_t links(rows_k, cols_k);
    std::size_t          row_index(0);

//  std::cerr << "total rows: " << rows_k << ", cols: " << cols_k << std::endl;

    for (std::size_t i(0); i < N; ++i)
    {
        for (std::size_t j(0); j < N; ++j)
        {
            std::size_t R(i);
            std::size_t F(j);
            std::size_t A(i + j);
            std::size_t B(N - 1 - i + j);

#if 0
            std::cerr   << row_index << ") "
                        << " R" << R << "(" << R << ")"
                        << " F" << F << "(" << F + file_base << ")"
                        << " A" << A << "(" << A + diagonals_base << ")"
                        << " B" << B << "(" << B + rdiagonals_base << ")"
                        << std::endl;
#endif

            links.set(row_index, R);
            links.set(row_index, F + file_base);
            links.set(row_index, A + diagonals_base);
            links.set(row_index, B + rdiagonals_base);

            links.set_secondary_column(A + diagonals_base);
            links.set_secondary_column(B + rdiagonals_base);

            ++row_index;
        }
    }

    return links.search(std::numeric_limits<std::size_t>::max());
}

/****************************************************************************************************/

template <std::size_t N>
void dancing_queen_iteration()
{
    adobe::timer_t  timer;
    std::size_t     solutions(dancing_queens<N>());
    double          time(timer.split());

    std::cout   << "Found " << solutions << " solutions to the "
                << N << "-queens problem in " << time << " miliseconds ("
                << (time / 1e3) << " seconds )" << std::endl;

    dancing_queen_iteration<N+1>();
}

/****************************************************************************************************/

template <>
void dancing_queen_iteration<18>()
{
    adobe::timer_t  timer;
    std::size_t     solutions(dancing_queens<18>());
    double          time(timer.split());

    std::cout   << "Found " << solutions << " solutions to the "
                << 18 << "-queens problem in " << time << " miliseconds ("
                << (time / 1e3) << " seconds )" << std::endl;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main()
{
    dancing_queen_iteration<1>();

    return 0;
}
