#pragma once

class fglVector;

class fglVector
{
public:
    static const fglVector Zero;
    static const fglVector Identity;

    // Forward = +Y, Right = +X, Up = +Z
    static const fglVector Forward;
    static const fglVector Right;
    static const fglVector Up;

public:
    fglVector( float X = 0.0f, float Y = 0.0f, float Z = 0.0f )
        : x( X ), y( Y ), z( Z )
    {

    }

    ~fglVector() = default;

public: // Some general methods
    float Length() const;
    float Length2D() const;

public: // Operators
    // + and +=
    inline fglVector operator+ ( const fglVector& rhs ) const
    {
        return fglVector( x + rhs.x, y + rhs.y, z + rhs.z );
    }
    inline fglVector operator+= ( const fglVector& rhs )
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    // - and -=
    inline fglVector operator- ( const fglVector& rhs ) const
    {
        return fglVector( x - rhs.x, y - rhs.y, z - rhs.z );
    }

    inline fglVector operator-= ( const fglVector& rhs )
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    // * and *=
    inline fglVector operator* ( const float& rhs ) const
    {
        return fglVector( x * rhs, y * rhs, z * rhs );
    }
    inline fglVector operator*= ( const float& rhs )
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    // / and /=
    inline fglVector operator/ ( const float& rhs ) const
    {
        return fglVector( x / rhs, y / rhs, z / rhs );
    }
    inline fglVector operator/= ( const float& rhs )
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

public:
    float x, y, z;
};

/*
Copyright (c) 2021 Admer456

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
