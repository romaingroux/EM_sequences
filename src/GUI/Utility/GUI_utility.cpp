#include "GUI_utility.hpp"
#include <string>
#include <SFML/System/Vector2.hpp>

Axis* constructXAxisDNALogo(const sf::Vector2f& upperLeft, const sf::Vector2f& lowerRight,
                            const std::string& file_font, size_t n_position)
{   // the axis title
    std::string title = "positions" ;

    // create 1 label per position (the position number)
    std::vector<std::string> labelValues ;
    std::vector<float>       labelAt ;
    for(size_t i=0; i<n_position; i++)
    {   labelValues.push_back(std::to_string(i+1)) ;
        float f = static_cast<float>(i) ;
        float n = static_cast<float>(n_position) ;
        labelAt.push_back((2*f+1)/(2*n)) ; // this is (f/n) + (1/2n) where (1/2n) centers
                                            // the label in the middle of the position
    }

    // create the axis
    Axis* axis = nullptr ;
    axis = new Axis(upperLeft, lowerRight, labelValues, labelAt, file_font, 30, 10., title, x_axis) ;
   return axis ;
}


Axis* constructYAxisDNALogo(const sf::Vector2f& upperLeft, const sf::Vector2f& lowerRight,
                            const std::string& file_font)
{   // the axis title
    std::string title = "bits" ;

    // labels for 0,1,2 bits
    std::vector<std::string> labelValues ({"0", "1", "2"}) ;
    std::vector<float>       labelAt ({0., 0.5, 1.}) ;

    // create the axis
    Axis* axis = nullptr ;
    axis = new Axis(upperLeft, lowerRight, labelValues, labelAt, file_font, 30, 10., title, y_axis) ;
    return axis ;
}
