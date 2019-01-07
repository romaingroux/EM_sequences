#ifndef DNA_UTILITY_HPP
#define DNA_UTILITY_HPP

#include <iostream>
#include <stdexcept>  // invalid_argument
#include "Matrix/Matrix2D.hpp"

namespace dna
{
    /*!
     * \brief Returns a string containing all the valid DNA characters.
     * \return a string containing the valid DNA characters.
     */
    std::string get_valid_dna_char() ;

    /*!
     * \brief Returns a string containing all the invalid DNA characters.
     * \return a string containing the invalid DNA characters.
     */
    std::string get_invalid_dna_char() ;

    /*!
     * \brief Computes a hash for a DNA base.
     * The hashes are :
     * 0 for 'A' or 'a',
     * 1 for 'C' or 'c',
     * 2 for 'G' or 'g',
     * 3 for 'T' or 't'.
     * If rev_compl is set, then the hash corresponding
     * to the complementary base is returned.
     * \param base a DNA base.
     * \param rev_compl whether the hash for the complementary
     * base should be returned.
     * \throw std::invalid_argument if the given base is not
     * recognized.
     * \return the base hash.
     */
    size_t hash(char base, bool rev_compl=false) throw (std::invalid_argument) ;

    /*!
     * \brief Returns the complementary base of the
     * given base.
     * \param base the base of interest.
     * \throw std::invalid_argument if the given base is not
     * recognized.
     * \return the complement of the given base.
     */
    char complement(char base) throw (std::invalid_argument) ;

    /*!
     * \brief Scores a specific part of a sequence contained in the given matrix
     * using the given motif. The motif is expected to containing log probability.
     * Only the sub-sequence [from->from+motif_length) from the seq_index-th row
     * of the sequence matrix will be scored.
     * \param sequences a reference to the matrix containing the sub-sequence to
     * score.
     * \param seq_index the index of the row in sequences containing the sequence
     * to score.
     * \param from the first position of the subsequence to score using the motif
     * \param motif_log a matrix containing log probabilities. The matrix should be
     * a motif in horizontal format, that is with 4 rows corresponding to A (0th),
     * C (1st), G (2nd) and T (4th).
     * \return the log likelihood of the sequence given the model.
     */
    double score_sequence(const Matrix2D<char>& sequences, size_t seq_index, size_t from, const Matrix2D<double>& motif_log) ;

    /*!
     * \brief Computes the base composition of a set of sequences contained in a matrix.
     * \param sequences a matrix containing the sequences of interest.
     * \param both_strands also accounts for the reverse complement of the sequences.
     * \throw std::invalid_argument if a non-supported character is found in the matrix.
     * \return a vector of 4 values corresponding to the frequencies of A,C,G and T
     * respectively.
     */
    std::vector<double> base_composition(const Matrix2D<char>& sequences, bool both_strands=false) throw (std::invalid_argument) ;

}

#endif // DNA_UTILITY_HPP
