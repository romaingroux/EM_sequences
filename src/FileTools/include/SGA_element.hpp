#ifndef SGA_ELEMENT_H
#define SGA_ELEMENT_H

#define SGA_DELIMITER '\t'

#include "Data_element.hpp"
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>

/*!
 * \brief The SGA_element class is designed to store information from a SGA file line.
 * This class does only account for the first 5 fields of this file format.
 */
class SGA_element : public Data_element
{
    public:
        SGA_element() ;
        /*!
         * \brief Constructs an instance given a value for each field.
         * \param chromosome the chromosome value.
         * \param feature the feature value.
         * \param position the position value.
         * \param strand the strand value, should be +/-/0 otherwise throws an error.
         * \param count the count value.
         * \throw std::invalid_argument if <strand> is not +/-/0.
         */
        SGA_element(std::string chromosome, std::string feature, size_t position, std::string strand, size_t count) throw (std::invalid_argument) ;
        /*!
         * \brief Construct a SGA element from a string. The string should contain at leat the
         * five first SGA fields (separated by whatever blank separator). Typically, this method
         * should be used when reading a file.
         * \param str a string containing at least the 5 mendatory SGA fields.
         * Any other field will no be taken into account.
         * \throw std::invalid_argument if the string is empty, if the position <=0, if the
         * strand is not +/-/0 or if the count <=0.
         */
        SGA_element(const std::string& str) throw (std::invalid_argument) ;

        ~SGA_element() ;


        // operator overloading
        /*!
         * \brief Assignment operator.
         * \param other another instance to copy the values from.
         * \return a reference to the current instance.
         */
        SGA_element& operator = (const SGA_element& other) ;

        /*!
         * \brief Equality operator .
         * \param other an other instance to compare the current one with.
         * \return true if both instances have the same field values, false otherwise.
         */
        bool operator == (const SGA_element& other) const ;

        /*!
         * \brief Inequality operator.
         * \param other an other instance to compare the current one with.
         * \return true if both instances don't have the same field values, false otherwise.
         */
        bool operator != (const SGA_element& other) const ;

        // methods
        /*!
         * \brief Return the distance in bp between 2 SGA_element instances. If these instances
         * are not on the same chrosomomes, no distance can be computed and error_byte is
         * set to 1 and 0 is returned. If both instances are on the same chromosome, error_byte
         * is set to 0 and the distance (other - this) is returned.
         * \param other another SGA_element to compute the distance from
         * \param error_byte a reference to a int, if the distance can be computed, the value
         * pointed will be set to 0, otherwise to 1.
         * \return  the distance (other - this), 0 if no distance could be computed. Be careful,
         * 0 may be a correct results (if error_byte is 0).
         */
        int get_distance(const SGA_element& other, int& error_byte) const ;

        /*!
         * \brief Friend function overloading the << operator for printing/writting SGA
         * element to out streams.
         * \param stream the stream of interest.
         * \param sga the instance of interest.
         * \return a reference to the stream.
         */
        friend std::ostream& operator << (std::ostream& stream, const SGA_element& sga) ;

        // fields
        /*!
         * \brief chromosome field
         */
        std::string chromosome ;
        /*!
         * \brief feature field
         */
        std::string feature ;
        /*!
         * \brief position field
         */
        size_t position ;
        /*!
         * \brief the strand field
         */
        std::string strand ;
        /*!
         * \brief the count field
         */
        size_t count ;
} ;

#endif // SGA_ELEMENT_H
