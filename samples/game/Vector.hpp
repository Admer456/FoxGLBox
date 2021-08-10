#pragma once

class fglVector;

class fglVector
{
public:
    static const fglVector Zero;
    static const fglVector Identity;

    // Forward = Y, Right = X, Up = Z
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


