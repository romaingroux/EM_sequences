#ifndef CLUSTERINGENGINE_HPP
#define CLUSTERINGENGINE_HPP

#include <iostream>
#include <stdexcept> // std::runtime_error
#include "Utility/Constants.hpp"


class ClusteringEngine
{
    public:
        /*!
         * \brief The destructor.
         */
        virtual ~ClusteringEngine() ;

        /*!
         * \brief Should make the instance run a single round
         * of data assignment to the clusters/classes.
         * \return a constant indicating what to do next :
         * in case of convergence nothing more to do as the
         * solution is stable, in case of success other calls
         * to this method may be required, in case of failure
         * the clustering should be dropped.
         */
        virtual Constants::clustering_codes cluster() = 0 ;

        /*!
         * \brief Should implement seeding routines.
         * \param method a string indicating which method to use.
         */
        virtual void seeding(const std::string& method) throw (std::runtime_error) = 0 ;

        /*!
         * \brief Should implement a method printing the results to
         * the given stream.
         * \param stream the ouput stream of interest.
         */
        virtual void print_results(std::ostream& stream) const = 0 ;

    protected:
        /*!
         * \brief Should implement a test returniing whether the
         * last call to cluster() resulted in convergence.
         * \return whether the last call resulted in convergence.
         */
        virtual bool hasConverged() const = 0 ;

} ;

#endif // CLUSTERINGENGINE_HPP
