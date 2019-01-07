#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <boost/program_options.hpp>
#include <stdexcept> // std::runtime_error, std::invalid_argument

#include <Clustering/EMSequenceEngine.hpp>
#include <Utility/Constants.hpp> // Constants::clustering_codes


struct options
{   // data related
    /*!
     * \brief the file address
     */
    std::string file_data ;   // file address
    /*!
     * \brief whether the data file is a fasta file.
     */
    bool file_fasta ;
    /*!
     * \brief first position to use in sequences (included, 0based).
     */
    int from ;
    /*!
     * \brief last position to use in sequence (included, 0based).
     */
    int to ;
    // clustering parameters
    /*!
     * \brief the maximum number of iteration which will be run.
     */
    size_t iteration_n ;
    /*!
     * \brief the number of classes to optimize.
     */
    size_t classes_n ;
    /*!
     * \brief the motif length.
     */
    size_t motif_l ;
    /*!
     * \brief whether the reverse strand should be searched.
     */
    bool flip ;
    /*!
     * \brief whether an extra class should be added to
     * model the background. This class motif will remain
     * untrained but its probability can change.
     */
    bool bg_class ;
    /*!
     * \brief whether the shift probabilities should be
     * renormalized at iteration to make the density
     * fit a gaussian centered on the most central
     * shift state.
     */
    bool center_shift ;
    // seeding
    /*!
     * \brief the seed to initiate the random number
     * generator.
     */
    std::string seed ;
    /*!
     * \brief the seeding method to use.
     */
    std::string seeding ;
    // results related
    /*!
     * \brief the prefix for all the files which will
     * be written. It can contain directories such as
     * "/foo/dir/myfile".
     */
    std::string prefix ;
    /*!
     * \brief whether the GUI should be hidden.
     */
    bool nogui ;
} ;


/*!
 * \brief The Application class is the interface to all
 * the program functionalities.
 * Mainly it contains the run() method which runs the
 * application.
 */
class Application
{
    public:
        // constructors
        Application() = delete ;
        Application(const Application& other) = delete ;

        /*!
         * \brief Constructor, initialize the object
         * running options from the command line given
         * arguments.
         * \param argv the vector of argument as fetch
         * from the command line.
         * \param argc the number of argument fetch from
         * the command line.
         */
        Application(int argn, char** argv) ;

        /*!
          * Destructor.
          */
        ~Application() ;

        // methods
        /*!
         * \brief Gets the map containing the absolute paths to
         * the files located at ../res
         * "A.png"      -> ../res/A.png
         * "C.png"      -> ../res/C.png
         * "G.png"      -> ../res/G.png
         * "T.png"      -> ../res/T.png
         * "arial.tiff" -> ../res/T.png
         * \return this map.
         */
        std::unordered_map<std::string,std::string> get_path_map() const ;

        /*!
         * \brief Runs the classification procedure for the given
         * number of iterations or until convergence and take care
         * of returning the results properly.
         * \throw std::invalid_argument or std::runtime_error at least
         * in case of error during the process.
         * \return EXIT_SUCCESS upon success (convergence or maximum number of
         * iteration reached), EXIT_FAILURE otherwise.
         */
        int run() throw (std::invalid_argument, std::runtime_error);

    private:
        // methods
        /*!
         * \brief Dumps the posterior probabilies, class probabilities and
         * motif of the given instance to files with their addresses starting with
         * this->options.prefix. The posterior probabilities will be written to
         * <prefix>_postprob.mat, the class probabilities to <prefix>_classprob.mat
         * and the motifs to <prefix>_motif_<class_number>.mat.
         * \param em the sequence classifier instance of interest.
         */
        void write_results(const EMSequenceEngine& em) const throw (std::runtime_error) ;

        /*!
         * \brief Dumps the motifs of the given instance to files named
         * <this->options.suffix>_motif_>class_number>.mat. There are as many files
         * as there are classes.
         * \param em the sequence classifier instance of interest.
         */
        void write_motifs(const EMSequenceEngine& em) const throw (std::runtime_error) ;

        /*!
         * \brief Dumps the posterior probabilities of the given instance to
         * a file named <this->options.suffix>_postprob.mat.
         * \param em the sequence classifier instance of interest.
         */
        void write_post_prob(const EMSequenceEngine& em) const throw (std::runtime_error) ;

        /*!
         * \brief Dumps the class probabilities of the given instance to
         * a file named <this->options.suffix>_classprobtotal.mat.
         * \param em the sequence classifier instance of interest.
         */
        void write_class_prob(const EMSequenceEngine& em) const throw (std::runtime_error) ;

        /*!
         * \brief Dumps the overall class probabilities of the given instance to
         * a file named <this->options.suffix>_classprobtotal.mat.
         * \param em the sequence classifier instance of interest.
         */
        void write_class_prob_total(const EMSequenceEngine& em) const throw (std::runtime_error) ;


        /*!
         * \brief Constructs a map containing the absolute paths to
         * the files located at ../res
         * "A.png"     -> ../res/A.png
         * "C.png"     -> ../res/C.png
         * "G.png"     -> ../res/G.png
         * "T.png"     -> ../res/T.png
         * "arial.ttf" -> ../res/T.png
         */
        void set_path_map(char** argv) ;

        /*!
         * \brief Sets the options according to the options
         * given from the command line.
         * \param argn the argument corresponding to the argn argument of the main() function.
         * \param argv the argument corresponding to the argv argument of the main() function.
         * \throw std::runtime_error if an error occurs while parsing the options (for instance
         * a non-accepted value is detected).
         */
        void set_options(int argn, char** argv) throw (std::runtime_error) ;

        /*!
         * \brief Spawns one window per motif to display the logos. This method
         * contains the render loop for the display. The method returns once
         * all the windows have been closed.
         * \param motifs a vector containing the different motifs to display.
         * \param background_prob a vector containing the background probabilities
         * for A,C,G,T resprectively. The values are normlized to sum up to one.
         * \throw std::invalid_argument if the background probability vector contains
         * more or less than 4 elements.
         */
        void displayMotifs(const std::vector<Matrix2D<double>>& motifs,
                           const std::vector<double>& background_prob) throw (std::invalid_argument) ;

        // fields
        /*!
          * \brief stores the running options given from the command line.
          */
         struct options options ;

        /*!
         * \brief the application exit code. This value can be returned by the
         * main() function to the operating system.
         * The values are :
         * -1 if an error occure while parsing the command line options
         * EXIT_SUCCESS if the clustering could be run (until convergence or not).
         * EXIT_FAILURE if the clustering failed.
         */
        int exit_code ;

        /*!
         * A map containing the res/ file absolute paths.
         */
        std::unordered_map<std::string,std::string> path_map ;
} ;


std::vector<std::string> split(const std::string& str, char delim) ;


/*!
 * \brief Loads the content of a fasta file and stores the data in a
 * character matrix where each row contains one sequence.
 * The sequence in the file should all have the same length.
 * \param file_address
 * \param from the first position in the sequence to consider (included, 0-based).
 * By default -1, which means from the beginning of the sequences.
 * \param to the last position in the sequence to consider (included, 0-based).
 * By default -1, which means to the end of the sequences.
 * \throw std::runtime_error if the sequences do not all have the same
 * length, if an error happens while reading the file or if
 * from or to are out of range (that is, their values do not match the
 * read sequence matrix dimensions).
 * \return
 */
Matrix2D<char> load_fasta_into_matrix(const std::string& file_address, int from=-1 ,int to=-1) throw (std::invalid_argument, std::runtime_error);




#endif // APPLICATION_HPP
