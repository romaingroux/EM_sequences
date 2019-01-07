#ifndef UTILITY_HPP
#define UTILITY_HPP


/*! Checks whether a value is NaN.
 * According to IEEE754 NaN != NaN.
 * This should be portable...
 * \param x the value.
 * \return whether the value is a boolean.
 */
template<class T>
bool isNaN(T x)
{   return x != x ; }


/*!
 * \brief Performs an equality check between two doubles using a
 * given precision parameter.
 * \param x the first double.
 * \param y the second double.
 * \param epsilon the precision parameters
 * \return returns true if |x-y| < epsilon.
 */
bool isEqual(double x, double y, double epsilon) ;


#endif // UTILITY_HPP
