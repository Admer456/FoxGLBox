#include "Vector.hpp"
#include <math.h>

const fglVector fglVector::Zero		= fglVector( 0.0f, 0.0f, 0.0f );
const fglVector fglVector::Identity = fglVector( 1.0f, 1.0f, 1.0f );
const fglVector fglVector::Forward	= fglVector( 0.0f, 1.0f, 0.0f );
const fglVector fglVector::Right	= fglVector( 1.0f, 0.0f, 0.0f );
const fglVector fglVector::Up		= fglVector( 0.0f, 0.0f, 1.0f );

float fglVector::Length() const
{
	return sqrt( x*x + y*y + z*z );
}

float fglVector::Length2D() const
{
	return sqrt( x*x + y*y );
}
