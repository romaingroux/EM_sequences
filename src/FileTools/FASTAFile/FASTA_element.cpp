#include <FileTools/include/FASTA_element.hpp>

FASTA_element::FASTA_element()
    : sequence_one_based(false), sequence_length(0)
{}


FASTA_element::FASTA_element(const std::string& header, const std::string& sequence, bool one_based_seq)
{   this->sequence_one_based = one_based_seq ;
    this->header = header ;
    // add meaningless char to make it 1-based if required
    if(this->sequence_one_based)
    {   this->sequence = "@" ; }
    this->sequence += sequence ;
    this->sequence_length = this->sequence.size() ;
    // correct sequence length to account for @ if required
    if(this->sequence_one_based)
    {   this->sequence_length-- ; }

}


FASTA_element::~FASTA_element()
{}


// operator overloading
FASTA_element& FASTA_element::operator = (const FASTA_element& other)
{   this->header = other.header ;
    this->sequence = other.sequence ;
    this->sequence_one_based = other.sequence_one_based ;
    this->sequence_length = other.sequence_length ;
    return *this ;
}


bool FASTA_element::operator == (const FASTA_element& other) const
{   if((this->header).compare(other.header) == 0 &&
       (this->sequence).compare(other.sequence) == 0 &&
       (this->sequence_one_based) == other.sequence_one_based &&
       (this->sequence_length)    == other.sequence_length)
    {   return true ; }
    else
    {   return  false ; }
}


bool FASTA_element::operator != (const FASTA_element& other) const
{   return not ((*this) == other) ; }

std::ostream& operator << (std::ostream& stream, const FASTA_element& fasta_element)
{   stream << fasta_element.header << std::endl ;
    stream << fasta_element.sequence ;
    return stream ;
}
