/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/dancing_links.hpp>
#include <adobe/timer.hpp>

#include <iostream>
#include <limits>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

std::size_t dancing_queens(std::size_t N) {
    const std::size_t diag_count = N * 2 - 1;
    const std::size_t cols_k = N * 2 + diag_count * 2;
    const std::size_t rows_k = N * N;
    const std::size_t file_base = N;
    const std::size_t diagonals_base = file_base + N;
    const std::size_t rdiagonals_base = diagonals_base + diag_count;
    adobe::dancing_links_t links(rows_k, cols_k);
    std::size_t row_index(0);

    // std::cout << "total rows: " << rows_k << ", cols: " << cols_k << std::endl;

    for (std::size_t i(0); i < N; ++i) {
        for (std::size_t j(0); j < N; ++j) {
            std::size_t R(i);
            std::size_t F(j);
            std::size_t A(i + j);
            std::size_t B(N - 1 - i + j);

#if 0
            std::cout   << row_index << ") "
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

/**************************************************************************************************/

void dancing_queen_iteration(std::size_t N) {
    adobe::timer_t timer;
    std::size_t solutions(dancing_queens(N));
    double time(timer.split());

    std::cout << "Found " << solutions << " solutions to the " << N << "-queens problem in " << time
              << " miliseconds (" << (time / 1e3) << " seconds )\n";
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

int main() try {
    for (std::size_t i(1); i <= 10; ++i)
        dancing_queen_iteration(i);

    return 0;
} catch (std::exception& error) {
    std::cout << "Error: " << error.what() << '\n';

    return 1;
} catch (...) {
    std::cout << "Error: unknown\n";

    return 1;
}
