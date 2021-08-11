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
