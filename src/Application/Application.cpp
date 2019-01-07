#include <iostream>
#include <vector>
#include <list>
#include <boost/program_options.hpp>
#include <fstream>
#include <sstream>                       // istringstream
#include <stdexcept>                     // std::runtime_error, std::invalid_argument
#include <unordered_map>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp> // fs::path

#include <FileTools/include/FASTAFileReader.hpp> // FASTAFileReader
#include <FileTools/include/FASTA_element.hpp>   // FASTA_element

#include <Clustering/EMSequenceEngine.hpp>
#include <Matrix/Matrix2D.hpp>
#include <GUI/LogoWindow/LogoWindow.hpp>
#include <GUI/ConsoleProgressBar/ConsoleProgressBar.hpp>
#include <Utility/Constants.hpp>        // Constants::clustering_codes
#include <Utility/String_utility.hpp>   // ends_with()

#include "Application.hpp"


// some global variables
// version
std::string version("v1.0") ;
// possible seeding mode options
static std::string seeding_random("random") ;


namespace fs = boost::filesystem;
namespace po = boost::program_options ;



Application::Application(int argn, char **argv)
    : exit_code(0)
{   // try to set the options from what is given on the command line
    this->set_options(argn, argv) ;
    // construct required file path, need exec absolute path
    this->set_path_map(argv) ;
}

Application::~Application()
{}

std::unordered_map<std::string,std::string> Application::get_path_map() const
{   return this->path_map ; }

int Application::run() throw (std::invalid_argument, std::runtime_error)
{
    // displayed help or version
    if(this->exit_code == EXIT_FAILURE)
    {   return EXIT_FAILURE ; }

    // load data
    Matrix2D<char> sequences ;
    if(this->options.file_fasta)
    {   sequences = load_fasta_into_matrix(this->options.file_data,
                                           this->options.from,
                                           this->options.to) ;
    }
    else
    {   sequences = Matrix2D<char>(this->options.file_data) ; }

    // set things ready
    EMSequenceEngine* em = nullptr ;
    // motif are provided within files
    if(this->options.seeding.find(",") != std::string::npos)
    {   std::vector<Matrix2D<double>> priors ;
        for(auto& file : split(this->options.seeding, ','))
        {   priors.push_back(Matrix2D<double>(file)) ; }

        em = new EMSequenceEngine(sequences,
                                  priors,
                                  this->options.flip,
                                  this->options.center_shift,
                                  this->options.bg_class) ;
    }
    // de-novo discovery
    else
    {   em = new EMSequenceEngine(sequences,
                                  this->options.classes_n,
                                  this->options.motif_l,
                                  this->options.flip,
                                  this->options.center_shift,
                                  this->options.bg_class,
                                  this->options.seed,
                                  this->options.seeding) ;
    }

    ConsoleProgressBar bar(std::cerr, this->options.iteration_n, 50, "classification") ;

    // classify
    size_t n_iter_cur = 0 ;
    int code ;
    do
    {   code = em->cluster() ;
        n_iter_cur++ ;
        bar.update() ;
        bar.display() ;
    }
    while(n_iter_cur < this->options.iteration_n and code != Constants::clustering_codes::CONVERGENCE) ;

    // make sure that the progress bar is filled
    bar.fill() ;
    bar.display() ;
    std::cerr << std::endl ;

    if(code == Constants::clustering_codes::CONVERGENCE)
    {   std::cout << "Converged after " << n_iter_cur-1 << " iterations" << std::endl ;
        this->exit_code = EXIT_SUCCESS ;
    }
    else if(code == Constants::clustering_codes::SUCCESS)
    {   std::cout << "Finished after " << this->options.iteration_n-1 << " iterations" << std::endl ;
        this->exit_code = EXIT_SUCCESS ;
    }

    // write the results
    if(this->options.prefix.size())
    {   this->write_results(*em) ; }

    // display logos with uniform background
    if(not this->options.nogui)
    {   std::vector<double> bg_prob(4,0.25) ;
        std::vector<Matrix2D<double>> motifs = em->get_motifs() ;
        this->displayMotifs(motifs, bg_prob) ;
    }

    if(em != nullptr)
    {   delete em ; }
    em = nullptr ;

    return this->exit_code ;
}

void Application::set_options(int argn, char** argv) throw (std::runtime_error)
{

    // initialize the options to some default values
    this->options.file_data = "" ;
    this->options.file_fasta = false ;
    this->options.from = 0 ;          // first position to use in sequences (1-based)
    this->options.to   = 0 ;          // last position to use in sequence (included, 1-based)

    this->options.iteration_n  = 1 ;
    this->options.classes_n    = 1 ;
    this->options.motif_l      = 1 ;
    this->options.flip         = false ;
    this->options.center_shift = false ;

    this->options.bg_class     = false ;
    this->options.seed         = "" ;
    this->options.seeding      = seeding_random.c_str() ;
    this->options.nogui        = false ;

    if(argv == nullptr)
    {   this->exit_code = -1 ;
        return ;
    }

    // initialize variables to parse options
    char desc_msg[4096] ;
    sprintf(desc_msg, "\n"
                          "  EM-seq is a program to perform unsupervised probabilistic classification of DNA\n"
                          "  sequences using an expectation-maximization (EM) procedure. The DNA sequences are\n"
                          "  attributes a class belonging probability based on how well they are scored by the\n"
                          "  corresponding class motif. At the end of the procedure, DNA motif are trained and\n"
                          "  are returned together with the sequence class belonging probabilities and the\n"
                          "  class probabilities.\n"
                          "  Written by Romain Groux, March 2018"
                          "  %s\n", version.c_str()) ;

    boost::program_options::variables_map vm ;
    boost::program_options::options_description desc(desc_msg) ;

    std::string opt_help_msg       = "Produces this help message";
    std::string opt_version_msg    = "Prints the version number";

    std::string opt_data_msg       = "The data file address. It should be a character matrix "
                                     "with the sequences on the rows or a regular fasta file. "
                                     "In both cases, all the sequences should have the same "
                                     "length and only ACGTacgt are valid characters." ;
    std::string opt_fasta_msg      = "Whether the data file is a fasta file.";
    std::string opt_from_msg       = "The first position to use in the sequences (by default 1, 1-based).";
    std::string opt_to_msg         = "The last position to use in the sequences (by default the end, 1-based).";

    std::string opt_iter_msg       = "The maximum number of iterations." ;
    std::string opt_classes_msg    = "The number of classes to use." ;
    std::string opt_length_msg     = "The motif length in base pair. All the motifs trained will be this long." ;
    std::string opt_flip_msg       = "Searches the reverse complement of the sequences.";
    std::string opt_shift_center   = "The shift probabilities will be renormalized at each iteration to make "
                                     "the density fit a gaussian centered on the most central shift state." ;
    std::string opt_bg_class_msg   = "Whether an extra class should be added to model the background.";
    std::string opt_write_msg      = "A path which will be used as prefix to write the results.";
    std::string opt_nogui_msg      = "Disable the GUI at the end to display the motifs.";

    char seeding_msg[2048] ;
    sprintf(seeding_msg,
            "Specifies which method should be used to initialise the program. "
            "Two different way of doing are possible. First, it is possible to "
            "use a predefined seeding method using any word among '%s'. Second "
            "it is possible to provide motif to optimize as starting points, using "
            "a list of coma-separated file addresses (if one file is provided only, "
            "it has to end with a coma). "
            "By default, '%s' is used, which initialises the posterior probabilities "
            "at random from a beta distribution.",
            seeding_random.c_str(), seeding_random.c_str()) ;
    std::string opt_seeding_msg = seeding_msg ;
    std::string opt_seed_msg       = "A value to seed the random number generator.";

    desc.add_options()
            ("help,h",       opt_help_msg.c_str())
            ("version,v",    opt_version_msg.c_str())

            ("data,d",       po::value<std::string>(&(this->options.file_data)), opt_data_msg.c_str())
            ("from",         po::value<int>(&(this->options.from)),              opt_from_msg.c_str())
            ("to",           po::value<int>(&(this->options.to)),                opt_to_msg.c_str())
            ("fasta",                                                            opt_fasta_msg.c_str())

            ("iter,i",       po::value<size_t>(&(this->options.iteration_n)),    opt_iter_msg.c_str())
            ("classes,c",    po::value<size_t>(&(this->options.classes_n)),      opt_classes_msg.c_str())
            ("length,l",     po::value<size_t>(&(this->options.motif_l)),        opt_length_msg.c_str())
            ("flip",                                                             opt_flip_msg.c_str())
            ("centershift",                                                      opt_shift_center.c_str())
            ("bgclass",                                                          opt_bg_class_msg.c_str())

            ("write",        po::value<std::string>(&(this->options.prefix)),    opt_write_msg.c_str())
            ("nogui",                                                            opt_nogui_msg.c_str())

            ("seeding",      po::value<std::string>(&(this->options.seeding)),   opt_seeding_msg.c_str())
            ("seed",         po::value<std::string>(&(this->options.seed)),      opt_seed_msg.c_str()) ;

    // parse
    try
    {   po::store(po::parse_command_line(argn, argv, desc), vm) ;
        po::notify(vm) ;
    }
    catch(std::exception& e)
    {   std::string msg = std::string("error while parsing options!") + e.what() ;
        throw(std::runtime_error(msg)) ;
    }

    // checks unproper option settings
    // data
    if(this->options.file_data == "" and (not vm.count("help")) and (not vm.count("version")))
    {   std::string msg("error while parsing the options! no data file was given (--data)!") ;
        throw(std::runtime_error(msg)) ;
    }
    // seeding
    else if((this->options.seeding.find(",") == std::string::npos) and // this is not a motif file
            (this->options.seeding != seeding_random))                 // this is not a reconized method
    {   std::string msg("error while parsing options! unrecognized seeding method (--seeding!)!") ;
        throw(std::runtime_error(msg)) ;
    }
    // class number
    else if(this->options.classes_n == 0)
    {   std::string msg("error while parsing options! --class should at least be 1!") ;
        throw std::runtime_error(msg) ;
    }
    // motif length
    else if(this->options.motif_l == 0)
    {   std::string msg("error while parsing options! motif length should be at least 1 (--length)!") ;
        throw std::runtime_error(msg) ;
    }
    // from/to paramters
    else if(this->options.from < 0)
    {   std::string msg("error while parsing options! --from cannot be negative!") ;
        throw(std::runtime_error(msg)) ;
    }
    else if(this->options.to < 0)
    {   std::string msg("error while parsing options! --to cannot be negative!") ;
        throw(std::runtime_error(msg)) ;
    }

    if(vm.count("help"))    { std::cout <<    desc << std::endl ; this->exit_code = EXIT_FAILURE ; }
    if(vm.count("version")) { std::cout << version << std::endl ; this->exit_code = EXIT_FAILURE ; }
    if(vm.count("fasta"))   { this->options.file_fasta   = true ; }
    if(vm.count("flip"))    { this->options.flip         = true ; }
    if(vm.count("flip"))    { this->options.center_shift = true ; }
    if(vm.count("bgclass")) { this->options.bg_class     = true ; }
    if(vm.count("nogui"))   { this->options.nogui        = true ; }

    // make --from and --to 0-based
    this->options.from-- ;
    this->options.to-- ;
}


void Application::set_path_map(char** argv)
{   // get executable path
    fs::path exec_path(fs::initial_path<fs::path>()) ;
    exec_path = fs::system_complete(fs::path(argv[0])) ;
    fs::path root_path = exec_path.parent_path().parent_path() ;

    // fill the map
    this->path_map["A.png"]      = (root_path / fs::path("res") / fs::path("A.png")).string() ;
    this->path_map["C.png"]      = (root_path / fs::path("res") / fs::path("C.png")).string() ;
    this->path_map["G.png"]      = (root_path / fs::path("res") / fs::path("G.png")).string() ;
    this->path_map["T.png"]      = (root_path / fs::path("res") / fs::path("T.png")).string() ;
    this->path_map["arial.ttf"]  = (root_path / fs::path("res") / fs::path("arial.ttf")).string() ;
}


void Application::displayMotifs(const std::vector<Matrix2D<double>>& motifs,
                                const std::vector<double>& background_prob) throw (std::invalid_argument)
{
    std::vector<double> bg_prob(background_prob) ;

    if(bg_prob.size() != 4)
    {   throw std::invalid_argument("error! invalid number of background probabilities, 4 are expected.") ; }

    // normalize the prob, in case
    double sum = std::accumulate(bg_prob.begin(), bg_prob.end(), 0.) ;
    for(auto& p : bg_prob)
    {   p /= sum ; }

    // build the logos
    std::list<LogoWindow*> logos ;
    for(size_t i=0; i<motifs.size(); i++)
    {   char title[512] ; sprintf(title, "class %zu", i+1) ;
        LogoWindow* window = nullptr ;
        window = new LogoWindow(sf::VideoMode(600,800),
                                "EMSequence",
                                motifs[i],
                                bg_prob,
                                title,
                                this->path_map["A.png"],
                                this->path_map["C.png"],
                                this->path_map["G.png"],
                                this->path_map["T.png"],
                                this->path_map["arial.ttf"]) ;
        window->setFramerateLimit(10) ;
        logos.push_back(window) ;
    }

    // render loop for logos
    while(logos.size())
    {   for(auto iter = logos.begin(); iter != logos.end(); iter++)
        {   // (*iter)->draw() ;
            (*iter)->clear(sf::Color::White) ;
            (*iter)->display() ;

            sf::Event event ;
            while((*iter)->pollEvent(event))
            {   (*iter)->clear(sf::Color::White) ;
                if(event.type == sf::Event::Closed)
                {   (*iter)->close() ;
                    // after erase() iter points after the element erased
                    iter = logos.erase(iter) ;
                    // make iter points backward such that the next iter++ makes iter
                    // points to the element juste after the one erased
                    iter-- ;
                    break ;
                }
            }
        }
    }
}


void Application::write_results(const EMSequenceEngine& em) const throw (std::runtime_error)
{   try
    {   this->write_motifs(em) ;
        this->write_post_prob(em) ;
        this->write_class_prob(em) ;
        this->write_class_prob_total(em) ;
    }
    catch(std::runtime_error& e)
    {   throw e ; }
}

void Application::write_motifs(const EMSequenceEngine& em) const throw (std::runtime_error)
{
    std::vector<Matrix2D<double>> motifs = em.get_motifs() ;

    std::string file_name_str ;
    char file_name[512] ;

    for(size_t i=0; i<motifs.size(); i++)
    {   sprintf(file_name, "%s_motif_class%zu.mat", this->options.prefix.c_str(), i+1) ;
        file_name_str = std::string(file_name) ;
        std::ofstream f_motif(file_name_str) ;
        if(f_motif.fail())
        {   char msg[1024] ;
            sprintf(msg, "could not write motif in %s", file_name_str.c_str()) ;
            throw std::runtime_error(msg) ;
        }
        f_motif << transpose(motifs[i]) << std::endl ;
        f_motif.close() ;
        file_name_str.clear() ;
    }

}

void Application::write_post_prob(const EMSequenceEngine& em) const throw (std::runtime_error)
{
    Matrix4D<double> post_prob = em.get_post_prob() ;

    char file_name[512] ;
    sprintf(file_name, "%s_postprob.mat", this->options.prefix.c_str()) ;
    std::string file_name_str(file_name) ;
    std::ofstream f_post_prob(file_name_str) ;
    if(f_post_prob.fail())
    {   char msg[1024] ;
        sprintf(msg, "could not write motif in %s", file_name_str.c_str()) ;
        throw std::runtime_error(msg) ;
    }

    f_post_prob << post_prob << std::endl ;
    f_post_prob.close() ;
}

void Application::write_class_prob(const EMSequenceEngine& em) const throw (std::runtime_error)
{
    Matrix3D<double> class_prob = em.get_class_prob() ;

    char file_name[512] ;
    sprintf(file_name, "%s_classprob.mat", this->options.prefix.c_str()) ;

    std::string file_name_str(file_name) ;
    std::ofstream f_class_prob(file_name_str) ;
    if(f_class_prob.fail())
    {   char msg[1024] ;
        sprintf(msg, "could not write class prob in %s", file_name_str.c_str()) ;
        throw std::runtime_error(msg) ;
    }
    f_class_prob << class_prob << std::endl ;
    f_class_prob.close() ;
}

void Application::write_class_prob_total(const EMSequenceEngine& em) const throw (std::runtime_error)
{
    std::vector<double> class_prob_total = em.get_class_prob_total() ;

    char file_name[512] ;
    sprintf(file_name, "%s_classproboverall.mat", this->options.prefix.c_str()) ;

    std::string file_name_str(file_name) ;
    std::ofstream f_class_prob_total(file_name_str) ;
    if(f_class_prob_total.fail())
    {   char msg[1024] ;
        sprintf(msg, "could not write class prob in %s", file_name_str.c_str()) ;
        throw std::runtime_error(msg) ;
    }
    f_class_prob_total << class_prob_total << std::endl ;
    f_class_prob_total.close() ;
}



std::vector<std::string> split(const std::string& str, char delim)
{   std::vector<std::string> results ;
    std::istringstream iss(str);
    std::string item;
    while (std::getline(iss, item, delim))
    {   results.push_back(item) ;
        item.clear() ;
    }
    return results ;
}


Matrix2D<char> load_fasta_into_matrix(const std::string& file_address, int from, int to) throw (std::invalid_argument, std::runtime_error)
{

    // check from and to, only accepted negative value is -1
    if(from == -1)
    {  ; }
    else if(from < 0)
    {   throw std::invalid_argument("from parameter is negative!") ; }
    if(to == -1)
    {  ; }
    else if(to < 0)
    {   throw std::invalid_argument("to parameter is negative!") ; }

    std::list<std::string> seq_list ;
    try
    {
        FASTAFileReader reader(file_address) ;
        FASTA_element* seq = nullptr ;
        while(true)
        {   seq = reader.get_next() ;
            if(seq == nullptr)
            {   break ; }
            seq_list.push_back(seq->sequence) ;
            delete seq ;
        }
    }
    // thrown by reader.get_next()
    catch(std::runtime_error& e)
    {   throw e ; }

    // number and length of the sequences
    size_t n_row = seq_list.size() ;
    size_t l_seq = seq_list.begin()->size() ;

    // check that all sequences have the same length
    for(auto& seq : seq_list)
    {   if(seq.size() != l_seq)
        {   throw std::runtime_error("sequences have variable length!") ; }
    }

    // check from to coordinates and set looping parameters
    size_t loop_from = 0 ;
    size_t loop_to   = l_seq ;
    // from
    if(from == -1)
    {   loop_from = 0 ; }
    else if(static_cast<size_t>(from) > l_seq)
    {   throw std::invalid_argument("from parameter is out of range!") ; }
    else
    {   loop_from = from ; }
    // to
    if(to == -1)
    {   loop_to = l_seq ; }
    else if(static_cast<size_t>(to) >= l_seq)
    {   throw std::invalid_argument("to parameter is out of range!") ; }
    else
    {   loop_to = to + 1 ; }

    // update sequence length
    l_seq = loop_to - loop_from ;

    // store the sequences into the matrix
    Matrix2D<char> sequences(n_row, l_seq) ;
    std::list<std::string>::iterator iter = seq_list.begin() ;
    for(size_t i=0; i<n_row; i++)
    {   for(size_t j=loop_from; j<loop_to; j++)
        {   sequences(i,j-loop_from) = (*iter)[j] ; }
        iter++ ;
    }
    return sequences ;
}

