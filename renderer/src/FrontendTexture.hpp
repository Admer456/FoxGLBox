#pragma once

// =====================================================================
// FrontendTexture
// 
// A "base" class for textures, to be used by the render frontend
// (RenderWorld). API specifics are implemented in subclasses for
// each render backend, for example, OpenGL45/Texture.hpp.  
// 
// The render backend is the one that actually allocates the texture,
// however, the frontend is what stores a vector of them.  
// =====================================================================
class FrontendTexture : public ITexture
{
public:
	// API-specific, implemented by each backend's Texture class
	virtual void	Init() = 0;
	virtual void	Bind( uint8_t textureUnit ) = 0;
	virtual void	LoadDirect( int textureWidth, int textureHeight,
								TextureType textureType, uint16_t textureFlags, byte* data ) = 0;

	bool			LoadFromFile( const char* path ) override;
	
public: // Basic getters
	// @returns The full path to the texture, or a custom name if this texture is generated
	const char*		GetName() const override;

	// @returns The associated texture type
	TextureType		GetTextureType() const override { return type; }
	// Sets the texture type
	void			SetTextureType( const TextureType& newType ) override { type = newType; }

	// @returns The associated texture flags
	uint16_t		GetTextureFlags() const override { return flags; }
	// Sets the texture flags
	void			SetTextureFlags( const uint16_t& newFlags ) override { flags = newFlags; }

protected:
	// Naming
	std::string		name{ "Default" };
	std::string		fileName{ "#builtin" };

	// Texture metadata
	TextureType		type{ TextureType_Albedo };
	uint16_t		flags{ DefaultTextureFlags };

	// Texture dimensions
	int				width;
	int				height;
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