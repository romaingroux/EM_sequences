#include "Logo.hpp"

#include <iostream>
#include <vector>
#include <string>

#include <Matrix/Matrix2D.hpp>

#include <Utility/Sorting_utility.hpp>
#include <GUI/Utility/GUI_utility.hpp>


Logo::Logo(const sf::Vector2f& upperLeft,
           const sf::Vector2f& lowerRight,
           const Matrix2D<double>& matrix,
           const std::string& file_A,
           const std::string& file_C,
           const std::string& file_G,
           const std::string& file_T,
           const std::array<float, 4>& margins,
           std::vector<double> background,
           bool isFreqMatrix)
           // double pA, double pC, double pG, double pT)
    : Drawable(upperLeft, lowerRight, margins),
      background(background), file_A(file_A), file_C(file_C),
      file_G(file_G), file_T(file_T)
{   // load the textures from res/ directory
    this->loadTextures() ;

    // create a matrix of information content (RectangleShape height) from a
    // probability matrix
    if(isFreqMatrix)
    {   this->matrixHeight = this->convertMatrixProbToHeight(this->convertMatrixFreqToProb(matrix, 1)) ; }
    else
    {   this->matrixHeight = this->convertMatrixProbToHeight(matrix) ; }

   // creates all the RectangleShapes to be drawn, with their positions
   // heights and widths
   this->createMatrixRectangleShapes() ;
}

Logo::~Logo()
{}

void Logo::drawOn(sf::RenderWindow& target) const
{   for(size_t i=0; i<this->matrixHeight.get_nrow(); i++)
    {   for(size_t j=0; j<this->matrixHeight.get_ncol(); j++)
        {   target.draw(this->matrixRectangleShapes[i][j]) ; }
    }
}

Matrix2D<double> Logo::convertMatrixFreqToProb(const Matrix2D<double>& matrix, double pseudocounts) const
{   size_t nrow = matrix.get_nrow() ;
    size_t ncol = matrix.get_ncol() ;
    Matrix2D<double> matrix_prob(nrow, ncol, 0) ;

    for(size_t j=0; j<ncol; j++)
    {   double total = 0. ;
        for(size_t i=0; i<nrow; i++)
        {   matrix_prob(i,j) = (matrix(i,j)+pseudocounts) / this->background[i] ;
            total += matrix_prob(i,j) ;
        }
        for(size_t i=0; i<nrow; i++)
        {   matrix_prob(i,j) /= total ; }
    }
    // std::cerr << matrix      << std::endl << std::endl ;
    // std::cerr << matrix_prob << std::endl ;

    return matrix_prob ;
}

Matrix2D<double> Logo::convertMatrixProbToHeight(const Matrix2D<double>& matrix, double pseudocounts) const
{   size_t nrow = matrix.get_nrow() ;
    size_t ncol = matrix.get_ncol() ;
    Matrix2D<double> matrix_info(nrow, ncol, 0) ;

    std::vector<double> H(ncol,0) ;
    std::vector<double> R(ncol,0) ;

    // add the pseudocount and re-normalize
	Matrix2D<double> matrix2(matrix) ; matrix2 += pseudocounts ;
    for(size_t j=0; j<ncol; j++)
    {   double sum = 0. ;
        // column total
        for(size_t i=0; i<nrow; i++)
        {   sum += matrix2(i,j) ; }
        // norm. by column total
        for(size_t i=0; i<nrow; i++)
        {   matrix2(i,j) /= sum ; }
    }

    // a correction factor, it is normally computed as
    // (1 / ln(2) ) * (nrow / 2*nseq) where nseq is the number
    // of sequences used to make <matrix>
    double correction = 0. ;

    // compute entropy at each position
    for(size_t j=0; j<ncol; j++)
    {   double h = 0. ;
        for(size_t i=0; i<nrow; i++)
        {   h += matrix2(i,j) * log2(matrix2(i,j)) ; }
        H[j] = -h ;
    }

    // compute the information content at each position
    for(size_t j=0; j<ncol; j++)
    {   R[j] = log2(nrow) - (H[j] + correction) ; }

    // compute height of each base at each position
    for(size_t i=0; i<nrow; i++)
    {   for(size_t j=0; j<ncol; j++)
        {   matrix_info(i,j) = matrix(i,j) * R[j] ; }
    }

    return matrix_info ;
}

void Logo::loadTextures()
{   // texture files in res/ directory
    std::vector<std::string> files = {this->file_A,
                                      this->file_C,
                                      this->file_G,
                                      this->file_T} ;

    for(const auto& file : files)
    {   sf::Texture texture ;
        texture.loadFromFile(file) ;
        this->vectorTextures.push_back(texture) ;
    }
}

void Logo::createMatrixRectangleShapes()
{
    // for RectangleShape positionning
    float heightAvail = this->height- this->margins[0] - this->margins[2] ;
    float widthAvail  = this->width - this->margins[1] - this->margins[3] ;
    float currentX    = this->x + this->margins[1];
    float currentY ;
    float dx = widthAvail  / this->matrixHeight.get_ncol() ;
    // float dy = heightAvail / this->matrixHeight.get_nrow() ;

    // matrix dimensions
    size_t nrow = this->matrixHeight.get_nrow() ;
    size_t ncol = this->matrixHeight.get_ncol() ;

    // creates the matrix, yet with default values
    this->matrixRectangleShapes = std::vector<std::vector<sf::RectangleShape>>(nrow, std::vector<sf::RectangleShape>(ncol, sf::RectangleShape())) ;

    // treat all rows (bases) at a given position at a time
    for(size_t j=0; j<ncol; j++)
    {   // draw the high information (at the bottom) first
        // get the order in which the rows needs to be drawn
        // lowest info content at the top, highest at the bottom
        std::vector<size_t> ordered = order(this->matrixHeight.get_col(j), true) ;
        // reset the current y at each position
        // currentY is currently at the lowest edge
        currentY = this->y + this->margins[2] + this->height - this->margins[0] ;
        for(size_t i=0; i<nrow; i++)
        {   // needs to get the element from the end (biggest first) to draw
            // the most informative base first
            size_t index = ordered[i] ;

            // compute the xy coordinates and the height
            float widthRect   = dx ;
            float heightRect  = (heightAvail/2.) * this->matrixHeight(index,j) ;
            // create a rectangle shape with these param
            sf::RectangleShape rect ;
            rect.setTexture(&(this->vectorTextures[index])) ;
            rect.setPosition(currentX, currentY - heightRect) ;
            rect.setSize(sf::Vector2f(widthRect, heightRect)) ;
            // stores its and update the y coordinate for the next one
            this->matrixRectangleShapes[index][j] = rect ;
            currentY -= heightRect ;
        }
        currentX += dx ;
    }
}


void Logo::update()
{   this->loadTextures() ;
    this->createMatrixRectangleShapes() ;
}
