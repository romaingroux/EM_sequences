#include "Drawable.hpp"

Drawable::Drawable()
    : height(0), width(0), x(0), y(0), margins({0,0,0,0})
{}

Drawable::Drawable(const sf::Vector2f& upperLeft,
                   const sf::Vector2f& lowerRight,
                   const std::array<float, 4>& margins)
    : height(lowerRight.y - upperLeft.y),
      width(lowerRight.x - upperLeft.x),
      x(upperLeft.x),
      y(upperLeft.y),
      margins(margins)
{}

Drawable::~Drawable()
{}

sf::Vector2f Drawable::getUpperLeft() const
{   return sf::Vector2f(this->x,this->y) ; }

sf::Vector2f Drawable::getLowerRight() const
{   return sf::Vector2f(this->x+this->width, this->y+this->height) ; }

float Drawable::getHeight() const
{   return this->height ; }

float Drawable::getWidth() const
{   return this->width ; }

std::array<float, 2> Drawable::getPosition() const
{   return std::array<float, 2>({this->x, this->y}) ; }

std::array<float, 4> Drawable::getMargins() const
{   return this->margins ; }

void Drawable::setUpperLeft(const sf::Vector2f& upperLeft)
{   float x_old = this->x ;
    float y_old = this->y ;

    this->x = upperLeft.x ;
    this->y = upperLeft.y ;

    float dx = x_old - this->x ;
    float dy = y_old - this->y ;

    this->width  += dx ;
    this->height += dy ;
}

void Drawable::setLowerRight(const sf::Vector2f& lowerRight)
{   float x_old = this->x + this->width ;
    float y_old = this->y + this->height ;

    float dx = x_old - lowerRight.x ;
    float dy = y_old - lowerRight.y ;

    this->width  += dx ;
    this->height += dy ;
}

void Drawable::setMargins(const std::array<float,4>& margins)
{   this->margins = margins ; }
