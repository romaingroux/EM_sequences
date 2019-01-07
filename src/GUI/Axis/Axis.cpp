#include "Axis.hpp"

#include <algorithm>

Axis::Axis(const  sf::Vector2f& upperLeft,
           const  sf::Vector2f& lowerRight,
           const  std::vector<std::string>& tickValues,
           const  std::vector<float>& tickAt,
           const  std::string& file_font,
           size_t valueSize,
           float  tickSize,
           const  std::string& title,
           Side   side,
           const  std::array<float, 4> margins)
    : Drawable(upperLeft, lowerRight, margins),
      tickNumber(tickValues.size()),
      mainLine(sf::RectangleShape()),
      vectorTicks(std::vector<sf::RectangleShape>()),
      tickValues(tickValues),
      tickAt(tickAt),
      valueSize(valueSize),
      tickSize(tickSize),
      title(title),
      side(side),
      font(sf::Font()),
      file_font(file_font)
{   // load font
    this->loadFont() ;

    // compute the coordinates of the axis line
    this->createMainLine() ;
}

Axis::~Axis()
{}

void Axis::setTickSize(float size)
{   this->tickSize = size ; }

void Axis::setValueSize(size_t size)
{   this->valueSize = size ; }

void Axis::loadFont()
{   this->font.loadFromFile(this->file_font) ; }

void Axis::createMainLine()
{   float mainX0, mainX1 ;
    float mainY0, mainY1 ;
    float mainHeight, mainWidth ;

    float lineWidth = 2. ;

    // main line will be as high as possible in the drawing area
    if(side == x_axis)
    {   mainX0 = this->x + this->margins[1] ;
        mainX1 = mainX0  + this->width - this->margins[1] - this->margins[3]  ;
        mainY0 = this->y + this->margins[2] ;
        mainY1 = mainY0 + lineWidth ;
    }
    // main line will be as left as possible in the drawing area
    else
    {   mainX0 = this->x + this->width - this->margins[3] ;
        mainX1 = mainX0 + lineWidth ;
        mainY0 = this->y + this->margins[2] ;
        mainY1 = mainY0  + this->height - this->margins[2] - this->margins[0] ;
    }

    mainWidth  = mainX1 - mainX0 ;
    mainHeight = mainY1 - mainY0 ;

    this->mainLine.setPosition(mainX0, mainY0) ;
    this->mainLine.setSize(sf::Vector2f(mainWidth, mainHeight)) ;
    this->mainLine.setFillColor(sf::Color::Black) ;
}

void Axis::update()
{   this->loadFont() ;
    this->createMainLine() ;
}

void Axis::drawOn(sf::RenderWindow& target) const
{   target.draw(this->mainLine) ;
    this->drawTicks(target) ;
    this->drawTitle(target) ;
}

void Axis::drawTicks(sf::RenderWindow& target) const
{   // number of ticks to draw
    float nticks = static_cast<float>(tickValues.size()) ;

    // upperleft corner coordinates of the mainLine and its
    // width and heigth
    float mainLineX = this->mainLine.getPosition().x ;
    float mainLineY = this->mainLine.getPosition().y ;
    float mainLineWidth   = this->mainLine.getSize().x ;
    float mainLineHeight  = this->mainLine.getSize().y ;

    // to draw the ticks
    sf::RectangleShape tick ;
    tick.setFillColor(sf::Color::Black) ;
    // to draw the labels
    sf::Text text ;
    text.setFont(this->font) ;
    text.setFillColor(sf::Color::Black) ;

    for(size_t i=0; i<nticks; i++)
    {   text.setString(this->tickValues[i]) ;
        text.setCharacterSize(this->valueSize) ;
        if(this->side == x_axis)
        {   float posX = mainLineX + (mainLineWidth*this->tickAt[i]) ;
            tick.setPosition(sf::Vector2f(posX, mainLineY+mainLineHeight)) ;
            tick.setSize(sf::Vector2f(2, tickSize)) ;
            text.setPosition(sf::Vector2f(posX, mainLineY+3*mainLineHeight)) ;
            text.setOrigin(text.getLocalBounds().width / 2., 0) ;
        }
        else
        {   // the start of the axis is considered to be at the bottom of the screen (high Y values)
            float posY = mainLineY + mainLineHeight - (mainLineHeight*this->tickAt[i]) ;
            tick.setPosition(sf::Vector2f(mainLineX-tickSize, posY)) ;
            tick.setSize(sf::Vector2f(tickSize, 2)) ;
            text.setPosition(sf::Vector2f(mainLineX-3*tickSize, posY)) ;
            text.setOrigin(0, text.getLocalBounds().height / 1.5 ) ; // 1.5 was set by eye
        }
        target.draw(tick) ;
        target.draw(text) ;
    }
}

void Axis::drawTitle(sf::RenderWindow& target) const
{
    // upperleft corner coordinates of the mainLine and its
    // width and heigth
    float mainLineX = this->mainLine.getPosition().x ;
    float mainLineY = this->mainLine.getPosition().y ;
    float mainLineWidth   = this->mainLine.getSize().x ;
    float mainLineHeight  = this->mainLine.getSize().y ;

    // to draw the text
    sf::Text text(this->title, this->font) ;
    text.setFillColor(sf::Color::Black) ;

    if(this->side == x_axis)
    {   text.setPosition(mainLineX + mainLineWidth/2., mainLineY+5*this->tickSize) ;
        text.setOrigin(text.getLocalBounds().width/2., 0) ;
    }
    else
    {   text.setPosition(mainLineX-8*this->tickSize, mainLineY + mainLineHeight/2.) ;
        text.setRotation(270) ;
        text.setOrigin(text.getLocalBounds().height, 0 ) ;
    }
    target.draw(text) ;
}



