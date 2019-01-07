#include <FileTools/include/SpecificReading.hpp>


SpecificReading::~SpecificReading()
{ ; }


long long SpecificReading::get_entry_position(const std::string& header) const throw (std::runtime_error)
{   std::unordered_map<std::string,long long>::const_iterator map = this->_entry_map.find(header) ;
    if(map == this->_entry_map.end())
    {   return (*map).second ; }
    else
    {   char msg[512] ;
        sprintf(msg, "SpecificReading error! No such entry %s!", header.c_str()) ;
        throw std::runtime_error(msg) ;
    }
}


void SpecificReading::reset_entry_map()
{   this->_entry_map = std::unordered_map<std::string, long long>() ; }
