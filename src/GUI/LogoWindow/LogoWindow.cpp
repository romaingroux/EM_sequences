#include "LogoWindow.hpp"

#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include <vector>
#include <array>

#include <Matrix/Matrix2D.hpp>
#include <GUI/Drawable/Drawable.hpp>
#include <GUI/Logo/Logo.hpp>
#include <GUI/Axis/Axis.hpp>
#include <GUI/Title/Title.hpp>
#include <GUI/Utility/GUI_utility.hpp>

LogoWindow::LogoWindow(const sf::VideoMode& mode, const std::string &title,
                       const Matrix2D<double>& matrix,
                       const std::vector<double>& background,
                       const std::string& text,
                       const std::string& file_A,
                       const std::string& file_C,
                       const std::string& file_G,
                       const std::string& file_T,
                       const std::string& file_font)
    : sf::RenderWindow(mode, title), _title(nullptr), _xaxis(nullptr),
      _yaxis(nullptr), _logo(nullptr), _file_A(file_A), _file_C(file_C),
      _file_G(file_G), _file_T(file_T), _file_font(file_font)
{
    // define margins for each element to draw
    std::array<float, 4> logoMargins({  0, 0, 0, 10}) ;
    std::array<float, 4> xAxisMargins({ 0, 0, 0, logoMargins[3]}) ;

    // xy coordinates of each element to draw
    sf::Vector2f logoUpperLeft(         100, 100) ;
    sf::Vector2f logoLowerRight( mode.width, mode.height - 100) ;
    sf::Vector2f xAxisUpperLeft(        100, mode.height - 100) ;
    sf::Vector2f xAxisLowerRight(mode.width, mode.height) ;
    sf::Vector2f yAxisUpperLeft(        100, 100) ;
    sf::Vector2f yAxisLowerRight(       100, mode.height - 100) ;
    sf::Vector2f titleUpperLeft(          0, 0) ;
    sf::Vector2f titleLowerRight(mode.width, 100) ;

    // creates the logo
    this->_logo =   new Logo(logoUpperLeft,
                             logoLowerRight,
                             matrix,
                             this->_file_A,
                             this->_file_C,
                             this->_file_G,
                             this->_file_T,
                             logoMargins,
                             background,
                             false) ;

    // create the x axis
    this->_xaxis = constructXAxisDNALogo(xAxisUpperLeft,
                                         xAxisLowerRight,
                                         this->_file_font,
                                         matrix.get_ncol()) ;
    this->_xaxis->setMargins(xAxisMargins) ;
    this->_xaxis->update() ;

    // create the y axis
    this->_yaxis = constructYAxisDNALogo(yAxisUpperLeft,
                                         yAxisLowerRight,
                                         this->_file_font) ;

    // create the title
    this->_title = new Title(titleUpperLeft, titleLowerRight, text,
                             this->_file_font, 50) ;
}

void LogoWindow::close()
{   this->quit() ;
    sf::RenderWindow::close() ;
}

void LogoWindow::display()
{
    this->_title->drawOn(*this) ;
    this->_xaxis->drawOn(*this) ;
    this->_yaxis->drawOn(*this) ;
    this->_logo->drawOn(*this) ;

    sf::RenderWindow::display() ;
}


LogoWindow::~LogoWindow()
{   this->quit() ; }


void LogoWindow::quit()
{   // delete title
    if(this->_title != nullptr)
    {   delete this->_title ;
        this->_title = nullptr ;
    }
    // delete xaxis
    if(this->_xaxis != nullptr)
    {   delete this->_xaxis ;
        this->_xaxis = nullptr ;
    }
    // delete yaxis
    if(this->_yaxis != nullptr)
    {   delete this->_yaxis ;
        this->_yaxis = nullptr ;
    }
    // delete logo
    if(this->_logo != nullptr)
    {   delete this->_logo ;
        this->_logo = nullptr ;
    }
}


void LogoWindow::onResize()
{   sf::RenderWindow::onResize() ;
    /*
    this->updateTitleUpperLeft() ;
    this->updateTitleLowerRight() ;

    this->updateXAxisUpperLeft() ;
    this->updateXAxisLowerRight() ;
    this->updateXaxisMargins() ;

    this->updateYAxisUpperLeft() ;
    this->updateYAxisLowerRight() ;

    this->updateLogoUpperLeft() ;
    this->updateLogoLowerRight() ;
    this->updateLogoMargins() ;
    */
}


void LogoWindow::updateTitleUpperLeft()
{   // set it to 0,0
    sf::Vector2f upperLeft(0,0) ;
    this->_title->setUpperLeft(upperLeft) ;
    this->_title->update() ;
}

void LogoWindow::updateTitleLowerRight()
{   // set it to 1*width, 1/8*heigth
    sf::Vector2u size = this->getSize() ;
    sf::Vector2f lowerRight(size.x, (1./8.)*size.y) ;
    this->_title->setLowerRight(lowerRight) ;
    this->_title->update() ;
}

void LogoWindow::updateLogoUpperLeft()
{   // set it to 1/6*width, 1/8*height
    sf::Vector2u size = this->getSize() ;
    sf::Vector2f upperLeft((1./6.)*size.x, (1./8.)*size.y) ;
    this->_logo->setUpperLeft(upperLeft) ;
    this->_logo->update() ;
}

void LogoWindow::updateLogoLowerRight()
{   // set it to width, 7/8*height
    sf::Vector2u size = this->getSize() ;
    sf::Vector2f lowerRight(size.x, (7./8.)*size.y) ;
    this->_logo->setLowerRight(lowerRight) ;
    this->_logo->update() ;
}

void LogoWindow::updateLogoMargins()
{   sf::Vector2u size = this->getSize() ;
    std::array<float,4> margins ;
    margins[3] = (1./80.)*size.y ;
    this->_logo->setMargins(margins) ;
    this->_logo->update() ;
}

void LogoWindow::updateXAxisUpperLeft()
{   // set it to (1/6*width, 7/8*heigth)
    sf::Vector2u size = this->getSize() ;
    sf::Vector2f upperLeft((1./6.)*size.x, (7./8.)*size.y) ;
    this->_xaxis->setUpperLeft(upperLeft) ;
    this->_xaxis->update() ;
}

void LogoWindow::updateXAxisLowerRight()
{   // set it to (width, height)
    sf::Vector2u size = this->getSize() ;
    sf::Vector2f lowerRight(size.x, size.y) ;
    this->_xaxis->setLowerRight(lowerRight) ;
    this->_xaxis->update() ;
}

void LogoWindow::updateXaxisMargins()
{   this->_xaxis->setMargins({0.,0.,0.,0.}) ;
    this->_xaxis->update() ;
}

void LogoWindow::updateYAxisUpperLeft()
{   // set it to (1/6*width, 1/8*height)
    sf::Vector2u size = this->getSize() ;
    sf::Vector2f upperLeft((1./6.)*size.x, (1./8.)*size.y) ;
    this->_yaxis->setUpperLeft(upperLeft) ;
    this->_yaxis->update() ;
}

void LogoWindow::updateYAxisLowerRight()
{   // set it to (1/6*width, 7/8*height)
    sf::Vector2u size = this->getSize() ;
    sf::Vector2f lowerRight((1./6.)*size.x, (7./8.)*size.y) ;
    this->_yaxis->setUpperLeft(lowerRight) ;
    this->_yaxis->update() ;
}
