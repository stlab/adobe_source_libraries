/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <functional>
#include <iostream>

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <adobe/test/check_regular.hpp>
#include <adobe/test/check_less_than_comparable.hpp>
#include <adobe/test/check_null.hpp>

#include <adobe/copy_on_write.hpp>
#include <adobe/memory.hpp>
#include <adobe/string.hpp>

namespace {

template <typename T>
class noisy_allocator;

template < >
class noisy_allocator<void>
{
  public:
    void*               pointer;
    typedef const void* const_pointer;
    typedef void        value_type;
    template <class U> struct rebind { typedef noisy_allocator<U> other; };

    friend inline bool operator==(const noisy_allocator&, const noisy_allocator&)
    { return true; }

    friend inline bool operator!=(const noisy_allocator&, const noisy_allocator&)
    { return false; }
};

std::size_t noisy_check_allocation(bool count = false)
{
    static std::size_t alloc_count_s(0);

    std::size_t prev_allocation(alloc_count_s);

    if (count)
        ++alloc_count_s;
    else
        alloc_count_s = 0;

    return prev_allocation;
}

std::size_t noisy_check_deallocation(bool count = false)
{
    static std::size_t dealloc_count_s(0);

    std::size_t prev_deallocation(dealloc_count_s);

    if (count)
        ++dealloc_count_s;
    else
        dealloc_count_s = 0;

    return prev_deallocation;
}

template <typename T>
class noisy_allocator
{
 public:
    typedef std::size_t         size_type;
    typedef std::ptrdiff_t      difference_type;
    typedef T*                  pointer;
    typedef const T*            const_pointer;
    typedef T&                  reference;
    typedef const T&            const_reference;
    typedef T                   value_type;
    template <typename U> struct rebind { typedef noisy_allocator<U> other; };
    
    noisy_allocator() { }
    template <typename U>
    noisy_allocator(const noisy_allocator<U>&) { }
    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    pointer allocate(size_type n, noisy_allocator<void>::const_pointer = 0)
    {
        if (n > max_size()) throw std::bad_alloc();
        pointer result = static_cast<pointer>(::operator new(n * sizeof(T), std::nothrow));
        if (!result) throw std::bad_alloc();
        std::cout << "  alloc @ " << static_cast<void*>(result)
                  << "; sizeof(T): " << sizeof(T);
        if (n != 1)
            std::cout << "; n: " << n;
        std::cout << std::endl;
        noisy_check_allocation(true);
        return result;
    }
    void deallocate(pointer p, size_type)
    {
        ::operator delete(p, std::nothrow);
        std::cout << "dealloc @ " << static_cast<void*>(p)
                  << "; sizeof(T): " << sizeof(T)
                  << std::endl;
        noisy_check_deallocation(true);
    }
    size_type max_size() const { return size_type(-1) / sizeof(T); }
    void construct(pointer p, const T& x) { adobe::construct(p, x); }
    void destroy(pointer p) { adobe::destroy(p); }

    friend inline bool operator==(const noisy_allocator& x, const noisy_allocator& y)
    { return true; }

    friend inline bool operator!=(const noisy_allocator& x, const noisy_allocator& y)
    { return false; }
};

template <typename R, typename T>
R make_value(const T& x)
{ return R(x); }

template <>
adobe::string_t make_value(const long& x)
{
    std::stringstream s;
    s << x;
    return adobe::string_t(s.str());
}

template <typename CowType>
void test_copy_on_write()
{
    enum {
        is_noisy = boost::is_same<typename CowType::allocator_type,
                                  noisy_allocator<typename CowType::value_type> >::value
    };

    typename CowType::value_type (*mv)(const long&) = &make_value<typename CowType::value_type, long>;

    if (is_noisy)
    {
        // reset counters
        noisy_check_allocation();
        noisy_check_deallocation();

        std::cout << "Testing ";
        serialize(adobe::type_info<CowType>(), std::ostream_iterator<char>(std::cout));
        std::cout << "..." << std::endl;
    }

    // Test default constructor
    {
        CowType value_0;
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 1, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 0, "deallocation count mismatch");
    }

    // Test basic concept requirements
    {
        CowType value_1(mv(1)); // allocation
        CowType value_2(mv(2)); // allocation
        CowType value_3(mv(3)); // allocation
    
        //regular
        adobe::check_regular(value_1);
    
        //operator<
        adobe::check_less_than_comparable(value_1, value_2, value_3, std::less<CowType>());
        
        //operator>
        adobe::check_less_than_comparable(value_3, value_2, value_1, std::greater<CowType>());
    
        CowType value_test(mv(1)); // allocation
    
        BOOST_CHECK_MESSAGE(value_1 == value_test, "equality of non-identical values");
        BOOST_CHECK_MESSAGE(value_2 != value_test, "equality of non-identical values");
    
        BOOST_CHECK(value_test.unique_instance());
        
        value_test = value_2; // deallocation
    
        BOOST_CHECK(!value_test.unique_instance());
        BOOST_CHECK(value_test.identity(value_2));
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 4, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 4, "deallocation count mismatch");
    }

    // Test basic move semantics
    {
        CowType value_1(mv(42)); // allocation
        CowType value_2(mv(21)); // allocation
        CowType value_move(adobe::move(value_1));
    
        BOOST_CHECK_MESSAGE(value_move != value_1, "move failure");

        value_move = adobe::move(value_2); // deallocation
    
        BOOST_CHECK_MESSAGE(value_move != value_2, "move failure");
        BOOST_CHECK_MESSAGE(value_1 == value_2, "move failure"); // both should be object_m == 0
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 2, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 2, "deallocation count mismatch");
    }

    //Test custom allocator constructor and set
    {
        typename CowType::allocator_type my_allocator;
        CowType value_4(my_allocator); // allocation

        value_4.write() = mv(4);
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 1, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 1, "deallocation count mismatch");
    }

    // Test copy-assignment using null object_m
    {
        CowType foo(mv(1)); // allocation
        CowType bar(adobe::move(foo));

        foo = mv(2); // allocation
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 2, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 2, "deallocation count mismatch");
    }

    // Test copy-assignment using non-null object_m
    {
        CowType foo(mv(5)); // allocation
        CowType bar(foo);

        BOOST_CHECK(bar.identity(foo));

        bar = mv(6); // allocation

        BOOST_CHECK(bar.unique_instance() && foo.unique_instance());
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 2, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 2, "deallocation count mismatch");
    }

    // Test move-assignment using null object_m
    {
        CowType                      foo(mv(1)); // allocation
        CowType                      bar(adobe::move(foo));
        typename CowType::value_type value(mv(2));

        foo = adobe::move(value); // allocation
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 2, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 2, "deallocation count mismatch");
    }

    // Test move-assignment using unique instance
    {
        CowType                      foo(mv(1)); // allocation
        typename CowType::value_type value(mv(2));

        foo = adobe::move(value);
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 1, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 1, "deallocation count mismatch");
    }

    // Test move-assignment using new allocation
    {
        CowType                      foo(mv(1)); // allocation
        CowType                      bar(foo);
        typename CowType::value_type value(mv(2));

        foo = adobe::move(value); // allocation
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 2, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 2, "deallocation count mismatch");
    }

    // Test write() using unique instance
    {
        CowType foo(mv(1)); // allocation

        foo.write() = typename CowType::value_type(mv(2));
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 1, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 1, "deallocation count mismatch");
    }

    // Test write() using new allocation
    {
        CowType foo(mv(1)); // allocation
        CowType bar(foo);

        foo.write() = typename CowType::value_type(mv(2)); // allocation
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 2, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 2, "deallocation count mismatch");
    }

    // Test read()
    {
        CowType foo(mv(1)); // allocation

        BOOST_CHECK_MESSAGE(foo.read() == typename CowType::value_type(mv(1)), "read error");
        BOOST_CHECK_MESSAGE(static_cast<typename CowType::value_type>(foo) == typename CowType::value_type(mv(1)), "read error");
        BOOST_CHECK_MESSAGE(*foo == typename CowType::value_type(mv(1)), "read error");
        BOOST_CHECK_MESSAGE(*(foo.operator->()) == typename CowType::value_type(mv(1)), "read error");
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 1, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 1, "deallocation count mismatch");
    }

    // Test swap
    {
        CowType foo(mv(1)); // allocation
        CowType bar(mv(2)); // allocation

        swap(foo, bar);

        BOOST_CHECK_MESSAGE(foo.read() == typename CowType::value_type(mv(2)), "swap error");
        BOOST_CHECK_MESSAGE(bar.read() == typename CowType::value_type(mv(1)), "swap error");
    }
    // Check
    if (is_noisy) {
        BOOST_CHECK_MESSAGE(noisy_check_allocation() == 2, "allocation count mismatch");
        BOOST_CHECK_MESSAGE(noisy_check_deallocation() == 2, "deallocation count mismatch");
    }
}
} // namespace

ADOBE_NAME_TYPE_1("noisy_allocator", noisy_allocator<T0>)

BOOST_AUTO_TEST_CASE(CowType_allocator_rtti)
{
    using namespace adobe;

    {
    typedef copy_on_write<int> cow_t;
    type_info_t t = adobe::type_info<cow_t>();

    serialize(t, std::ostream_iterator<char>(std::cout));
    std::cout << std::endl;

    BOOST_CHECK(!t.requires_std_rtti());
    }
    {
    typedef copy_on_write<int, std::allocator<int> > cow_t;
    type_info_t t = adobe::type_info<cow_t>();

    serialize(t, std::ostream_iterator<char>(std::cout));
    std::cout << std::endl;

    BOOST_CHECK(t.requires_std_rtti());
    }
}

BOOST_AUTO_TEST_CASE(copy_on_write)
{
    // test nonmovable type with capture_allocator
    test_copy_on_write<adobe::copy_on_write<int> >();

    // test nonmovable type with std::allocator
    test_copy_on_write<adobe::copy_on_write<int, std::allocator<int> > >();

    // test nonmovable type with noisy_allocator
    test_copy_on_write<adobe::copy_on_write<int, noisy_allocator<int> > >();

    // test movable type with capture_allocator
    test_copy_on_write<adobe::copy_on_write<adobe::string_t> >();

    // test movable type with std::allocator
    test_copy_on_write<adobe::copy_on_write<adobe::string_t, std::allocator<adobe::string_t> > >();

    // test movable type with noisy_allocator
    test_copy_on_write<adobe::copy_on_write<adobe::string_t, noisy_allocator<adobe::string_t> > >();
}
