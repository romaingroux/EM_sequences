#ifndef LOGO_HPP
#define LOGO_HPP

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "Matrix/Matrix2D.hpp"
#include "GUI/Drawable/Drawable.hpp"

/*!
 * \brief The Logo class is a class to represent all the required information
 * to draw a sequence logo from a matrix representing a sequence specificity.
 *
 *
 * 0,0               sf::RenderWindow
 * ----------------------------------------------------
 * |
 * |  UpperLeft          Logo
 * |    xy___________________________________
 * |     |    _ _   _ _     |                |  /\
 * |     |     |   |       \/                |  |
 * |     |     /\  |                         |  height
 * |     |--> /--\ |_ _   and so on :-)  <---|  |
 * |     |                 /\                |  |
 * |     |_________________|_________________|  \/
 * |                                         lowerRight
 * |     <-------------- width -------------->
 *
 */

class Logo : public Drawable
{
    public:
        /* no default constructor */
        Logo() = delete ;

        /*!
         * \brief constructor with values.
         * \param upperLeft the object upper left corner coordinates.
         * \param lowerRight the object upper left corner coordinates.
         * \param matrix the sequence specificity matrix to represent.
         * \param file_A the absolute path to the file containing the
         * image to represent the A character.
         * \param file_C the absolute path to the file containing the
         * image to represent the C character.
         * \param file_G the absolute path to the file containing the
         * image to represent the G character.
         * \param file_T the absolute path to the file containing the
         * image to represent the T character.
         * \param margins the object margins, by default none.
         * \param background the background probabilities of A,C,G,T.
         * \param isFreqMatrix specifies that the given matrix is a frequency matrix
         * (and should be converted to a probability matrix).
         */
        Logo(const sf::Vector2f& upperLeft,
             const sf::Vector2f& lowerRight,
             const Matrix2D<double>& matrix,
             const std::string& file_A,
             const std::string& file_C,
             const std::string& file_G,
             const std::string& file_T,
             const std::array<float, 4>& margins={0,0,0,0},
             std::vector<double> background={0.25, 0.25, 0.25, 0.25},
             bool isFreqMatrix=false) ;

        /* non copiable. */
        Logo(const Logo& other) = delete ;

        /*!
         * \brief destructor.
         */
        virtual ~Logo() override ;

        /*!
         * \brief method inherited from Drawable to displays
         * the object on the given RenderWindow.
         * \param target the window to draw on.
         */
        virtual void drawOn(sf::RenderWindow & target) const override ;

        /*!
         * \brief update the object by reloading the textures and
         * recomputing the matrixRectangleShapes
         */
        void update() ;

   private:
        /*!
         * \brief converts a frequency (counts) matrix into a probability matrix,
         * using the background values stored in this->background.
         * \param matrix the matrix to convert
         * \param pseudocounts a pseudocount value
         * \return the corresponding matrix of probability
         */
        Matrix2D<double> convertMatrixFreqToProb(const Matrix2D<double>& matrix,
                                               double pseudocounts=1.) const ;

        /*!
         * \brief converts a probability matrix into a base height matrix.
         * Each element of the matrix should be interpreted as the propotion
         * of the total height at a position (column) that a base sprite (row)
         * should have when drawing the logo.
         * \param matrix a matrix of probability
         * \param pseudocounts a number of pseudocounts
         * \return a matrix
         */
        Matrix2D<double> convertMatrixProbToHeight(const Matrix2D<double>& matrix,
                                                 double pseudocounts=0.000001) const ;

        /*!
         * \brief loads the textures stored in res/ and stores them in this->vectorTextures
         */
        void loadTextures() ;

        /*!
         * \brief creates a matrix of sprites to represent the logo. The
         * sprite textures are loaded from this->vectorTextures.
         */
        void createMatrixRectangleShapes() ;


    private:
        std::vector<double>          background ;
        Matrix2D<double>             matrixHeight ;
        std::vector<sf::Texture>     vectorTextures ;
        std::vector<std::vector<sf::RectangleShape>> matrixRectangleShapes ;
        /*!
         * \brief The absolute path to the file containing the
         * image to represent the A character.
         */
        std::string file_A ;
        /*!
         * \brief The absolute path to the file containing the
         * image to represent the C character.
         */
        std::string file_C ;
        /*!
         * \brief The absolute path to the file containing the
         * image to represent the G character.
         */
        std::string file_G ;
        /*!
         * \brief The absolute path to the file containing the
         * image to represent the T character.
         */
        std::string file_T ;

} ;


#endif // LOGO_HPP
