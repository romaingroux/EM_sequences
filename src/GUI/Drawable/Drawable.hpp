#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <array>
#include <SFML/Graphics.hpp>

/*!
 * \brief The Drawable class is an interface offering the minimal
 * features (heigth, width, x and y coordinates) and a drawing method
 * for any class which will be displayed on a sf::RenderWindow.
 *
 * The height and width fields are floats instead of unsigned int (which would be
 * more natural) because it is i) signed and ii) more precise. Thus casting
 * from signed float to anything is more secured than casting from whatever unsigned
 * to anything.
 *
 * The parameters can be used to represent the following situation
 *
 * 0,0               sf::RenderWindow
 * ----------------------------------------------------
 * |
 * |   UpperLeft        Drawable
 * |    xy___________________________________
 * |     |                 |margins[2]       |  /\
 * |     |                \/                 |  |
 * |     | margins[1]              margins[3]|  height
 * |     |-->                            <---|  |
 * |     |                 /\ margins[0]     |  |
 * |     |_________________|_________________|  \/
 * |                                          lowerRight
 * |     <-------------- width -------------->
 * |
 */
class Drawable
{
    public:
        /*!
         * \brief default contructor.
         */
        Drawable() ;
        /*!
         * \brief constructor with values.
         * \param upperLeft the object upper left corner coordinates.
         * \param lowerRight the object upper left corner coordinates.
         * \param margins the object margins, by default none.
         */
        Drawable(const sf::Vector2f& upperLeft,
                 const sf::Vector2f& lowerRight,
                 const std::array<float, 4>& margins = {0,0,0,0}) ;

        /* non-copiable */
        Drawable(const Drawable&) = delete ;

        /*!
         * \brief destructor.
         */
        virtual ~Drawable() ;

        /*!
         * \brief Returns the coordinates of the upper
         * left corner.
         * \return the upper left corner coordinates.
         */
        virtual sf::Vector2f getUpperLeft() const ;

        /*!
         * \brief Returns the coordinates of the lower
         * right corner.
         * \return the lower right corner coordinates.
         */
        virtual sf::Vector2f getLowerRight() const ;

        /*!
         * \brief gets the object height.
         * \return the object height.
         */
        virtual float getHeight() const ;

        /*!
         * \brief gets the object width.
         * \return the object width,
         */
        virtual float getWidth()  const ;

        /*!
         * \brief gets the object x and y coordinates.
         * \return the object position.
         */
        virtual std::array<float, 2> getPosition() const ;

        /*!
         * \brief gets the object margins.
         * \return the object margins.
         */
        virtual std::array<float, 4> getMargins() const ;

        /*!
         * \brief sets the upper left corner coordinates.
         * \param upperLeft the upper left corner coordinates.
         */
        virtual void setUpperLeft(const sf::Vector2f& upperLeft) ;

        /*!
         * \brief sets the lower right corner coordinates.
         * \param upperLeft the lower right corner coordinates.
         */
        virtual void setLowerRight(const sf::Vector2f& lowerRight) ;

        /*!
         * \brief sets the object margins.
         * \param margins the new margins
         */
        virtual void setMargins(const std::array<float, 4>& margins) ;

        /*!
         * \brief displays the object on the given RenderWindow.
         */
        virtual void drawOn(sf::RenderWindow&) const = 0 ;

    protected:
        /*!
         * \brief the object height.
         */
        float height ;
        /*!
         * \brief the object width.
         */
        float width  ;
        /*!
         * \brief the object x coordinate.
         */
        float x ;
        /*!
         * \brief the y coordinate.
         */
        float y ;
        /*!
         * \brief margin values for bottom, left, top, right.
         * These values can be used to correct the position and
         * and size of the object at drawing time without
         * modifying the height, size, x, y object attributes.
         */
        std::array<float, 4> margins ;
} ;

#endif //DRAWABLE_HPP
