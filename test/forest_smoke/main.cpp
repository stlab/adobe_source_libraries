#include <iostream>
#include <limits>
#include <new>
#include <cassert>

#include <adobe/algorithm.hpp>
#include <adobe/forest.hpp>
#include <boost/range.hpp>

template <typename R> // R is a depth adaptor range
void output(const R& f)
{
    typedef typename boost::range_iterator<R>::type iterator;

    for (iterator first(boost::begin(f)), last(boost::end(f)); first != last; ++first)
    {
        for (typename iterator::difference_type i(first.depth()); i != 0; --i)
        {
            std::cout << "\t";
        }
        if (first.edge() == adobe::forest_leading_edge)
        {
            std::cout << "<" << *first << ">" << std::endl;
        }
        else
        {
            std::cout << "</" << *first << ">" << std::endl;
        }
    }
}
    
int main()
{
    typedef adobe::forest<std::string> forest;
    typedef forest::iterator iterator;

    std::cout << "<- default construction and insert ->" << std::endl;
    forest f;
    iterator i (f.begin());
    i = adobe::trailing_of(f.insert(i, "grandmother"));
    {
        iterator p (adobe::trailing_of(f.insert(i, "mother")));
        f.insert(p, "me");
        f.insert(p, "sister");
        f.insert(p, "brother");
    }
    {
        iterator p (adobe::trailing_of(f.insert(i, "aunt")));
        f.insert(p, "cousin");
    }
    f.insert(i, "uncle");
    
    output(adobe::depth_range(f));
    std::cout << "<- copy construction and reverse ->" << std::endl;
    
    forest f2(f);
    iterator f2_grandmother(adobe::find(adobe::preorder_range(f2), "grandmother").base());
    f2.reverse(adobe::child_begin(f2_grandmother), adobe::child_end(f2_grandmother));
    
    output(adobe::depth_range(f2));
    
    std::cout << "<- reverse iterator ->" << std::endl;
    
    output(adobe::depth_range(adobe::reverse_fullorder_range(f)));

    std::cout << "<- node deletion ->" << std::endl;
    
    forest f3(f);
    iterator f3_aunt(adobe::find(adobe::preorder_range(f3), "aunt").base());
    iterator f3_uncle(adobe::find(adobe::preorder_range(f3), "uncle").base());
    f3.erase(adobe::leading_of(f3_aunt), ++(adobe::trailing_of(f3_uncle)));

    output(adobe::depth_range(f3));

    std::cout << "<- splicing 1 ->" << std::endl;

    forest f4(f);
    forest f5(f);
    iterator f4_aunt(adobe::find(adobe::preorder_range(f4), "aunt").base());

    std::cout << "Size of f4 pre-splice: " << static_cast<unsigned long>(f4.size()) << std::endl;
    std::cout << "Size of f5 pre-splice: " << static_cast<unsigned long>(f5.size()) << std::endl;

    // Note that because f4_aunt is on the leading edge, the spliced forest's
    // top nodes will be siblings to f4_aunt.

    f4.splice(f4_aunt, f5);    

    output(adobe::depth_range(f4));

    std::cout << "Size of f4 post-splice: " << static_cast<unsigned long>(f4.size()) << std::endl;
    std::cout << "Size of f5 post-splice: " << static_cast<unsigned long>(f5.size()) << std::endl;

    std::cout << "<- splicing 2 ->" << std::endl;

    forest f6(f);
    forest f7(f);
    iterator f6_aunt(adobe::find(adobe::preorder_range(f6), "aunt").base());

    std::cout << "Size of f6 pre-splice: " << static_cast<unsigned long>(f6.size()) << std::endl;
    std::cout << "Size of f7 pre-splice: " << static_cast<unsigned long>(f7.size()) << std::endl;

    f6_aunt = adobe::trailing_of(f6_aunt);

    // Note that because f6_aunt is on the trailing edge, the spliced forest's
    // top nodes will be children to f6_aunt.

    f6.splice(f6_aunt, f7);    

    output(adobe::depth_range(f6));

    std::cout << "Size of f6 post-splice: " << static_cast<unsigned long>(f6.size()) << std::endl;
    std::cout << "Size of f7 post-splice: " << static_cast<unsigned long>(f7.size()) << std::endl;

    return 0;
}
