#ifndef SPECIFICREADER_H
#define SPECIFICREADER_H

#include <string>
#include <stdexcept>
#include <unordered_map>
#include "Data_element.hpp"


class SpecificReading
{
    public:
        virtual ~SpecificReading() = 0 ;

         /*!
         * \brief  This method search in the file an entry having a header corresponding to one
         * given as argument and return a pointer to an object containing the entry. The returned
         * element should be deleted to avoid memory leaks.
         * \param pattern a pattern to search in the file entry headers.
         * \throw std::runtime_error if the stream to the file is not opened.
         * \return a pointer to an object containing the entry data, a nullptr if no entry
         * with a corresponding header was found.
         */
        virtual Data_element* get(const std::string& pattern) throw (std::runtime_error) = 0 ;

    protected:
        /*!
         * \brief This method should implement the way the map storing the pairs of header/pointer
         * is filled.
         * \throw std::runtime_error if two entries with a same header are found in the file during
         * the process.
         */
        virtual void fill_entry_map() throw (std::runtime_error) = 0 ;

        /*!
         * \brief Resets the entry map to an empty map.
         */
        void reset_entry_map() ;

        /*!
         * \brief Gets the position of the entry having the given header in the file.
         * \throw std::runtime_error if no entry with this header could be found.
         * \return the position in the file of the entry starting with this header.
         */
        long long get_entry_position(const std::string& header) const throw (std::runtime_error) ;

        /*!
         * \brief A map to store pairs of entry header and pointer position for the beginning of
         *  the entry with this header in the file. Pointers are offsets from the start of the
         * file.
         */
        std::unordered_map<std::string, long long> _entry_map ;

} ;

#endif // SPECIFICREADER_H
