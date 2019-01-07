#include <UnitTest++/UnitTest++.h>
#include <stdexcept> // invalid_argument
#include <string>
#include <cmath>     // log(), exp()

#include "Utility/DNA_utility.hpp"
#include "Matrix/Matrix2D.hpp"

#include "Utility/Vector_utility.hpp"

SUITE(DNA_utility)
{
    // displays message
    TEST(message)
    {   std::cout << "Starting DNA_utility tests..." << std::endl ; }

    // tests the dna::hash() function
    TEST(hash)
    {   // check accepted characters
        CHECK_EQUAL(0, dna::hash('A')) ;
        CHECK_EQUAL(0, dna::hash('a')) ;
        CHECK_EQUAL(1, dna::hash('C')) ;
        CHECK_EQUAL(1, dna::hash('c')) ;
        CHECK_EQUAL(2, dna::hash('G')) ;
        CHECK_EQUAL(2, dna::hash('g')) ;
        CHECK_EQUAL(3, dna::hash('T')) ;
        CHECK_EQUAL(3, dna::hash('t')) ;

        CHECK_EQUAL(3, dna::hash('A', true)) ;
        CHECK_EQUAL(3, dna::hash('a', true)) ;
        CHECK_EQUAL(2, dna::hash('C', true)) ;
        CHECK_EQUAL(2, dna::hash('c', true)) ;
        CHECK_EQUAL(1, dna::hash('G', true)) ;
        CHECK_EQUAL(1, dna::hash('g', true)) ;
        CHECK_EQUAL(0, dna::hash('T', true)) ;
        CHECK_EQUAL(0, dna::hash('t', true)) ;

        // check all other characters
        for(const auto& c : dna::get_invalid_dna_char())
        {   CHECK_THROW(dna::hash(c),       std::invalid_argument) ;
            CHECK_THROW(dna::hash(c, true), std::invalid_argument) ;
        }
    }

    // tests the dna::complement() function
    TEST(complement)
    {
        // check accepted characters
        CHECK_EQUAL('T', dna::complement('A')) ;
        CHECK_EQUAL('t', dna::complement('a')) ;
        CHECK_EQUAL('G', dna::complement('C')) ;
        CHECK_EQUAL('g', dna::complement('c')) ;
        CHECK_EQUAL('C', dna::complement('G')) ;
        CHECK_EQUAL('c', dna::complement('g')) ;
        CHECK_EQUAL('A', dna::complement('T')) ;
        CHECK_EQUAL('a', dna::complement('t')) ;

        // check all other characters
        for(const auto& c : dna::get_invalid_dna_char())
        {   CHECK_THROW(dna::complement(c), std::invalid_argument) ; }
    }

    // tests dna::score_sequence() function
    TEST(score_sequence)
    {   // allowed error
        double error = 1e-6 ;

        // contains ACGT and TACG
        Matrix2D<char> sequences(2,4) ;
        sequences(0,0) = 'A' ; sequences(0,1) = 'C' ; sequences(0,2) = 'G' ; sequences(0,3) = 'T' ;
        sequences(1,0) = 'T' ; sequences(1,1) = 'A' ; sequences(1,2) = 'C' ; sequences(1,3) = 'G' ;

        // represent a sequence ACG w/o any missmatch allowed
        // in log prob
        Matrix2D<double> motif(4,3) ;
        motif(0,0) = log(1.)   ; motif(0,1) = log(1e-8) ; motif(0,2) = log(1e-8) ;
        motif(1,0) = log(1e-8) ; motif(1,1) = log(1.)   ; motif(1,2) = log(1e-8) ;
        motif(2,0) = log(1e-8) ; motif(2,1) = log(1e-8) ; motif(2,2) = log(1.) ;
        motif(3,0) = log(1e-8) ; motif(3,1) = log(1e-8) ; motif(3,2) = log(1e-8) ;

        // scores ->ACG<-T
        CHECK_CLOSE(1., exp(dna::score_sequence(sequences, 0, 0, motif)), error) ;
        // scores A->CGT<-
        CHECK_CLOSE(0., exp(dna::score_sequence(sequences, 0, 1, motif)), error) ;
        // scores ->TAC<-G
        CHECK_CLOSE(0., exp(dna::score_sequence(sequences, 1, 0, motif)), error) ;
        // scores T->ACG<-
        CHECK_CLOSE(1., exp(dna::score_sequence(sequences, 1, 1, motif)), error) ;


        // represent a sequence A[C/G]G with equally likely C or G
        // in log prob
        Matrix2D<double> motif2(4,3) ;
        motif2(0,0) = log(1.)   ; motif2(0,1) = log(1e-8) ; motif2(0,2) = log(1e-8) ;
        motif2(1,0) = log(1e-8) ; motif2(1,1) = log(0.5)  ; motif2(1,2) = log(1e-8) ;
        motif2(2,0) = log(1e-8) ; motif2(2,1) = log(0.5)  ; motif2(2,2) = log(1.) ;
        motif2(3,0) = log(1e-8) ; motif2(3,1) = log(1e-8) ; motif2(3,2) = log(1e-8)  ;

        // scores ->ACG<-T
        CHECK_CLOSE(.5, exp(dna::score_sequence(sequences, 0, 0, motif2)), error) ;
        // scores A->CGT<-
        CHECK_CLOSE(0., exp(dna::score_sequence(sequences, 0, 1, motif2)), error) ;
        // scores ->TAC<-G
        CHECK_CLOSE(0., exp(dna::score_sequence(sequences, 1, 0, motif2)), error) ;
        // scores T->ACG<-
        CHECK_CLOSE(.5, exp(dna::score_sequence(sequences, 1, 1, motif2)), error) ;
    }

    // tests dna::base_composition()
    TEST(base_composition)
    {
       // with valid DNA char
       Matrix2D<char> sequences(2,4) ;
       sequences(0,0) = 'A' ; sequences(0,1) = 'C' ; sequences(0,2) = 'G' ; sequences(0,3) = 'T' ;
       sequences(1,0) = 'A' ; sequences(1,1) = 'A' ; sequences(1,2) = 'A' ; sequences(1,3) = 'A' ;

       std::vector<double> comp_exp     = {5./8.,  1./8.,  1./8.,  1./8.} ;
       std::vector<double> comp_exp_rev = {6./16., 2./16., 2./16., 6./16.} ;

       std::vector<double> comp     = dna::base_composition(sequences, false) ;
       std::vector<double> comp_rev = dna::base_composition(sequences, true) ;

       CHECK_ARRAY_EQUAL(comp_exp,     comp, comp_exp.size()) ;
       CHECK_ARRAY_EQUAL(comp_exp_rev, comp_rev,  comp_exp_rev.size()) ;

       // with invalid DNA char
       for(const auto& i : dna::get_invalid_dna_char())
       {    Matrix2D<char> sequences_2(2,4,'A') ;
            sequences_2(0,0) = i ;
            CHECK_THROW(dna::base_composition(sequences_2, false), std::invalid_argument) ;
            CHECK_THROW(dna::base_composition(sequences_2, true),  std::invalid_argument) ;
       }
    }
}
