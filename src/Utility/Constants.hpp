
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP


/*!
 * \brief The Constants class contains miscellaneous constants
 * for the program.
 */
class Constants
{
   public:
    Constants() = delete ;
    Constants(Constants&) = delete ;

    // numerical constants
    static const double delta_max ;     // an delta value for double comparisons
    static const double pseudo_counts ; // a pseudo count value

    // collections of values
    enum flip {FORWARD=0, REVERSE, N_FLIP_STATES=2} ;
    enum clustering_codes {CONVERGENCE=0, SUCCESS, FAILURE, N_CODES=3} ;
} ;

#endif // CONSTANTS_HPP
