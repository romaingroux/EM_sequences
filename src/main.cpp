#include <iostream>
#include <stdexcept>
#include <Application/Application.hpp>


using namespace std ;


int main(int argn, char** argv)
{
    try
    {   Application app(argn, argv) ;
        int exit_code = app.run() ;
        return exit_code ;
    }
    catch(std::exception& e)
    {   std::cerr << "An error occured!" << std::endl
                  << e.what() << std::endl ;
        return -1 ;
    }
}

