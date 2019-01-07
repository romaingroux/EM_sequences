#ifndef AXIS_HPP
#define AXIS_HPP

#include <string>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>

#include <GUI/Drawable/Drawable.hpp>

enum Side {x_axis, y_axis} ;


/*!
 * \brief The Axis class is a class to represent plot
 * axis on a figure. It is able to handle the x-axis
 * (axis below the figure, from left to right) and the
 * y-axis case (on the left from bottom to top).
 * So far, this class draws the axis ticks, values and title
 * OUTISDE the area determined by upperLeft and lowerLeft values
 *
 * This schema represents a x-axis case. The y-axis case
 * has ticks on the left of the mainLine and the axis start
 * is considered to be at the bottom and the axis end at the
 * top.
 *
 * 0,0               sf::RenderWindow
 * ----------------------------------------------------
 * |
 * |                    Axis
 * |    upperLeft
 * |     <-------------- width -------------->
 * |    xy___________________________________
 * |     |              mainLine             |           /|\
 * |     |___________________________________|           \|/ height
 * |     |                |                  |lowerRight /|\
 * |     |tick            |                  |            |  tickSize
 * |     |                |                  |           \|/
 * | tickValue0      tickValue1         tickValue2
 * |
 * |     >  tickAt0
 * |     .................> tickAt1
 * |     ....................................> tickAt2
 * |
 * |                    title
 * |
 *
 */

class Axis :public Drawable
{
    public:
        // no default constructor
        Axis() = delete ;

        /*!
         * \brief constructor with values.
         * \param upperLeft the axis main line upper left  corner coordinates.
         * \param lowerRight the axis main line lower right corner coordinates.
         * \param lowerRight the object lower right corner coordinates.
         * \param upperRight the object upper right corner coordinates.
         * \param tickValues the axis tick values, determine the number
         * of ticks and their spacing.
         * \param tickAt the location where each value given in <values>
         * should be placed along the axis. A location is expressed as
         * a proportion of the axis, i.g. 0.0 means the beginning, 0.5
         * the middle and 1.0 of the end of the axis.
         * \param file_font the absolute path to the file containing
         * character font to draw texts.
         * \param valueSize the size of the axis labels, this referes
         * to a sf::Text.characterSize, 30 by default (like
         * sf::Text.characterSize).
         * \param tickSize the length of a tick, from the axis to the
         * tick edge, 10 by default
         * \param title the axis title, nothing by default
         * \param side whether the axis is an x-axis or a y-axis.
         * \param margins the object margins, by default none.
         */
        Axis(const  sf::Vector2f& upperLeft,
             const  sf::Vector2f& lowerRight,
             const  std::vector<std::string>& tickValues,
             const  std::vector<float>& tickAt,
             const  std::string& file_font,
             size_t valueSize=30,
             float  tickSize=10,
             const  std::string& title="",
             Side  side=x_axis,
             const std::array<float, 4> margins=std::array<float, 4>({0,0,0,0})) ;
        /*!
         * \brief the destructor
         */
        virtual ~Axis() ;

        /*!
         * \brief sets the size of the ticks (their length from the axis to
         * their edge)
         * \param size the new size
         */
        void setTickSize(float size) ;

        /*!
         * \brief sets the size of the ticks values. This is
         * equivalent as setting sf::Text::setCharacterSize()
         * for the value drawn at the ticks (this->tickValues).
         * \param size the new size
         */
        void setValueSize(size_t size) ;

        /*!
         * \brief update the object by reloading the font and
         * recomputing all the coordinates
         */
        void update() ;

        /*!
         * \brief method inherited from Drawable to displays
         * the object on the given RenderWindow.
         * \param target the window to draw on.
         */
        virtual void drawOn(sf::RenderWindow& target) const override ;

        /*!
         * \brief draws the ticks.
         * \param target the window to draw on.
         */
        void drawTicks(sf::RenderWindow& target) const ;

        /*!
         * \brief draws the title.
         * \param target the window to draw on.
         */
        void drawTitle(sf::RenderWindow& target) const ;

    private:
        size_t                          tickNumber ;
        sf::RectangleShape              mainLine ;
        std::vector<sf::RectangleShape> vectorTicks ;
        std::vector<std::string>        tickValues ;
        std::vector<float>              tickAt ;
        size_t                          valueSize ;
        float                           tickSize ;
        std::string                     title ;
        Side                            side ;
        /*!
         * \brief The text font.
         */
        sf::Font font ;
        /*!
         * \brief The absolute path to the file containing arial
         * character font to draw texts.
         */
        std::string file_font ;

    private:
        void loadFont() ;
        void createMainLine() ;
} ;

#endif // AXIS_HPP
