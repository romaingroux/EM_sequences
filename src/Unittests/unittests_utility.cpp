#include <UnitTest++/UnitTest++.h>
#include <iostream>
#include <string>
#include <cmath>   // nan()

#include "Utility/Utility.hpp" // isNaN()


// Utility.cpp unittests
SUITE(utility_tests)
{
    // a message indicating the tests started
    TEST(start_message)
    {   std::cout << "start utility unittests..." << std::endl ; }

    // tests isNaN() function
    TEST(nan)
    {   double x = nan("") ;
        bool results = isNaN(x) ;
        bool expected = true ;
        CHECK_EQUAL(results, expected) ;
    }

}
