
#include "EMSequenceEngine.hpp"

#include <iostream>
#include <iomanip>    // setprecision(), setw(), fixed
#include <cassert>
#include <stdexcept>  // runtime_error
#include <numeric>
#include <cmath>      // log(), log2(), exp()
#include <algorithm>  // inner_product()
#include <random>     // normal_distribution()

#include "Matrix/Matrix2D.hpp"
#include "Matrix/Matrix3D.hpp"
#include "Matrix/Matrix4D.hpp"
#include "Random/RandomNumberGenerator.hpp"
#include "Random/BetaDistribution.hpp"
#include "Utility/DNA_utility.hpp"
#include "Utility/Constants.hpp"      // Constants
#include "Utility/Vector_utility.hpp"
#include "Utility/Utility.hpp"        // isEqual()
#include "Statistics/Statistics.hpp"  // sd()

EMSequenceEngine::EMSequenceEngine(const Matrix2D<char>& sequences,
                                   size_t n_class,
                                   size_t l_motif,
                                   bool flip,
                                   bool center_shift,
                                   bool bg_class,
                                   const std::string& seed,
                                   const std::string& seeding) throw (std::invalid_argument)
    : _sequences(sequences), _bg_prob({0.25, 0.25, 0.25, 0.25}), _n_iter(0),
      _n_seq(_sequences.get_nrow()), _l_seq(_sequences.get_ncol()), _n_class(n_class),
      _l_motif(l_motif), _n_shift(_l_seq-_l_motif+1), _n_flip(1+flip), _bg_class(bg_class),
      _shift_center(center_shift)
{
    // check number of classes and motif length
    if((this->_n_class == 0) or (this->_n_class > this->_n_seq) )
    {   throw std::invalid_argument("error! the number of classes should at least be 1, at most the number of sequences!") ; }
    else if((this->_l_motif == 0) or (this->_l_motif > this->_l_seq))
    {   throw std::invalid_argument("error! the motif length should be at least 1, at most the sequence length!") ; }

    // init the data structures
    this->_likelihood      = Matrix4D<double>(this->_n_seq,   this->_n_class+bg_class, this->_n_shift, this->_n_flip) ;
    this->_post_prob       = Matrix4D<double>(this->_n_seq,   this->_n_class+bg_class, this->_n_shift, this->_n_flip) ;
    this->_class_prob      = Matrix3D<double>(this->_n_class+bg_class, this->_n_shift, this->_n_flip) ;
    this->_class_prob_tot  = std::vector<double>(this->_n_class, 0.) ;
    this->_motifs          = std::vector<Matrix2D<double>>(this->_n_class, Matrix2D<double>(4,this->_l_motif)) ;

    // compute background from sequences
    this->_bg_prob = dna::base_composition(this->_sequences, flip) ;

    // add background class if needed (also increases this->_n_class)
    if(this->_bg_class)
    {   this->add_background_class() ; }

    // seeds the random number generator BEFORE USING IT
    if(seed != std::string(""))
    {   getRandomGenerator(seed) ; }
    else
    {   getRandomGenerator() ; }
    // seeds
    this->seeding(seeding) ;
}

EMSequenceEngine::EMSequenceEngine(const Matrix2D<char>& sequences,
                                   const std::vector<Matrix2D<double> >& motifs,
                                   bool flip,
                                   bool center_shift,
                                   bool bg_class) throw (std::invalid_argument)
    : _sequences(sequences), _motifs(motifs), _bg_prob({0.25, 0.25, 0.25, 0.25}), _n_iter(0),
      _n_seq(_sequences.get_nrow()), _l_seq(_sequences.get_ncol()), _n_class(_motifs.size()),
      _l_motif(_motifs[0].get_ncol()), _n_shift(_l_seq-_l_motif+1), _n_flip(1+flip), _bg_class(bg_class),
      _shift_center(center_shift)
{
    // check the number of classes and the motif length
    if((this->_n_class == 0) or (this->_n_class > this->_n_seq) )
    {   throw std::invalid_argument("error! the number of classes should at least be 1, at most the number of sequences!") ; }
    else if((this->_l_motif == 0) or (this->_l_motif > this->_l_seq))
    {   throw std::invalid_argument("error! the motif length should be at least 1, at most the sequence length!") ; }

    // check that all motifs have the same length
    for(auto& motif : this->_motifs)
    {   if(motif.get_ncol() != this->_l_motif)
        {   throw std::invalid_argument("error! the motifs should all have the same length!") ; }
    }

    // init the data structures
    this->_likelihood      = Matrix4D<double>(this->_n_seq,   this->_n_class+bg_class, this->_n_shift, this->_n_flip) ;
    this->_post_prob       = Matrix4D<double>(this->_n_seq,   this->_n_class+bg_class, this->_n_shift, this->_n_flip) ;
    this->_class_prob      = Matrix3D<double>(this->_n_class+bg_class, this->_n_shift, this->_n_flip) ;
    this->_class_prob_tot  = std::vector<double>(this->_n_class, 0.) ;

    // compute background from sequences
    this->_bg_prob = dna::base_composition(this->_sequences, flip) ;

    // add background class if needed (also increases this->_n_class)
    if(this->_bg_class)
    {   this->add_background_class() ; }

    // Initialise posterior probabilities :
    // option 1) initialise as equally likely
    /*
    double p = 1. / static_cast<double>(this->_n_class*this->_n_shift*this->_n_flip) ;
    this->_post_prob  = Matrix4D<double>(this->_n_seq,   this->_n_class, this->_n_shift, this->_n_flip, p) ;
    this->compute_class_prob() ;
    */

    // option 2) compute likelihood given the current model use this to set the initial
    // posterior probabilities

    this->compute_likelihood() ;
    for(size_t i=0; i<this->_n_seq; i++)
    {   double sum = 0. ;
        for(size_t k=0; k<this->_n_class; k++)
        {   for(size_t s=0; s<this->_n_shift; s++)
            {   for(size_t f=0; f<this->_n_flip; f++)
                {   sum += this->_likelihood(i,k,s,f) ; }
            }
        }
        for(size_t k=0; k<this->_n_class; k++)
        {   for(size_t s=0; s<this->_n_shift; s++)
            {   for(size_t f=0; f<this->_n_flip; f++)
                {
                    if(this->_likelihood(i,k,s,f) == 0.)
                    {   this->_post_prob(i,k,s,f) = Constants::pseudo_counts ; }
                    else
                    {   this->_post_prob(i,k,s,f) = this->_likelihood(i,k,s,f)/sum ; }
                }
            }
        }
    }
    this->compute_class_prob() ;
    // this->center_shifts() ;

}


EMSequenceEngine::~EMSequenceEngine()
{}

Constants::clustering_codes EMSequenceEngine::cluster()
{
    // keep track of last iteration results
    if(this->_n_iter > 0)
    {   this->_post_prob_prev = this->_post_prob ; }

    // E-step
    this->compute_likelihood() ;
    this->compute_posterior_prob() ;
    this->compute_class_prob() ;

    // this->print_alignment() ;

    // M-step
    this->compute_motifs() ;
    this->normalise_motifs() ;
    this->_n_iter++ ;

    // normalize the shifts
    if(this->_shift_center)
    {   this->center_shifts() ; }

    // the clustering cannot fail in the sense that
    // a class can never get a 0 probability (care is
    // taken that posterior prob. can never go to 0)
    if(this->hasConverged())
    {   return Constants::clustering_codes::CONVERGENCE ; }
    else
    {   return Constants::clustering_codes::SUCCESS ; }
}

void EMSequenceEngine::seeding(const std::string &method) throw (std::runtime_error)
{   if(method == "random")
    {   this->seeding_random() ; }
    else
    {   throw std::runtime_error("unkown seeding") ; }
}

std::vector<Matrix2D<double>> EMSequenceEngine::get_motifs() const
{   return this->_motifs ; }

Matrix4D<double> EMSequenceEngine::get_post_prob() const
{   return this->_post_prob ; }

Matrix3D<double> EMSequenceEngine::get_class_prob() const
{   return this->_class_prob ; }

std::vector<double> EMSequenceEngine::get_class_prob_total() const
{   return this->_class_prob_tot ; }

void EMSequenceEngine::print_results(std::ostream& stream) const
{

    for(const auto& motif : this->_motifs)
    {   stream << motif << std::endl << std::endl ; }

    // stream << this->_post_prob << std::endl << std::endl ;
    // stream << this->_likelihood << std::endl << std::endl ;
}

bool EMSequenceEngine::hasConverged() const
{   bool convergence = true ;

    // there were no previous value, cannot check for convergence
    if(this->_n_iter == 1)
    {   convergence = false ; }
    // let's check the probs
    else
    {   for(size_t i=0; i<this->_post_prob.get_data_size(); i++)
        {   if(std::abs(this->_post_prob.get({i}) - this->_post_prob_prev.get({i})) > Constants::delta_max)
            {   convergence = false ;
                break ;
            }
        }
    }
    return convergence ;
}

void EMSequenceEngine::add_background_class()
{   Matrix2D<double> bg_motif(4, this->_l_motif) ;
    for(size_t i=0; i<bg_motif.get_nrow(); i++)
    {   for(size_t j=0; j<bg_motif.get_ncol(); j++)
        {   bg_motif(i,j) = this->_bg_prob[i] ; }
    }
    this->_motifs.push_back(bg_motif) ;
    this->_n_class++ ;
}

void EMSequenceEngine::seeding_random()
{   // random sampling
    beta_distribution<> beta(1, this->_n_seq) ;
    for(size_t i=0; i<this->_post_prob.get_data_size(); i++)
    {   this->_post_prob.set(i, beta(getRandomGenerator())) ; }

    // normalization
    for(size_t i=0; i<this->_n_seq; i++)
    {   double sum = 0. ;
        for(size_t k=0; k<this->_n_class; k++)
        {   for(size_t s=0; s<this->_n_shift; s++)
            {   for(size_t f=0; f<this->_n_flip; f++)
                {   sum += this->_post_prob(i,k,s,f) ; }
            }
        }
        for(size_t k=0; k<this->_n_class; k++)
        {   for(size_t s=0; s<this->_n_shift; s++)
            {   for(size_t f=0; f<this->_n_flip; f++)
                {   this->_post_prob(i,k,s,f) /= sum ; }
            }
        }
    }

    // class probabilities update
    this->compute_class_prob() ;

    // motif updates
    this->compute_motifs() ;
}

void EMSequenceEngine::center_shifts()
{
    if(this->_n_shift == 1)
    {   return ; }

    // the possible shift states
    std::vector<double> shifts(this->_n_shift) ;
    std::iota(shifts.begin(), shifts.end(), 1.) ;

    // the shift probabilities and the class probabilies (no need to norm., _class_prob sums to 1)
    double shifts_prob_measured_tot = 0. ;
    std::vector<double> shifts_prob_measured(this->_n_shift) ;
    for(size_t s=0; s<this->_n_shift; s++)
    {   for(size_t k=0; k<this->_n_class; k++)
        {   for(size_t f=0; f<this->_n_flip; f++)
            {   shifts_prob_measured[s]  += this->_class_prob(k,s,f) ;
                shifts_prob_measured_tot += this->_class_prob(k,s,f) ;
            }
        }
    }

    // the shift mean and (biased) standard deviation
    // double shifts_mean = mean(shifts, shifts_prob_measured) ;
    double shifts_sd = sd(shifts, shifts_prob_measured, false) ;
    // double shifts_m  = mean(shifts, shifts_prob_measured) ;

    // the shift probabilities under the assumption that is distributed as a gaussian centered on
    // the central shift state with sd and mean as in the data
    // sd as the data
    std::vector<double> shifts_prob_centered(shifts.size(), 0.) ;
    double shifts_prob_centered_tot = 0. ;
    for(size_t i=0; i<shifts.size(); i++)
    {   shifts_prob_centered[i]   = dnorm(shifts[i], (this->_n_shift/2)+1, shifts_sd) ;
        shifts_prob_centered_tot += shifts_prob_centered[i] ;
    }

    for(size_t k=0; k<this->_n_class; k++)
    {   for(size_t f=0; f<this->_n_flip; f++)
        {   for(size_t s=0; s<this->_n_shift; s++)
            {   this->_class_prob(k,s,f) = this->_class_prob_tot[k] * shifts_prob_centered[s] / (this->_n_flip * shifts_prob_centered_tot) ;

            }
        }
    }

    // shifts_prob_measured_tot = 0. ;
    shifts_prob_measured.clear() ;
    shifts_prob_measured.resize(this->_n_shift) ;
    for(size_t s=0; s<this->_n_shift; s++)
    {   for(size_t k=0; k<this->_n_class; k++)
        {   for(size_t f=0; f<this->_n_flip; f++)
            {   shifts_prob_measured[s]  += this->_class_prob(k,s,f) ;
                // shifts_prob_measured_tot += this->_class_prob(k,s,f) ;
            }
        }
    }
}

void EMSequenceEngine::compute_class_prob()
{
    // reset
    this->_class_prob_tot = std::vector<double>(this->_n_class, 0.) ;

    // sum over posterior prob
    double prob_tot = 0. ;
    for(size_t k=0; k<this->_n_class; k++)
    {   for(size_t s=0; s<this->_n_shift; s++)
        {   for(size_t f=0; f<this->_n_flip; f++)
             {  double prob = 0. ;
                for(size_t i=0; i<this->_n_seq; i++)
                {   prob += this->_post_prob(i,k,s,f) ; }
                prob_tot += prob ;
                this->_class_prob(k,s,f) = prob ;
                this->_class_prob_tot[k] += prob ;
            }
        }
    }
    // normalization
    for(size_t k=0; k<this->_n_class; k++)
    {   for(size_t s=0; s<this->_n_shift; s++)
        {   for(size_t f=0; f<this->_n_flip; f++)
            {   this->_class_prob(k,s,f) /= prob_tot ; }
        }
        this->_class_prob_tot[k] /= prob_tot ;
    }

    // for(size_t i=0; i<prob_class.size(); i++)
    // {   prob_class[i] /= prob_class_tot ;
    //     std::cerr << "prob class " << i << "  " << prob_class[i] << std::endl ;
    // }
    // std::cerr << "class probs" << std::endl ;
    // std::cerr << this->_class_prob << std::endl << std::endl ;
}

void EMSequenceEngine::compute_motifs()
{
    // int corr = this->debug() ;

    // if there is a background class, don't touch it, leave it untrained
    size_t n_class = this->_n_class - this->_bg_class ;

    // reset motifs
    for(size_t i=0; i<4; i++)
    {   for(size_t j=0; j<this->_l_motif; j++)
        {   for(size_t k=0; k<n_class; k++)
            {   this->_motifs[k](i,j) = 0. ; }
        }
    }

    // recompute them
    for(size_t k=0; k<n_class; k++)
    {   for(size_t s=0; s<this->_n_shift; s++)
        {   for(size_t j=0; j<this->_l_motif; j++)
            {   std::vector<double> base_prob(4,0.) ;     // base prob on forward strand
                std::vector<double> base_prob_rev(4,0.) ; // base prob on reverse strand

                for(size_t i=0; i<this->_n_seq; i++)
                {   char base = this->_sequences(i, s+j) ;
                    // forward strand
                    {   base_prob[dna::hash(base)]           += this->_post_prob(i,k,s,Constants::FORWARD) ; }
                    // reverse strand
                    if(this->_n_flip == 2)
                    {   base_prob_rev[dna::hash(base, true)] += this->_post_prob(i,k,s,Constants::REVERSE) ; }
                }

                for(size_t i=0,i_rev= base_prob.size()-1; i<4; i++,i_rev--)
                {   // forward strand
                    {   this->_motifs[k](i,j)                  += base_prob[i] ; }
                    // {   int corrected_j = j - corr ;
                    //     if(corrected_j >= static_cast<int>(this->_l_motif) or
                    //        corrected_j < 0)
                    //     {   continue ; }
                    // }
                    // reverse strand
                    if(this->_n_flip == 2)
                    {   // this->_motifs(i_rev,j,k) += base_prob_rev[i] ;
                        this->_motifs[k](i,this->_l_motif-j-1) += base_prob_rev[i] ;
                    }
                }
            }
        }

        // normalize the columns and avoid 0 values by adding some pseudocounts
        for(size_t j=0; j<this->_l_motif; j++)
        {   double sum = 0. ;
            for(size_t i=0; i<4; i++)
            {   // avoid 0 values
                this->_motifs[k](i,j) = this->_motifs[k](i,j) + Constants::pseudo_counts ;
                sum += this->_motifs[k](i,j) ;
            }
            // normalize
            for(size_t i=0; i<4; i++)
            {   this->_motifs[k](i,j) = this->_motifs[k](i,j) / sum; }
        }
    }

    // std::cerr << "motifs" << std::endl ;
    // for(const auto& motif : this->_motifs)
    // {   std::cerr << motif << std::endl << std::endl ; }
}

void EMSequenceEngine::compute_likelihood()
{   for(size_t k=0; k<this->_n_class; k++)
    {
        // compute the log prob motif and the log prob reverse-complement motif
        size_t nrow = 4, ncol = this->_l_motif ;
        Matrix2D<double> motif_log(nrow, ncol) ;
        Matrix2D<double> motif_log_rev(nrow, ncol) ;
        for(size_t i=0; i<nrow; i++)
        {   for(size_t j=0; j<ncol; j++)
            {   motif_log(i,j)                   = log(this->_motifs[k](i,j)) ;
                motif_log_rev(nrow-i-1,ncol-j-1) = log(this->_motifs[k](i,j));
            }
        }

        // std::cerr << motif_log << std::endl<<std::endl ;
        // std::cerr << motif_log_rev << std::endl << "---------------------------------------" << std::endl ;

        for(size_t i=0; i<this->_n_seq; i++)
        {   for(size_t s=0; s<this->_n_shift; s++)
            {   // forward strand
                {   this->_likelihood(i,k,s,Constants::FORWARD) = exp(dna::score_sequence(this->_sequences, i, s, motif_log)) ; }
                // reverse strand
                if(this->_n_flip == 2)
                {   this->_likelihood(i,k,s,Constants::REVERSE) = exp(dna::score_sequence(this->_sequences, i, s, motif_log_rev)) ; }
            }
        }
    }

    // std::cerr << "likelihoods" << std::endl ;
    // std::cerr << this->_likelihood << std::endl << std::endl ;
}

void EMSequenceEngine::compute_posterior_prob()
{   // compute
    for(size_t i=0; i<this->_n_seq; i++)
    {   for(size_t k=0; k<this->_n_class; k++)
        {   for(size_t s=0; s<this->_n_shift; s++)
            {   for(size_t f=0; f<this->_n_flip; f++)
                {   // add pseudo counts to avoid 0 values
                    this->_post_prob(i,k,s,f) = (this->_likelihood(i,k,s,f) * this->_class_prob(k,s,f)) ;
                }
            }
        }
    }
    // normalize
    for(size_t i=0; i<this->_n_seq; i++)
    {   double sum = 0. ;
        for(size_t k=0; k<this->_n_class; k++)
        {   for(size_t s=0; s<this->_n_shift; s++)
            {   for(size_t f=0; f<this->_n_flip; f++)
                {   sum += this->_post_prob(i,k,s,f) ; }
            }
        }
        for(size_t k=0; k<this->_n_class; k++)
        {   for(size_t s=0; s<this->_n_shift; s++)
            {   for(size_t f=0; f<this->_n_flip; f++)
                {   this->_post_prob(i,k,s,f) = this->_post_prob(i,k,s,f) / sum;
                    if(this->_post_prob(i,k,s,f) == 0.)
                    {   this->_post_prob(i,k,s,f) = Constants::pseudo_counts ; }
                }
            }
        }
    }

    // std::cerr << "posteriors" << std::endl ;
    // std::cerr << this->_post_prob << std::endl << std::endl ;
}

void EMSequenceEngine::normalise_motifs()
{
    size_t n_class = this->_n_class - this->_bg_class ;

    for(size_t k=0; k<n_class; k++)
    {   // get background base prob in motif
        Matrix2D<double>& motif = this->_motifs[k] ;
        size_t n_row = motif.get_nrow() ;
        size_t n_col = this->_l_motif ;
        std::vector<double> base_comp(n_row, 0.) ;
        for(size_t i=0; i<n_row; i++)
        {   double sum = 0. ;
            for(size_t j=0; j<n_col; j++)
            {   base_comp[i] += motif(i,j) ;
                sum += motif(i,j) ;
            }
            base_comp[i] /= sum ;
        }
        // computing the ratio motif/base_comp and turn them into prob
        Matrix2D<double> ratio(n_row, n_col, 0.) ;
        for(size_t j=0; j<n_col; j++)
        {   double sum = 0. ;
            // compute ratio
            for(size_t i=0; i<n_row; i++)
            {   ratio(i,j) = motif(i,j) / base_comp[i] ;
                sum += motif(i,j) ;
            }
            // turn into prob
            for(size_t i=0; i<n_row; i++)
            {   motif(i,j) = ratio(i,j) / sum ; }
        }
    }
}


/*
 *normalize.motifs = function(motifs, comp=NULL)
{ n_class = dim(motifs)[3] # the number of classes
  for(i in 1:n_class)
  { # base composition
    if(is.null(comp))
    { comp      = rowMeans(motifs[,,i]) }
    # ratio to base composition
    ratio       = motifs[,,1] / comp
    # turn ratio to a probability matrix
    motifs[,,i] = t(t(ratio) / colSums(ratio))
  }
  return(motifs)
}
 *
 *
 *
 */

int EMSequenceEngine::debug()
{
    // compute the information content OF CLASS 1 ONLY
    std::vector<double> info_cont = this->compute_information_content() ;

    // remove any negative value
    for(auto& x : info_cont)
    {   if(x < 0.)
        {   x = 0. ; }
    }
    // find the position where the center of gravity of the information content is
    double info_cont_tot = std::accumulate(info_cont.begin(), info_cont.end(), 0.) ;
    double info_cont_tot_half = info_cont_tot / 2. ;
    size_t i=0 ;
    double info_cont_cur = 0. ;
    do
    {   info_cont_cur += info_cont[i] ;
        i++ ;
    } while(info_cont_cur < info_cont_tot_half) ;
    i-- ;
    int motif_l     = this->_l_motif ;
    int motif_l_mid = (this->_l_motif/2)+1 ;
    int diff_to_mid = motif_l_mid - motif_l ;

    std::cerr << "    info content           " << info_cont << std::endl ;
    std::cerr << "    seq length             " << this->_l_seq << std::endl ;
    std::cerr << "    center gravity (motif) " << i << std::endl ;
    std::cerr << "    middle motif           " << motif_l_mid << std::endl ;
    std::cerr << "    diff to middle motif   " << diff_to_mid << std::endl ;
    std::cerr << "1/2 diff to middle motif   " << diff_to_mid/2 << std::endl ;
    std::cerr << "    center shift           " << (this->_n_shift/2)+1 << std::endl << std::endl ;

    return diff_to_mid/2 ;
}


std::vector<double> EMSequenceEngine::compute_information_content() const
{   std::vector<double> H(this->_l_motif,0) ; // enthropy
    std::vector<double> R(this->_l_motif,0) ; // information content

    // normalize
    // normally, pseudo counts should be added to avoid 0
    // values however this is already done, this matrix
    // should not have any 0 value.
    Matrix2D<double> matrix2(this->_motifs[0]) ;
    for(size_t j=0; j<this->_l_motif; j++)
    {   double sum = 0. ;
        // column total
        for(size_t i=0; i<4; i++)
        {   sum += matrix2(i,j) ; }
        // norm. by column total
        for(size_t i=0; i<4; i++)
        {   matrix2(i,j) /= sum ; }
    }

    // a correction factor, it is normally computed as
    // (1 / ln(2) ) * (nrow / 2*nseq) where nseq is the number
    // of sequences used to make <matrix>
    // double correction = 0. ;
    double correction = (1. / log2(2.)) * (4./(2.*this->_n_seq)) ;
    // compute entropy at each position
    for(size_t j=0; j<this->_l_motif; j++)
    {   double h = 0. ;
        for(size_t i=0; i<4; i++)
        {   h += matrix2(i,j) * log2(matrix2(i,j)) ; }
        H[j] = -h ;
    }

    // compute the information content at each position
    for(size_t j=0; j<this->_l_motif; j++)
    {   R[j] = log2(4) - (H[j] + correction) ; }
    return R ;
}


void EMSequenceEngine::print_alignment() const
{
    for(size_t k=0; k<this->_n_class; k++)
    {   std::cerr << "class " << k+1 << std::endl ;
        for(size_t f=0; f<this->_n_flip; f++)
        {    for(size_t i=0; i<this->_n_seq; i++)
            {   for(size_t s=0; s<this->_n_shift; s++)
                {   // print the subseq
                    for(size_t j=0; j<this->_l_motif; j++)
                    {   std::cerr << this->_sequences(i,j+s) ; }
                    // print the prob
                    std::cerr << "    " << std::setprecision(4) << this->_post_prob(i,k,s,f) << std::endl ;
                }
            }
        }
    }
    std::cerr << "-----------------------------------------" << std::endl ;
}







