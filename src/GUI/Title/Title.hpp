#ifndef TITLE_HPP
#define TITLE_HPP

#include <GUI/Drawable/Drawable.hpp>

#include <string>
#include <array>
#include <SFML/Graphics.hpp>



/*!
 * \brief The Title class is a class to represent plot
 * title on a figure.
 *
 *
 * 0,0               sf::RenderWindow
 * ----------------------------------------------------
 * |
 * |                    Title
 * |    upperLeft
 * |     <-------------- width -------------->
 * |    xy___________________________________
 * |     |                                   | /|\
 * |     |             Some text             |  |  height
 * |     |___________________________________| \|/
 * |                                          lowerRight
 *
 */

class Title : public Drawable
{
    public:
        /*!
         * \brief constructor with values.
         * \param upperLeft the title upper left  corner coordinates.
         * \param lowerRight the title lower right  corner coordinates.
         * \param text the title text.
         * \param file_font the absolute path to the file containing
         * character font to draw texts.
         * \param characterSize the title character size, this referes
         * to a sf::Text.characterSize, 30 by default (like
         * sf::Text.characterSize).
         * \param margins the object margins, by default none.
         *
         */
        Title(const sf::Vector2f& upperLeft,
              const sf::Vector2f& lowerRight,
              const std::string& text,
              const std::string& file_font,
              unsigned int characterSize = 30,
              const std::array<float, 4>& margins = {0,0,0,0}) ;

        /*!
         * \brief destructor
         */
        ~Title() ;

        /*!
         * \brief sets the title text.
         * \param text the title text.
         */
        void setText(const std::string& text) ;

        /*!
         * \brief sets the character size.
         * \param size the character size.
         */
        void setCharacterSize(unsigned int size) ;

        /*!
         * \brief method inherited from Drawable to displays
         * the object on the given RenderWindow.
         * \param target the window to draw on.
         */
        virtual void drawOn(sf::RenderWindow& target) const override ;

        /*!
         * \brief update the object by reloading the font and
         * recomputing all the coordinates
         */
        void update() ;

    private:
        unsigned int characterSize ;
        sf::Text text ;
        sf::Font font ;
        std::string file_font ;

    private:
        /*!
         * \brief loads the text font from this->file_font
         */
        void loadFont() ;
        /*!
         * \brief sets Text position according to the text length,
         * the height and the width.
         */
        void setTextPosition() ;
} ;


#endif // TITLE_HPP
