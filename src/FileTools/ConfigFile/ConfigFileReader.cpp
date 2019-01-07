#include <FileTools/include/ConfigFileReader.hpp>


std::ostream& operator << (std::ostream& o, const section_map& map)
{	for(auto it = map.begin(); it != map.end(); ++it)
    {	o << "section " << (*it).first << std::endl ;
        for(auto it2 = (*it).second.begin() ; it2 != (*it).second.end() ; ++it2)
        {	o << "   option " << (*it2).first << " " << (*it2).second << std::endl ;
        }
    }
    return o ;
}


bool stobool(const std::string& s) throw (std::invalid_argument)
{   // false
    if((s == "0") or
       (s == "False") or
       (s == "false") or
       (s == "Off") or
       (s == "off"))
    {   return false ; }
    // true
    else if((s == "1") or
            (s == "True") or
            (s == "true") or
            (s == "On") or
            (s == "on"))
    {   return true ; }
    // error
    else
    {   char msg[512] ;
        sprintf(msg, "Error! Cannot convert %s to bool, unrecognized value!", s.c_str()) ;
        throw std::invalid_argument(msg) ;
    }
}


ConfigFileReader::ConfigFileReader()
    : _map()
{}


ConfigFileReader::ConfigFileReader(const std::string& file_adresse) throw (std::runtime_error)
    : ConfigFileReader()
{   try
    { this->set_file(file_adresse) ; }
    catch(std::runtime_error& e)
    {   throw e ; }
}


ConfigFileReader::~ConfigFileReader()
{}


ConfigFileReader& ConfigFileReader::operator = (const ConfigFileReader& other) throw (std::runtime_error)
{   try
    {   this->set_file(other.get_file()) ; }
    catch(std::runtime_error& e)
    {   throw e ; }
    return *this ;
}


void ConfigFileReader::set_file(const std::string& file_address) throw (std::runtime_error)
{   // reset map content
    this->reset_map() ;
    // read file
    FileReader::set_file(file_address) ;
    try
    {   this->read_file() ; }
    catch(std::runtime_error& e)
    {   throw e ; }
    // close file
    this->close() ;
}




bool ConfigFileReader::has_section(const std::string& section) const
{  if(this->find_section(section) != this->_map.end())
   {    return true ; }
    return false ;
}


bool ConfigFileReader::has_option(const std::string& section, const std::string& option) const
{   if(this->find_option(section, option) != this->_map.end())
    {   return true ; }
    return false ;
}

section_map ConfigFileReader::get_options() const
{   return this->_map ; }




std::string ConfigFileReader::getString(const std::string& section, const std::string& option) throw(std::runtime_error)
{   section_map::iterator s_iter = this->find_option(section, option) ;
    if(s_iter != this->_map.end())
    {   option_map::const_iterator o_iter = (*s_iter).second.find(option) ;
        // no option-value, raise an error
        if(o_iter == (*s_iter).second.end())
        {   char msg[512] ;
            sprintf(msg, "ConfigFileReader error! Cannot find %s option in %s section value!",
                    option.c_str(), section.c_str()) ;
            throw std::runtime_error(msg) ;

        }
        return (*o_iter).second ;
    }
    // no section, raise an error
    else
    {   char msg[512] ;
        sprintf(msg, "ConfigFileReader error! Cannot find %s section!",
                section.c_str()) ;
        throw std::runtime_error(msg) ;
    }
}

double ConfigFileReader::getDouble(const std::string& section, const std::string& option) throw(std::runtime_error)
{   section_map::iterator s_iter = this->find_option(section, option) ;
    if(s_iter != this->_map.end())
    {   option_map::const_iterator o_iter = (*s_iter).second.find(option) ;
        // no option-value, raise an error
        if(o_iter == (*s_iter).second.end())
        {   char msg[512] ;
            sprintf(msg, "ConfigFileReader error! Cannot find %s option in %s section value!",
                    option.c_str(), section.c_str()) ;
            throw std::runtime_error(msg) ;

        }
        return stod((*o_iter).second) ;
    }
    // no section, raise an error
    else
    {   char msg[512] ;
        sprintf(msg, "ConfigFileReader error! Cannot find %s section!",
                section.c_str()) ;
        throw std::runtime_error(msg) ;
    }
}

int ConfigFileReader::getInt(const std::string& section, const std::string& option) throw(std::runtime_error)
{   section_map::iterator s_iter = this->find_option(section, option) ;
    if(s_iter != this->_map.end())
    {   option_map::const_iterator o_iter = (*s_iter).second.find(option) ;
        // no option-value, raise an error
        if(o_iter == (*s_iter).second.end())
        {   char msg[512] ;
            sprintf(msg, "ConfigFileReader error! Cannot find %s option in %s section value!",
                    option.c_str(), section.c_str()) ;
            throw std::runtime_error(msg) ;

        }
        return stoi((*o_iter).second) ;
    }
    // no section, raise an error
    else
    {   char msg[512] ;
        sprintf(msg, "ConfigFileReader error! Cannot find %s section!",
                section.c_str()) ;
        throw std::runtime_error(msg) ;
    }
}


bool ConfigFileReader::getBool(const std::string& section, const std::string& option) throw(std::runtime_error, std::invalid_argument)
{   section_map::iterator s_iter = this->find_option(section, option) ;
    if(s_iter != this->_map.end())
    {   option_map::const_iterator o_iter = (*s_iter).second.find(option) ;
        // no option-value, raise an error
        if(o_iter == (*s_iter).second.end())
        {   char msg[512] ;
            sprintf(msg, "ConfigFileReader error! Cannot find %s option in %s section value!",
                    option.c_str(), section.c_str()) ;
            throw std::runtime_error(msg) ;

        }
        try
        {   return stobool((*o_iter).second) ; }
        catch(std::invalid_argument e)
        {   throw e ; }
    }
    // no section, raise an error
    else
    {   char msg[512] ;
        sprintf(msg, "ConfigFileReader error! Cannot find %s section!",
                section.c_str()) ;
        throw std::runtime_error(msg) ;
    }
}


void ConfigFileReader::read_file() throw (std::runtime_error)
{   // file not open
    if(not this->is_open())
    {   char msg[512] ;
        sprintf(msg, "ConfigFileReader error! Attempt to read file %s which is closed!", this->get_file().c_str()) ;
        throw std::runtime_error(msg) ;
    }

    // go to file start
    this->seekg(0, std::ios::beg) ;
    // read each line
    std::string section, option, value ;
    while(this->_f.getline(this->_buffer, BUFFER_SIZE))
    {   std::string buffer(this->_buffer) ;
        // empty line
        if(buffer.size() == 0)
        {   continue ; }
        // comment line
        else if(buffer.front() == '#')
        {   continue ; }

        // search section header
        // not proper section header
        if(((buffer.front() == '[') and (buffer.back() != ']')) or
           ((buffer.front() != '[') and (buffer.back() == ']')))
        {   char msg[512] ;
            sprintf(msg, "ConfigFileReader error! Incorrect format in section header %s!", buffer.c_str()) ;
            throw std::runtime_error(msg) ;
        }
        // proper section header
        // "[<header>]", nothing else admitted
        else if(buffer.front() == '[' and buffer.back() == ']')
        {   section.clear() ;
            size_t i=0 ;
            while(buffer[i] != '\0' and i < BUFFER_SIZE)
            {   // don't retain these char
                if(buffer[i] == ']' or buffer[i] == '[')
                {}
                // this is an error, no space allowed
                else if(buffer[i] == ' ')
                {   char msg[512] ;
                    sprintf(msg, "ConfigFileReader error! Incorrect format in section header %s!", buffer.c_str()) ;
                    throw std::runtime_error(msg) ;
                }
                else
                {   section.push_back(buffer[i]) ; }
                i++ ;
            }
            // store the header
            if(this->has_section(section))
            {   char msg[512] ;
                sprintf(msg, "ConfigFileReader error! Section %s found several times!", section.c_str()) ;
                throw std::runtime_error(msg) ;
            }
            else
            {   this->add_new_section(section) ; }
        }
        // search an option and a value
        // "<option>...=...<value>" where '...' can be spaces
        // TODO -> the current implementation accepts spaces within <option> and <value>
        // but don't read them -> correct to raise an error when this happens?
        else
        {
            option.clear() ; value.clear() ;
            // should contain exactly one '=' char
            size_t first = buffer.find('=') ;
            size_t last  = buffer.rfind('=') ;
            // error, 0 or >1 =
            if((first == std::string::npos) or
               (first != last))
            {   char msg[512] ;
                sprintf(msg, "ConfigFileReader error! Incorrect format in option %s!", buffer.c_str()) ;
                throw std::runtime_error(msg) ;
            }
            else
            {
                // option -> from start to '=' char (minus the spaces)
                for(size_t i=0; i<first; i++)
                {   if(buffer[i] != ' ')
                    {   option.push_back(buffer[i]) ; }
                }
                // value -> from '=' (minus the spaces) to end
                for(size_t i=first+1; i<buffer.size(); i++)
                {   if(buffer[i] != ' ')
                    {   value.push_back(buffer[i]) ; }
                }

                // store option and value
                if(this->has_option(section, option))
                {   char msg[512] ;
                    sprintf(msg, "ConfigFileReader error! Option %s in section %s found several times!",
                            option.c_str(), section.c_str()) ;
                    throw std::runtime_error(msg) ;
                }
                // option has no section (option comes before first
                // section header in file)
                else if(section == "")
                {   char msg[512] ;
                    sprintf(msg, "ConfigFileReader error! Option %s outside section in %s",
                            option.c_str(), this->get_file().c_str()) ;
                    throw std::runtime_error(msg) ;
                }
                else
                {   // option has no value
                    if(value == "")
                    {   value = ConfigFileReader::novalue ; }
                    this->add_new_option(section, option, value) ;
                }
            }
        }
    }
}


void ConfigFileReader::reset_map()
{   this->_map = section_map() ; }




section_map::iterator ConfigFileReader::find_section(const std::string& section)
{   return this->_map.find(section) ; }

section_map::const_iterator ConfigFileReader::find_section(const std::string& section) const
{   return this->_map.find(section) ; }

section_map::iterator ConfigFileReader::find_option(const std::string& section, const std::string& option)
{   // search section
    section_map::iterator s_iter = this->_map.find(section) ;
    // section present -> search option
    if(s_iter != this->_map.end())
    {   option_map::iterator o_iter = (*s_iter).second.find(option) ;
        // option present
        if(o_iter != (*s_iter).second.end())
        {   return s_iter ; }
        // option absent
        else
        {   return this->_map.end() ; }
    }
    // section absent -> return map end
    else
    {   return this->_map.end() ; }
}

section_map::const_iterator ConfigFileReader::find_option(const std::string& section, const std::string& option) const
{   // search section
    section_map::const_iterator s_iter = this->_map.find(section) ;
    // section present -> search option
    if(s_iter != this->_map.end())
    {   option_map::const_iterator o_iter = (*s_iter).second.find(option) ;
        // option present
        if(o_iter != (*s_iter).second.end())
        {   return s_iter ; }
        // option absent
        else
        {   return this->_map.end() ; }
    }
    // section absent -> return map end
    else
    {   return this->_map.end() ; }
}

void ConfigFileReader::add_new_section(const std::string& section)
{   // only create a new element if the section is not already contained in the map
    if(not this->has_section(section))
    {   this->_map.emplace(section, option_map()) ; }
}

void ConfigFileReader::add_new_option(const std::string& section, const std::string& option, const std::string& value)
{   section_map::iterator s_iter = this->find_section(section) ;
    // section exists
    if(s_iter != this->_map.end())
    {   section_map::iterator s_iter_opt = this->find_option(section, option) ;
        // option exists -> overwrite its value
        if(s_iter_opt != this->_map.end())
        {   // get back to option
            option_map::iterator o_iter = (*s_iter_opt).second.find(option) ;
            (*o_iter).second = value ;

        }
        // option does not exists -> create it
        else
        {   (*s_iter).second.emplace(option, value) ; }
    }
    // create section and option
    else
    {   this->add_new_section(section) ;
        s_iter = this->find_section(section) ;
        (*s_iter).second.emplace(option, value) ;
    }
}



// static member
std::string ConfigFileReader::novalue = std::string("NO_VALUE") ;
