#ifndef CONFIGFILEREADER_H
#define CONFIGFILEREADER_H

#include "FileReader.hpp"

#include <string>
#include <unordered_map>
#include <stdexcept>      // std::invalid_argument

using option_map  = std::unordered_map<std::string, std::string> ; // option -> value
using section_map = std::unordered_map<std::string, option_map> ;  // section -> option map

/*!
 * \brief Overloads the operator << for option_map objects.
 * \param o an output stream to send the unordered map to.
 * \param map the unordered map of interest.
 * \return a reference to the stream.
 */
std::ostream& operator << (std::ostream& o, const section_map& map) ;


/*!
 * \brief Converts a string to a boolean. Accepted values are
 * "0"/"1", "False"/"True", "false"/"true", "Off"/"On" and
 * "off"/"on".
 *  Any other value will raise an error.
 * \param s the string to convert.
 * \throw std::runtime_error in case <s> is not recognized.
 * \return a bool.
 */
bool stobool(const std::string& s) throw (std::invalid_argument) ;


/*!
 * \brief The ConfigFileReader class is designed to parse and retrieve information from .ini
 * configuration files. In short, these files are divided in sections which starts with a
 * header formatted as '[section_title]'. Then, each section can contains an unspecified
 * number of different options-values pairs (each pair on a single line). Here is an
 * example of proper file
 * --------------
 * [section1]
 * opt1 = 1
 * opt2 = mystring
 * opt3 = 1.3
 * [section2]
 * my_opt = true
 * --------------
 * The file will be read at once by the parser and all the option-value pairs will be stored
 * within an unordered_map (sections) of unordered_map (option-value pairs). All values are
 * stored as strings but can be retrieved as int, double, bool or strings. Any value can
 * always be retrieved as a string. However, not all value can be converted to int, double
 * or bool. Typically, any string will be converted as a bool with value true except "". But
 * this makes little sense. On the other side, "on"/"off", "On"/"Off", "true"/"false", "True"/
 * "False" and "1"/"0" strings are correctly converted into bool.
 * For convertion to int or to double, strings containing numerical expressions should be used.
 * For instance "1" can be converted to int and to double easily. ".1" will be easily converted
 * to double but not to int. For any other expression, the behaviour is unknown.
 */
class ConfigFileReader : public FileReader
{    public:
        // static member
        /*!
         * \brief a special value attributed to options for which not value
         * could be found within the file.
         */
        static std::string novalue ;


    public:
        ConfigFileReader() ;

        /*!
         * \brief Constructs a ConfigFileReader object from the given file. At return
         * time, the file has been entirely read and its content stored within the
         * object. Additionally, the stream to the file is closed.
         * \param file_adresse
         * \throw std::runtime_error if a same section is found more than once or
         * if a given option is found more than once in the same section.
         */
        ConfigFileReader(const std::string& file_adresse) throw (std::runtime_error) ;

        virtual ~ConfigFileReader() override ;

        /*!
         * \brief Assignment operator.
         * \param other an other instance to get the values from.
         * \throw std::runtime_error if a same section is found more than once or
         * if a given option is found more than once in the same section.
         * \return a reference to the current instance.
         */
        ConfigFileReader& operator = (const ConfigFileReader& other) throw (std::runtime_error) ;

        /*!
         * \brief Sets to which file the object should be streamed to.
         * At returned time, the file has been read, the map reset and filled
         * with new values and the connection to the stream is closed.
         * \param file_address file_address the file address.
         * \throw std::runtime_error if a same section is found more than once or
         * if a given option is found more than once in the same section.
         */
        void set_file(const std::string& file_address) throw (std::runtime_error) ;

        /*!
         * \brief Checks whether a given section was found in the file.
         * \param section a section name.
         * \return whether the section was found.
         */
        bool has_section(const std::string& section) const ;

        /*!
         * \brief Checks whether a given option within a given section was found
         * in the file.
         * \param section a section name.
         * \param options an option name.
         * \return whether the option within the given section was found.
         */
        bool has_option(const std::string& section, const std::string& option) const ;

        /*!
         * \brief Returns the unordered map with all sections and options.
         * \return a copy of the unordered map with all sections and options.
         */
        section_map get_options() const ;

        /*!
         * \brief Gets the value corresponding to the given option value in the given
         * section, as a string.
         * \param section the section of interest.
         * \param options the option of interest.
         * \throw std::runtime_error if no such section nor option could be found.
         * \return a string corresponding to the value of to the given option in the given
         * section
         */
        std::string getString(const std::string& section, const std::string& options) throw(std::runtime_error) ;

        /*!
         * \brief Gets the value corresponding to the given option value in the given
         * section, as a double.
         * \param section the section of interest.
         * \param options the option of interest.
         * \throw std::runtime_error if no such section nor option could be found.
         * \return a double corresponding to the value of the given option in the given
         * section
         */
        double getDouble(const std::string& section, const std::string& options) throw(std::runtime_error) ;

        /*!
         * \brief Gets the value corresponding to the given option value in the given
         * section, as an integer.
         * \param section the section of interest.
         * \param options the option of interest.
         * \throw std::runtime_error if no such section nor option could be found.
         * \return an integer corresponding to the value of the given option in the given
         * section
         */
        int getInt(const std::string& section, const std::string& options) throw(std::runtime_error) ;

        /*!
         * \brief Gets the value corresponding to the given option value in the given
         * section, as a boolean. The conversion is done using stobool().
         * \param section the section of interest.
         * \param options the option of interest.
         * \throw std::runtime_error if no such section nor option could be found or
         * std::invalid_argument if stobool could not properly convert the value.
         * \return an integer corresponding to the value of the given option in the given
         * section
         */
        bool getBool(const std::string& section, const std::string& options) throw(std::runtime_error, std::invalid_argument) ;

        /* makes no sense to keep them, stream is always read at once at construct time and
         * closed straight after */
        bool good() const = delete ;
        bool bad() const = delete ;
        bool eof() const = delete ;
        bool fail() const = delete ;

    private:
        // *** methods ***
        /*!
         * \brief Routine reading the config file and storing all the values in the map.
         * \throw std::runtime_error if i) two section with the same name are found, ii)
         * two options with the same name in the same section are found or iii) an
         * option before the first section is found.
         */
        void read_file() throw (std::runtime_error) ;
        /*!
         * \brief Resets the map to an empty unordered map of sections.
         */
        void reset_map() ;

        /*!
         * \brief Performs a search operation in the map to find a given section.
         * \param section a section of interest.
         * \return an iterator to the given bucket if the section was found, _map::end()
         * otherwise.
         */
        section_map::iterator find_section(const std::string& section) ;
        /*!
         * \brief Performs a search operation in the map to find a given section.
         * \param section a section of interest.
         * \return an iterator to the given bucket if the section was found, _map::end()
         * otherwise.
         */
        section_map::const_iterator find_section(const std::string& section) const ;

        /*!
         * \brief Performs a search operation in the map to find a given option in
         * a given section.
         * \param section a section of interest.
         * \param option an option of interest.
         * \return an iterator to the given section bucket (not to the option bucket) if the section
         * was found, _map::end() otherwise.
         */
        section_map::iterator find_option(const std::string& section, const std::string& option) ;

        /*!
         * \brief Performs a search operation in the map to find a given option in
         * a given section.
         * \param section a section of interest.
         * \param option an option of interest.
         * \return an iterator to the given section bucket (not to the option bucket) if the section
         * was found, _map::end() otherwise.
         */
        section_map::const_iterator find_option(const std::string& section, const std::string& option) const ; // todo

        /*!
         * \brief Emplace a new section bucket for a given section in _map. If a bucket for the
         * given section already exists, nothing is done.
         * \param section a section of interest.
         */
        void add_new_section(const std::string& section) ;

        /*!
         * \brief Emplace a new option bucket (option-value) for a given option in a given section in _map.
         * If there is no bucket for the section already, one is created. If there is not bucket
         * for the option in the section bucket, one is created. If there is already an option
         * bucket in the section bucket, the option value is over-written.
         * \param section a section of interest.
         * \param option an option of interest.
         * \param value the value associated to the option.
         */
        void add_new_option(const std::string& section, const std::string& option, const std::string& value) ;

        // *** fields ***
        section_map _map ; // all sections are contained within one map with all their options
} ;

#endif // CONFIGFILEREADER_H
