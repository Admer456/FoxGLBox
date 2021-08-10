#include "IRenderWorld.hpp"
#include "FrontendTexture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool FrontendTexture::LoadFromFile( const char* path )
{
	fileName = path;

	int channels;
	byte* imageData = stbi_load( path, &width, &height, &channels, 3 );
	if ( nullptr == imageData )
	{
		stbi_image_free( imageData );
		return false;
	}

	if ( channels == 1 )
	{
		flags &= ~(TextureFlag_RGB | TextureFlag_RGBA);
		flags |= TextureFlag_Greyscale;
	}
	else if ( channels == 3 )
	{
		flags &= ~(TextureFlag_RGBA | TextureFlag_Greyscale);
		flags |= TextureFlag_RGB;
	}
	else //if ( channels == 4 )
	{
		flags &= ~(TextureFlag_RGB | TextureFlag_Greyscale);
		flags |= TextureFlag_RGBA;
	}

	LoadDirect( width, height, type, flags, imageData );
	stbi_image_free( imageData );

	return true;
}

const char* FrontendTexture::GetName() const
{
	if ( fileName == "#builtin" )
	{
		return name.c_str();
	}

	return fileName.c_str();
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
