#ifndef VECTOR_UTILITY_HPP
#define VECTOR_UTILITY_HPP

#include <iostream>
#include <vector>

template<class T>
std::ostream& operator << (std::ostream& stream, const std::vector<T>& v)
{   for(const auto& i : v)
    {   stream << i  << ' ' ; }
    return stream ;
}

#endif // VECTOR_UTILITY_HPP
