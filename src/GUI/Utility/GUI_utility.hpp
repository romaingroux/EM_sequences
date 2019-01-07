#ifndef GUI_UTILITY_HPP
#define GUI_UTILITY_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <vector>
#include <string>

#include <GUI/Axis/Axis.hpp>

/*!
 * \brief constructs an x-axis for a DNA logo, containing the specified number
 * of positions
 * \param upperLeft the coordinates of the upper left corner of the axis in the screen
 * where the axis will be drawn.
 * \param lowerRight the coordinates of the lower right corner of the axis in the screen
 * where the axis will be drawn.
 * \param file_font the absolute path to the file containing
 * character font to draw texts.
 * \param n_position the number of positions (ticks) the axis should contains
 * \return a pointer to an axis dynamically allocated within the function
 */
Axis* constructXAxisDNALogo(const sf::Vector2f& upperLeft, const sf::Vector2f& lowerRight,
                            const std::string& file_font, size_t n_position) ;

/*!
 * \brief constructs a y-axis for a DNA logo, with 3 ticks : 0, 1 and 2 bits
 * at 0%, 50% and 100% of the axis length respectively.
 * \param upperLeft the coordinates of the upper left corner of the axis in the screen
 * where the axis will be drawn.
 * \param lowerRight the coordinates of the lower right corner of the axis in the screen
 * where the axis will be drawn.
 * \param file_font the absolute path to the file containing
 * character font to draw texts.
 * \return a pointer to an axis dynamically allocated within the function
 */
Axis* constructYAxisDNALogo(const sf::Vector2f& upperLeft, const sf::Vector2f& lowerRight,
                            const std::string& file_font) ;


# endif // GUI_UTILITY_HPP
