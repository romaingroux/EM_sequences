#include <FileTools/include/FileReader.hpp>

FileReader::~FileReader()
{}

void FileReader::set_file(const std::string& file_address)
{
    if(this->is_open())
    {   this->close() ; }
    this->_f_address = file_address ;
    this->open() ;
}

std::string FileReader::get_file() const
{   return this->_f_address ; }


bool FileReader::is_open() const
{   return this->_f_open ; }


void FileReader::close()
{   if(this->is_open())
    {   this->_f.close() ;
        this->_f_open = false ;
    }
}

void FileReader::open()
throw (std::runtime_error)
{   this->_f.open(_f_address, std::ios::in) ;
    if(this->_f.fail())
    {   char msg[512] ;
        sprintf(msg, "FileReader error! Could not open file %s\n", _f_address.c_str()) ;
        throw std::runtime_error(msg) ;
    }
    this->_f_open = true ;
}


void FileReader::seekg(long long pos, std::ios_base::seekdir from) throw (std::runtime_error)
{   if(not this->is_open())
    {   char msg[512] ;
        sprintf(msg, "FileReader error! Attempt to read file %s which is closed!", this->get_file().c_str()) ;
        throw std::runtime_error(msg) ;
    }
    this->_f.clear() ;
    this->_f.seekg(pos, from) ;
}


long long FileReader::tellg()
{   return this->_f.tellg() ; }


bool FileReader::good() const
{   return this->_f.good() ; }


bool FileReader::bad() const
{   return this->_f.bad() ; }


bool FileReader::fail() const
{   return this->_f.fail() ; }


bool FileReader::eof() const
{   return this->_f.eof() ; }


