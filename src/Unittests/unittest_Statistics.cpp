
#include <iomanip>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <UnitTest++/UnitTest++.h>

#include "Statistics/Statistics.hpp"
#include "Matrix/Matrix2D.hpp"
#include "Utility/Utility.hpp"


using namespace std;


// Statistics.cpp unittests
SUITE(Statistics_testsuit)
{
    TEST(start_message)
    {   cout << "Starting Statistics tests..." << endl ; }

    // tests mean()
    TEST(mean_weigthed)
    {   // tolerated error for equality testing
        double error = 0.0001 ;

        vector<double> x  = {-2.5, 4.3, 5.0, -10,  33.0} ;
        vector<double> p1 = {0.2,  0.2, 0.2,  0.2, 0.2} ;
        vector<double> p2 = {0.1,  0.2, 0.3,  0.4, 0.0} ;
        vector<double> p3 = {1.0,  2.0, 3.0,  4.0, 5.0} ;
        vector<double> p4 = {0.0,  0.0, 0.0,  0.0, 0.0} ;

        double expected1   = 5.96 ;
        double expected2   = -1.89 ;
        double expected3   = 9.74 ;
        bool   expected4   = true ;

        CHECK_CLOSE(mean(x, p1), expected1, error) ;
        CHECK_CLOSE(mean(x, p2), expected2, error) ;
        CHECK_CLOSE(mean(x, p3), expected3, error) ;
        CHECK_EQUAL(isNaN(mean(x, p4)), expected4) ; // NaN, division by 0
    }

    // tests sd()
    TEST(sd_weigthed)
    {   // tolerated error for equality testing
        double error = 0.0001 ;

        vector<double> x  = {-2.5, 4.3, 5.0, -10,  33.0} ;
        vector<double> p1 = {0.2,  0.2, 0.2,  0.2, 0.2} ;
        vector<double> p2 = {0.1,  0.2, 0.3,  0.4, 0.0} ;
        vector<double> p3 = {1.0,  2.0, 3.0,  4.0, 5.0} ;
        vector<double> p4 = {0.0,  0.0, 0.0,  0.0, 0.0} ;

        double expected1   = 16.28751 ;
        double expected2   = 8.302401 ;
        double expected3   = 20.01518 ;
        bool   expected4   = true ;

        CHECK_CLOSE(sd(x, p1), expected1, error) ;
        CHECK_CLOSE(sd(x, p2), expected2, error) ;
        CHECK_CLOSE(sd(x, p3), expected3, error) ;
        CHECK_EQUAL(isNaN(sd(x, p4)), expected4) ; // NaN, division by 0
    }

    TEST(cor_pearson_test)
    {   // tolerated error for equality testing
        double error = 0.000001 ;

        vector<int> v1 = {0, 0, 1, 2, 3, 0, 0, 1} ;
        vector<int> v2 = {1, 2, 3, 0, 0, 0, 0, 1} ;

        // correlation over the whole vector
        double expected1 = -0.2394366 ;
        double results1  = cor_pearson(v1, v2) ;
        double results2  = cor_pearson(v1, v2, -1, -1, -1, -1) ;
        double results3  = cor_pearson(v1, v2,  0,  8,  0,  8) ;
        double results4  = cor_pearson(v2, v1) ;
        CHECK_CLOSE(expected1, results1, error) ;
        CHECK_EQUAL(results1, results2) ;
        CHECK_EQUAL(results2, results3) ;
        CHECK_EQUAL(results3, results4) ;

        // correlation over part of the vector
        double expected5 = 1.0 ;
        double results5  = cor_pearson(v1, v2, 2, 5, 0, 3) ;
        CHECK_CLOSE(expected5, results5, error) ;

        // correlation of non-variable vectors (sd1 == sd2 == 0)
        double expected6 = true ;
        double results6  = cor_pearson(v1, v2, 0, 2, 3, 5) ;
        CHECK_EQUAL(expected6, isNaN(results6)) ;

        // correlation using single values
        double expected7 = true ;
        double results7  = cor_pearson(v1, v2, 0, 1, 3, 4) ;
        CHECK_EQUAL(expected7, isNaN(results7)) ;
    }

    TEST(cor_pearson_rev_test)
    {   // tolerated error for equality testing
        double error = 0.000001 ;

        vector<int> v1 = {0, 0, 1, 2, 3, 0, 0, 1} ;
        vector<int> v2 = {1, 0, 0, 0, 0, 3, 2, 1} ;

        // correlation over the whole vector
        double expected1 = -0.2394366 ;
        bool   expected2 = true ;
        double results1  = cor_pearson_rev(v1, v2) ;
        double results2  = cor_pearson_rev(v1, v2, -1, -1, -1, -1) ;
        double results3  = cor_pearson_rev(v1, v2,  0,  8,  7, -1) ;
        double results4  = cor_pearson_rev(v2, v1) ;
        CHECK_CLOSE(expected1, results1, error) ;
        CHECK_EQUAL(results1,  results2) ;
        CHECK_EQUAL(results2,  results3) ;
        CHECK_EQUAL(expected2, results3 == results4) ;

        // correlation over part of the vector
        double expected5 = 1.0 ;
        double results5  = cor_pearson_rev(v1, v2, 2, 5, 7, 4) ;
        CHECK_CLOSE(expected5, results5, error) ;

        // correlation of non-variable vectors (sd1 == sd2 == 0)
        double expected6 = true ;
        double results6  = cor_pearson_rev(v1, v2, 0, 2, 4, 2) ;
        CHECK_EQUAL(expected6, isNaN(results6)) ;

        // correlation using single values
        double expected7 = true ;
        double results7  = cor_pearson(v1, v2, 0, 1, 3, 4) ;
        CHECK_EQUAL(expected7, isNaN(results7)) ;
    }
}

































