#include <FileTools/include/FASTAFileReader.hpp>

FASTAFileReader::FASTAFileReader()
{   this->_f_open = false ; }


FASTAFileReader::FASTAFileReader(const std::string& fasta_file_address, bool one_based_seq, size_t sequence_alloc_size)
{  this->set_file(fasta_file_address, one_based_seq, sequence_alloc_size) ; }


FASTAFileReader::~FASTAFileReader()
{   this->close() ; }


FASTAFileReader& FASTAFileReader::operator = (const FASTAFileReader& other)
{   this->set_file(other.get_file(), other.is_1based(), other.get_alloc_size()) ;
    return *this ;
}


bool FASTAFileReader::is_1based() const
{   return this->_one_based_seq ; }


void FASTAFileReader::set_0based()
{   this->_one_based_seq = false ; }


void FASTAFileReader::set_1based()
{   this->_one_based_seq = true ; }


size_t FASTAFileReader::get_alloc_size() const
{   return this->_seq_allocation_size ; }


void FASTAFileReader::set_alloc_size(size_t size)
{   this->_seq_allocation_size = size ; }


void FASTAFileReader::set_file(const std::string& fasta_file_address, bool one_based_seq, size_t sequence_alloc_size)
{   if(this->is_open())
    {   this->close() ; }
    if(one_based_seq)
    {   this->set_1based() ; }
    else
    {   this->set_0based() ; }
    this->set_alloc_size(sequence_alloc_size) ;
    // modify _f_address and opens it
    FileReader::set_file(fasta_file_address) ;
    // reset map
    this->reset_entry_map() ;
    // fills the map
    this->fill_entry_map() ;
}


FASTA_element* FASTAFileReader::get(const std::string& pattern) throw (std::runtime_error)
{
    // remember file initial position
    long long start_pos = this->_f.tellg() ;

    // if not open, raise an error
    if(not this->is_open())
    {   char msg[512] ;
        sprintf(msg, "FASTAFileReader error! Attempt to read file %s which is closed!", this->get_file().c_str()) ;
        throw std::runtime_error(msg) ;
    }

    std::unordered_map<std::string, long long>::const_iterator iter ;
    iter = this->_entry_map.find(pattern) ;

    FASTA_element* entry = nullptr ;

    // found a corresponding entry
    if(iter != this->_entry_map.end())
    {   // jump to entry start
        this->seekg((*iter).second, std::ios::beg) ;
        entry = this->get_next() ;
        // get back to initial pos in the file
        this->seekg(start_pos, std::ios::beg) ;
    }
    return entry ;
}


FASTA_element* FASTAFileReader::get_next() throw (std::runtime_error, std::invalid_argument)
{   // if _f_seq is not open, raise an error
    if(not this->is_open())
    {   char msg[512] ;
        sprintf(msg, "FASTAFileReader error! Attempt to read file %s which is closed!", this->get_file().c_str()) ;
        throw std::runtime_error(msg) ;
    }
    // value to be returned
    // directly put the read data into the FASTA_element in order to avoid large memory
    // consumption with really large sequences.
    FASTA_element* fasta_element = nullptr ;
    bool header_found = false ;
    bool seq_found = false ;
    // get the header
    while(this->_f.getline(this->_buffer, BUFFER_SIZE))
    {   // looking for a header
        if(not header_found)
        {   // header found, this is an entry to be returned
            if(this->_buffer[0] == '>')
            {   // allocate memory for the FASTA_element to be returned
                fasta_element = new FASTA_element() ;
                // reserve memory for the sequence
                fasta_element->sequence.reserve(this->get_alloc_size()) ;
                if(this->is_1based())
                {   fasta_element->sequence += "@" ;
                    fasta_element->sequence_one_based = true ;
                }
                // store header
                fasta_element->header = this->_buffer ;
                header_found = true ;
                // leave the file pointer here,
                break ;
            }
        }
    }
    // get the sequence
    if(header_found)
    {   seq_found = this->get_sequence(*fasta_element) ; }

    // store sequence length
    if(this->is_1based() and seq_found)
    {   // remove 1 to account for the first character added.
        fasta_element->sequence_length = fasta_element->sequence.length() - 1 ;
    }
    else if(seq_found)
    {   fasta_element->sequence_length = fasta_element->sequence.length() ; }

    return fasta_element ;
}


bool FASTAFileReader::get_sequence(FASTA_element& fasta_element)
{
    // before reading, check that the next character to be read from the file is not a header,
    // if not then the next line is a sequence
    bool seq_found = false ;
    while(this->_f and (this->_f.peek() != '>'))
    {   seq_found = true ;
        this->_f.getline(this->_buffer, BUFFER_SIZE) ;
        fasta_element.sequence += this->_buffer ;
    }
    return seq_found ;
}


void FASTAFileReader::fill_entry_map() throw (std::runtime_error)
{   // go to file start
    this->seekg(0, std::ios::beg) ;
    char c ;

    while(this->_f.get(c))
    {   // header start
        if(c == '>')
        {   // get back 1 byte (right before header) and read whole header
            this->seekg(-1, std::ios::cur) ;
            long long pos = this->tellg() ;
            this->_f.getline(this->_buffer, BUFFER_SIZE) ;
            std::string header(this->_buffer) ;
            // check this header was not seen before
            if(this->_entry_map.find(header) != this->_entry_map.end())
            {   char msg[512] ;
                sprintf(msg, "FASTAFileReader error! Header %s is present multiple times in %s",
                        header.c_str(), this->get_file().c_str()) ;
                throw std::runtime_error(msg) ;
            }
            this->_entry_map.emplace(header, pos) ;
        }
    }
    // get back to file start
    this->seekg(0, std::ios::beg) ;
}
