#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

// for file reading
#define BUFFER_SIZE 4096


/*!
 * \brief The FileReader class is an interface to subclass for other classes dedicated
 * to specific file format reading.
 * It implements the basic methods to handle a input stream from a file.
 */
class FileReader
{
    public:
        // *** methods ****
        virtual ~FileReader() = 0 ;

        /*!
         * \brief Sets to which file the object should be streamed to.
         * A stream is open on this stream upon the end of the
         * call.
         * \param file_address the file address.
         */
        void set_file(const std::string& file_address) ;

        /*!
         * \brief Gets the address of the file to which the object
         * is (if open) or will be streamed on (if not yet opened).
         * \return the address of the file.
         */
        std::string get_file() const ;

        /*!
         * \brief Checks whether a input stream is currently open.
         * \return whether a stream is open.
         */
        bool is_open() const ;

        /*!
         * \brief Closes the stream to the file.
         */
        void close() ;

        /*!
         * \brief Modifies the position of the pointer within the file through
         * a call to seekg(). Each call to this function also calls clear()
         * on the stream and reset the stream state.
         * \param pos the new wanted position of the pointer in the
         * file.
         * \param from the reference to set pos from, i.e. ios_base::beg,
         * ios_base::cur, ios_base::end
         * \throw runtime_error upon an attempt to seek on a closed file.
         */
        void seekg(long long pos, std::ios_base::seekdir from) throw (std::runtime_error);

        /*!
         * \brief Returns the current position of the pointer within the file through
         * a call to tellg.
         */
        long long tellg() ;

        /*!
         * \brief Calls good() on stream.
         * \return whether goodbit is set
         */
        bool good() const ;

        /*!
         * \brief Calls bad() on stream.
         * \return whether badbit is set
         */
        bool bad() const ;

        /*!
         * \brief Calls fail() on stream.
         * \return whether failbit or badbit (or both) is set
         */
        bool fail() const ;

        /*!
         * \brief Calls eof() on stream.
         * \return whether eofbit is set
         */
        bool eof() const ;

    protected:
        // *** methods ****
        /*!
         * \brief Opens a input stream to the file
         * this->_f_seq_address.
         * \throw runtime_error the stream cannot
         * be open.
         */
        void open() throw (std::runtime_error) ;

        // **** fields ****
        /*!
         * \brief the SGA file address.
         */
        std::string _f_address ;
        /*!
         * \brief a stream to the SGA file.
         */
        std::ifstream _f ;
        /*!
         * \brief a buffer to read from the file.
         */
        bool _f_open ;
        /*!
         * \brief a buffer to read from the file.
         */
        char _buffer[BUFFER_SIZE] ;
} ;


#endif // FILEREADER_H
