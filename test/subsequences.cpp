#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE at
#include <boost/test/unit_test.hpp>

#include "../src/subsequences.hpp"

#include <vector>
#include <iterator>
#include <sstream>

using std::vector;
using std::back_inserter;
using std::ostringstream;


BOOST_AUTO_TEST_CASE(unite_test)
{
    subset_t a = 0b011;
    subset_t b = 0b101;
    subset_t c = 0b110;


    BOOST_CHECK_EQUAL(unite(a, b), unite(b, c));
}

BOOST_AUTO_TEST_CASE(print_test)
{
    subset_t a = 0b00101011;
    subset_t b = 0b10101010;
    subset_t c = 0b00000000;

    ostringstream oss;
    print_subset(a, oss);
    BOOST_CHECK_EQUAL(oss.str(), "[0, 1, 3, 5]");

    oss = ostringstream{};
    print_subset(b, oss);
    BOOST_CHECK_EQUAL(oss.str(), "[1, 3, 5, 7]");

    oss = ostringstream{};
    print_subset(c, oss);
    BOOST_CHECK_EQUAL(oss.str(), "[]");
}

BOOST_AUTO_TEST_CASE(two_subsequences_test)
{
    relevant_subsets_t<2> expected1 = {{0, 2, 1, 3}};

    relevant_subsets_t<2> got1 = relevant_subsets<2>();
    BOOST_CHECK(got1 == expected1);


    relevant_subsets_t<3> expected2 = {{0, 4, 2, 6, 1, 5, 3, 7}};

    relevant_subsets_t<3> got2 = relevant_subsets<3>();
    BOOST_CHECK(got2 == expected2);
}


