#include "Statistics.hpp"

#include <cmath>

double dnorm(double x, double mean, double sd)
{   static double pi_2 = 2.*M_PI ;
    return ( 1. / ( sd * sqrt(pi_2) )) * exp(-0.5 * pow((x-mean)/sd, 2.0 ) );
}
