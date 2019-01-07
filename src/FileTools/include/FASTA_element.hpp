#ifndef FASTA_ELEMENT_H
#define FASTA_ELEMENT_H

#include "Data_element.hpp"
#include <iostream>
#include <string>
#include <cmath>

/*!
 * \brief The FASTA_element class is designed to store a FASTA entry. This class can store
 * both the sequence and the header as well as a flag to indicate whether the sequence should
 * be considered as 1-based or not.
 */

class FASTA_element : public Data_element
{
    public:

        FASTA_element() ;

        FASTA_element(const std::string& header, const std::string& sequence, bool one_based_seq=false) ;

        ~FASTA_element() ;

        // **** functions ****
        // operator overloading

        /*!
         * \brief Assignment operator.
         * \param other another instance to copy the values from.
         * \return a reference to the current instance.
         */
        FASTA_element& operator = (const FASTA_element& other) ;

        /*!
         * \brief Equality operator.
         * \param other an other instance to compare the current one with.
         * \return true if both instances have the same field values, false otherwise.
         */
        bool operator == (const FASTA_element& other) const ;

        /*!
         * \brief Inequality operator.
         * \param other an other instance to compare the current one with.
         * \return true if both instances don't have the same field values, false otherwise.
         */
        bool operator != (const FASTA_element& other) const ;

        /*!
         * \brief A friend function which overload the << operator for printing/writting
         * FASTA_element.
         * \param stream the stream to send the FASTA_element to
         * \param fasta_element the FASTA_element of interest
         * \return a reference to the stream.
         */
        friend std::ostream& operator << (std::ostream& stream, const FASTA_element& fasta_element) ;


        // **** fields ****
        /*!
         * \brief entry header
         */
        std::string header ;
        /*!
         * \brief entry sequence
         */
        std::string sequence ;
        /*!
         * \brief whether the sequence is one based. If true, the first character is meaningless
         * and only added to make the 1st character of the sequence at index 1
         */
        bool   sequence_one_based ;
        /*!
         * \brief he sequence length, if 1 based it is the real string length - 1
         */
        size_t sequence_length ;
} ;


#endif // FASTA_ELEMENT_H
