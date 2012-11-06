#! /usr/bin/perl

@badfile_set =
(
    "adobe/adam_parser.hpp",
    "adobe/adam.hpp",
    "adobe/algorithm.hpp",
    "adobe/algorithm/filter.hpp",
    "adobe/algorithm/reverse.hpp",
    "adobe/poly_controller.hpp",
    "adobe/poly_placeable.hpp",
    "adobe/any_regular_fwd.hpp",
    "adobe/any_regular.hpp",
    "adobe/poly_view.hpp",
    "adobe/array_fwd.hpp",
    "adobe/array.hpp",
    "adobe/circular_queue.hpp",
    "adobe/cmath.hpp",
    "adobe/controller_concept.hpp",
    "adobe/conversion.hpp",
    "adobe/copy_on_write.hpp",
    "adobe/dictionary_fwd.hpp",
    "adobe/dictionary.hpp",
    "adobe/documentation/asl/adam_evaluate.dox",
    "adobe/empty.hpp",
    "adobe/eve_parser.hpp",
    "adobe/extents.hpp",
    "adobe/forest.hpp",
    "adobe/functional.hpp",
    "adobe/future/debounce.hpp",
    "adobe/future/timer.hpp",
    "adobe/future/widgets/headers/widget_utils.hpp",
    "adobe/future/widgets/headers/edit_number.hpp",
    "adobe/future/widgets/headers/widget_factory.hpp",
    "adobe/implementation/lex_shared_fwd.hpp",
    "adobe/implementation/lex_stream_fwd.hpp",
    "adobe/implementation/lex_stream.hpp",
    "adobe/implementation/token.hpp",
    "adobe/istream.hpp",
    "adobe/iterator.hpp",
    "adobe/layout_attributes.hpp",
    "adobe/name.hpp",
    "adobe/static_table.hpp",
    "adobe/typeinfo.hpp",
    "adobe/value_fwd.hpp",
    "adobe/value.hpp",
    "adobe/view_concept.hpp",
    "adobe/xml_parser.hpp",
    "adobe/xstring.hpp",
);

$here = `dirname $0`;

chomp $here;

`rm -rf $here/html`;

print "Now building documentation...\n";

@result_set = `doxygen $here/doxyfile 2>&1`;

open(BADFILE, "> $here/known_issues.txt");

foreach $line (@result_set)
{
    chomp $line;

    $found = 0;

    foreach $file (@badfile_set)
    {
        if ($line =~ /$file/)
        {
            $found = 1;
            last;
        }
    }

    if ($found)
    {
        $old_fh = select(BADFILE);
        print "$line\n";
        select($old_fh);
    }
    else
    {
        $old_fh = select(STDOUT);
        print "$line\n";
        select($old_fh);
    }
}
