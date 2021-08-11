#include "IRenderWorld.hpp"
#include "FrontendTexture.hpp"
#include "Texture.hpp"

#define GLEW_STATIC 1
#include <GL/glew.h>

extern bool GLError( const char* why = nullptr );

// =====================================================================
// Texture::Init
// =====================================================================
void Texture::Init()
{
	glCreateTextures( GL_TEXTURE_2D, 1, &textureHandle );
	GLError( "Texture::Init: created a texture" );
}

// =====================================================================
// Texture::Bind
// =====================================================================
void Texture::Bind( uint8_t textureUnit )
{
	// A texture is sometimes bound while being loaded,
	// so it can be buffered to the GPU
	if ( !loaded )
	{
		glActiveTexture( GL_TEXTURE0 + textureUnit );
	}

	glBindTexture( GL_TEXTURE_2D, textureHandle );

	if ( loaded )
		GLError( "Texture::Bind: activated the texture and bound it" );
	else 
		GLError( "Texture::Bind: bound a texture for loading" );
}

// =====================================================================
// PrintTextureInfo
// A certain debugging helper function, converts some OpenGL enums
// into strings and prints them
// =====================================================================
#define texInfo( glEnum ) { glEnum, #glEnum },
void PrintTextureInfo( int textureDataType, int textureFormat )
{
	printf( "## TextureInfo:\n" );
	struct TexInfo { int glEnum; const char* glString; };
	constexpr TexInfo infos[] =
	{
		texInfo( GL_UNSIGNED_BYTE )
		texInfo( GL_FLOAT )
		texInfo( GL_RGB )
		texInfo( GL_RGB8 )
		texInfo( GL_RGBA )
		texInfo( GL_RGBA8 )
		texInfo( GL_RGB32F )
		texInfo( GL_RGBA32F )
		texInfo( GL_R )
		texInfo( GL_RED )
		texInfo( GL_R8 )
		texInfo( GL_R32F )
	};

	for ( const auto& info : infos )
	{
		if ( textureDataType == info.glEnum )
		{
			printf( "## Data type: %s\n", info.glString );
		}
		if ( textureFormat == info.glEnum )
		{
			printf( "## Texture format: %s\n", info.glString );
		}
	}
}
#undef texInfo

// =====================================================================
// Texture::LoadDirect
// =====================================================================
void Texture::LoadDirect( int textureWidth, int textureHeight,
						  TextureType textureType, uint16_t textureFlags, byte* data )
{
	loaded = false;
	flags = textureFlags;
	type = textureType;

	int textureDataType = GL_UNSIGNED_BYTE;
	int textureFormat = GL_RGB;

	// TODO: maybe put this in a separate function?
	if ( flags & TextureFlag_FloatSized )
	{
		textureDataType = GL_FLOAT;
		textureFormat = GL_RGB32F;
	}

	if ( flags & TextureFlag_RGBA )
	{
		if ( flags & TextureFlag_FloatSized )
		{
			textureFormat = GL_RGBA32F;
		}
		else
		{
			textureFormat = GL_RGBA8;
		}
	}

	if ( flags & TextureFlag_Greyscale )
	{
		if ( flags & TextureFlag_FloatSized )
		{
			textureFormat = GL_R32F;
		}
		else
		{
			textureFormat = GL_R8;
		}
	}

	Bind( 0 );
	PrintTextureInfo( textureDataType, textureFormat );
	glTexImage2D( GL_TEXTURE_2D, 0, textureFormat, textureWidth, textureHeight, 0, textureFormat, textureDataType, data );
	GLError( "Texture::LoadDirect: buffered a texture" );

	DetermineTextureRepeat();
	DetermineTextureFilter();

	glGenerateMipmap( GL_TEXTURE_2D );
	GLError( "Texture::LoadDirect: generated mipmaps for a texture" );

	loaded = true;
}

// =====================================================================
// Texture::DetermineTextureRepeat
// =====================================================================
void Texture::DetermineTextureRepeat()
{
	int repeatType = GL_REPEAT;

	if ( flags & TextureFlag_RepeatClampToEdge )
	{
		repeatType = GL_CLAMP_TO_EDGE;
	}
	if ( flags & TextureFlag_RepeatMirror )
	{
		repeatType = GL_MIRRORED_REPEAT;
	}

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeatType );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeatType );
	GLError( "Texture: set a repeat type" );
}

// =====================================================================
// Texture::DetermineTextureFilter
// =====================================================================
void Texture::DetermineTextureFilter()
{
	int filterTypeMag = GL_LINEAR;
	int filterTypeMin = GL_LINEAR_MIPMAP_LINEAR;

	if ( flags & TextureFlag_Nearest )
	{
		filterTypeMag = GL_NEAREST;
		filterTypeMin = GL_NEAREST_MIPMAP_LINEAR;
	}

	if ( flags & TextureFlag_NoMip )
	{
		filterTypeMin = filterTypeMag;
	}

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterTypeMag );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterTypeMin );
	GLError( "Texture: set the filtering type" );
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
