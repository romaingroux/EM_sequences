#include "Title.hpp"

#include <iostream>
Title::Title(const sf::Vector2f& upperLeft,
             const sf::Vector2f& lowerRight,
             const std::string& text,
             const std::string& file_font,
             unsigned int characterSize,
             const std::array<float, 4>& margins)
    : Drawable(upperLeft, lowerRight, margins),
      characterSize(characterSize),
      file_font(file_font)
{   // loads font, creates the Text object and sets positioning
    this->loadFont() ;
    this->text = sf::Text(text, this->font, this->characterSize) ;
    this->text.setFillColor(sf::Color::Black) ;
    this->setTextPosition() ;
}

Title::~Title()
{}


void Title::setText(const std::string& text)
{   this->text.setString(text) ;
    this->update() ;
}

void Title::setCharacterSize(unsigned int size)
{   this->characterSize = size ;
    this->update() ;
}

void Title::drawOn(sf::RenderWindow& target) const
{   target.draw(this->text) ; }

void Title::update()
{   this->loadFont() ;
    this->text.setCharacterSize(this->characterSize) ;
    this->setTextPosition() ; }

void Title::loadFont()
{   std::string file = this->file_font ;
    this->font.loadFromFile(file) ;
}

void Title::setTextPosition()
{   float textHeight = this->text.getLocalBounds().height ;
    float textWidth  = this->text.getLocalBounds().width ;
    this->text.setPosition(this->x + (this->width/2.f) - (textWidth/2.f),
                           this->y + (textHeight/2.f)) ;   
}
