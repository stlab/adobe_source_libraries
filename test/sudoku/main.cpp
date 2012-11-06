/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/timer.hpp>

#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include "sudoku.hpp"
#include "sudoku_utilities.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <numeric>

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/*************************************************************************************************/

namespace {

/****************************************************************************************************/

class application_t
{
    typedef std::vector<sudoku::sudoku_t> sudoku_set_t;
    typedef std::size_t (*solver_proc_t)(sudoku::sudoku_t&);

public:
    enum
    {
        setting_detailed_printout_k = 0,
        setting_use_lone_candidate_k,
        setting_use_unique_candidate_k,
        setting_use_disjoint_subset_k,
        setting_use_unique_subset_k,
        setting_use_intersection_k,
        setting_set_size
    };

    explicit application_t(bfs::path prefs_path) :
        prefs_path_m(prefs_path)
    {
        for (std::size_t i(0); i < setting_set_size; ++i)
            solver_usage_m[i] = true;

        solver_usage_m[setting_detailed_printout_k] = false;
    }

    void run();

private:
    void import_preferences();

    void menu();

    void solve_puzzle();

    std::size_t deductive_step(solver_proc_t proc);

    void modify_settings();

    void pick_new_puzzle();

    bfs::path                               prefs_path_m;
    sudoku_set_t                            sudoku_set_m;
    sudoku::sudoku_t                        puzzle_m;
    boost::array<bool, setting_set_size>    solver_usage_m;
};

/****************************************************************************************************/

void application_t::import_preferences()
{
    char                buffer[1024];
    bfs::ifstream       input(prefs_path_m, std::ios_base::in | std::ios_base::binary);

    if (input.fail())
    {
        std::cerr << "Could not open preferences file." << std::endl;

        puzzle_m = sudoku::generate_puzzle();

        return;
    }

    input.unsetf(std::ios_base::skipws);

    while (!input.eof())
    {
        sudoku::sudoku_t    cur;
        int                 count(0);

        input.getline(&buffer[0], 1024);

        std::size_t readsize(static_cast<std::size_t>(input.gcount()));

        if (readsize < 81) continue;

        while (true)
        {
            char c(buffer[count]);

            if (c == '\t') break;

            if (c == '.' || c >= '1' && c <= '9')
            {
                if (c != '.')
                    cur.square(count / 9, count % 9).value_m = c - '0';

                ++count;
            }
        }

        if (readsize <= 81) break;

        ++count; // to get past the tab between the name and the 

        cur.name_m = std::string(&buffer[count], &buffer[readsize]);

        sudoku_set_m.push_back(cur);
    }
}

/****************************************************************************************************/

const char* solver_enum_to_string(int solver_enum)
{
    switch (solver_enum)
    {
        case application_t::setting_detailed_printout_k:    return "Iterative Puzzle Printout";
        case application_t::setting_use_lone_candidate_k:   return "Lone Candidate Selection";
        case application_t::setting_use_unique_candidate_k: return "Unique Candidate Selection";
        case application_t::setting_use_disjoint_subset_k:  return "Disjoint Subset Reduction";
        case application_t::setting_use_unique_subset_k:    return "Unique Subset Reduction";
        case application_t::setting_use_intersection_k:     return "Box/Row/Col Intersection Reduction";
        default: return "Unknown Setting";
    };
}

/****************************************************************************************************/

void application_t::modify_settings()
{
    while (true)
    {
        std::cout   << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
        std::cout   << "Please toggle settings" << std::endl;

        for (int count(0); count < setting_set_size; ++count)
            std::cout   << count+1 << ". Use " << solver_enum_to_string(count) << ": "
                        << (solver_usage_m[count] ? "On" : "Off") << std::endl;

        std::cout << std::endl;
        std::cout << "(0 to exit) ?> ";

        std::size_t choice;

        std::cin >> choice;

        if (choice <= 0 || choice > setting_set_size)
            break;
        else
            solver_usage_m[choice - 1] = !solver_usage_m[choice - 1];
    }
}

/****************************************************************************************************/

void application_t::pick_new_puzzle()
{
    std::cout   << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
    std::cout   << "Please pick a new puzzle" << std::endl;

    std::size_t count(0);
    for (sudoku_set_t::iterator first(sudoku_set_m.begin()),
        last(sudoku_set_m.end()); first != last; ++first)
    {
        std::cout << "   " << ++count << ". " << first->name_m << std::endl;
    }

    std::cout << std::endl;
    std::cout << "?> ";

    std::size_t choice;

    std::cin >> choice;

    if (choice <= 0 || choice > sudoku_set_m.size())
        std::cerr << "Choice is not viable. Aborting." << std::endl;
    else
        puzzle_m = sudoku_set_m[choice - 1];
}

/****************************************************************************************************/

std::size_t application_t::deductive_step(solver_proc_t proc)
{
    std::size_t usages(proc(puzzle_m));

    if (usages != 0)
    {
        sudoku::pinned_reduction(puzzle_m);

        if (!verify_integrity(puzzle_m))
            throw std::runtime_error("Puzzle integrity check failed");
    }

    return usages;
}

/****************************************************************************************************/

void application_t::solve_puzzle()
{
    // do a pinned reduction first before we start on the puzzle
    // to make sure the possibility sets are legit to begin with
    sudoku::pinned_reduction(puzzle_m);

    while (true)
    {
        std::size_t usages(0);

        try
        {
            if (solver_usage_m[setting_use_lone_candidate_k])
                usages += deductive_step(sudoku::lone_candidate_reduction);

            if (solver_usage_m[setting_use_unique_candidate_k])
                usages += deductive_step(sudoku::unique_candidate_reduction);

            if (solver_usage_m[setting_use_disjoint_subset_k])
                usages += deductive_step(sudoku::disjoint_subset_reduction);

            if (solver_usage_m[setting_use_unique_subset_k])
                usages += deductive_step(sudoku::unique_subset_reduction);

            if (solver_usage_m[setting_use_intersection_k])
                usages += deductive_step(sudoku::intersection_reduction);
        }
        catch (const std::exception& error)
        {
            std::cerr << "Exception: " << error.what() << std::endl;

            break;
        }
        catch (...)
        {
            std::cerr << "Exception: Unknown" << std::endl;

            break;
        }

        if (usages == 0)
            break;
        else if (solver_usage_m[setting_detailed_printout_k])
            detailed_status(puzzle_m);
    }

    std::cout << "No more squares can be solved" << std::endl;
    std::cout << puzzle_m;
}

/****************************************************************************************************/

void application_t::menu()
{
    while (true)
    {
        std::cout << std::endl;
        std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
        std::cout   << "Adobe Sudoku Test App " << ADOBE_VERSION_MAJOR << '.' << ADOBE_VERSION_MINOR
                    << '.' << ADOBE_VERSION_SUBMINOR << std::endl;
        std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
        std::cout << sudoku_set_m.size() << " puzzles loaded. \"";
        std::cout << puzzle_m.name_m << "\" is the current puzzle" << std::endl;
        std::cout << "   1. Solve current puzzle" << std::endl;
        std::cout << "   2. Change solver techniques used" << std::endl;
        std::cout << "   3. Print current puzzle (simple)" << std::endl;
        std::cout << "   4. Print current puzzle (detailed)" << std::endl;
        std::cout << "   5. Switch to another puzzle" << std::endl;
        std::cout << "   6. Import a new puzzle (unimp.)" << std::endl;
        std::cout << "   7. Save puzzle database (unimp.)" << std::endl;
        std::cout << "   8. Generate random puzzle" << std::endl;
        std::cout << "   9. Brute-force solve current puzzle" << std::endl;
        std::cout << "   R. Rate current puzzle" << std::endl;
        std::cout << "   M. Mass puzzle generation" << std::endl;
        std::cout << "   C. Count number of solutions to current puzzle" << std::endl;
        std::cout << "   P. Dancing links performance benchmarking" << std::endl;
        std::cout << std::endl;
        std::cout << "   Q. Quit" << std::endl;
        std::cout << std::endl;
        std::cout << "?> ";

        char choice(0);

        std::cin >> choice;

        std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
        std::cout << std::endl;

        switch (choice)
        {
            case '1':
                solve_puzzle();
                break;

            case '2':
                modify_settings();
                break;

            case '3':
                std::cout << puzzle_m;
                break;

            case '4':
                sudoku::pinned_reduction(puzzle_m);
                sudoku::detailed_status(puzzle_m);
                break;

            case '5':
                pick_new_puzzle();
                break;

            case '6':
            case '7':
                std::cout << "not implemented yet, sorry." << std::endl;
                break;

            case '8':
                puzzle_m = sudoku::generate_puzzle();
                break;

            case '9':
                sudoku::timed_solve(puzzle_m);
                break;

            case 'R':
            case 'r':
                sudoku::rate_puzzle(puzzle_m);
                break;

            case 'M':
            case 'm':
                sudoku::mass_puzzle_generation();
                break;

            case 'C':
            case 'c':
                sudoku::count_solutions(puzzle_m);
                break;

            case 'P':
            case 'p':
                sudoku::performance_benchmark();
                break;

            case 'Q':
            case 'q':
                return;

            default:
                std::cerr << "Invalid choice. Please choose again." << std::endl;
                break;
        }
    }
}

/****************************************************************************************************/

void application_t::run()
{
    import_preferences();

    if (!sudoku_set_m.empty()) puzzle_m = sudoku_set_m[0];

    menu();
}

/*************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main(int argc, char** argv)
{
    std::srand(std::time(0));

    std::string db_name("sudokus.txt");

    if (argc > 1) db_name.assign(argv[1]);

    bfs::path filepath(db_name.c_str(), argc > 1 ? bfs::native : bfs::portable_name);

    {
    application_t(filepath).run();
    }

    return 0;
}
