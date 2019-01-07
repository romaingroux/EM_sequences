#ifndef LOGOWINDOW_HPP
#define LOGOWINDOW_HPP

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

#include <Matrix/Matrix2D.hpp>
#include <GUI/Drawable/Drawable.hpp>
#include <GUI/Axis/Axis.hpp>
#include <GUI/Title/Title.hpp>
#include <GUI/Logo/Logo.hpp>

class LogoWindow : public sf::RenderWindow
{

    public:
        // constructors
        LogoWindow(const sf::VideoMode& mode,
                   const std::string& title,
                   const Matrix2D<double>& matrix,
                   const std::vector<double>& background,
                   const std::string& text,
                   const std::string& file_A,
                   const std::string& file_C,
                   const std::string& file_G,
                   const std::string& file_T,
                   const std::string& file_font) ;

        LogoWindow(const LogoWindow& other) = delete ;

        /*!
         * Destructor.
         */
        ~LogoWindow() ;

        // methods
        /*!
         * \brief Quits and closes the windows.
         */
        void close() ;

        /*!
         * \brief Displays the windows.
         */
        void display() ;

        /*!
         * \brief Frees the memory.
         */
        void quit() ;

    private:

        // methods
        /*!
         * \brief Called upon window resize. Resets each component
         * coordinates given the current window size.
         */
        virtual void onResize() ;

        /*!
         * \brief Resets the title upper left coordinates such
         * that it matches (0,0).
         */
        void updateTitleUpperLeft() ;

        /*!
         * \brief Resets the title lower right coordinates such
         * that it matches (1*width, 1/8*heigth) given the current
         * window size.
         */
        void updateTitleLowerRight() ;

        /*!
         * \brief Resets the logo upper left coordinates such that
         * it matches (1/6*width, 1/8*height) given the current
         * window size.
         */
        void updateLogoUpperLeft() ;

        /*!
         * \brief Resets the logo lower right coordinates such that
         * it matches (width, 7/8*height) given the current window
         * size.
         */
        void updateLogoLowerRight() ;

        void updateLogoMargins() ;

        /*!
         * \brief Resets the x-axis upper left coordinates such that
         * it matches (1/6*width, 7/8*heigth) given the current
         * window size.
         */
        void updateXAxisUpperLeft() ;

        /*!
         * \brief Resets the x-axis lower right coordinates such that
         * it matches (width, height) given the current window size.
         */
        void updateXAxisLowerRight() ;

        void updateXaxisMargins() ;

        /*!
         * \brief Resets the y-axis upper left coordinates such that
         * it matches (1/6*width, 1/8*height) given the current
         * window size.
         */
        void updateYAxisUpperLeft() ;

        /*!
         * \brief  Resets the y-axis lower right coordinates such that
         * it matches (width, 7/8*height) given the current window
         * coordinates.
         */
        void updateYAxisLowerRight() ;


        // fields
        /*!
         * \brief The title of the figure, will be displayed at
         * upper left (0,0) and lower right (1*width, 1/8*heigth).
         */
        Title* _title ;
        /*!
         * \brief The figure x-axis, will be displayed at
         * upper left (1/6*width, 7/8*heigth) and lower right
         * (width, height).
         */
        Axis* _xaxis ;
        /*!
         * \brief The figure y-axis, will be displayed at
         * upper left (1/6*width, 1/8*height) and lower
         * left (1/6*width, 7/8*height).
         */
        Axis* _yaxis ;
        /*!
         * \brief The logo display, will be displayed at
         * upper left (1/6*width, 1/8*height) and lower
         * right (width, 7/8*height).
         */
        Logo* _logo ;
        /*!
         * \brief The absolute path to the file containing the
         * image to represent the A character.
         */
        std::string _file_A ;
        /*!
         * \brief The absolute path to the file containing the
         * image to represent the C character.
         */
        std::string _file_C ;
        /*!
         * \brief The absolute path to the file containing the
         * image to represent the G character.
         */
        std::string _file_G ;
        /*!
         * \brief The absolute path to the file containing the
         * image to represent the T character.
         */
        std::string _file_T ;
        /*!
         * \brief The absolute path to the file containing
         * character font to draw texts.
         */
        std::string _file_font ;
} ;



#endif // LOGOWINDOW_HPP
