#ifndef MATRIX_HPP
#define MATRIX_HPP


#include <vector>
#include <numeric> // accumulate()
#include <iostream>
#include <iomanip>   // setw(), setprecision(), fixed
#include <stdexcept> // out_of_range, invalid_argument
#include <utility>   // swap()f



/*!
 * \brief The Matrix class is a generic class to store data in a matrix.
 * The matrix dimensionality can be any value : 1 is a vector, 2 is a regular
 * 2D matrix, 3 is a 3D matrix, etc.
 *
 * In order to store the data properly and to perform all operations smoothly, the
 * internal representation format differs from the "usual format". That is : the user
 * provides coordinates as (x,y,z,...) where x referes to the row number, y to
 * the column number, z the the z slice, etc.
 * Internally however, x corresponds to the column number and y to the row number.
 * Every other dimension has the same meaning.
 *
 * Internal representation :
 *
 * Here is an example of a 2x3 matrix (2D)
 *
 * {0,1,2,3,4,5} vector is turned to
 *      X
 * ---------->
 *  0  1  2 |
 *  3  4  5 | Y
 *         \|/
 *
 * dimensions are stored as {nx, ny} which corresponds to {ncol, nrow}. Coordinates
 * are given using the universal format coord=(x,y) which are interpreted as {row, col}.
 * Thus a simple swap(coord[0],coord[1]) should be performed to ensurethat the user given
 * coordinates can be used in this referencial.
 *
 *
 * Here is an example of a 2x3x2x2 matrix(4D)
 * {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23} is turned to
 *
 *      X
 * ----------->     |     |
 *  0   1   2 |     |     |
 *  3   4   5 | Y   |     |
 *           \|/    | Z   |
 *  6   7   8 |     |     |
 *  9  10  11 | Y   |     |
 *           \|/   \|/    |
 *                        | A
 * 12  13  14 |     |     |
 * 15  16  17 | Y   |     |
 *           \|/    | Z   |
 * 18  19  20 |     |     |
 * 21  22  23 | Y   |     |
 *           \|/   \|/   \|/
 *
 * dimensions are stored as {nx, ny, nz, na} which corredponds to {ncol, nrow, nz, na}.
 * Coordinates are given using the universal format coord=(x,y,z,a) which are interpreted
 * as {row, col, z, a}. Thus a simple swap(coord[0],coord[1]) should be performed to ensure
 * that the user given coordinates can be used in this referencial.
 *
 *
 */

template <class T>
class Matrix
{
    public:
        // constructors
        Matrix() = default ;
        /*!
         * \brief Constructs an matrix with the given dimension with
         * 0 values.
         * \param dim the dimensions.
         */
        Matrix(const std::vector<size_t>& dim) ;
        /*!
         * \brief Constructs a matrix with the given dimensions and
         * initialize the values to the given value.
         * \param dim the dimensions.
         * \param value the value to initialize the matrix content
         * with.
         */
        Matrix(const std::vector<size_t>& dim, T value) ;

        /*!
         * \brief Copy constructor.
         * \param other the matrix to copy.
         */
        Matrix (const Matrix& other) ;

        /*!
         * \brief Destructor.
         */
        virtual ~Matrix() = default ;

        // methods
        /*!
         * \brief Gets the element at the given offset.
         * \param offset the offset of the element to get.
         * \throw std::out_of_range exception if the offset
         * is out of range.
         * \return the element.
         */
        T get(size_t offset) const throw(std::out_of_range) ;

        /*!
         * \brief Gets the element at the given coordinates.
         * \param coord the coordinates of the element to get.
         * \throw std::out_of_range exception if the coordinates
         * are out of range.
         * \return the element.
         */
        T get(const std::vector<size_t>& coord) const throw(std::out_of_range) ;

        /*!
         * \brief Sets the element at the given offset
         * to the given value.
         * \param offset the offset of the element to set.
         * \param value the new value.
         * \throw std::out_of_range exception if the offset
         * is out of range.
         */
        void set(size_t offset, T value) throw(std::out_of_range) ;
        /*!
         * \brief Sets the element at the given coordinates
         * to the given value.
         * \param coord the coordinates of the element to set.
         * \param value the new value.
         * \throw std::out_of_range exception if the coordinates
         * are out of range.
         */
        void set(const std::vector<size_t>& coord, T value) throw(std::out_of_range) ;

        /*!
         * \brief Gets the matrix dimensions.
         * \return the dimensions.
         */
        std::vector<size_t> get_dim() const ;

        /*!
         * \brief Gets the data vector.
         * \return a a vector containing the data.
         */
        std::vector<T> get_data() ;

        /*!
         * \brief Gets the number of dimensions (the length
         * of the dimension vector).
         * \return the number of dimensions
         */
        size_t get_dim_size() const ;

        /*!
         * \brief Gets the number of elements contained in the
         * matrix.
         * \return the number of element contained in the
         * matrix.
         */
        size_t get_data_size() const ;

        /*!
         * \brief Returns the partial products of the dimensions.
         * \return the partial products of the dimensions.
         */
        std::vector<size_t> get_dim_product() const ;
		
        /*!
         * \brief Produces a nice representation of the matrix on the given
         * stream.
         * \param stream the stream.
         * \param precision the rounding precision.
         * \param width the column width in number of characters.
         * \param sep the character separator.
         */
        virtual void print(std::ostream& stram, size_t precision=4, size_t width=8, char sep=' ') const ;

        // operator
        /*!
         * \brief Assignment operator.
         * \param other an other matrix to copy the values from.
         * \return a reference to the current instance.
         */
        Matrix& operator = (const Matrix<T>& other) ;

        /*!
         * \brief Adds value to each element.
         * \param value the value to add.
         * \return a reference to the instance.
         */
        Matrix& operator += (T value) ;

        /*!
         * \brief Substracts value to each element.
         * \param value the value to substract.
         * \return a reference to the instance.
         */
        Matrix& operator -= (T value) ;

        /*!
         * \brief Multiplies each element by value.
         * \param value the value to multiply the elements by.
         * \return a reference to the instance.
         */
        Matrix& operator *= (T value) ;

        /*!
         * \brief Divides each element by value.
         * \param value the value to multiply the elements by.
         * \throw std::invalid_argument if value is 0.
         * \return a reference to the instance.
         */
        Matrix& operator /= (T value) throw (std::invalid_argument) ;

        /*!
         * \brief Comparison operator, returns true if
         * both matrices are identical, that is do not
         * have the same data and dimensions.
         * \param other an other matrix.
         * \return true if both matrices have the same
         * data and dimensions.
         */
        bool operator == (const Matrix<T>& other) const ;

        /*!
         * \brief Comparison operator, returns true if
         * both matrices are different, that is do not
         * have the same data and dimensions.
         * \param other an other matrix.
         * \return true if both matrices are different.
         */
        bool operator != (const Matrix& other) const ;

        /*!
         * \brief Returns a reference to the corrresponding
         * element. This method does not perform any check on
         * the coordinates.
         * \param coord coord the coordinates of the element to get.
         * \return a reference to this element.
         */
        T& operator () (const std::vector<size_t>& coord) ;

        /*!
         * \brief Returns a const reference to the corrresponding
         * element. This method does not perform any check on
         * the coordinates.
         * \param coord coord the coordinates of the element to get.
         * \return a const reference to this element.
         */
        const T& operator () (const std::vector<size_t>& coord) const ;

    protected:
        // methods
        /*!
         * \brief Computes the partial dimension products and fills
         * this->dim_prod according to the current values of
         * this->_dim and this->dim_size.
         */
        void compute_dim_product() ;

        /*!
         * \brief Given a vector of at least 2 dimensional coordinates,
         * it simply swaps the elements at index 0 (row number) and 1
         * (column number) to make them fit the x,y,... matrix
         * reprensetation (x:number of columns, y:number of rows).
         * \param coord a vector of coordinates (row, column, ...).
         * \return a vector of coordinates corresponding to (x,y,...).
         */
        std::vector<size_t> swap_coord(const std::vector<size_t>& coord) const ;

        /*!
         * \brief Complementary function of convert_coord(). Given
         * a vector of coordinates in (x,y,...) format, it turns it
         * into (row,col,...) format.
         * \param coord a vector of coordinates (x,y, ...).
         * \return a vector of coordinates corresponding to (row,col,...).
         */
        std::vector<size_t> convert_coord_back(const std::vector<size_t>& coord) const ;

        /*!
         * \brief Checks whether a given offset is a valid offset or
         * whether it is out of range.
         * \param offset the offset to check.
         * \return whether the offset is valid.
         */
        bool is_valid(size_t offset) const ;

        /*!
         * \brief Checks whether coordinates in (x,y,...) format are
         * valid or whether they are out of range.
         * \param offset the offset to check.
         * \return whether the offset is valid.
         */
        bool is_valid(const std::vector<size_t>& coord) const ;

        /*!
         * \brief Converts a vector of VALID (x,y,...) coordinates to a
         * the corresponding offset allowing to get an element in the
         * data vector.
         * If the coordinate vector has a (row, column, ...) format, the
         * result will be wrong.
         * \param coord a vector of coordinates with (x,y,...) format.
         * \return the corresponding offset.
         */
        size_t convert_to_offset(const std::vector<size_t>& coord) const ;

        /*!
         * \brief Complementary function of convert_to_offset(). Given an
         * offset, this function returns the corresponding coordinate
         * vector in (x,y,...) format.
         * \param offset a given offset.
         * \return the corresponding vector of (x,y,..) coordinates.
         */
        std::vector<size_t> convert_to_coord(size_t offset) const ;

        // fields
        /*!
         * \brief The dimensions values.
         */
        std::vector<size_t> _dim ;
        /*!
         * \brief Stores the data.
         */
        std::vector<T> _data ;
        /*!
         * \brief The number of dimensions.
         */
        size_t _dim_size ;
        /*!
         * \brief The number of data elements stored.
         */
        size_t _data_size ;

        /*!
         * \brief Contains the partial product of the dimensions. That is,
         * the ith element contains the product of all the i-1 precedent
         * dimensions :
         * element 0 : 1, element 1 : x, element 2 : x*y, element 3 : x*y*z,
         * and so one.
         * This is used for coordinates to offset and offset to coordinates
         * conversions.
         */
        std::vector<size_t> _dim_prod ;
} ;

// operators
/*!
 * \brief Addition operator.
 * \param m the matrix of interest
 * \param value the value to add to each element.
 * \return the resulting matrix.
 */
template<class T>
const Matrix<T> operator + (Matrix<T> m, T value)
{   Matrix<T> other(m) ;
    other += value ;
    return other ;
}

/*!
 * \brief Substraction operator
 * \param m the matrix of interest.
 * \param value the value to substract to each element.
 * \return the resulting matrix.
 */
template<class T>
const Matrix<T> operator - (Matrix<T> m, T value)
{   Matrix<T> other(m) ;
    other -= value ;
    return other ;
}

/*!
 * \brief Multiplication operator.
 * \param m the matrix of interest.
 * \param value the value to multiply each elements by.
 * \return the resulting matrix.
 */
template<class T>
const Matrix<T> operator * (Matrix<T> m, T value)
{   Matrix<T> other(m) ;
    other *= value ;
    return other ;
}

/*!
 * \brief Division operator.
 * \param m the matrix of interest.
 * \param value the value to divide each elements by.
 * \throw std::invalid_argument if value is 0.
 * \return the resulting matrix.
 */
template<class T>
const Matrix<T> operator / (Matrix<T> m, T value) throw (std::invalid_argument)
{   if(value == static_cast<T>(0))
    {   throw std::invalid_argument("division by 0!") ; }
    Matrix<T> other(m) ;
    other /= value ;
    return other ;
}

/*!
 * \brief Sends a representation of the matrix to the stream.
 * \param stream the stream of interest.
 * \param m the matrix of interest.
 * \return a reference to the stream.
 */
template<class T>
std::ostream& operator << (std::ostream& stream, const Matrix<T>& m)
{	m.print(stream) ;   
    return stream ;
}



// method implementation
template<class T>
Matrix<T>::Matrix(const std::vector<size_t>& dim)
    : Matrix(dim, 0)
{}


#include "Utility/Vector_utility.hpp"

template<class T>
Matrix<T>::Matrix(const std::vector<size_t>& dim, T value)
{   this->_dim_size  = dim.size() ;
    this->_dim       = this->swap_coord(dim) ;
    this->_data_size = std::accumulate(dim.begin(), dim.end(), 1, std::multiplies<size_t>()) ;
    this->_data      = std::vector<T>(this->_data_size, value) ;
    this->compute_dim_product() ;
}

template<class T>
Matrix<T>::Matrix(const Matrix &other)
{   *this = other ; }


template<class T>
T Matrix<T>::get(size_t offset) const throw(std::out_of_range)
{   if(not this->is_valid(offset))
    {   throw std::out_of_range("offset is out of range!") ; }
    return this->_data[offset] ;
}

template<class T>
T Matrix<T>::get(const std::vector<size_t>& coord) const throw(std::out_of_range)
{   std::vector<size_t> coord_new = this->swap_coord(coord) ;
    if(not this->is_valid(coord_new))
    {   throw std::out_of_range("coordinates are out of range!") ; }
    return this->_data[this->convert_to_offset(coord_new)] ;
}


template<class T>
void Matrix<T>::set(size_t offset, T value) throw(std::out_of_range)
{   if(not this->is_valid(offset))
    {   throw std::out_of_range("offset is out of range!") ; }
    this->_data[offset] = value ;
}

template<class T>
void Matrix<T>::set(const std::vector<size_t>& coord, T value) throw(std::out_of_range)
{   std::vector<size_t> coord_new = this->swap_coord(coord) ;
    if(not this->is_valid(coord_new))
    {   throw std::out_of_range("coordinates are out of range!") ; }
    this->_data[this->convert_to_offset(coord_new)] = value ;
}


template<class T>
std::vector<size_t> Matrix<T>::get_dim() const
{   return this->swap_coord(this->_dim) ; }

template<class T>
std::vector<T> Matrix<T>::get_data()
{   return this->_data ; }

template<class T>
size_t Matrix<T>::get_dim_size() const
{   return this->_dim_size ; }

template<class T>
size_t Matrix<T>::get_data_size() const
{   return this->_data_size ; }

template<class T>
std::vector<size_t> Matrix<T>::get_dim_product() const
{   return this->_dim_prod ; }

template<class T>
void Matrix<T>::print(std::ostream& stream, size_t precision, size_t width, char sep) const
{	stream.setf(std::ios::left) ;
    stream << std::setprecision(precision) << std::fixed ;
	for(size_t i=0; i<this->get_data_size(); i++)
    {   stream << std::setw(width) << this->get(i) << sep ; }
}

template<class T>
Matrix<T>& Matrix<T>::operator = (const Matrix<T>& other)
{
    this->_dim       = other._dim ;
    this->_dim_size  = other._dim_size ;
    this->_data      = other._data ;
    this->_data_size = other._data_size ;
    this->_dim_prod  = other._dim_prod ;
    return *this ;
}

template<class T>
Matrix<T>& Matrix<T>::operator += (T value)
{   for(auto& i : this->_data)
    {   i += value ; }
    return *this ;
}

template<class T>
Matrix<T>& Matrix<T>::operator -= (T value)
{   for(auto& i : this->_data)
    {   i -= value ; }
    return *this ;
}

template<class T>
Matrix<T>& Matrix<T>::operator *= (T value)
{   for(auto& i : this->_data)
    {   i *= value ; }
    return *this ;
}

template<class T>
Matrix<T>& Matrix<T>::operator /= (T value) throw (std::invalid_argument)
{
    if(value == static_cast<T>(0))
    {   throw std::invalid_argument("division by 0!") ; }

    for(auto& i : this->_data)
    {   i /= value ; }
    return *this ;
}

template<class T>
bool Matrix<T>::operator == (const Matrix<T>& other) const
{   if(&other == this)
    {   return true ; }
    // check dim
    if(this->_dim_size != other._dim_size)
    {   return false ; }
    for(size_t i=0; i<this->_dim_size; i++)
    {   if(this->_dim[i] != other._dim[i])
        {   return false ; }
    }
    // check data
    if(this->_data_size != other._data_size)
    {   return false ; }
    for(size_t i=0; i<this->_data_size; i++)
    {   if(this->_data[i] != other._data[i])
        {   return false ; }
    }
    return true ;
}

template<class T>
bool Matrix<T>::operator !=(const Matrix<T>& other) const
{   return not ((*this) == other) ;}

template<class T>
T& Matrix<T>::operator () (const std::vector<size_t>& coord)
{   std::vector<size_t> coord_new = this->swap_coord(coord) ;
    return this->_data[this->convert_to_offset(coord_new)] ;
}

template<class T>
const T& Matrix<T>::operator () (const std::vector<size_t>& coord) const
{   std::vector<size_t> coord_new = this->swap_coord(coord) ;
    return this->_data[this->convert_to_offset(coord_new)] ;
}


template<class T>
void Matrix<T>::compute_dim_product()
{   this->_dim_prod = std::vector<size_t>(this->_dim_size, 0) ;
    this->_dim_prod[0] = 1 ;
    if(this->_dim_size > 1)
    {   this->_dim_prod[1] = this->_dim[0] ; }
    if(this->_dim_size > 2)
    {   for(size_t i=2; i<this->_dim_size; i++)
        {   this->_dim_prod[i] = this->_dim_prod[i-1]*this->_dim[i-1] ; }
    }
}


template<class T>
std::vector<size_t> Matrix<T>::swap_coord(const std::vector<size_t> &coord) const
{   std::vector<size_t> coord_new = coord ;
    // reformat coord = (row,col,...) = (y,y,...) into coord = (col,row,...) = (x,y,...)
    if(this->_dim_size > 1)
    {   std::swap(coord_new[0], coord_new[1]) ; }
    return coord_new ;
}


template<class T>
bool Matrix<T>::is_valid(size_t offset) const
{   if(offset > this->_data_size-1)
    {   return false ; }
    return true ;
}

template<class T>
bool Matrix<T>::is_valid(const std::vector<size_t>& coord) const
{   if(coord.size() != this->_dim_size)
    {   return false ; }
    for(size_t i=0; i<coord.size(); i++)
    {   if(coord[i] > this->_dim[i])
        {   return false ; }
    }
    return true ;
}



template<class T>
size_t Matrix<T>::convert_to_offset(const std::vector<size_t>& coord) const
{   size_t offset = 0 ;

    for(size_t i=0; i<this->_dim_size; i++)
    {   offset += coord[i] * this->_dim_prod[i] ; }

    return offset ;
}


template<class T>
std::vector<size_t> Matrix<T>::convert_to_coord(size_t offset) const
{
    std::vector<size_t> coord(this->_dim_size, 0) ;

    for(int i=this->_dim_size-1; i>=0; i--)
    {   size_t c = offset / this->_dim_prod[i] ;
        coord[i] = c ;
        offset -= (this->_dim_prod[i]*c) ;
    }

    return coord ;
}




#endif // MATRIX_HPP
