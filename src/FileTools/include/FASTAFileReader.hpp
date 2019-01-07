#ifndef FASTAFILEREADER_H
#define FASTAFILEREADER_H

#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>

#include "FileReader.hpp"
#include "FASTA_element.hpp"
#include "Data_element.hpp"
#include "SerialReading.hpp"
#include "SpecificReading.hpp"


// for file reading
#define BUFFER_SIZE 4096

/*!
 * \brief The FASTAFileReader class handles connection and data fetching from
 * FASTA files. This class uses the class FASTA_element.
 */
class FASTAFileReader : public FileReader, SerialReading, SpecificReading
{
    public:
        // *** methods ****
        /*!
         * \brief Construct a non-connected instance. Connection can
         * be established later using set_file()
         */
        FASTAFileReader() ;

        /*!
         * \brief Construct an instance connected to a FASTA file.
         * If <one_based_seq> is set to true, the sequences are
         * sequences are treated as 1-based, 0-based otherwise.
         * \param fasta_file_address the file address.
         * \param one_based_seq whether to consider the sequences
         * as 1-based. If true, an meaningless '@' char will be
         * insterted at index 0 of every sequence reported from the
         * file to make the 1st char of the sequence be at index 1.
         * \param sequence_alloc_size a memory space (in number of char)
         * to reserve when searching in the file, to store the found
         * sequence. This can avoid recurrent reallocation of the space
         * and shorten the running time. As a rule of thumb, use the
         * average length of the sequence stored in the file. For instance,
         * if the sequences are 2Mb long and a space for only 2000 char is
         * reserved, then the program will have to reallocate memory at least
         * 10 times. Using a value higher than 2'000'000 will result in no
         * reallocation.
         */
        FASTAFileReader(const std::string& fasta_file_address, bool one_based_seq=false, size_t sequence_alloc_size=1000) ;

        /*!
         * \brief Destructor. Closes the file.
         */
        virtual ~FASTAFileReader() override ;

        /*!
         * \brief Assignment operator.
         * \param other another instance to copy the values from during
         * the assignment.
         * \return a reference to the current instance.
         */
        FASTAFileReader& operator = (const FASTAFileReader& other) ;

        /*!
         * \brief Checks whether sequences are currently considered as 1-based.
         * \return Whether sequences are considered as 1-based (and thus whether
         * an initial '@' should be added). Sequences are considered 0-based
         * otherwise.
         */
        bool is_1based() const ;
        /*!
         * \brief Sets sequences as being considered as 1-based (and thus an
         * initial '@' will be added). This will affect the next call of
         * get_specific_entry() or get_next().
         */
        void set_1based() ;
        /*!
         * \brief Sets sequences as being considered as 0-based This will
         * affect the next call of get_specific_entry() and get_next().
         */
        void  set_0based() ;
        /*!
         * \brief Gets the size of the amount of memory allocated for
         * sequence reading (in number of characters).
         */
        size_t get_alloc_size() const ;
        /*!
         * \brief Sets the size of the amount of memory allocated for
         * sequence reading (in number of characters). This impacts the
         * run time of the next call of get_specific_entry() and get_next().
         */
        void set_alloc_size(size_t size) ;

        /*!
         * \brief This method allows to change the file to which the instance is connected. If
         * the a stream to the previous file was open, it is closed and a new stream to the new
         * file is open. _f_seq, _f_seq_address, _f_seq_open and _one_based_seq are reset
         * accordingly. Also, the entry map is reset and refilled with new values.
         * \param fasta_file_address the address of the new FASTA file to which the instance
         * should be connected to.
         * \param one_based_seq whether the sequences of this file should be considered as
         * 1-based or not. If true, an meaningless '@' char will be insterted at index 0 of
         * every sequence reported from the file to make the 1st char of the sequence be at
         * index 1.
         * \param sequence_alloc_size a memory space (in number of char) to reserve when
         * searching in the file, to store the found sequence. This can avoid recurrent
         * reallocation of the space and shorten the running time. As a rule of thumb,
         * use the average length of the sequence stored in _f_seq. For instance, if the
         * sequences are 2Mb long and a space for only 2000 char is reserved, then the program
         * will have to reallocate memory at least 10 times. Using a value higher than 2'000'000
         * will result in no reallocation.
         */
        void set_file(const std::string& fasta_file_address, bool one_based_seq=false, size_t sequence_alloc_size=1000) ;

        /*!
         * \brief  This method searches an entry in the stream having a header corresponding to one
         *  given as argument and return the corresponding entry if one is found.
         * Also, this method guarantees that after a call, the file pointer position is restored to
         * the same position as it was before the call. However, the stream state is not preserved
         * (the method calls seekg() which calls clear() on the stream). If the file pointer was at
         * the end of the file and that the stream was eof before the call, after the call the file
         * pointer will still be at the end of the file but the state will be ok. But a subsequent
         * call to get_next() will return nullptr.
         * \param pattern a pattern to search in the file headers.
         * \throw runtime_error upon an attempt to read from a closed file.
         * \return A pointer to a dynamically FASTA_element if an entry has been found, a NULL
         * pointer otherwise. This FASTA_element should be deleted to avoid memory leaks.
         */
        virtual FASTA_element* get(const std::string& pattern) throw (std::runtime_error) override ;

        /*!
         * \brief This method returns the next entry in _f_seq compared to the current file pointer
         * position. The next entry is defined as the next header followed by a sequence. The
         * returned FASTA_element will need to be deleted to avoid memory leaks.
         * \throw runtime_error upon an attempt to read from a closed file.
         * \return A pointer to a FASTA_element if an entry has been found, nullptr
         * otherwise.
         */
        virtual FASTA_element* get_next() throw (std::runtime_error, std::invalid_argument) override ;

    private:
        // *** methods ****
        /*!
         * \brief Reads the file from the current position to get a sequence.
         * This should be called only after a header has been read, never
         * else where.
         * \param fasta_element a pointer to a FASTA_element where the
         * sequence should be stored.
         * \return a boolean, whether some sequence could be read, which
         * means that initially the file state was correct (not eof) and
         * that the current line to read was not a header (starting with a
         * '>' character).
         */
        bool get_sequence(FASTA_element& fasta_element) ;

        /*!
         * \brief This method fills the map of pairs header/file pointers. At return time,
         * the file pointer position is set to the beginning of the file.
         * \throw std::runtime_error if two entries with a same header are found in the file during
         * the process.
         */
        virtual void fill_entry_map() throw (std::runtime_error) override ;

        //*** fields ****        
        /*!
         * \brief The size of the memory allocation (std::string.reserve) for
         * FASTA_element.sequence
         */
        size_t _seq_allocation_size ;

        /*!
         * \brief whether the sequences should be treated as 1-based in which
         * case all the sequences read will be added an extra '@' character
         * at position 0 to make the sequence start at position 1.
         */
        bool _one_based_seq ;

};

#endif // FASTAFILEREADER_H
