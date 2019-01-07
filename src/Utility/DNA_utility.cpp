#include "DNA_utility.hpp"

#include <unordered_map>
#include <cassert>
#include <stdexcept>  // invalid_argument
#include <limits>     // numeric_limits
#include <vector>

#include "Matrix/Matrix2D.hpp"


std::string dna::get_valid_dna_char()
{   std::string valid_dna_char = "ACGTacgt" ;
    return valid_dna_char ;
}

std::string dna::get_invalid_dna_char()
{   std::string invalid_dna_char ;
    std::string valid_dna_char = dna::get_valid_dna_char() ;

    int n_max = std::numeric_limits<char>::max() - std::numeric_limits<char>::min() ;
    int n_curr = 0 ;
    char c = std::numeric_limits<char>::min() ;
    while(n_curr <= n_max)
    {   // not a valid DNA char
        if(valid_dna_char.find(c) == std::string::npos)
        {   invalid_dna_char.push_back(c) ; }
        c++ ;
        n_curr++ ;
    }
    return invalid_dna_char ;
}


size_t dna::hash(char base, bool rev_compl) throw (std::invalid_argument)
{
    static bool init  = false ;
    static std::unordered_map<char,size_t> hash_map ;
    static std::unordered_map<char,size_t> hash_map_rev ;
    if(not init)
    {   hash_map['A'] = 0 ;
        hash_map['a'] = 0 ;
        hash_map['C'] = 1 ;
        hash_map['c'] = 1 ;
        hash_map['G'] = 2 ;
        hash_map['g'] = 2 ;
        hash_map['T'] = 3 ;
        hash_map['t'] = 3 ;

        hash_map_rev['A'] = hash_map['T'] ;
        hash_map_rev['a'] = hash_map['t'] ;
        hash_map_rev['C'] = hash_map['G'] ;
        hash_map_rev['c'] = hash_map['g'] ;
        hash_map_rev['G'] = hash_map['C'] ;
        hash_map_rev['g'] = hash_map['c'] ;
        hash_map_rev['T'] = hash_map['A'] ;
        hash_map_rev['t'] = hash_map['a'] ;

        init = true ;
    }

    try
    {   if(rev_compl)
        {   return hash_map_rev.at(base) ; }
        else
        {   return hash_map.at(base) ; }
    }
    // key could not be found
    catch(std::out_of_range& e)
    {   char msg[256] ;
        sprintf(msg, "unrecognized DNA base : %c", base) ;
        throw std::invalid_argument(msg) ;
    }
}


char dna::complement(char base) throw (std::invalid_argument)
{
    static bool init = false ;
    static std::unordered_map<char,char> compl_map ;

    if(not init)
    {   compl_map['A'] = 'T' ;
        compl_map['a'] = 't' ;
        compl_map['C'] = 'G' ;
        compl_map['c'] = 'g' ;
        compl_map['G'] = 'C' ;
        compl_map['g'] = 'c' ;
        compl_map['T'] = 'A' ;
        compl_map['t'] = 'a' ;
    }
    try
    {   return compl_map.at(base) ; }
    // key could not be found
    catch(std::out_of_range& e)
    {   char msg[256] ;
        sprintf(msg, "unrecognized DNA base : %c", base) ;
        throw std::invalid_argument(msg) ;
    }
}


double dna::score_sequence(const Matrix2D<char>& sequences, size_t seq_index, size_t from, const Matrix2D<double>& motif_log)
{
    assert(sequences.get_ncol() >= motif_log.get_ncol()) ;
    assert(sequences.get_nrow() > seq_index) ;
    assert(motif_log.get_nrow() == 4) ;

    size_t to = from + motif_log.get_ncol() ; // will score [from, to)

    assert(to <= sequences.get_ncol()) ;

    double log_likelihood = 0 ;
    for(size_t i=from, j=0; i<to; i++, j++)
    {   log_likelihood += motif_log(dna::hash(sequences(seq_index,i)), j) ; }
    return log_likelihood ;
}


std::vector<double> dna::base_composition(const Matrix2D<char> &sequences, bool both_strands) throw (std::invalid_argument)
{
    double total = 0. ;
    std::vector<double> base_comp(4,0.) ;

    try
    {   for(size_t i=0; i<sequences.get_nrow(); i++)
        {   for(size_t j=0; j<sequences.get_ncol(); j++)
            {   // forward strand
                char c = sequences(i,j) ;
                size_t c_hash = dna::hash(c) ;
                base_comp[c_hash] += 1. ;
                total += 1. ;
                // reverse complement strand
                if(both_strands)
                {   // size_t c_hash_rev = dna::hash(c, true) ;
                    base_comp[4-c_hash-1] += 1. ;
                    total += 1. ;
                }
            }
        }
    }
    // invalid char given to dna::hash()
    catch(std::invalid_argument& e)
    {   throw e ; }

    // normalize
    for(auto& i : base_comp)
    {   i /= total ; }

    return base_comp ;
}
