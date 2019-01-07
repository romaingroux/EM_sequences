#ifndef EMSEQUENCEENGINE_HPP
#define EMSEQUENCEENGINE_HPP

#include "ClusteringEngine.hpp"

#include <iostream>
#include <vector>
#include <stdexcept>             // std::runtime_error
#include "Utility/Constants.hpp" // clustering_codes
#include "Matrix/Matrix2D.hpp"
#include "Matrix/Matrix3D.hpp"
#include "Matrix/Matrix4D.hpp"


class EMSequenceEngine : public ClusteringEngine
{
    public:
        // constructors
        EMSequenceEngine() = delete ;
        EMSequenceEngine(const EMSequenceEngine& other) = delete ;

        /*!
         * \brief Constructs an instance to classify the given sequences,
         * using the given paramters. The backgroud probabilities are
         * computed from the sequence content.
         * \param sequences the sequence to classify.
         * \param n_class the number of classes to discover. At least 1.
         * \param l_motif the motif length in bp.
         * \param flip whether the reverse complement strand should also
         * be used for classification.
         * \param center_shift hether the shift probabilities should be
         * renormalized at iteration to make the density fit a gaussian
         * centered on the most central shift state.
         * \param bg_class whether an extra class modelling the background
         * should be added.
         * \param center_shift hether the shift probabilities should be
         * renormalized at iteration to make the density fit a gaussian
         * centered on the most central shift state.
         * \param seed a sequence to initialise the random number generator.
         * \param seeding the seeding method to use among : "random".
         * \throw std::invalid_argument if one of the given argument has
         * a wrong value.
         */
        EMSequenceEngine(const Matrix2D<char>& sequences,
                         size_t n_class,
                         size_t l_motif,
                         bool flip,
                         bool center_shift,
                         bool bg_class,
                         const std::string& seed,
                         const std::string& seeding) throw (std::invalid_argument);

        /*!
         * \brief Constructs an instance to classifiy the given sequnces
         * using the given motif as starting point.
         * \param sequences the sequence to classify.
         * \param motifs a vector containing the motif to use as starting
         * point and to optimize. The number of motif determines the
         * number of classes.
         * \param flip whether the reverse complement strand should also
         * be used for classification.
         * \param center_shift hether the shift probabilities should be
         * renormalized at iteration to make the density fit a gaussian
         * centered on the most central shift state.
         * \param bg_class whether an extra class modelling the background
         * should be added.
         * \throw std::invalid_argument if one of the given argument has
         * a wrong value.
         */
        EMSequenceEngine(const Matrix2D<char>& sequences,
                         const std::vector<Matrix2D<double>>& motifs,
                         bool flip,
                         bool center_shift,
                         bool bg_class) throw (std::invalid_argument);

        /*!
         * \brief Destructor.
         */
        virtual ~EMSequenceEngine() override ;

        // methods
        /*!
         * \brief Runs one iteration of optimization, that is
         * the sequence are scored with the current motifs and
         * are assigned to the classes (E-step) and the motifs
         * are updated accordingly.
         * \return a constant indicating what to do next :
         * in case of convergence nothing more to do as the
         * solution is stable, in case of success other calls
         * to this method may be required to obtained a better
         * (more optimazed) solution.
         */
        Constants::clustering_codes cluster() override ;

        /*!
         * \brief Sets the initial state using the designated method.
         * \param method the designated method to use among : "random".
         */
        void seeding(const std::string& method) throw (std::runtime_error) override ;

        /*!
         * \brief Returns the motifs.
         * \return a vector containing the motifs.
         */
        std::vector<Matrix2D<double>> get_motifs() const ;

        /*!
         * \brief Returns the posterior probabilities.
         * \return a matrix containing the posterior
         * probabilities.
         */
        Matrix4D<double> get_post_prob() const ;

        /*!
         * \brief Returns the class probabilities.
         * \return a matrix containing the class
         * probabilities.
         */
        Matrix3D<double> get_class_prob() const ;

        /*!
         * \brief Returns the overall class probabilities
         * instead of the details for each shift and flip
         * states as get_class_prob() does.
         * \return the overall class probabilities.
         */
        std::vector<double> get_class_prob_total() const ;

        /*!
         * \brief Prints the motifs to the given stream.
         * \param stream the ouput stream of interest.
         */
        void print_results(std::ostream& stream) const override ;

    private:
        // methods
        /*!
         * \brief Should implement a test returniing whether the
         * last call to cluster() resulted in convergence.
         * \return whether the last call resulted in convergence.
         */
        bool hasConverged() const override ;

        /*!
         * \brief Adds an extra motif initialise to the current
         * background probabilities. Updates the number of classes
         * but not the other data structure dimensions!
         */
        void add_background_class() ;

        /*!
         * \brief Sets the poterior probabilities at random using a beta
         * distribution and updates the class probabilities.
         */
        void seeding_random() ;

        /*!
         * \brief Modifies the class probabilities in such a way that the
         * shift probabilities are then normaly distributed, centered on
         * the middle shift state. However, the overall class probabilities
         * are remains unchanged.
         */
        void center_shifts() ;

        /*!
         * \brief Computes the class probabilities according to the
         * the current posterior probabilities.
         */
        void compute_class_prob() ;

        /*!
         * \brief Computes the motif according to the current posterior
         * probabilities and class probabilities.
         */
        void compute_motifs() ;

        /*!
         * \brief Computes the sequence likelihood (that is, the probability
         * of the sequences given the current motifs).
         */
        void compute_likelihood() ;

        /*!
         * \brief Computes the posterior probability of each sequence to
         * belong to each class, for each shift and flip state.
         */
        void compute_posterior_prob() ;

        /*!
         * \brief Normalizes the motifs according the their own
         * base composition. For each motif (but an eventual background
         * class), 1) the base composition over the whole motif is
         * computed, 2) the ratio motif probabilities over the
         * corresponding base probability are computed and 3) turned
         * to probability by normalizing each column value by the
         * sum of the column. These last values are then the
         * normalized motif.
         */
        void normalise_motifs() ;

        int debug() ;
        std::vector<double> compute_information_content() const ;
        void print_alignment() const ;


        // fields
        /*!
         * \brief the sequences (on each row).
         */
        Matrix2D<char> _sequences ;
        /*!
         * \brief a vector containing each class motif.
         */
        std::vector<Matrix2D<double>> _motifs ;
        /*!
         * \brief the base background probabilities.
         */
        std::vector<double> _bg_prob ;
        /*!
         * \brief the class probabilities.
         */
        Matrix3D<double> _class_prob ;
        /*!
         * \brief the overall class probabilities (equal to
         * the values in _class_prob when summing over the
         * shift and flip states).
         */
        std::vector<double> _class_prob_tot ;
        /*!
         * \brief the sequence posterior probabilities to belong
         * to each of the classes.
         */
        Matrix4D<double> _post_prob ;
        /*!
         * \brief the sequence posterior probabilities to belong
         * to each of the classes at the previous iteration.
         */
        Matrix4D<double> _post_prob_prev ;
        /*!
         * \brief the sequence likelihoods.
         */
        Matrix4D<double> _likelihood ;
        /*!
         * \brief the current number of iterations.
         */
        size_t _n_iter ;
        /*!
         * \brief the number of sequences.
         */
        size_t _n_seq ;
        /*!
         * \brief the sequence lengths.
         */
        size_t _l_seq ;
        /*!
         * \brief the number of classes.
         */
        size_t _n_class ;
        /*!
         * \brief the motif length in bp.
         */
        size_t _l_motif ;
        /*!
         * \brief the number of shifts.
         */
        size_t _n_shift ;
        /*!
         * \brief the number of flip states (1 or 2).
         */
        size_t _n_flip ;
        /*!
         * \brief whether an extra class modelling the background
         * has been added.
         */
        bool _bg_class ;
        /*!
         * \brief whether the shift probabilities should be renormalized
         * at each iteration to make the density fit a gaussian centered
         * on the most central shift state.
         */
        bool _shift_center ;

} ;

#endif // EMSEQUENCEENGINE_HPP
