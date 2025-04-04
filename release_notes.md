# RELEASE NOTES

## v1.0.46

2024-01-12

The release notes were dropped some time back and we haven't been doing official releases even though the libraries have been kept building. This marks the first official release in some time. These release notes are incomplete. We'll try and do better going forward.

- default branch was renamed to `main`. To update your repository:
  ```bash
  git branch -m master main
  git fetch origin
  git branch -u main main
  git remote set-head origin -a
  ```
- Updated to C++17
- We are now using github releases.
- Fixes for clang static analyzer warnings.
- Many boost dependencies replaced with standard library equivalents.
- Added github actions for CI.
- Consistent use of `ADOBE_ASSERT()` instead of `assert()`.
- Removed `random_shuffle` from algorithms in line with C++ standard.
- Added transform_forest algorithm.
- enum_ops with support for enum classes.
- Added const iterator variants to forest.
- Added capability to bypass comments in CEL expressions.
- Added JSON support.
- Added SHA hash support.
- Numerous bug fixes and improvements.
- Removed `any_iterator`.
- Cleanup and bug fixes.

## Older release notes

```text
2010/11/02 : 1.0.43
    ASL:
        - Updated projects to support Boost 1.44.0
        - Updated boost 1.44.0 patches
            - Add back security-theater setting to boost build
            - Addressing filesystem v2 compilation errors on windows
        - Simplified move library. Removed move_sink and copy_sink in favor of
            always using RVO.
        - Added the "to" token ("->") to the lexer
        - Removed gil from ASL (clients can use boost version instead)
        - Added support for gcc 4.5.0 with -std=C++0x.
        - Updated to Visual Studio 2010, older versions not supported.
        - Renamed jamfiles to jamfile.jam
        - Added support for hexadecimal numbers to the lexer (0xDEADBEEF,
            0xffe0, etc.)
        - Extended the dancing links algorithm to include colorized nodes;
            updated test cases as well.
        - gcc 4.2 and OS X 10.5 SDK support (deprecated function warnings
            selectively disabled).
        - Replaced of basic_sheet_t with sheet_t in layouts and associated Eve
            grammar updates.
        - New external_model_t with support for cell creation in Adam parser.
        - once.hpp fix (Thanks John (Eljay) Love-Jensen)
        - Added support for overriding the index operator in the virtual machine.

        - Property model library

            Added support for multi-out relate terms. For example:
            ---
            logic:
                relate {
                    x, y <== [a, b];
                    a, b <== [x, y];
                }
            ---
            The rhs value must be an array with the same arity as the number of
            cells named on the left. A cell can appear more than once on the
            left so long as the term can be uniquely selected.

            Fixed bugs in flow algorithm that cuased cells to resolve out of order.
            Fixed enablement bug in initial call to monitor_enabled.

    User supported libs (APL):
        - Fixed platform_slider on windows: now works to use scroll bar ranges
            different from 0 to 100 in eve
        - Binary file format debugger (bffd) submitted


2009/05/13 : 1.0.42
    ASL:

        - Property model library (Adam)

            Changed calculation logic so that when an interface cell is defined
            by a direct expression and by a relate clause, both expression are
            applied. Prior it was one or the other. For example:
            ---
            interface:
                x;
                y <== round(y);
            logic:
                relate {
                    x <== y;
                    y <== x;
                }
            ---
            When x is of higher priority, y will be evaluated as:
            ---
            y'  = x;
            y'' = round(y')
            ---
            This avoids having to replicate filters (self referential
            expression on an interface cell) in relate clauses.
            Note that if the expression defining y was not self referential,
            then the relate clause would be ignored. For example, if y were
            defined as:
            ---
            y <== 10;
            ---
            Then y would have the value of 10 regardless of the value of x.

        	- Fixed issue introduced in 1.0.41 with incorrect poison set being
        	    calculated for invariants.
	        - Reworked invariant cells they now behave more like output cells
	          and can be refered to in expressions.
	        - Updated the image_size example to demonstrate an ignored/disabled
	          control (resample method when not resampling).
        - Some 64 bit fixes and cleanup.
        - Minor cleanup, performance improvements and bug fixes.

2009/02/13 : 1.0.41

    ASL:

        - Property model library (Adam) - rewrote main solver
	        - no longer uses exceptions as flow control.
	        - fixed bug with initializers that refer to cells with
              define_expressions has causing the define_expressions to evaluate
              which could refer to cells which had not been declared.
	        - exported get() member, it is now the client responsibility to wire
              sheet_t::get() to the variable lookup in the VM (this also allows
              the clients to hook the variable lookup for supporting custom
              variables). In the future the VM will be separated entirely from
              the property model library.
	        - added some more error reporting including reporting duplicate cell
              names.
        - virtual machine library
            - removed the contributing buts which were tracked through
              calculations. The VM stack is now just a stack of any_regular_t.
            - removed the use of storing size_t in an any_regular_t in the VM
              (was breaking serialization among other things because size_t is
              not a C++ basic type on Win64).
        - any_regular_t
            - improved serialization by removing one heap alloc/dealloc per item
              serialized, also uses logN search for type instead of linear
              search.
                - removed the hack to handle size_t serialization on Win64.
            - serialization will now throw an exception when it encounters a
              type other than one of the 7 CEL types (it used to assert).
            - in debug builds, the version field of the any_regular_t vtable
              contains a short type name for debugging purposes only.
              See the wiki documentation.

2008/12/23 : 1.0.40

    ASL:

        - The pmeval (property model evaluator) interpreter is ready
          for general purpose use. It has been enhanced to allow
          flexible exploration and testing of property models.
          Commands include check assertions, print statements, update,
          reinitialize, and property model dumps (showing enablement,
          contributing/intent). pmeval can be built from source (all
          platforms, including cygwinand *N*X) or downloaded from
          sourceforge for Mac or Windows.

        - Property models can also be evaluated via an experimental
          web form-based interface to pmeval.  The service, along with
          an intermediate property modeling tutorial is available
          through http://emarcus.org/#pmeval .

        - Rewrote enablement for the property model engine.  Enabled
          status is set to a conservative approximation of enablement
          detection (erring towards enablement): it is set to false if
          a change to the cell cannot affect the output of a correct
          sheet, and true if it *may* effect the output.

        - Replaced monitor_active with monitor_enabled (conservative
          enablement, touch_set support).  See the documentation for
          monitor_enabled in adam.hpp, and the calls to
          monitor_enabled in (APL's) widget_factory.hpp for details.
          (Note that it is incorrect to unconditionally pass NULLs to
          monitor_enabled.  The bug will manifest as incorrect
          enablement behavior in the presence of touch sets in eve
          sheets.)

        - Fixed bug where interface cells appearing in a when clause
	  condition were not contributing.

        - New papers and presnetations at
          http://stlab.adobe.com/wiki/index.php/Papers_and_Presentations

        - Patched boost to allow preservation of test targets (now the
          default).

        - Patched to serializes std::size_t on MSVC when compiling for
          64 bit architecture.

        - Added qualification to binary_search algorithm to remove
          ambiguity.

        - Refactored pmeval, introduced (test-only) queryable_sheet_t.
          pmeval now supports multiple input files, sheet separate
          from commands, multiple simultaneous sheets.

        - Allow monitoring of invariants under monitor_value.

        - Improved release notes and build scripts for *N*X/cygwin

    User supported libs (APL):

        - Updated widget factory so that widgets monitor_considered
	  instead of monitor_active, to properly enable in presence of
	  touch sets.

        - Fixed reveal widget bug

        - Update so that both stlab and internal adobe frameworks can
          leverage widget_factory.hpp; also rolled in updates from
          other teams within adobe

2008/11/06 : 1.0.39

    ASL:
        - Added sheet_t::reinitialize(). "Input cells are
          re-initialized, in sheet order, and interface cell
          initializers are re-evaluated. Priorities are updated, but
          no callbacks are triggered. Calls to reinitialize should be
          followed by calls to update must be called, just as if the
          cells were updated by set operations on sorted ranges."
        - Adam documentaion improvements
        - Updated to (patched) boost 1.37.0
        - Added a command line property model evaluator tool, and
          programmer-oriented tutorial at:
          http://easyurl.net/pm_tutorial
        - New (theoretical) paper on Property Models from ACM GPCE
          2008 added to: http://easyurl.net/stlab_publications
        - Regressions now also run under Vista 64-bit
        - Regressions now also run under Mac OS X 64-bit
        - Deprecated pin library. Replaced with clamp library. Also
	  added median(of 3) and some select algorithms.

    User supported libs (APL):
        - Formatter improvements.

2008/09/10 : 1.0.38

    ASL:
        - New papers and presnetations at
          http://stlab.adobe.com/wiki/index.php/Papers_and_Presentations
        - Major improvements to documentation structure
        - Fix 64-bit poly_test issue
        - Fixes for identity<> and pointer_to<> and their use.
        - Eliminated concept macros
        - Minor fix for static_table.hpp to include stdexcept
        - Fix for lexer to allow keywords as names, i.e. @empty.
        - Fix for MSVC compiler warning in memory.hpp (from Jon
           Clauson)
        - Predicates for conditional relate declarations are now
          calculated up-front and cannot be dependent themselves on a
          relate declaration.
        - Raising limit on number of interface cells to 256.
        - Added parameter to final to avoid misuse.
        - Added -L to curl commands in net setup scripts to protect
          against redirects
        - Made selection_t equality_comparable
        - Added bool has_input(name) and bool has_output(name) to the
          property model library.
        - removed outdated expression_filter routines; updated
          adam_tutorial output format; deleting some unnecessary
          sources
        - Cleaned out static intializers
        - Collapsed entity lookup to a single place -- removed the
          once stuff from xml_parser
        - Fixed issue with implicit casts of enum node state in
          closed_hash.hpp
        - Fixed bug in property model library where an active monitor
          wasn't called on connect.
        - Added code to layout library to reset metrics prior to
          calling client measure function to avoid errors where client
          code assumes defaulted value.
        - Fixed bug with active monitor not being called on connection
          in Adam.

    User supported libs (APL):
        - Improvements to the expression, property model, and layout
          formatters. they print are much more readably now,
          especially with arrays and dictionaries with many items.
        - Added shift as a modifier key to the new modifier key
          functionality
        - Move controller and view concepts (compile and runtime) to
          APL
        - BBedit language modules
        - xml_to_forest test app
        - Added xml_element_forest, a utility header that round-trips
          an XML file to a forest of dictionaries; also gives the user
          the ability to hand-generate a forest of dictionaries with a
          specific format and stream out a resulting XML file.
        - Moved functions_as_controller to widget_proxies.hpp in APL
        - Renamed to_utf* to value_to_utf* in the singleton code point
        - Removed subview factory
        - Gave behaviors the ability to have their verbs pointed to
          another callback once they've been established. This gives
          clients the ability to do more than just resize the eve layout
          at the appropriate time (e.g., to resize a parent container
          not managed by eve.)
        - sequence_model and associated view and controller concepts
          and implementations. Multiple changes and to improve the
          overall mvc architecture of the system. Also added a
          multiplexer/demultiplexer system to allow for the routing of
          commands between the sequence_model and any attached
          sequence views and controllers.
        - dictionary_set migration from obscure widgets lib location
          to APL
        - Added breakpoint-compatible formatting to some of the
          casting type checks
        - Logic fix for one of the selection algorithms
        - Fixed error reporting code in Begin.
        - Added formatter_tokens to eliminate duplicate symbols when
          both formatters are included in a single project
        - Cleaned up a lot of the serialization code, pulled out
          dynamic memory allocations, etc
        - Added expression_filter, for the escaping and unescaping of
          xml entities in a string; useful for serialization work
        - Fixed some of the image_t leaks under win32. Problems run
          deeper though, since the architecture fails to store
          platform resources for cleanup when bitmaps updated or when
          window closes. Deeper fixes will be required to plug the
          rest of the leaks
        - Added the ability to add 'raw' functions to the function
          pack, bypassing all the function pack magic in case you'd like
          to do the work yourself.
        - Update example code. inlined some function_pack routines,
          and added an nbitpath helper routine to the bitpath structure


2008/05/02 : 1.0.37
    ASL:
        - Update ASL to depend on boost 1.35.0
        - Patch boost 1.35.0 to fix bugs in boost threads and boost
          filesystem
        - Patch boost build to add a link-incompatible feature
          allowing _SECURE_SCL to be disabled, thus avoiding
          unjustifiable performance penalty on release builds.
        - Multiple net setup and build improvements.
        - Workaround the bjam MkDir bug that was causing build errors
          and the need for multiple invocations of bjam.
        - Tested against gcc 4.3.0.
        - Update ASL to depend on Intel Thread Building Blocks
          2008.04.08 stable release, now supporting atomics under ppc 32bit.
        - Patch TBB to support windows/cygwin-based gcc 4.1.1 and
          later.
        - Fix bug on 64 bit little-endian machines in closed_hash
        - Documentation improvements and corrections for any_regular_t
          and some algorithms
        - Fixed a potential memory leak in any_regular_t
        - Fixed thread safety issue with forest<>::size()
        - New binary_search (unlike std::binary_search, this is useful)
        - Added gather algorithm (thanks to Marshall Clow)
        - Started to break-up functional -
            functional/operator.hpp
            functional/is_member.hpp
        - Deleted find_first_of_set.hpp - use find_if with is_member
          instead
        - Change sheet and basic sheet API to remove
          attach_view/detach_view in favor of signals
          mechanism. Remove function_as_view since function/bind
          results can once again be used directly. poly_view is now
          optional for clients who find it more convenient.
        - Change adam API to remove
          attach_controller/detach_controller in favor of signals
          mechanism. poly_controller is now optional for clients who
          find it convenient.
        - Remove dependence of adam_tutorial on APL and move to
          ASL. Add visual project for adam_tutorial.
        - Rewrite copy_on_write to use a well-defined binary
          representation, add custom allocator support, move to
          version1 namespace.
        - Added script to update a directory in p4 based on "offline"
          changes, and other scripts to automate documentation update
          process.
        - Fixes to vector and string for windows min/max problems,
          thanks to Jesper.
        - Add check ensuring operator== interoperability of forest
          iters and const iters.

    APL:
        - Add property_model_formatter.hpp and layout_formatter.hpp
          for parsing layout and property model descriptions into a
          data structure instead of to the widget factory.
        - Add layout_tidy and property_model_tidy which check layout
          and property model definitions validity.


2008/03/20 : 1.0.36
    ASL:
        - version_1 types (replace version_0 types) - better, defined binary
        layout and carry allocator for applications with replaced new/delete.
        - capture_allocator, an std::allocator compatible allocator which
        carries a paired new/delete.
        - Allocator support in closed_hash_set<>, closed_hash_map<>, and
        vector<>.
        - Vastly improved move library (Thanks to Dave Abrahams!).
        - Vastly improved copy-on-write library (now uses move library to
        reduce number of increment/decrements).
        - Bug fix in layout library that caused containers with the width
        driven by the frame width to layout children incorrectly.
        - Bug fix in forest::erase where the beginning of the erased range has
        a lower depth then the end of the range.
        - Forest test cases and minor fixes to forest types.
        - Misc bug fixes.
        - Fixes to build and distribution tools.
        - Fixed search on documentation site to point to stlab and not
        opensource domain.

2008/01/28 : 1.0.35
    ASL:
        - More property model library simplifications and the removal
        of "weak" which never worked.
        - Bug fix for copy-on-write assignment which was leaking.
        - Work on runtime_cast<> for any_regular_t.
        - Fixed a bug with edit_number and disabled items with multiple units.
        - Fixed a bug with image_size (introduced in some recent cleanup).
        - removed adobe/algorithm/assign.hpp
        - improved rvalue assignment in some classes
        - renamed copy_bound to copy_bounded
        - minor improvements to operator+() for strings
        - updated doxyfile for latest doxygen
        - More algorithm documentation clean-up. Greately improved erase/erase_if
        container algorithms and container storage type function.
        - Templatize erase_if test cases
        - Initial work on forest unit_test


2008/01/03 : 1.0.34
    ASL:
        - Lots of clean up and removed defered initialization of
        property model cells.
        - changes to the initializers for the property model library
        you must now call upate() prior to monitoring or attaching a
        view (you'll get an assert if you don't) and the monitor
        function (or view) is called immeditely when attached with the
        current state.
        - support for stateful function object (hash, key function,
        and compare) to closed_hash_set
        - hash_index class which is similar to table_index but
        implemented with a hash_set instead of a vector -
        there is stills some room here to generalize the two.
        -  Changed ordering in compressed_pair so it actually
        compresses on Windows

    GIL:
        - Makefile and documentation fixes. Intel fixes.


2007/12/06 : 1.0.33
    ASL:
        - Microsoft Visual Studio 2008 support
        - ASL now uses Intel Thread Building Blocks Atomics (for copy_on_write)
        - XCode 3.0 Support
        - ASL 64 bit Mac support (However, APL still Carbon)
        - Mac shared library support
        - Mac universal build support update (to appear in boost 1.35)
        - ASL Command Shell environment for windows
        - lower_bound_n, upper_bound_n, and equal_range_n added
        - forms of lower_bound, upper_bound, and equal_range which take a key
        function
        - removed bound_if (use new form of lower or upper_bound instead).
        - minor work around to gcc optimizer bug in name_t
        - removed count_min_max (bad algorithm - use min/max_element and count instead).
        - boost patches for unused variables
        - significant boost patches for bjam to support new builds
        -
    GIL:
        - split the main test into small ones
        - removed unused Jamfile
        - updated GIL to version 2.1.2 (see GIL release notes for more details)

2007/11/01 : 1.0.32
    ASL:
        - With most of the re-factoring completed we're focusing on
        documentation and test cases (with some code cleanup). As a first pass
        we've "surfaced" all of ASL in the table-of-contents and are working to
        organize and structure the documentation. Our plan file on the wiki has
        more detailed information.
        - Finished removal of dependencies on boost::serialization.
        - Working to add Vista64 and Leopard64 to our supported builds.
        - Fixing bug in insert_parent for empty range.
        - (re)added overlay support for eve_evaluate.
        - Fixed a minor issue with type_info.
        - Made any_regular_t robust against struct padding/alignment settings.

    GIL:
        - Fixed a big-endian bug in dynamic_channel_reference
        - fixed C4503 warnings
        - removed ALL the unnecessary pragma warning directives from GIL sources
        - Added BitField parameter to bit_aligned_pixel_reference (there are
        rare cases where it cannot be automatically inferred). Also replaced
        tabs with spaces.
        - Added a BitField template parameter to bit_aligned_pixel_reference.
        (There are rare cases when it cannot be inferred)
        - fixed incorrect header files for std::bad_cast
        - changed int to std::ptrdiff_t in utilities.hpp and
        iterator_from_2d.hpp for problems under 64-bit platforms
        - removed several extra semi-colons after GIL_CLASS_REQUIRE
        - Updated to GIL 2.1.1 (see GIL release notes for more).

2007/09/06 : 1.0.31
    ASL:
        - Major re-factoring of the libraries (and supporting
        scripts/projects, etc.) so that adobe_source_libraries
        (ASL/GIL), adobe_platform_libraries (APL), and boost_libraries
        (Boost) are all peers at the top level directory. In upcoming
        releases we will be focusing on "completing" the core ASL
        portion of the library, with full documentation and test
        suites. ASL contains the layout engine, property model,
        algorithms, poly library and other components.  APL contains
        the "platform" widget code and other miscellaneous pieces. APL
        depends on ASL depends on Boost.


    GIL:
        - Added back a BitField template parameter to
        channel_reference and dynamic_channel_reference to fix an
        endianness bug.
        - iterator_from_2d now maintains the current y coordinate. It
        is needed in rare cases to properly determine equality of two
        iterators.
        - kth_element_type, kth_element_reference_type and
        kth_element_const_reference_type are now specified outside the
        class.
        - Fixed copy_pixels to compile on Intel compilers.
        - Resample (and thus resize) now allows the source and
        destination to be incompatible
        - Fixed VC projects to compile with the new paths of ASL and
        GIL

    Boost:
        - now lives under .../boost_libraries at the root level

2007/08/02 : 1.0.30
    ASL:
        - Improved documentation and tutorial for move library.
        - Property model library (Adam) now filters input cells from
        contributing.
        - Minor bug fixes for pair and closed_hash.

    Widgets:
        - Fix for preset saving.
        - More examples for Begin and some bug fixes.
    GIL:
        - Moved GIL out of the third party directory so that it no longer needs
        to be overlaid with
        an install of Boost.
        - Added code snippet examples to documentation.
        - Swapping the template arguments for color_element_type,
        color_element_reference_type and
        color_element_const_reference_type to take ColorBase first. This makes
        them consistent with
        the other similar metafunctions.
        - Fixed a bug in default channel conversion when the channel min value
        is not zero.
        - Made any C pointer to a pixel model HomogeneousPixelBasedConcept and
        HasDynamicXStepTypeConcept. Also made pixel_reference_is_mutable be
        automatically inferred.
        - Added pixel_reference_is_proxy and changed is_pixel_reference to
        pixel_is_reference
    Boost:
        - Moved to Boost 1.34.1

2007/07/05 : 1.0.29
    ASL:
        - We didn't make it as far as we would have liked for this release with
        vacations and the holiday - but we did manage to get some important bug
        fixes in so we're going to go ahead with the release.
        - Initial fix for visibility queue update issue with multiple optional
        panels toggled at once.
        - Fixed a type cast issue in unicode.hpp.
        - Added several functions to string_t and string16_t: clear, reserve, capacity, append,
        push_back, operator+=, and operator+ (as a free function). operator+() relies on the move
        library for efficient operation even when chained.
        - Bug fix for any_regular_t. x.assign(u); where x and u are any_regular_t no longer results
        in an any_regular_t inside an any_regular_t - it just does a simple assignment.
        - Killed const_once_name_t (use static_name_t instead).
        - Fixed a bug with the parser/virtual_machine with mismatched types in an any_regular_t that
        caused a failure on Win64.

    Widgets:
        - platform_image now at parity between mac and windows.
        - removed 'extern 'C'' from the platform cursor code.
        - fix to catch all throws out of make_cursor
        - added scrubability to image widget, bound via bind_controller to a dictionary
        - Minor edits for widgets to compiler for Win64.

    GIL:
        - Made any C pointer to a pixel model HomogeneousPixelBasedConcept and
        HasDynamicXStepTypeConcept. Also made pixel_reference_is_mutable be automatically inferred.
        - Added pixel_reference_is_proxy and changed is_pixel_reference to pixel_is_reference
        - Swapping the template arguments for color_element_type, color_element_reference_type and
        color_element_const_reference_type to take ColorBase first. This makes them consistent with
        the other similar metafunctions.
        - Fixed a bug in default channel conversion when the channel min value is not zero.
        - Added Doxygen code snippets for many GIL constructs
        - Minor fixes to web page and compile issue on VC7
        - GIL 2.1 release <http://stlab.adobe.com/gil/gil2.1_changes.pdf>

2007/06/14 : 1.0.28
    ASL:
        - Pardon our dust... We delayed this release so we could finish the work on the version_0
        libraries - we believe the code is now complete but we still have documentation to write
        (and not all the test cases are written, although Begin uses the libraries quite heavily).
        We will continue to work on test cases and documentation for version_0 for 1.0.29 although
        we aren't expecting any code changes, and certainly no breaking changes. A quick overview:
            -- move library: This reduces the overhead of copying objects with remote parts by
            automatically moving from r-values. Move sinks can accept movable r-values without
            copying.
            -- typeinfo library: (static) RTTI that doesn't depend on compiler RTTI for binary
            compatibility.
            -- any_regular library. Any regular is now movable and a move sink and leverages
            typeinfo library and removes dependency on v-table layout for binary compatibility.
            -- closed_hash library: An implementation of closed_hash_set and closed_hash_map. These
            containers are movable and move sinks.
            -- vector library: A high performance vector container which is movable and a move sink.
            -- string library: Added a string_t and string16_t which sit on the vector
            implementation and are movable objects.
            -- name library: added an static_name_t which replaces const_once_name_t (which is
            now deprecated and will be removed in 1.0.29). To construct an static_name_t simply
            write "static_name_t key_width = { "key_width" };" There is no need for once
            initialization (nor is it possible). Note that the c_str() for names are no longer
            guaranteed to be unique.
            -- array_t and dictionary_t are now typedefs for a vector<any_regular_t> and
            closed_hash_map<name_t, any_regular_t>. These are no longer copy-on-write objects and
            now rely on move to reduce the number of copies made.
        - The version_0 libraries are not the whole story though. There has been quite a bit of work
        in the poly library including:
            -- added poly_regular and poly_copyable
            -- poly is now a movable type
            -- simpler poly framework (eliminated multiple inheritance and poly_state)
            -- reduced heap allocations in poly
            -- better swap support
        - Pulled boost serialization support. ADOBE_STD_SERIALIZATION is very minimal (will be
        addressed in 1.0.29).
        - Added an ignore_unused() function that can make conditional usage of parameters a little
        easier to manage (thanks to Jeremy Siek).
        - Fixed a bug in message_handler_t (thanks to Eric Berdahl)
        - Fix for memory leak of cursors on Mac (thanks to Jesper Bache)
        - Moved to Boost 1.34.0.
        - Improved Macintosh event handler classes (thanks to Eric and Foster).
        - Start of drag and drop support as a sub-controller for widgets (Foster's efforts).
        - Some directory structure cleanup.
    GIL:
        -  Added kth_channel_view (like nth_channel_view, but operating on heterogeneous views).
        Added anonymous color space of 1 channel (which made resample work on grayscale pixels).

2007/05/03 : 1.0.27
    ASL:
        - clean up of file and class names. concept header files end in _concept.hpp. any_->poly_.
        - eliminate unnecessary heap allocation from attach_view
        - Eliminate copy of Placeable inside of layout engine. Rename add_view_element to
        add_placeable
        - Added high performance vector for 'version 0' binary compatibility.
        - added funcion_as_poly_view to allow boost::bind to work with adam w/o needing to create
        objects.
        - added no-debug-symbols config file to xcode steup; reworked the widget factory mechanism
        to add another layer to customizability for the clent
        - added vm extensions to support the loading of images
        (adobe/future/virtual_machine_extension); also added View-concept modeling to the image
        widget, so you can now specify a cell that Holds An Image In An Adam Sheet and it'll be
        reflected in the image widget (with auto-relayout)
        - Pulled out the parameter dictionary from eve into  eve_evaluate - new function called
        apply_layout_parameters can be used prior to adding items to eve. This removes the last
        vistages of parser/layout library entanglement. Yea! Doing this as part of improving RTL
        support. Much cleanup of header file includes came along (please watch your includes!)
        - Added explicit default construction for eve_client_holder::root_display_m data member.
        This allows client code to detect that the data member has not yet been assigned -- that
        there is not yet a root node, as opposed to the data member holding uninitialized garbage.
        - Update C++ conformance ofr gcc 4.3.0. Improve conceptgcc concept support
        - Added ADOBE_DELETE_PTR_SPECIALIZATION for EventRef
        - Add small object optimization to poly<>
        - Moving close_hash implementation for new version_0 work.
        - remove equality_comparable requirement from poly<>
        - Adding find_first_of_set() - suggestion came in an e-mail for this algorithm.
        - Fixed bugs in asl_net_setup.bat (thanks to Michael Marcin)

    GIL:
        - Improved the speed of image creation (avoids unnecessarily default-constructing every
        pixel). Added operator*(int, point2)
        - Calling the destructor of each pixel when destructing an image. Also fixed a potential
        memory leak if the pixel default constructor throws.

2007/04/05 : 1.0.26
    ASL:

        - replaced any_object and supporting machinery with poly<>
        template. Poly supports creation of regular runtime
        polymorphic value_types.
        - any_view, any_placeable, any_controller, any_key_handler, any_sequence_view, any_iterator
         upgraded to use the new poly<> mechanism
        - Add basic concept checking to any_regular.
        - Improve hybrid C++/C++++ concept support.
        - Removed superfluous uses of adobe:: throughout the codebase.
        - Simply swap: now ASL swap calls are always unqualified,
        (this is possible thanks to using std::swap; declaration in adobe/implementation/swap.hpp)
        - algorithm.hpp split into finer grained files
        - removed ISOLATION_AWARE macro from the MSVC build; also added inline editing of values to
        the list manager on win32; also cleaned up some message forwarding code
        - array_t::assign now has the same semantics as std::vector<T>::assign
        - adding array_t::assign and array_t::resize implementations (they'd been missing all this
        time!) -- thanks to Eric Berdahl for catching this one (and many others).

    GIL:
        - Various minor bug fixes, mostly with virtual image, make_step_iterator and tiff_io.
        - Major cleanup of the test files, especially image.cpp. Now using checksums to compare
        images with reference data. No external library dependencies are needed to build GIL
        regression tests (in the default mode), but that also means GIL i/o is not being tested.
        - Removed the large number of reference images.

        - MAJOR GIL update. GIL 2.0 released.
        GIL is now in the boost tree.

        See a better formatted version of this description here:
        //submission/adobe/documentation/gil/gil_standalone/gil/gil2.0_changes.pdf

        _________________
        GIL 2.0 Changes
        This document outlines the more significant changes in GIL introduced in version 2.0

        Overall changes:
        Added deprecated.hpp - a file that maps many of the deprecated names to current ones.
        Including it will help porting your code to GIL 2.0. After porting to GIL 2.0, however, make
        sure that your code works when this file is not included.
        File structure:
         - Directories follow the boost convention
         - Removed (flattened) the 'core' directory
         - Different models are usually now split in separate files
         - Renamed some files to be more consistent
         - Renamed classes, functions and template arguments with longer but clearer and more
         consistent names.
        - Now compiles with GCC 4.1.1.

        Changes in image, image_view, any_image and any_image_view: There are no longer global
        functions get_width(), get_height(), get_dimensions(), num_channels(). Use class methods
        width(), height(), dimensions() instead.
        Changes in models of pixel, pixel iterator, pixel locator, image view and image:
        There used to be different ways of getting to a pixel, channel, color space, etc. of an
        image view, pixel, locator, iterator and image. Some classes were using traits, other -
        member typedefs. In GIL 2.0 all pixel-based GIL constructs (pixels, pixel iterators,
        locators, image views and images) model PixelBasedConcept, which means they provide the
        following metafunctions:
        color_space_type
        channel_mapping_type
        is_planar
        num_channels

        And for homogeneous constructs we also have:
        channel_type
        Example:
        BOOST_STATIC_ASSERT((boost::is_same<
                       color_space_type<rgb8_image_t>::type,rgb_t>::value));
        BOOST_STATIC_ASSERT((is_planar<rgb8_planar_view_t>::value==true));

        To get the pixel type or pixel reference/const reference type of an image, image view,
        locator, and pixel, use member typedefs value_type, reference and const_reference.
        Changes in locator, image, image_view, any_image and any_image_view:
        Removed dynamic_x_step_t, dynamic_y_step_t, dynamic_xy_step_t, and
        dynamic_xy_step_transposed_t as member typedefs of locators and image views. Instead, there
        are separate concepts HasDynamicXStepTypeConcept, HasDynamicYStepTypeConcept,
        HasTransposedTypeConcept which all GIL provided locators, views and images model. Those
        concepts require a metafunction to get the corresponding type. Analogously, all GIL pixel
        iterators model HasDynamicXStepTypeConcept.
        - Fixed some bugs in defining reference proxies. Also added the required swap function for
        reference proxies, since the std::swap default does not do the right thing.
        - Added metafunctions iterator_type_from_pixel and view_type_from_pixel to allow creating
        standard iterators and views associated with a pixel type.
        Documentation:
        -The design guide and tutorial have been updated with GIL 2.0 changes. The syntax used in
        concepts is updated with the latest concepts proposal (though there is still some syntax we
        are using that is probably not legal)
        -The Doxygen documentation has been updated and restructured, so that concepts and models
        are closer in the browse tree.
        Channel changes:
        - The channel min/max value is now part of the channel traits. For all built-in types the
        channel range equals the physical range (as determined by numeric_traits<T>::max()).
        - Added scoped_channel_value, a channel adaptor that changes the operational range of a
        channel. bits32f is defined as a float with range 0.0 to 1.0
        - Added packed_channel_value, packed_channel_reference and packed_dynamic_channel_reference
        - models of channels operating on bit ranges. (see below for an example)
        - Added support for channel_convert between any of the GIL provided channel types. The
        operation is also consistent - conversion is done as a linear mapping that maps the min/max
        to the min/max
        - Added a comprehensive regression test for channels
        Pixel changes:
        - Major redesign of pixel-level constructs. Got rid of the channel accessors. color_base is
        renamed to homogeneous_color_base and is defined once, not for each color space. In general,
        the work needed to define a new color space is very minimal. Here is all you need to create
        an RGB color space with RGB and BGR ordering:
        // create channel names
        struct red_t {};
        struct green_t {};
        struct blue_t {};

        // create a color space
        typedef mpl::vector3<red_t,green_t,blue_t> rgb_t;

        // create layouts (color space with associated channel ordering)
        typedef layout<rgb_t> rgb_layout_t;
        typedef layout<rgb_t, mpl::vector3_c<int,2,1,0> > bgr_layout_t;

        - As the example shows, the color space now only specifies the set of channels. Their
        ordering in memory is specified by a layout. pixel is now templated over the channel value
        and layout:
        typedef pixel<bits8, bgr_layout_t> bgr8_pixel_t;

        - Color base is a first-class concept. Think of color base as a bundle of color elements. A
        pixel is a color base whose color elements are channels. A planar pixel iterator is a color
        base whose elements are channel iterators. A planar pixel reference proxy is a color base
        whose elements are channel references. A planar image can be represented as a color base
        whose elements are image planes, etc.
        All former pixel-level algorithms and accessors now operate on color bases. The elements of
        a color base can be accessed by physical or semantic index or by name. Example:
        rgb8_pixel_t rgb8(1,2,3);
        bgr8_pixel_t bgr8(rgb8);

        // Physical, semantic and named element accessors.
        assert(at_c<0>(bgr8) != at_c<0>(rgb8));
        assert(semantic_at_c<0>(bgr8) == semantic_at_c<0>(rgb8));
        assert(get_color(bgr8,blue_t()) == get_color(rgb8,blue_t()));

        // Physical element accessor whose index is specified at run time.
        // Only works for homogeneous bases
        assert(dynamic_at_c(bgr8,0) != dynamic_at_c(rgb8,0));
        assert(bgr8[0] != rgb8[0]); // for pixels only, operator[] does the same

        - channel names can no longer be accessed as members of the pixel (my_pixel.gray = 0). Use
        get_color instead, as shown above.
        Renamed:
        FROMTO
        equal_channels    static_equal
        copy_channels      static_copy
        fill_channels          static_fill
        generate_channelsstatic_generate
        for_each_channel   static_for_each
        transform_channelsstatic_transform
        min_channel          static_min
        max_channel        static_max
        channelat_c
        semantic_channelsemantic_at_c
        get_nth_channel     dynamic_at_c
        planar_ptr   planar_pixel_iterator
        planar_ref   planar_pixel_reference
        PixelConcept   HomogeneousPixelConcept
        HeterogeneousPixelConcept   PixelConcept


        - added metafunctions to get the k-th element of a color base (or its reference):
        kth_semantic_element_type
        kth_semantic_element_reference_type
        kth_semantic_element_const_reference_type

        color_element_type
        color_element_reference_type
        color_element_const_reference_type

        element_type
        element_reference_type
        element_const_reference_type

        Example:
        kth_semantic_element_type<rgb8_pixel_t,1>::type green = semantic_at_c<1>(my_rgb);

        my_pixel::num_channels is no longer available. To get the number of elements of a color base
        use the metafunction size:
        BOOST_STATIC_ASSERT(gil::size<rgb8_pixel_t>::value == 3);

        - Added heterogeneous_packed_pixel, a model of a pixel whose channels are bit ranges. For
        example, here is how to define a 16-bit RGB pixel in the '565' format:
        typedef const packed_channel_reference<uint16_t, 0,5,true> rgb565_channel0_t;
        typedef const packed_channel_reference<uint16_t, 5,6,true> rgb565_channel1_t;
        typedef const packed_channel_reference<uint16_t,11,5,true> rgb565_channel2_t;

        typedef mpl::vector3<rgb565_channel0_t,rgb565_channel1_t,rgb565_channel2_t>
                                                                   rgb565_channels_t;

        typedef heterogeneous_packed_pixel<uint16_t,rgb565_channels_t,rgb_layout_t>
                                                                      rgb565_pixel_t;

        - most color base algorithms now can take heterogeneous pixels (i.e. pixels each channel of
        which may have a different type). color_convert can operate on heterogeneous pixels with the
        exception of to/from RGBA.
        - got rid of pixel_traits. Use nested typedefs value_type, reference and const_reference or
        metafunctions implementing PixelBasedConcept (see below).
        - No more LAB and HSB color space, because there is no color conversion support implemented
        for these. New color spaces can be added with just a few lines of code, as shown above.
        - added a comprehensive regression test for pixels

        Pixel iterator changes:
        - got rid of pixel_iterator_traits. Use std::iterator_traits, PixelBasedConcept
        metafunctions or the following new metafunctions for pixel iterators:
        const_iterator_type
        iterator_is_mutable
        is_iterator_adaptor

        In addition, iterator adaptors have these new metafunctions:
        iterator_adaptor_get_base
        iterator_adaptor_rebind

        - renamed pixel_image_iterator to iterator_from_2d
        Pixel locator changes:
        Renamed is_contiguous to is_1d_traversable.
        Renamed membased_2d_locator to byte_addressable_2d_locator.
        Image view changes:
        - added algorithms uninitialized_fill_pixels and uninitialized_copy_pixels.
        - added method is_1d_traversable.
        Image changes:
        -Images don't allow for getting access to the pixels - only through views. Got rid of the
        ability to directly navigate the pixels of an image. So image no longer models STL�s random
        access container concept
        -The class image is no longer templated over the image view. It is now templated over pixel
        value and a Boolean indicating if the image is planar:
        typedef image<rgb8_pixel_t, true> rgb8_planar_image_t;

        -Added support for creating images with a new value to fill.
        -Images now invoke the default constructor of the pixels they allocate.
        -Renamed resize_clobber_image to Image::recreate. Also allowed for optionally specifying the
        initial value.
        Dynamic image changes:
        -No cross_vector_image_types and cross_vector_image_view_types. Instead, just create a
        vector to explicitly enumerate your types. This resulted in removing a lot of MPL related
        code and simplified significantly the design.

    Widgets:
        - Improved display_number implementation on both mac and windows to improve measurement.
        Also added the long-needed metrics::measure_text so we can measure strings without having to
        dance around widgets to make it happen
        - Now, instead of a global registry, there are these factory objects one can construct.
        There's also the 'default ASL' factory that is used by default, but the user can copy it and
        modify their copy to provide custom implementations. Clients can hold several of these factories
        and use them interchangeably as they so desire. Each factory, then, is a context of widgets, and
        by applying a different context to a layout, you get a different platform view constructed out
        the other end.
        - More list model work; selections can now be appended, removed, and set for the
        sequence_model; you can erase items by selection now, instead of by index (much more flexible);
        list.cpp for both mac and windows has been updated to the new system, though win32 still doesn't
        do a) in place editing, and 2) drag and drop support; fixed a couple bugs in the selection code
        - added an omitted overlay_factory function prototype
        - full erase implementation pushed to win32 list widget now.
        - widget_factory updates, and some more sequence model work.
        - visible_update_queue processing moved to be a function of the views of the model, not the
        controllers. This is a significantly more correct behavior.
        - the 'using CEL' macro in presets_common has been replaced with ADOBE_STD_SERIALIZATION
        - checkbox and slider on mac now properly respond to size attribute
        - fixed an issue where mac's panel was growing by one after every refresh of the dialog

2007/03/01 : 1.0.25
    ASL:
        - Extended the bjam test suite to include additional tests
        - Added adobe::sequence_model_t and supporting headers and sources. The sequence_model_t is
          a notify-on-write structure that manages a sequence of homogeneous items; it gives clients
          the ability to attach controllers and views to the model, allowing for correct architecture
          of a sequence-based MVC system. While not complete, it is a step in the right direction.
        - Introduced adobe/selection.hpp; adobe::selection_t is a container of indices that
          represent a selection. The intent is that this data strucuture will be used as the basis for
          a selection_model_t (or augmented to the sequence_model_t structure), as the architecture of
          a sequence-based MVC system calls for the need to model the selection of that sequence as
          well.
        - Added adobe/algorithm/selection_algorithms.hpp, a collection of algorithms centered around
          the new Selection concept. This includes selection_stable_partition, a modified version of
          stable_partition_selection, and its supporting algorithms.
        - Cleaned up the top level Jamfile
        - Changes to support gcc-4.1.1 and conceptgcc a5
        - Removing manifest nonsense, including manifest.jam, in favor of a patch to msvc.jam

    Begin:
        - Manifest file nonsense has been resolved, resulting in Win64 support

    Boost:
        - The boost patch file has been updated; please repatch your boost distribution
        - Added bjam code to support manifest file auto-embedding at link time (msvc)

    Documentation:
        - Altering the language slightly in the contributing page
        - Began documentation for adobe::sequence_model and supporting data structures and algorithms
        - Began documentation for adobe::selection_t and supporting data structures and algorithms

    Widgets:
        - Added stub headers for the platform-specific requirements within the widgets library
          (cygwin/other platform support)
        - Setting up the stub variable for widgets' Jamfile (cygwin/other platform support)
        - Simplify assemblage to be a collection of nullary function objects invoked in LIFO order
          upon destruction.
        - Added a list implementation for mac and windows (the mac is in a much better place than
          win32, though).

2007/02/01 : 1.0.24
    ASL:
        - Improved build scripts a little
        - Added Alex Stepanov's reduction algorithm and supporting files as described at
          http://www.stepanovpapers.com/notes.pdf
        - Added partition_selection_copy
        - Added algorithms for rotate, split_selection, and stable_partition_selection

    Begin:
        - Simplify overloading and namespace strategy: remove namespaces fn and adl, move to direct
          overloading strategy. Begin elimination of qualified calls.

    GIL:
        - Fixed typos
        - Updated the web page to include GIL wish list
        - Added link to GIL discussion forum from the GIL web site

    Documentation:
        - Updated the read_me
        - Updated copyrights

2007/01/04 : 1.0.23
    ASL:
        - Implement updated namespace strategy for widget library
        - Fixed a memory leak in the mac locale code

    Begin:
        - Added a path to the even client holder so subdialogs can be
          created relative to the files that are opened.
        - GIL: Posted a minor GIL patch (allowing again for channels to be
          assigned to grayscale pixels)

    Widgets:
        - New resource lookup scheme- it searches the whole stack of paths
          to find a resource instead of suing the top one only; also fixed a
          nasty subdialog issue for windows on the mac.
        - layout_attributes migrated from the concrete widget to a table in
          the widget factory registry
        - fix for sf.net bug 1533328 ] Overriding type 2 debouncing when
          debounce_t is in ca
        - default_extents removed; added layout_attributes to the factory
          registry
        - is_container query moved from the widget implementations to the
          widget factory registry

    GIL:
        - Allows channels to be assigned to grayscale pixels again.
        - Minor fixes to color_convert_view for dynamic images
        - Fixes for GCC 3.4 (adding any_color_converted_view)
        - Fixed assigning channels to grayscale pixels. Fixed dynamic image
          color converted view.

2006/12/07 : 1.0.22
    ASL:
        - Migrated modal_dialog_interface documentation from the wiki to
          the sources
        - build.bat now bails if it finds a space in the directory
          structure in which it lies
        - Added adobe/future/platform_primitives.hpp, along with
          platform-specific subheader files for carbon and win32
        - Added Adam reentrancy check to update for debug builds
        - Moved alert.cpp to widgets, fix adam-tutorial jamfile include
          requirements
        - A proper respect for the owner window implemented for
          handle_dialog
    Widgets:
        - Adobe folder now created if the presets widget needs it
          (thanks to Alan Shouls)
        - Adding a dialog_parent variable to pick_file, pick_save
        - Preset subdialogs now pass in a handle to the currently open
          dialog though handle_dialog
        - Removed display_t tree node wrapping nonsense
        - Removed yet more files, including client_assembler
        - Cleaned up display::insert<HWND>
        - ui_core_common, uncategorized_functions, and client_assembler
          all gone. We hardly knew ye.
        - Eliminated invisible_parent on win32!
        - Broke window_server out of client_assembler
        - ui_core.* and ui_core_implementation.* were removed
        - widgets' platform controls are now created at display::insert
          time instead of when the widget_t is ctor'd. This allows proper
          ownership to be established for a widget on win32, as well as
          several drastic code reductions and other improvements
        - removed control_t and event_dispatcher from win32
          implementation
        - Show focus rects when tabbing through dialogs on win32

2006/11/02 : 1.0.21
    ASL:
        - Boost patches. Please update your boost installation
        - Objective-C and Objective C++ compiler support for bjam under
          Darwin
        - Added any_key_handler.cpp, key_handler.cpp and keyboard.hpp for
          generic keyboard event handling
        - Making sure _all_ normal dictionary types serialize correctly via
          boost::serialization
        - Added a small tool to convert from CEL to the new xml archival
          format via boost::serialization
        - adobe::value_t has been replaced by adobe::any_regular_t
        - Number formatting (via number_formatter_t) has been greatly
          improved on win32 and mac to account for the currently selected
          machine locale. Changing the locale mid-execution of the application
          also updates the number formatters.
        - added a dynamic_cast of sorts for the Carbon CFTypeRef
          'polymorphic' type; added locale-specific information to the
          current_locale dictionary in Carbon to match win32
        - adding a 'first time' flag to debounce to override type 2
          debouncing the first time -- when the sheet initially sets the value
          of the view, type 2 debouncing is enabled. Then the immediately
          following operation by that controller is debounced mistakenly. This
          shorts that effect.
        - Update build.bat for VC8
        - Add flexibility to vsprops chain (thanks to Scott Byer)
        - Removed a mac os minimum version flag that didn't belong in the
          top level Jamfile because we have since specified them as features
          in darwin.jam (by Hubert Figuiere)
    Begin:
        - Added the ability to set property model and leyout sheet cells
          directly from within the Begin editor.
        - Added several new examples to stretch the functionality of some of
          our more recent widgets.
    Widgets:
        - Basic Accelerator (Alt-mnemonic) behavior in place for modal
          dialogs on win32; Underlined letters don't appear when a certain
          OS setting is checked, however; improvements still need to be made.
        - Default push button now looks like one on win32.
        - Enable tabbing through fields on win32, with proper field order.
          Many widgets types now work properly, though there are still some
          issues with checkboxes and sliders. I had hoped that asking Foster
          to add a call to IsDialogMessage to prior to Translatemessage would
          do the trick, but that in itself was not sufficient. It turns out
          that Win32 uses z-order of visible widgets with WS_TABSTOP for tab
          order. But our calls to SetParent when inserting into the display_t
          hierarchy were messing up the z-order. That is, SetParent forces the
          widget to the front of its peer group. Now we call SetWindoPos after
          SetParent to put the widget at the bottom of the z-order, so that
          ultimate z-order matches creation order (amongst peers). This worked
          well except for some compound widgets that did things in the other
          direction. The compound widget display_t inserts were also adjusted.
          Now new_doc and image_size ase following correct tab order. To Do...
          still need to work on highlighting, group order, and parent vs.
          owner issues for handle_dialog, but I beleive that the code in its
          present form is stable enough for milestone release.
        - modal dialog change -- now supports tabstops.
        - win32 edit text field now loses its selection when it goes out of
          focus.
        - fixed the issue where a slider with a negative number snaps
          incorrectly
        - fixed an issue on win32 when edit_number prefilter failed the
          value would still be sent to the sheet.
        - added disabled state for presets for when the bind_output cell is
          invalid by means of an invariant
        - result cell not required now for modal_dialog_interface -- you'll
          just get nothing back.
        - edit_number better handles the case when its text is in an
          intermediate state (e.g., blank)
        - holding down shift while scrubbing jumps the delta by a factor of
          10
        - better autorelease code for the cocoa bridge; it still says we're
          leaking a resource, though; I can't figure out why
        - better scrub behavior on mac for edit_number label
        - edit number label scrubby behavior working on win32.
        - custom cursor support for Mac (via Cocoa bridge) and win32 (win32:
          max 32x32 size cursor)
        - Popups now set themselves to 'custom' should there be no option
          they have that matches the cell they represent.
        - fix for edit_number trying to display a value for which it has no
          unit specified
        - presets are now views upon the result cell of the dialog -- they
          set themselves to a preset automatically if the state of the result
          cell matches one of them.
        - In a modal dialog, arrow up/down in an edit_number field
          increments the value
        - edit_number now communicates the selected unit through a cell in
          the layout sheet. This allows for multiple edit_numbers to be bound
          to the same cell, as well as allowing one to set which unit should
          show up at the initial opening of the dialog.
        - basic_sheet now has contributing(); handle_dialog uses this to get
          the layout display state
        - adding min/max values to edit number widget
        - With popups, if the value of the popup exists in the new set of
          menu items when the menu item set is repopulated, the menu item
          corresponding to the last value is selected
        - Improved modifier key handling
        - top-level window no longer 'scoots' in some cases when the client
          application munges with the window origin (by Scott Byer)
        - Added boost::serialization support. Enabling old serialization
          support under the conditional ADOBE_STD_SERIALIZATION
        - Many, many minor bug fixes on both platforms for varying issues.
        - Improvbed handle_dialog behaviors
        - Added a display_number widget -- a label bound to a cell for the
          purposes of displaying a numeric value
        - edit_number debounce_type now inits to
          std::numeric_limits<debounce_type>::min() to prevent an
          initialization to 0 from getting debounced
        - removal of widget-side localization of strings; pushed to vm via
          localize() operation
        - adding localize() to the VM -- also setting up presets so one can
          localize their internal strings
        - Widgetes: win32 label_t no longer snaps to the top-left corner of
          its parent window when measured
        - Fix Add presets "font" assert on win32
        - Fix for sf.net bug #156752 : up arrow key in edit num doesn't
          always add 1
        - Added scrollwheel support to edit_number on win32
        - Add parent parameter to handle_dialog (WIN32 only)
        - removed the static-text -> edit-text theme remapping that took
          place in the win32 label initializer, in favor of the better (and
          more general) remapping in metrics.cpp
        - revamped the resource system, combining both platforms under a
          single image format (targa), but it's easy to change. Cleaned up
          some APIs as a result of not needing to pass around a working
          directory. Also eliminates the need for resources to be built into
          the app -- just put them all in a directory and point the library to
          that directory, and Bob's your uncle.
        - adding a resource root path API
        - Clean up widget-factory (renaming factory routines), cleanup
          insert<> for display_t a little bit, remove stale overlay code.
    GIL:
        - Updated web site with latest fixes
        - Fixes to channel_max_value (compile error on VS2003). Fixes to
          resampling code in the numeric extension.
        - submitted a safer version of bilinear sampler
        - fixed two glitches in the implementation of resize_view() and
          sample().
        - Updated web page with regression tests and code samples, and with
          the latest GIL
        - Minor update to tutorial
        - Minor fixes to pixel (fixed gray_pixel_t::operator== with channel)
        - Updated the examples directory and the regression tests directory.
          Provided a more generic pixel constructor
        - added explicit channel type casting for gray-scale pixels into the
          tutorial.
        - test submitted the place holder for the directories.
        - initial submission of gil/test
        - examples added "return 0;" to each of the example files in example
          directory
        - completed an initial version of GIL example tests. These are the
          ones we have now: affine convolution histogram interleaved_ptr
          mandelbrot packed_pixel resize x_gradient.
        - documentation: Adding integration with Vigra example
        - Adding an example of packed pixel model
        - fixed some warnings for release builds under Mac.
        - fixed return types for channel_min_value() and channel_max_value()
        - Updated stand-alone version with latest changes
        - Fix to physical channel accessors of planar reference
        - Fixed up include files in i/o. Changes to nth_channel_view
        - Fixes to code bloat reduction mechanism
        - Fixed a 64-bit issue in numeric extension
        - Update to the web page. Added the numeric extension
        - Fixed a bug happening when taking nth-channel view of virtual
          image
        - Making gil_reinterpret_cast_c for constant casts
        - numerics: simplified interface of convolution with fixed kernel
        - Cleaned up #defines (prefixed all with GIL_ and #undef-ed the temp
          ones). Replaced reinterpret_cast with static_cast since the former
          is implementation-dependent.
        - propagate pixel_traits into the numeric extension
        - Added pixel_traits. Updated the documentation accordingly.
        - Making point2::operator[] alignment-safe
        - added different versions of copy_channels(), fill_channels(),
          generate_channels(), for_each_channel() and transform_channels()
        - swapped the order of source and destination pixels in
          copy_channels(). Now source pixel is before destination pixel
        - changed the name of equal_to() to equal_pixels()
        - Changing the template arguments from THIS_STYLE to ThisStyle
        - More cleanup of kernel
        - Simplifications to the numeric extension
        - added generate_pixles() (algorithm.hpp)
        - a better version of GIL_FORCEINLINE (gil_config.hpp)
        - added one header file (locator.hpp)
        - added generate_channels() (pixel_algorithm.hpp)
        - added versions of for_each_channel() that take input pixels by
          reference instead of const reference (pixel_algorithm.hpp)
        - Added numeric extension which is intended to be the place for
          numeric image algorithms. Currently, we provide correlations and
          convolutions with fixed-size and variable-size one-dimensional
          kernels and image re-sample.
        - Consistently using mpl::bool_ as the superclass for compile-time
          predicates
        - Fixed member initialization ordering in rgba-variant color spaces.
        - Simplified some color conversion stuff. Updated the documentation
          with the color conversion changes. Made NO_ASL to be determined
          automatically
        - MAJOR update (overhaul of GIL color-convert mechanism)
        - GIL now has a new infrastructure for doing color conversion which
          accepts use-defined conversion objects. These objects would work not
          only for use-defined pixel types but also for GIL built-in pixel
          types, i.e. the user can now overwrite the behavior of GIL built-in
          color conversions. Under the new infrastructure, the ONLY
          requirement for color conversion objects is that they have
          operator() defined for source and destination pixels that passed in
          for conversion. Such operator is invoked as void operator()(const
          SRC& src,DST& dst). All color-conversion related GIL structs and
          functions have been updated to conform to this new infrastructure.
          GIl still provides simple built-in conversions and overloads for
          functions to use the built-in ones.
        - moved planar_xxx_view for various color spaces out of
          image_view_factory.hpp to the own files. For instance:
          planar_hsb_view() is now in hsb.hpp instead of
          image_view_factory.hpp
        - moved color_convert_deref_fn from color_convert.hpp to
          image_view_factory.hpp
        - added a template argument to color_convert_deref_fn
        - moved the definition of _color_converter out of pixel.hpp into
          color_convert.hpp
        - added color_converted_view() which accepts use-defined
          color_conversion objects
        - in color_convert.hpp we added an overload for
          color_converted_view() which makes use of GIL default
          color_converter
        - changed the syntax for color_converter. Now it takes two
          arguments: source and destination and does not return anything
        - added color_converter_unary which is a metafunction that can
          convert a general color-conversion object into a unary function
          given the destination type
        - added a non-templated color-conversion objects
          (default_color_converter) based on color_converter
        - updated copy_and_convert_pixels() to be aware of user-defined
          color-conversion objects
        - added an overload for copy_and_convert_pixels() to use GIL default
          color-conversion objects
        - removed throw(...) in the generic implementation of
          channel_multiply(). Instead, we leave the function un-defined for
          unknown types. This way, we will get a compiler error instead of a
          run-time exception
        - updated ALL io functions (PNG,JPEG,TIFF) to make them aware of
          user-defined color-conversion objects
        - updated related places in dynamic extension
          (copy_and_convert_pixels, etc)
        - updated the test file
        - Added NO_ASL in the GIL stand-alone version
        - Fixed numerous small inconsistencies and typos in the code and
          documentation. Added some example code. Got it to compile on 64-bit
          systems.

2006/09/07 : 1.0.20
    property_model (adam.hpp):
        - Changed how contributing values are reported - much more precise though
        still not perfect (will change again when the new propery_model library is
        complete). This removed the need for current_mark() and the array of touched cells.

    ASL:
        - Changing the way the generated IDE libraries link to Begin (XCode); we
        used to link the libraries to those libraries that depended upon them
        directly; now we hold off any linking of all static libs until we get to
        the top of the build hierarchy (that is, the application). This is to
        cut back on potentially serious symbol conflicts.
        - Adding long branching option to xcconfigs
        - Extending the list of configurations for XCode (from 2 to 8)
        - Adding rset, a bidirectional lookup table good for a relatively small
        number of entries
        - Fixed a bug in enum_ops regarding unary -
        - adobe::regular_object has been renamed to adobe::any_object
        - adobe::number_formatter_t was extracted from the UI Core into a standalone
        set of sources
        - Introduced the controller, view and placeable headers, their any_*
        counterparts, and concept checking.
        - Begin elimination of client_assembler and ui_* files, breaking out widgets
        and, separately, their factories to reduce concrete widget dependencies on
        high level ASL components.
        - Factor out edit_text edit_num from client_assembler.
        - Eliminate some stale files, begin uniformazation of factory mechanism.
        - Added an exists() function for associative containers.
        - Updated documentation and added an example for any_iterator.
        - Added a build_docs perl script and updating the doxyfile a bit, too, in
        order to ease the process of building the documentation with known warnings
        - Minor fix to make number parsing properly locale agnostic. Need better
        number parsing code.

    GIL:
        - Extending read support of tiff files
        - Small typos to tutorial/design guide
        - Adding support for DeviceN images of up to 5 unnamed channels. Providing
        default typedefs for signed channel types.
        - Refactored the locator model so that creating virtual image views is easy.
        - Added as an example a virtual view of the Mandelbrot set.
        - Added file virtual_locator.hpp (see below for more)
        - There is a new concept, PixelDereferenceAdaptorConcept. It is a unary
        function that can be applied upon derefencing a pixel iterator/locator. Its
        argument may be anything and its result type must model PixelConcept
            template <boost::UnaryFunctionObject D>
            concept PixelDereferenceAdaptorConcept {
                typename const_t;         where PixelDereferenceAdaptorConcept<const_t>;
                typename value_type;      where PixelValueConcept<value_type>;
                typename reference;       where PixelConcept<reference>;      // may be mutable
                typename const_reference; where PixelConcept<const_reference>;// must not be mutable
                static const bool is_mutable;
            };
        - Provided a few models:
          - nth_channel_deref_fn:   Returns a grayscale pixel of the N-th channel of
          a given pixel type
          - color_convert_deref_fn: Returns the source pixel color-converted to a
          given destination pixel type
          - deref_compose:          Composes two pixel dereference adaptors. Very
          similar to std::unary_compose, but also propagates the typedefs.
        - Abstracted out common pixel locator functionality in a base class,
        pixel_2d_locator_base. There are now two concrete subclasses, memory-based
        locator, membased_2d_locator (which was the old pixel_2d_locator) and
        virtual_2d_locator. Both model PixelLocatorConcept. The base class provides
        defaults for many of the methods and typedefs required by the
        PixelLocatorConcept. Subclasses may override some with faster
        implementations.
        - Locator's old dynamic_step_t is replaced by four typedefs:
             - dynamic_x_step_t:            // a locator whose Y iterator can have a dynamic step
             - dynamic_y_step_t:            // a locator whose X iterator can have a dynamic step
             - dynamic_xy_step_t:           // a locator both of whose iterators can have a dynamic step
             - dynamic_x_step_transposed_t: // dynamic_xy_step_t that also has the X and Y iterators swapped
            (The equivalent changes are made to image_view and any_image_view models and concepts)
        - Locators and image views have the following new factory struct inside
        (part of the concepts):
            template <typename DEREF>
            struct add_deref {
                typedef ... type;
                static type make(const this_t& loc, const DEREF& nderef);
            };
        where 'this_t' is the encompasing class, a locator or an image_view and
        DEREF models PixelDereferenceAdaptorConcept. It allows us to construct the
        type of (and an instace of) a locator / image view that invokes the given
        derefence adaptor upon dereferencing.
        - Iterators have the equivalent factory struct, except that it is
        free-floating because iterators may be pointers:
            template <typename IT, typename DEREF>
            struct iterator_add_deref {
                typedef ... type;
                static type make(const IT& it, const DEREF& d);
            };
        - RandomAccess2DLocatorConcept now requires additional constructors/methods:
        - locator::locator(const locator& loc2, y_coord_t y_step);
            Makes a locator just like the given one except it has additional step
            vertically. Only valid for locators that have dynamic step in Y
        - locator::locator(const locator& loc2, x_coord_t x_step, y_coord_t y_step,
        bool transposed=false); Makes a locator just like the given one except it
        has additional steps horizontally and vertically. Only valid for locators
        that have dynamic step in both X and Y. In addition transposed must be true
        if and only if the locator is of transposed type (i.e. of type where X
        navigates vertically and Y horizontally)
        - bool locator::is_contiguous(x_coord_t width) const;
            Determines whether incrementing an x_iterator positioned at the last
            pixel of a row will move it to the first pixel of the next row, i.e. if
            the image is contiguous. Some algorithms may use this fact to provide
            faster versions. It needs to know the number of pixels per row (width).
        - Virtual locators are usually not contiguous whereas sometimes memory-based
        are.
        - y_coord_t locator::y_distance_to(const locator& loc2, x_coord_t xDiff) const;
            Returns the vertical distance between this and another locator from the
            same image view given their horizontal distance.
        - The following memory-based methods and constructors are moved to the
        memory-based locator (and are no longer part of PixelLocatorConcept)
                locator::locator(x_iterator,row_bytes);
                std::ptrdiff_t         locator::row_bytes();
                std::ptrdiff_t         locator::pix_bytestep();
        - Similarly, the corresponding methods are no longer part of
        ImageViewConcept and are removed from image_view:
                image_view::image_view(const point_t& sz, const x_iterator& x_it, coord_t row_bytes);
                image_view::image_view(coord_t width, coord_t height, const x_iterator& x_it, coord_t row_bytes);
                std::ptrdiff_t         image_view::row_bytes();
                std::ptrdiff_t         image_view::pix_bytestep();
        - The image view factory methods now use the new interface for locators and
        image views and can work for virtual image views as well.
        - flipped_left_right_view's return type is VIEW::dynamic_x_step_t;
        flipped_up_down_view's return type is VIEW::dynamic_y_step_t subsampled_view
        and rotated180_view return VIEW::dynamic_xy_step_t. rotated90cw_view,
        rotated90ccw_view and transposed_view return
        VIEW::dynamic_xy_step_transposed_t since they also swap the X and the Y.
        - color_converted_view uses the add_deref factory struct to add a color
        conversion dereference adaptor to the given view (unless the color
        conversion is a no-op)
        - nth_channel_view uses the add_deref factory struct to add an
        nth_channel_deref_fn dereference adaptor, which returns a grayscale pixel of
        the N-th channel of the source. For performance, it still has
        specializations for memory-based views to return a simple view or a step
        view, but it can now work with virtual images as well.
        - The image class assumes the locator is memory-based and has access to
        memory-specific methods such as row_bytes(), pix_bytestep(), etc. So do the
        factory methods interleaved_view and planar_xxx_view.
        - Provided an implementation for a virtual image view (new file
        virtual_locator.hpp). To create a virtual image view one needs to
        instantiate the image view with a virtual locator and provide a function
        object (model of PixelDereferenceAdaptorConcept) to be invoked upon
        dereferencing. The function object takes a point (the current coordinates)
        and returns a pixel (const) reference. The virtual image view uses
        position_iterator for the X and Y iterators - an iterator that maintains its
        coordinates
        - Added metafunctions for pixel references (analogous to the metafunctions
        for iterators, locators, views, images):
            - pixel_reference_type and derived_pixel_reference_type for constructing
            references
            - pixel_reference_is_basic, pixel_reference_is_planar,
            pixel_reference_is_mutable,
            - derived_iterator_type for constructing iterators
            - also added is_pixel_reference and make_pixel_reference for easy
            conversion between values and references
        - Various other changes:
             - The dereference_iterator_adaptor now accepts a
             PixelDereferenceAdaptorConcept as the function object. It dereferences
             the given iterator before invoking it
               (dereferencing used to happen inside the function object which had
               its own advantages)
             - Removed the pixel_data_is_real bool constant from pixel iterator's
             traits
        - Cleaned up the image.cpp test file. Removed testing stuff that is not
        released. Added an implementation of virtual image using Mandelbrot set and
        included it in the tests.
        - Also uncommented other tests commented out by Hailin and made them compile
        and run properly.
        - Fixing a few outdates examples in the documentation
        - Some fixes to doxygen warnings + updating external documentation.

    Widgets:
        - Adding label to reveal widget
        - Fixed the transparent scrubby slider so it moves with the label widget to
        which it is bound
        - You can now change the value of an edit_number field with the scroll
        wheel. Holding down the option key multiplies the increment amount by 10.
        you can specify the increment amount in the eve declaration of the
        edit_number field. (scroll wheel support is mac-only for now)
        - two-tiered presets for mac and windows
        - Initial removal of value_t in favor of model_type for concrete widgets
        - Added load/add/delete preset functionality
        - Added default presets to the preset widget implementation; it uses the
        resource directory for the widget to get the defaults
        - Added widget factory support headers
        - Selecting a popup's menu item by typing on the keyboard now works on win32

2006/08/03 : 1.0.19
    Adam (property_model):
        - API names are being refactored to reflect a model-centric view of
          the MVC system
        - Rename set_model_value to attach_setter
        - Rename monitor to set_model_value
        - Deferring token initialization to first use (removing from static
          time)
        - Using external polymorphism (view_t) for views instead of using
          signals

    ASL:
        - FLTK support has been dropped
        - Changed the type of the lex and the xml parser to take unsigned
          char* pointers instead of char*; this permits std::isspace, et al,
          to work without needing a cast
        - Adding const declarations to file_slurp's empty() and size()
        - Added adobe/localization.hpp, a resource registrar/provider of
          localization strings

    Widgets:
        - Adding adobe::debounce_t as a general standalone header
        - Fix for win32 image_t implementation to get the new targa code
          working
        - Fix for sf.net bug 1533374: Icons not drawing properly
        - Fix for sf.net bug 1533440: empty_containers tab group not wide
          enough
        - Fix for sf.net bug 1533240: Win32 Clicking in page of trackbar
          breaks inequality example
        - Fixing up edit_number handling when not bound to any item
        - adobe::popup_t has been pulled from client_assembler and put into
          its own set of standalone sources
        - Fix for sf.net bug 1530593: text wrap broken in win32
        - Created adobe/future/carbon, a directory with some utilities that
          came out of the widgets work that might be useful elsewhere.
        - Moving carbon event handler to adobe/future/carbon
        - Altering adobe::labelt_t::name_m on win32 so it measures properly
        - Transparent scrubby sliders for Carbon edit_number fields. We
          still need to be able to specify the min, init and max bounds in the
          eve definition, though
        - Added increment support for edit_number widgets via the arrow keys
          (specified by increment in the eve declaration)

    GIL:
        - Moved multimedia GIL documeentation to the //media/... depot

2006/07/06 : 1.0.18
    ASL:
        - Added a couple more macro tests to future/endian.hpp (for Mac)
        - Fix for SourceForge bug 1514948 :
          any_iterator<...>::implementation_t lacks a virtual dtor
        - MSVC 7.1 compatibility has been reinstated (thanks to Andy Little
          for the push)
        - Improved future/iomanip_pdf.hpp code
        - Improved future/iomanip_asl_cel.hpp code to produce correctly
          round-trippable serializations of the basic adobe types (dictionary,
          array, name, and value).
        - Replace layout_attributes_t parameter to measure routines with
          extents_t. Move layout_attributes_t adjustments to default_extents()
          for now.
        - Fix for SourceForge bug 1492999 : Problems building 1.0.16 on
          Linux

    Begin:
        - Fix if/else type error in checkbox_control example
        - Improved error message handling from OS routines on Win32
        - Extended "My Dialog" a bit to include alt_text and multiple button
          texts/functions
        - Binding OK in the opening "My Dialog" to the output again, so
          something comes up in the results
        - Image Size example updated with Tooltips

    Boost:
        - Boost patch update; Please repatch your Boost distribution.

    Build:
        - Universal Binary building working again on the Mac with XCode 2.3
        - Updating the minimum operating system requirements in order to
          build begin on the Mac; also added a blurb in the readme to that
          effect
        - Fix for SourceForge bug 1514410 : zuidgen not building on Mac OS X

    Documentation:
        - We have a new Wiki! Hopefully this one will fare better than the
          last: http://stlab.adobe.com/wiki/
        - Many devil-in-the-details documentation updates (thanks to Mike
          Engler for looking over the docs so thoroughly)
        - Updating CSS for Doxygen 1.4.7
        - Removing generated GIL docs from the html directory -- they are
          generated and uploaded directly to the standalone site now
        - Tweaks to the readme to make more explicit the need to replace a
          string in a pseudo-URL

    GIL:
        - Documentation:
            - Removed GIL docs from the main opensource site; referencing
              the standalone GIL site now (http://stlab.adobe.com/gil/)
            - Added a Breeze presentation of GIL
              (http://stlab.adobe.com/gil/presentation/index.htm)
            - Fixed an include bug in documentation
            - Updated GIL stand-alone documentation to use tar.gz in
              addition to zip
            - Fixed backslash in doxygen specification
            - Updated main.dox so that the ASL version of GIL will have the
              same page as before
            - Added doxygen documentation for the I/O extension
            - Updated the tutorial and design guide with the I/O extension
              and the changes in metafunctions
        - Simplified color spaces a bit, fixed a small bug in I/O and some
          more work on the test files to use the new I/O
        - Added two missing forward declarations in pxiel_algorithm.hpp
        - Fixed an incorrect referenec to jpeg_read_support in
          jpeg_io_private (should be jpeg_read_support_private)
        - I/O Update. Merged the read_and_convert files into the templated
          I/O files. Moved ASL I/O into a separate extension, asl_io
        - Removed the immutable image concept.
        - Removed the ability to do math operations on pixels (and planar
          pointers).
        - Updated the test files to compile without image_io.hpp.
        - Added support for reading and color converting images for all
           three image types. The main interfaces are
            - png_read_and_convert_view()
            - png_read_and_convert_image().
        - Small fixes in png_io.hpp, mainly comments
        - I/O Added boilerplates to I/O files
        - Major cleanup of I/O extension:
            - Created a file_mgr class that all I/O classes subclass from.
              It handles properly opening and closing a FILE. That simplified
              a lot of code that used to do low-level I/O
            - Removed intermediate global private I/O functions - they added
              unnecessary complexity
            - All the compile-time format checks now happen at the first
              line of the public functions. Removed any other places (in
              private) that do that
            - Removed lots of obsolete code in jpeg_io_private
            - BUG: Added a check in jpeg read view to throw if the view
              dimensions don't match the file
            - Removed the validity check API for png, because it was
              inconsistent with the other formats. It is now invoked every
              time you open a file for read and exception is thrown if the
              file doesn't work. We no longer need non-standard API for png
              (passing around the number of read bytes for example). Validity
              can be added back for all files by simply creating the reader
              class and catching any exception that it may throw.
            - Made resize_clobber_image reallocate only if the dimensions
              don't match. Removed that check from the I/O code
            - BUG: Added error checks for every call to a TIFF function
        - In dynamic_io files:
            - Moved all format checker predicates outside and before the
              corresponding dynamic read class (some were inside, some were
              after the class)
            - Created a method write_view in each dynamic_writer class - it
              corresponds to "read_image" in the dynamic reader classes
            - The dynamic writers now subclass from the corresponding writer
              classes (jpeg_writer, tiff_writer, png_writer). This is so that
              the code matches with the dynamic readers
            - Added quality parameter for the dynamic reader
            - BUG: Moved the apply_operation function object as a separate
              object outside the class. This is because apply_operation takes
              it by value. Passing the heavyweight reader/writer class was a
              bad idea - for example, its destructor cleans up I/O which used
              to cause a crash.
            - Simplified the six function objects into one generic one and
              moved it for now into io_error (the only file shared by all)
        - I/O Update:
            - Added reader and writer classes to have better exception safety
            - Fixed some other exception safety issues
            - Cleaned up the code
        - Updated tutorial and made the API more consistent to use .x() to
          get x-iterator and .x_pos() to get the x coordinate

    XML Parser:
        - Added extensions to provide for pseudo full-xml-document parsing
          (by Eric Berdahl)
        - Added extensions to handle properly-formed xml comments found
          during a parse (by Eric Berdahl)
        - Added bjam test suite and documentation for xml_parser_t (by Eric
          Berdahl)
        - Added logic to xml_parser_t::is_bom to recognize utf16 boms and to
          throw an exception when they are encountered. (by Eric Berdahl)
        - Migrated ASL implementation-detail xml_element_parser.hpp to
          first-class header xml_parser.hpp

    Widgets (UI Core):
        - Added a reveal widget. See
          http://stlab.adobe.com/wiki/index.php/Reveal_Widget
        - Added a general-use preset widget. See
          http://stlab.adobe.com/wiki/index.php/Preset_Widget
        - Added general tooltip functionality to both Mac and Win32 widget
          sets. While not all widgets support tooltips yet, adding support is
          relatively easy. See
          http://stlab.adobe.com/wiki/index.php/Tooltips
        - Rewrote metrics system on Win32 to remove object inheritance,
          thereby increasing dispatching performance and readability.
        - Tweaked the metrics code to return valid values in the case of a slider.
        - Fix edit_number alignment bug on windows
        - Fix for SourceForge bug 1179547 : link widget does not draw link icon
        - UI Core place() (aka set_bounds) calls now take place_data_t
        - Fixed a shifty label issue when it was being recalculated
        - Fixed a nasty heap violation in mac/metrics.cpp (by Eric Berdahl)
        - Figured out the resources issue on win32- the external bitmap
          files are no longer needed as a sibling to the final binary.
        - Set up the resources section of the adobe widgets library
        - Added error detection in convert_utf and win32 error message
          formatter (by Mike Engler)

2006/06/01 : 1.0.17
    ASL:
        - Eliminated eve's boost::signals client communication mechanism
          in favor of a Regular Object based mechanism (see for example:
          adobe::layout_element_t)
        - We have migrated to Perforce as our source code management solution
        - Greatly improved the speed of the net_install scripts by
          extracting boost once, not twice
        - Created the p4checkout scripts for *nix and Win32 to allow for
          anonymous Perforce checkout access
        - Support for MSVC 7.1 has been dropped.

    Begin:
        - Updating the Begin Jamfile to link statically with the C++
          library (Mac OS X, XCode 2.3)

    Boost:
        - Boost patch update; Please repatch your Boost distribution.
        - Removed the requirement that there be a source and a dest
          directory to patch boost

    Documentation:
        - Initial update of eve tutorial dox for layout_element. Still
          in progress...
        - Add layout_element_t documentation to eve.dox
        - CSS overhaul: faster and cleaner now, though may need some
          prettyness later
        - Updates to contributing documentation and policy, the read me,
          and the structure of the main web site
        - Made the readme file markdown-formatted and added an HTML
          version of it

    GIL:
        - Image align
        - Made num_channels constant available in locator, image_view and image
        - Added a PDF version of the new tutorial; split
          any_image_view.hpp out of any_image.hpp
        - Added resize_clobber_image for any_image
        - Tutorial updated and rewritten
        - Adding for_each_pixel_position and transform_pixel_positions.
          Adding "f" for floating point channels.
        - Fixing Doxygen errors
        - copy_view -> copy_pixels, copy_and_convert_view ->
          copy_and_convert_pixels, fill_view -> fill_pixels,
          transform_view -> transform_pixels, resample_view ->
          resample_pixels.
        - New GIL has a new image io extension.
            - This extension is in charge of reading and writing
              various image files.
            - Currently, we support JPEG, TIFF and PNG through
              interfacing with libjpeg, libtiff, libpng and zlib.
            - New formats will be added in the future to deal with
              other image formats.
            - Both static typed and dynamic typed images/views are supported.
            - Check out the installation notes (install.txt) for how to
              obtain individual libraries and how to use the new io
              extension.

    Widgets:
        - Fix control leak in factory

    XCode:
        - Bringing xcode projects up to 2.3
        - New xcconfig architecture for the 2.3 projects -- one
          top-level configuration file that selectively includes
          subconfiguration files based on the type and settings of your
          project
        - More darwin-specific features to in the top level Jamfile

2006/05/04 : 1.0.16
    ASL:
        - adobe/future/file_monitor.hpp was added; a utility for
          Mac/Win32 that runs a callback when a specified file is modified
          on-disk by a 3rd party.
        - Began support for building and binding against Lua
          (http://www.lua.org). To activate it you must download Lua and
          place it in /third_party/lua_tp/lua, and activate its building
          (see project-root.jam on how to do this).
        - Added adobe/algorithm/filter.hpp suggested by a question from
          Vladimir Samarskiy.

    Begin:
        - Fix for guide lists being appended instead of copied.
        - Fixed a bug where making a change in the Begin IDE would cause
          a relayout in the dialog.
        - Fix for sf.net bug [ 1475350 ] win32\label.cpp CreateWindowEx

    Build:
        - The top-level ASL Jamfile has been modified so that you can
          specify --asl-all-boost from the command line to build all boost
          libraries that need to be built. Alternatively, you can specify
          one or more --asl-boost-<library> to build a specific <library>
          within boost. The build will first build the boost files
          requested, then build ASL (including any boost libraries that
          ASL may need.) Examples:
            - bjam --asl-all-boost # builds all of Boost and all of ASL*
            - bjam --asl-boost-regex # builds Boost Regex and all of ASL*
            - bjam --asl-boost-date_time release # builds release
              version of Boost Date Time and all of ASL*
                * = Also builds the Boost libraries upon which ASL
                depends (currently filesystem, signals, and thread)
        - Note that you cannot build a specific subproject of ASL and
          use the --asl-* construct at the same time. To wit:
            - bjam --asl-boost-date_time adobe/test/begin #
              "--asl-boost-date_time" is ignored

    Documentation:
        - Added several documentation templates for use when documenting
          ASL components
        - View, Controller, and Placeable concept documentation drafted
        - Improvements to the forest documentation, including examples
        - Added revisions of the SGI STL documentation on the site.
        - Other general cleanup.

    GIL:
        - _MAJOR_ improvements to GIL-s run-time type specification. It
          is now possible to specify a vector of allowed image types
          directly when defining any_image (and equivalently for
          any_view). You can use mpl::vector or your own type that must
          model a few simple metafunctions.
        - You can also define implicitly the types of allowed images by
          as a cross-product of dimensions by specifying each dimension
          separately, for example [{8-bit, 16-bit} and {rgb,cmyk} and
          {planar+interleaved}] results in eight images.
        - You can also define type sets as unions of other type sets,
          for example a cross product plus an enumeration of image types.
        - Algorithms taking variants can auto-generate the appropriate
          return variant type. For example, passing any_view to
          subsample_view will return any_view over step view types.
        - Variants can now be used in nth_channel_view and
          color_converted_view.
        - Any_image and any_view are now subclasses of variant, which
          makes the code more readable and type-safe. Some member typedefs
          are defined. For example any_image<T>::view_t is the
          any_image_view class with corresponding view types.
        - The concept_space GIL extension is removed.
        - Added reduce and reduce with variant. Created a macro for
          standard GIL typedefs. Removed the TypeVector abstraction
        - Moved runtime-instantiated image support into a new GIL
          extension, dynamic_image

    Layout Engine (Eve):
        - Bug fix for frames (top of frame used as bottom).
        - Minor changes to Eve API suggested by Ralph Thomas to allow
          layout dimentions to be provided by evaluate. Also allows width
          and height to be supplied independently for both  evaluate and adjust.

2006/04/10 : 1.0.15
    ASL:
        - Fix cmath.hpp for gcc 3.4 and bug with name hiding.
        - Added adobe::behavior_t, a tree based defered processing mechanism used for queuing
          function in predefined order of execution.
        - Alert API now has a fixed number of buttons (3) and the API is cleaned up to reflect that.
    Layout Library (Eve):
        - Significant overhaul of guides to support mixed right-to-left (RTL) and
          left-to-right layouts as well as the adding support for constructing reversed layouts.
          See <http://stlab.adobe.com/group__right__to__left__layout.html>.
        - The guide rework for RTL also addressed other issues. Guides are now properly suppressed
          for items which are aligned centered or proportional, leaf node guide suppression now
          works correctly. See the align_reverse and align_right test example for Begin.
    Widgets:
        - Many changes to the ui core layout element concept design and implementation. Only a
          handful of the ui core widgets are moved over to the new design, and it is likely to stay
          that way until the design is shaken out a little further.
        - Added a widget_factory_registry, where UI widget developers will register their widgets
          in order that they may be used by Eve.
        - Added controller.*pp to the ui core.
    Begin:
        - Fixed a bug where the file contents wouldn't show up properly when the file is resident
          only in memory (the initial temp state).
        - Hide/show working with new behavior system -- multiple window support too.
        - Support for deferred window deletion upon closing of a dialog. This fixes sf.net
          bug 1436580.
    Build:
        - Building bjam with the release variant automatically forces fat binary building. (Darwin)
        - Multithreaded builds are now default instead of required in top level jam file
    Documentation:
        - Updated documentation for circular_queue library.
        - DOT graphs removed from ASL documentation
        - Other documentation tweaks and updates
    GIL:
        - Renamed recur{} in pixel.hpp to struct channel_recursion{}
        - Renamed set_channels to fill_channels and set_val to copy_channels
        - Changed the implementation of recur (or channel_recursion) and min_max_recur to stop at 0
          instead of -1
        - Removed max_channel_index from pixel{} as it is not used any more
        - Removed operator+(), operator-(), operator*() and operator/() from pixel.hpp. However,
          operator+=(), operator-=(), operator*=(), operator/=() are kept.
        - Added bits64 (which is double) to channel.hpp
        - Changed r,g,b for rgb and bgr to red, green, blue and changed r,g,b,a for rgba to
          red,green,blue,alpha
        - Changed c,m,y,k for cmyk to cyan, magenta, yellow and black
        - Changed l,a,b for lab to luminance,a,b
        - Changed per_channel_op to for_each_channel and per_channel_set_op to transform_channels
        - bgra is now inside GIL core (removed the extension). However, it is not yet added as part
          of any_image, any_view
        - In general, constness rules have changed at the pixel level. Constness used to be defined
          as having "const T" as the channel type template parameter.
        - "const T" is no longer used anywhere (whereas "const T*" and "const T&" are still used).
          For example pixel<const T,CS> is no longer legal; const pixel<T,CS> should be used
          instead.
        - Const planar reference is still pixel<const T&, CS>
        - Interleaved const iterator is const pixel<T,CS>* (not pixel<const T,CS>*).
        - Planar const iterator is planar_ptr<const T*,CS> (not planar_ptr<const T, CS>)
        - planar_ptr now takes channel iterator type (not channel type). It could be T* or const T*
        - planar_ptr now inherits from color_base<T*,CS> (instead of containing one). As a result
          the color plane pointers could be accessed directly: ptr.red, etc.
        - planar_ptr has the same logical/physical channel accessors as pixel, which allows using it
          in per-channel algorithms, such as for_each_channel, transform_channels, etc.
        - There is a new version of physical channel accessor that takes its index as a template
          parameter. Future heterogeneous pixels can only use this accessor.
        - The physical channels of a pixel can be accessed via operator[].
        - The physical pointers to color planes in a planar_ptr can be accessed via an accessor
          planar_ptr::channel_ptr(size_t)
        - The metafunctions iterator_type, step_iterator_type, pixel_xy_locator_type, view_type,
          image_type etc. now take channel iterator type (not channel type)
        - channel_traits::value_type has the constness stripped. There is no longer
          channel_traits::const_value_type.
        - interleaved_view factory method is now templated over pixel iterator (used to be pixel
          value). It also takes a pixel iterator to the first pixel (used to be channel pointer to
          the first channel)
        - a new typedef iterator_traits::channel_pointer is added to pixel iterator traits.
        - Removed dynamic_step_base_type typedef in places that don't need it
        - Modified the constructor of bgr and argb so that the order of the input is consistent with
          the order of channels.
        - Renamed fliplr_view() to flipped_left_right_view(), flipud_view() to
          flipped_up_down_view() and transpose_view() and transpose_view().
        - Renamed all *_type to *_t, except value_type, size_type, difference_type, allocator_type
          (which are STL legacy names).
        - Cleaned up the pixel class (used channel_traits instead of boost::type_traits).
        - Added support for the arrow operator of planar pointers.
        - Renamed channel_traits::min() to channel_traits::min_value(). Same for max.
        - Renamed pixel::nth_channel_t to pixel::kth_channel_t.
        - Changed the argument order of for_each_channel() and transform_channels(). Now the
          function object is the last one and the destination pixel in transform_channels() is the
          one next to the last. The same changes are applied to
          channel_recursion::for_each_channel() and channel_recursion::transform_channels() too.
        - In addition to re-ordering, I also added a return value for for_each_channel() and
          transform_channels(). Now their behaviors are more similar to the STL ones.
        - Added back the versions of for_each_channel() and transform_channels() with function
          objects passed in by either reference or const reference. The version with function
          objects passed in by value is still there. There is a macro to switch between them. I
          did some preliminary tests. Both versions yield the same code. Therefore, the "by-value"
          version is current ON by default.
        - Changed std::iterator_traits to GIL::pixel_iterator_traits. Replaced most of the places
          where std::iterator_traits is used. However, I didn't change those calls to the standard
          types which are defined already in std::iterator_traits. For instance,
          std::iterator_traits<IT>::value_type.
        - Modified the implementation in pixel_operations.hpp. Now they are more generic.
        - Renamed color_convert_view -> color_converted_view
        - Renamed channel() to semantic_channel()
        - Renamed physical_channel() to channel()
        - renamed pixel_xy_locator to pixel_2d_locator.
        - added pixel_t to image, image_view, pixel_2d_locator, planar_ptr_base,
          pixel_image_iterator, and various pixel_iterator_traits.
        - renamed xxx_tag to xxx_t. For instance, rgb_tag was renamed to rgb_t and lab_tag was
          renamed to lab_t.
        - Iterator traits cleanup and fixes
        - Created a custom planar reference class
        - modified transform_pixels() to return the the function object.
        - removed non-const access function in planar_ref.
        - moved planar_ref::operator->() to private and added planar_ref to the friend list of
          planar_ptr.
        - Simplification of channel accessors:
        - We don't need logical_channel_accessor at all - we simply call the
          physical_channel_accessor of the base color space. That saved lots of specializations.
        - Now that we have only physical_channel_accessor, I renamed it to channel_accessor. That is
          consistent with us calling the physical channel method simply "channel".
        - The channel_accessor now takes a generic type P (not our color_base). That class must
          provide the type of a reference and const reference of its n-th element. For our color
          base they are all the same. As a result, channel_accessor no longer needs to change when
          we introduce heterogeneous pixels. We only need to change color_base.
        - The only remaining glitch is where to put the homogeneous_color_base - right now it is in
          gil_config.hpp, but we should find a better place for it.
        - cached_location -> cached_location_t
        - All GIL typedefed constructs now have _t suffix
        - Fixed rgba8 type.
        - Updated tutorial, design guide and concepts.

2006/03/02
    ASL:
        - Added adobe::alert(), a one-function API that leverages the
          modal dialog integration kit to display an alert to the user with a
          consistent look and feel
        - Added adobe/future/endian.hpp, a file intended to help in the
          conversion of  data from in one endian-specific format to another
          (including "this computer's endianness")
        - adobe/bin/build.sh execution warnings have been fixed
        - adobe/future/timer.hpp operator == and operator < functions
          are now inlined (by Alex Dixon)
        - Changed the XCode projects to build the native architecture in
          debug mode, and Universal Binary in release mode
        - Created an action callback for the handle_dialog API to better
          handle the behavior of invoking a button
        - Jamfile improvements include pushing the arch flag on Mac OS X all the
          way back to darwin.jam. Also cleaned up the top-level JF to remove all
          references to arch, cleaning up a lot of cruft.
        - Removed stale stationery files
    Begin:
        - Split file_buffer_t into its own file (adobe/future/file.hpp
          and related source file) in Begin in preparation for fleshing it out
          some more. This includes some native code for picking a save file path,
          and leverages the adobe::alert() API to query the user regarding file
          operations
    Documentation:
        - Added documentation to the Adobe Source Libraries Wiki for ASL
          works in progress
        - Added the GIL tutorial to the tutorial listings
        - Edits from Mike Engler
        - Many other tweaks and updates
    GIL:
        - Added adobe/gil/extension/io/targa as a file format
          detect/import/export module to be used by the GIL image IO factory.
        - Added an ARGB pixel format specification
          (adobe/gil/extension/argb/argb.hpp)
        - Fixes for planar references
        - Moved std::copy_n stuff into detail
        - Wrapped immutable interleaved pixel iterators in an adaptor
          class that allows them to be constructed from mutable.
        - Large GIL checkin by Lubomir Bourdev:
            I took a stab at the GIL changes Sean proposed. I had to
            change some ASL files so please review my changes.

            This involved moving some stuff from GIL's utilities.hpp to
            various places in ASL.

            To preserve the standalone capability of GIL, the code that
            moved outside GIL is also copied inside GIL_config.hpp. GIL_config.hpp
            provides all the glue to make GIL work outside of ASL if necessary. Let
            me know if you can think of a better solution.

            The changes I made:

                - moved pixel_step_iterator to adobe/iterator.hpp
                - made all operations on pixel_step_iterator and point2
                  as global functions.
                - renamed round, round_down, round_up to iround, ifloor
                  and iceil (returning int) and provided the equivalent overloads for
                  point2. (See comments below.)
                - moved copy_n to a new file - adobe/algorithm/copy.hpp
                  and moved its helper functions in detail namespace
                - replaced size_t with std::size_t
                - renamed get_aligned with align
                - renamed plus to plus_asymmetric, since unlike
                  std::plus it takes arguments of different types (I use it to advance an
                  iterator by a given distance)
                - moved plus_asymmetric, inc, dec to
                  adobe/functional.hpp
                - replaced GIL::bind1st and GIL::bind2nd with
                  boost::bind
                - removed GIL::equal_to (and fixed a bug in the place that called it)

            Some remaining issues:

                - is plus_asymmetric a good name? I know Alex doesn't
                  like asymmetric plus, but I haven't thought of a way to avoid it in this
                  case.
                - what shall we do about rounding to an int? I currently
                  have iround, ifloor and iceil; I don't know if that's what you want to
                  have. GIL uses ptrdiff_t (since this is the distance type for pixel
                  iterators) which is an int. I am open to changing it to other types.
                  (Right now these have remained in GIL's utilities.hpp)
                - point2 is very similar to adobe::point_2d. Any
                  thoughts about combining them? One issue is that in the most abstract
                  form GIL concepts of images (and thus points) are N-dimensional. So
                  gil::point2 models N-dimensional point and has some API that would make
                  sense for higher dimensions but would not be very useful. Right now this
                  has remained in GIL's utilities.hpp, since it models a GIL concept.
    Widgets:
        - Add set_theme_name to metrics classes on win32 for more
          flexibility with static text.
        - Added a fix regarding Win32 unit edit text parity with the Mac
          (by Thomas Witt)
        - adobe::image_t has been added as a standalone UI core widget.
          Further documentation can be found on the ASL Wiki.
        - adobe::label_t and adobe::image_t are separated from the rest
          of the ui_core_implementation. This has required some deep cuts through
          the remainder of ui_core_implementation, but they are good cuts. Also,
          there is an issue present where the static_disabled text in those
          widgets that support it are not showing up in the dialog because they
          are not being set as children of their containing view, which is done
          once at construction time but static_disabled_text items are constructed
          as they are needed at runtime, and so do not get the chance to be
          embedded in their parents. The eventual (and correct) fix for this is to
          remove the cross-widget dependencies within edit_text, popup, and
          unit_edit_text so each is implemented completely independently of the
          others.
        - adobe::static_text_t has been removed and replaced with
          adobe::label_t, which is a better semantic name than static_text_t, and
          has a more refined API than its predecessor
        - Bug fixes for the mlte implementation
        - Release build fixes
        - The background of static text widgets in a tab container now
          draw with the correct background brush (by Ralph Thomas)
        - The virtual machine in ui_core's mac/metrics.cpp is now
          thread-local storage, so it only needs to be constructed one time,
          speeding up metrics expression evaluations
        - We started a new widget API / factory implementation that takes
          into account the various concepts that a UI element may model.

2006/02/02
    Adam:
        - Added adobe::basic_sheet_t, a class used to manage dialog
          state-related values, a subset of features of the full Adam
        - Added support for "weak" relations.
        - Better error reporting for over-contraint cases.
        - Unlinked cells are once again allowed to relate to each other when
          figuring out whether or not to pin a new value on the model.
        - Extended information at:
          <http://stlab.adobe.com/twiki/bin/view/AdobeSource/AdamEvolution>
    ASL:
        - Tabs have been replaced with 4 spaces in the entire ASL code
          repository. This includes everything but Doxygen-generated html files
          and IDE project files.
          base:
        - Added a nearest_cast<> from Mark Ruzon. Used it as an excuse to
          clean-up the rounding code in ASL to conform with C99 round() and added
          "path stability" to Adam solver.
        - adobe/cmath.hpp has been augmented to include support for more
          C99/TR1 extensions
        - Added modal_dialog_interface and adobe::handle_dialog, a
          self-contained API used to manage a modal dialog instance, leveraging
          Adam, Eve and the ui_core.
        - More fixes for stream error reporting has been changed; you now
          supply a callback to fetch a line from the stream and the stream
          formatter will use the callback before formatting the stream error
    Begin:
        - Fixing code from Eve parser rework and stripping of widget
          serialization and overlay from non-debug builds.
        - More examples added to demonstrate various Adam/Eve features
        - Much work has been done to improve manifest support for Win32.
          Manifest generation has been added MSVC 8 projects. bjam was augmented
          with a rule that copies the appropriate manifest file into place
        - Reworked the factory_token and setting the root widget display
          element in the eve_client_holder, which allows for us to extract the
          platform-specific root window view out of it and manipulate it (e.g.,
          run it under a modal loop).
    Boost:
        - Unpatched msvc.jam. Use conditional based on msvc-toolset:version
          in Begin's Jamfile.v2 instead. Fixes merged 8.0/7.1 settings bug
        - Please repatch your Boost distribution
    Eve:
        - Eve now supports a minimum height and width for containers; Both
          the Mac and Win32 ui_core implementations have been updated to leverage
          this
        - Hide/show updates
    GIL:
        - Many bug fixes and other improvements
        - Updates to the design guide. Added tutorial.
    IDE:
        - Moved iomanip files from widgets to asl_dev (Mac and Win32)
        - MSVC project now has a build directory and copy sheets and
          glossary from sources (by Thomas Witt)
    Widgets Library:
        - Began fixing up static text rect size on win32
        - Fixed a Mac bug where a widget's static disabled text's bounds and
          position were incorrect when shown at the first instance
        - Resolved the bug where the combo box drop down would take up the
          entire screen if there were enough elements. It turned out that this
          behavior was being explicitly set (!); now we will show enough room for
          all of the elements up to a maximum of 10 (at which point you will get a
          scrollbar). This still doesn't verify that the specified size can fit on
          the screen (e.g.: Windows PocketPC might have trouble), and uses a fixed
          value. (by Ralph Thomas)
        - The Mac ui_core implementation has been completely reworked,
          removing inheritance and cleaning up general code flow. Consequently the
          essentials() and widget serialization hooks have been stripped and
          stubbed
        - This commit partially addresses the Win32 nested group issue.
          There is more information on the fix in the comments of
          ui_core_implementation.cpp. Inner groups labels are struck through by
          the group line; there may be another message that we can catch to fix
          this... dialogs now look better, but still not perfect (by Ralph Thomas)
        - Fix Win32 edit_number crashes when label is empty. (by Thomas
          Witt)
        - Mac ui_core implementation caches widget metrics now, speeding up
          metrics-related operations (by Ralph Thomas)

2006/01/05
    - Added the Generic Image Library (GIL). This library allows for
      writing generic imaging algorithms with performance comparable to
      hand-coding for a particular image type.
    - Lubomir has been hard at work getting the GIL tutorial and design
      guide ready for release. We anticipate it within a couple weeks;
      hang in there.
    - Begin handles line endings a little better now. It accepts all
      three common line ending formats and converts all of them to the
      line ending for the current platform upon read-in and saving the
      Adam or Eve file.
    - The text in an edit_text widget now takes more than 1023
      characters (by Thomas Witt)
    - The edit_text widget now grows vertically (by Thomas Witt)
    - Added forest_smoke as a test framework for adobe::forest
    - Metrowerks CodeWarrior support has been completely dropped for
      this release. Consequently this allowed for a lot of directory and
      file clean-up.
    - Improved Adam error reporting.
    - Save button semantics in Adobe Begin have been changed so you can
      now save the modifications without first having to push the update
      button. (by Thomas Witt)
    - IDE projects were reworked and added back in to the ASL
      distribution. Most notably, both the MSVC 8 and XCode 2.2 projects
      take advantage of features in these IDEs to specify common
      settings in files external to the projects themselves. This allows
      for increased consistency and compatibility between projects.
    - Warning-as-error compilation settings turned back on; removed
      deprecated warnings for the time being
    - cmd_system now requires every command be unique. Fixed up the APIs
      to reflect that.
    - ASL now compiles against Boost 1.33.1. The old 1.33.0 patchfile
      has been removed, and a 1.33.1 patchfile has been started.
    - Patch Boost's cast.hpp for VS 2003 (Not needed for VS 2005)
    - Other bugs squashed and code enhancements
    - Multiple documentation tweaks and enhancements
    - Major rework of <adobe/forest.hpp>. I'm hoping one more pass for
      the next release and forest.hpp will be complete.
        - preorder and postorder iterator adaptors collapsed into a
          single edge_iterator<> adaptor.
    - fullorder iterator interface changed
        - depth() moved into a separate iterator adaptor, this makes
              fullorder iterators significantly less expensive.
            - leading() replaced by edge() which return
              forest_leading_edge (1) or forest_trailing_edge (0).
        - set_leading() replaced with assignment to edge().
        _ pivot() removed (can use edge() ^= 1 to pivot).
        - reverse_fullorder_cursor reworked to be fully symmetric. See
          discussion at:
          <http://stlab.adobe.com/twiki/bin/view/AdobeSource/ForestEdge>
        - added depth_fullorder_cursor adaptor which provides the old
          depth() feature to any fullorder iterator.
        - Changed implementation to make use of
          <adobe/iterator/set_next.hpp> which greatly simplifies the
          implementation and allows for developing generic node based
          algorithms.
        - Allow set_next() for forest::iterator and
          forest::child_iterator.
        - Added support for copy-construction, assignment, and equality-
          comparison for forest.
        - Added max_size(), empty(), and clear() member functions.
        - Added a root() member function which return an iterator.
          root() cannot be dereferenced but can be used with set_next().
        - Fixed a bug with the result of erase().
        - Added insert() which can insert a complete sub-forest.
        - Fixed several bugs with splice and added a result iterator
          to the spliced in elements (similar to insert).
        - Greatly improved the size() invalidation logic for splice().
        - Added a size_valid() member function which can be used to
          determine if size() will execute in O(1) or O(N) time.
        - Added a variant of splice which excepts a distance for the
          provided range to eliminate many cases where size would be
          invalidated.
        - Added a result to insert_parent() to return an iterator to the
          inserted element.
        - Added a reverse() member function which can reverse any
          sub-forest in the forest.
        - An empty forest no longer performs a heap allocation.
        - Added helper functions to construct reversed or depth ranges.

2005/12/01
    - Every header file now includes adobe/config.hpp for compatibility
      checking
    - Compiler-specific checks added to adobe/config.hpp for MSVC 8
    - Implemented missing pieces of event handling for Win32 widgets
      This fixes progress bars, edit, edit_num entry boxes and sliders
    - Warnings as errors (/WX) turned on under msvc-8.0
    - msvc 7.1 compiler compatibility changes
    - Updated VC manifest for common controls to work even in the
      presence of third party DLLs
    - Began work on an experimental menu/command system
    - MSVC 8 IDE projects added
    - Changed Adam contributing to report touched cells
    - Many compatibility updates to the Begin example dialogs: began the
      process of removing bevel_button from the ui_core implementation
    - Removing adobe::successor in favor of boost next/prior
    - Defined UNICODE for Win32 BBv2 builds
    - Sliders (among other controls) were broken on Windows. The reason
      seemed to be that the name for the control class expected by
      CreateWindowEx is different from the name expected by
      OpenThemeData in many cases. Implemented a conversion facility to
      recover the theme name from the class name so that opening the
      theme data no longer fails. Needs a bit more research, but seems
      to work for now
    - Documentation for ui_core enumerations added, along with
      refactoring suggestions
    - Updated the Begin projects for XCode 2.2
    - Updated Boost patch file; please re-patch your Boost distribution.
    - Other bugs squashed and code enhancements
    - Multiple documentation tweaks and enhancements

2005/11/03
    - The script-generated user-config.jam file has been updated for
      Win32 to extend toolset choices when building with multiple
      versions of MSVC. Please delete the user-config.jam file
      from the default location and rerun the build script.
    - Updates for MSVC 8 (Visual Studio 2005) compatibility
    - Updated ASL for compatibility with Boost 1.33.1. A patchfile
      for the new version of boost is included, however the auto-
      patching scripts are still set up for boost 1.33.0. You will
      have to patch boost 1.33.1 with the necessary patches manually.
    - Updates for CW 9.6 compatibility. CW 10 is not supported.
    - Support for creating polymorphic regular objects as an alternative
      to object oriented programming (adobe::regular_object)
    - Began a Boost 1.33.1 patchfile
    - #define _CRT_SECURE_NO_DEPRECATE for MSVC toolset in top level JF
      to defeat MS's "Safe STL" deprecation.
    - Pulled deprecated APIs from the Mac OS implementation.
    - ATSUI text measurement now instead of QuickDraw in the Mac ui_core
      implementation.
    - Added UTF16 conversion support to adobe/unicode.hpp.
    - Integrated xstring localization to Adobe Begin. Dialogs can now
      show in English, Japanese, Korean, German, and Pig Latin. A sample
      glossary for the image_size sample dialog is included in the
      release.
    - Updating FLTK port to be up to date with recent FLTK2 snapshots
      (anything from r4301+). Also added changes for some ASL changes.
      The FLTK window now supports resizing. Adding two functions for
      calculating and placing external widgets. There is a little usage
      example embedded in the Doxygen comments. (by Ralph Thomas)
    - Made an improvement to touch in Adam (and how it manifests in
      Eve, see the Image Size example). Relative order of touched cells
      is now prerved which gives more stable behavior and a better
      contributing set for scripting.
    - Improved markers in Adam so that getting the contributing values
      from a mark will include values that were derived, but now contribute
      because of a change in a conditional relation.

2005/10/06
    - Removed eve_container_defaults_proc_m and replaced it with
      adobe::eve_t::insert_element_t::initial_m
    - Many improvements to the Adobe Begin metrics code in MacOS X.
    - Experimental closed hash implementation (in future) - interesting
      but so far not useful.
    - Added bit-oriented implementations of the Secure Hash Algorithms
      SHA-1, -224, -256, -384, and -512, as well as a test framework.
    - Version tagging on built library artifacts has been removed
    - Varied changes to promote CodeWarrior compatibility
    - begin_no_bjam added: an XCode project that needs only the Boost
      libraries built under bjam in order to build Adobe Begin
    - Added Knuth's Dancing Links Algorithm, a toroid specialized
      container for the algorithm, and a Sudoku puzzle test app that
      utilizes and tests it. Also added an N-queens test app for the
      dancing links algorithm test platform.
    - Preorder proc and element handler in the xstring system are now
      context-sensitive
    - BBv2 now builds the Application Package for MacOS X, and installs
      Adobe Begin in a separate install folder in bin.v2. It does
      similar steps for Win32 as well, moving all necessary files to a
      separate install folder.
    - Updated Boost patch file; please re-patch your Boost distribution.
    - Other bugs squashed and code enhancements
    - Multiple documentation tweaks and enhancements

2005/09/01
    - Updated to Boost 1.33
    - Updated naming convention of the adobe-source libraries to match
      Boost 1.33
    - Updated Boost 1.33 patch file; please re-patch your Boost
      distribution. (Updated darwin.jam and gcc.jam with patches for
      XCode/GCC 4/MacTel)
    - Lots of xstring work to improve on the engine. Introduced a
      file_slurp buffer container that releases ownership of the buffer
      to the xstring system for parsing. This allows the xstring system
      to parse the buffer in-place, resulting in minimized memory
      allocations, and greatly speeding up the parsing process (15,000
      element parse down to ~ .25 seconds on a 1.67 GHz Powerbook G4).
    - Patch boost Jamfile.v2 for test module to build as static under
      darwin, just as on windows
    - Some CodeWarrior cleanup
    - build.sh bug fixes for Cygwin; also it now rebuilds bjam every
      time
    - Rewrite enum_ops to follow original intent of limiting operations
      to *specifically* designated enum types
    - Updated the all-in-one scripts, fixing bugs and started to add
      robustness
    - Extended iterator documentation. Thanks to Jon Reid for this
      contribution.
    - Fixed a nasty bug that was preventing Adobe Begin Win32 from
      launching when its path had any whitespace in it.
    - Other bugs squashed and code enhancements
    - Multiple documentation tweaks and enhancements

2005/08/04
    - Added hide/show visibility support to eve. Support "hacked" into
      panels in Adobe Begin (Mac ui_core). Eve support is solid but demo
      still needs some work.
    - Added filter_fullorder_cursor to forest library. Made
      pre/postorder cursors into proper adaptors.
    - Added proper reverse_fullorder_cursor to forest library. Thanks to
      Eric Berdahl for this contribution.
    - Add any_iterator (a polymorphic adaptor for iterators) and
      index_iterator (adapts functional indexing to an iterator). Thanks
      to Jon Reid for this contribution.
    - Put in a conservative fix for a bug in Adam for tracking indirect
      conditional contributions. For example:

        when(p) relate { x <== y; y <== x; }

      Evaluation of this conditional will be deferred until all but one
      of the cells in the relation has been resolved and all non-
      conditional relations have been resolved. Then p will be
      evaluated. If p is true it will contribute to the remaining cell.
      If p is false, it will contribute, indirectly, to all cells
      subsequently evaluated.

      This is a conservative answer, if p is false, it should only
      contribute to any cell which could have been effected by the
      evaluation of the remaining cell in the relation. Adam currently
      doesn't have a way to precisely track such an indirect dependency,
      this is a usably close approximation.
    - Replaced once_name_t with const_once_name_t - eliminates the evil
      const casting.
    - Renamed adobe::advance_of() to adobe::successor() and added
      adobe::predecessor().
    - Fixed problems with "Release" variants of applications under OS X.
    - Stationery projects for MSVC and XCode.
    - Updated Boost patch file; please re-patch your Boost distribution.
    - Universal version of Adobe Begin (MacTel/MacPPC).

2005/07/07
    - Mac OS X 10.4 (Tiger) support within BBv2. CodeWarrior support
      will slowly decline as MacTel/gcc 4 support increases
    - Adobe Begin has been moved from ./adobe/test/visual to
      ./adobe/test/begin
    - Increased XCode support, including updating to XCode 2.1
    - XCode- and MSVC-based projects now rely on BBv2 for building ASL
    - Automated building of ASL from within XCode- and MSVC-based
      projects for Adobe Begin
    - Library hierarchy reorganization to comply more with current
      industry standards
    - Added adobe/numeric.hpp, beginning similar support for <numeric>
      as we have for <algorithm>
    - Added any_itearator and index_iterator adaptor
    - Pulled UI code from Adobe Begin and made into its own library,
      adobe_widgets.lib
    - Renamed xstring-related files from "xstr" to "xstring"
    - Improvements made to the lexical analyzer, parser, and processing
      of xstrings
    - Updated Boost patch file; please repatch your Boost distribution
    - Some test app bug fixes (by Jamie Gadd)
    - Countless bugs squashed and code enhancements
    - Multiple documentation tweaks and enhancements, including removal
      of the frame (replaced it with a link table)

2005/06/03
    - Updated Boost patch file (by Jamie Gadd)
    - Added several test folders which should have been a part of
      yesterday's release (thanks to Dr. Bruce Stephens for the catch)
    - Added missing Jamfile.v2 (thanks to Dr. Bruce Stephens for the
      catch)
    - Beginnings of FLTK support (by Ralph Thomas)
    - Modified settings in boost_lib that prevented Mac CodeWarrior
      builds from succeeding
    - Removed many outdated and/or superfluous files from the CVS
      repository
    - Updated the read_me.txt with more build help and troubleshooting

2005/06/02
    - Buildsystem completely reworked using Boost Build version 2
      project files. Full builds and tests now supported. Tested under
      msvc, darwin, and cygwin gcc, though some issues remain with Adobe
      Begin. See Compiling the Libraries section below for more
      information
    - Windows builds now supported
    - Warnings are now treated as errors
    - ZUID class added (UUID-like identifiers that do not contain
      personal data)
    - thread_id operation added (thread-unique identifier)
    - Updated to_utf32 API in Unicode conversion for more streamlined
      throughput
    - Begin Win32 now has the Editor palette & a drag-and-drop
      targetable window
    - Win32 ui_core enhancements and bug fixes (by Ralph Thomas)
    - Continued XCode support (by David Catmull)
    - Adobe Begin proxy class removal (cont'd), improving ui_core public
      API (cont'd)
    - More Boost bug fixes; please repatch your Boost distribution
    - Countless bugs squashed and code enhancements
    - Multiple documentation tweaks and enhancements

2005/05/03
    - Added simple support for utf-8 <=> utf-32 (both single code points
      and strings)
    - Added xstring glossary lookup system for localizing strings in
      your app
    - Improved our Jamfile usage (by Ralph Thomas and Tobias Schwinger)
    - Improved performance in adobe::value_t by leveraging
      boost::variant
    - Improved ui, countless bug fixes in Win32 for Adobe Begin (by
      Ralph Thomas)
    - Adobe Begin modularization - factory work and more (by Ralph
      Thomas)
    - Adobe Begin proxy class removal (begun), improving ui_core public
      API (begun)
    - Many compiler warnings squashed (by Tobias Schwinger)
    - MinGW support (by Tobias Schwinger)
    - Countless bugs squashed
    - Multiple documentation tweaks and enhancements

2005/04/01
    - Started bringing Adobe Begin for Win32 online. Took a good chunk
      out of it, but bugs remain.
    - Reworking of Win32 metrics measurement / UI enhancement on Win32
      (by Ralph Thomas)
    - Reworking of bjam build environment within adobe-source (by Ralph
      Thomas)
    - Separation of platform code from express_viewer.cpp (by Ralph
      Thomas)
    - Added eve_smoke, a test app for verifying Eve2 view definitions
      from the command line
    - Reworked ui_core API for edit_text, popup, and unit_edit_text
      widgets
    - Improved ui_core fudge code, though it still needs improvement
      (Mac)
    - Added command line tutorial program for Adam (thanks to Peter
      Kummel for the discussion prompting the work)
    - Fixed several ASL compilation bugs for various platforms.
    - Added boost patch file to the distro (thanks to Nick Kraft for
      pointing out the omission)
    - Multiple documentation tweaks and enhancements
    - Added XCode projects for asl_lib, boost_lib, and Adobe Begin (by
      David Catmull)

2005/02/28
    - First public release! Lots of kinks to work out still, though.

2005/01/24
    - Many content-related documentation tweaks
    - Documentation compiling with Doxygen 1.40
    - Added documentation on Adam
    - Added documentation for table_index
    - Move iomanip to future directory
    - Added an Adobe Begin widget reference
    - Documentation CSS tweaks
    - Added build support for GCC using Bjam
    - ASL compiles with VC 7.1 again
    - Started working on the tutorial section of the site

2004/12/22
    - Initial release to external testers.

```
