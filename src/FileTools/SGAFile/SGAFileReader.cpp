#include <FileTools/include/SGAFileReader.hpp>

SGAFileReader::SGAFileReader()
{ this->_f_open = false ; }

SGAFileReader::SGAFileReader(const std::string& sga_file_address)
{   this->set_file(sga_file_address) ; }

SGAFileReader::~SGAFileReader()
{   this->close() ; }


SGAFileReader& SGAFileReader::operator = (const SGAFileReader& other)
{   this->set_file(other.get_file()) ;
    return *this ;
}


SGA_element* SGAFileReader::get_next() throw (std::runtime_error, std::invalid_argument)
{   // file is not open
    if(not this->is_open())
    {   char msg[512] ;
        sprintf(msg, "FASTAFileReader error! Attempt to read file %s which is closed!", _f_address.c_str()) ;
        throw std::runtime_error(msg) ;
    }

    SGA_element* sga_element = nullptr ;

    this->_f.getline(this->_buffer, BUFFER_SIZE) ;
    if(this->_f)
    {   // SGA_element constructor can throw invalid_argument exceptions
        try
        {   sga_element = new SGA_element(this->_buffer) ; }
        catch(std::invalid_argument e)
        {   delete sga_element ;
            if(sga_element != nullptr)
            {   delete sga_element ; }
            throw e ;
        }
    }
    return sga_element ;
}
