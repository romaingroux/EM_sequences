#ifndef SERIALREADING_H
#define SERIALREADING_H

#include <stdexcept>
#include "Data_element.hpp"

class SerialReading
{   public:
        virtual ~SerialReading() = 0 ;
    /*!
         * \brief Should implement the method to get the next piece of data in the file.
         * \return a pointer to an object containing the element read from the file.
         */
        virtual Data_element* get_next() throw (std::runtime_error, std::invalid_argument) = 0 ;
} ;



#endif // SERIALREADING_H
