/*
==========
Copyright 2002 Energon Software

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
==========
*/


#if !defined VECTOR_H
#define VECTOR_H


#include <stdexcept>
#include <iostream>

#include <math.h>


template <class T>
class Vector
{
public:
    explicit Vector(const T& x=T(), const T& y=T(), const T& z=T())
    {
        m_x = static_cast<T>(x);
        m_y = static_cast<T>(y);
        m_z = static_cast<T>(z);
    }

    template <class V>
    Vector(const Vector<V>& vector)
    {
        m_x = static_cast<T>(vector.x());
        m_y = static_cast<T>(vector.y());
        m_z = static_cast<T>(vector.z());
    }

public:
    // resets the vector to its default values
    void clear()
    {
        m_x = T();
        m_y = T();
        m_z = T();
    }

public:
    // cross product
    template <class V>
    Vector<T> cross(const Vector<V>& rhs) const
    {
        return Vector<T>(m_y*rhs.z() - m_z*rhs.y(), m_z*rhs.x() - m_x*rhs.z(), m_x*rhs.y() - m_y*rhs.x());
    }

    // makes this vector a unit vector
    void normalize()
    {
        *this /= length();
    }

    // returns the unit vector
    Vector<T> unit() const
    {
        return (*this)/length();
    }

    // returns the length (norm) of the vector
    T length() const
    {
        float len = sqrt(static_cast<float>((*this) * (*this)));
        return (len != 0.0f) ? static_cast<T>(len) : static_cast<T>(1.0f);
    }

    // returns the length (norm) of the vector squared
    T length_squared() const
    {
        return (*this) * (*this);
    }

    // the distance between this and vector
    template <class V>
    T distance(const Vector<V>& vector) const
    {
        return (vector - *this).length();
    }

    // takes the vector, casts it to an integer
    // returns the new vector in the original type
    Vector<T> snap() const
    {
        Vector<int> t = static_cast<Vector<int> >(*this);
        return static_cast<Vector<T> >(t);
    }

    // equality/inequality
    template <class V>
    bool operator==(const Vector<V>& rhs) const
    {
        return (m_x == rhs.x() && m_y == rhs.y() && m_z == rhs.z());
    }
    template <class V>
    bool operator!=(const Vector<V>& rhs) const
    {
        return !((*this) == rhs);
    }

    // negation
    Vector<T> operator-() const
    {
        return Vector<T>(-m_x, -m_y, -m_z);
    }

    // vector addition
    template <class V>
    Vector<T> operator+(const Vector<V>& rhs) const
    {
        return Vector<T>(m_x + rhs.x(), m_y + rhs.y(), m_z + rhs.z());
    }
    template <class V>
    const Vector<T>& operator+=(const Vector<V>& rhs)
    {
        m_x += rhs.x(); m_y += rhs.y(); m_z += rhs.z();
        return *this;
    }

    // vector subtraction
    template <class V>
    Vector<T> operator-(const Vector<V>& rhs) const
    {
        return (*this) + (-rhs);
    }
    template <class V>
    const Vector<T>& operator-=(const Vector<V>& rhs)
    {
        return (*this) += (-rhs);
    }

    // dot product
    template <class V>
    T operator*(const Vector<V>& rhs) const
    {
        return (m_x * rhs.x()) + (m_y * rhs.y()) + (m_z * rhs.z());
    }

    // scalar multiplication
    Vector<T> operator*(const T& rhs) const
    {
        return Vector(m_x * rhs, m_y * rhs, m_z * rhs);
    }
    const Vector<T>& operator*=(const T& rhs)
    {
        m_x *= rhs; m_y *= rhs; m_z *= rhs;
        return *this;
    }

    // scalar division
    Vector<T> operator/(const T& rhs) const
    {
        return Vector<T>(m_x / rhs, m_y / rhs, m_z / rhs);
    }
    const Vector<T>& operator/=(const T& rhs)
    {
        m_x /= rhs; m_y /= rhs; m_z /= rhs;
        return *this;
    }

public:
    const T& x() const { return m_x; }
    const T& y() const { return m_y; }
    const T& z() const { return m_z; }

    template <class V> void add_x(const V& x) { m_x += static_cast<T>(x); }
    template <class V> void add_y(const V& y) { m_y += static_cast<T>(y); }
    template <class V> void add_z(const V& z) { m_z += static_cast<T>(z); }

    template <class V> void set_x(const V& x) { m_x = static_cast<T>(x); }
    template <class V> void set_y(const V& y) { m_y = static_cast<T>(y); }
    template <class V> void set_z(const V& z) { m_z = static_cast<T>(z); }

    void snap_x() { m_x = static_cast<T>(static_cast<int>(m_x)); }
    void snap_y() { m_y = static_cast<T>(static_cast<int>(m_y)); }
    void snap_z() { m_z = static_cast<T>(static_cast<int>(m_z)); }

    void clear_x() { m_x = T(); }
    void clear_y() { m_y = T(); }
    void clear_z() { m_z = T(); }

private:
    T m_x;
    T m_y;
    T m_z;
};


// prints the vector to an output stream
template <class T>
std::ostream& operator<<(std::ostream& lhs, const Vector<T>& rhs)
{
    lhs << "(" << rhs.x() << ", " << rhs.y() << ", " << rhs.z() << ")";
    return lhs;
}


#endif
