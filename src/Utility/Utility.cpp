#include "Utility.hpp"

#include <cmath>   // std::abs()
#include <vector>


bool isEqual(double x, double y, double epsilon)
{   return std::abs(x - y) < epsilon ; }


