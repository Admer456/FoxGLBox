#pragma once

// =====================================================================
// Render frontend material class
// Render backends do not implement their own
// =====================================================================
class Material : public IMaterial
{
public:
	// @returns The full name of the material
	const char* GetName() const override 
	{ return name.c_str(); }

	// Sets the material's name
	void SetName( const char* newName ) override
	{ name = newName; }

	// Get the material's shader
	IShader* GetShader() const override 
	{ return shader; }

	// Changes this material's shader
	void SetShader( IShader* newShader ) override 
	{ shader = newShader; }

	// Returns a texture associated with this material
	// @param type: The type of the texture (TextureType_Albedo etc.)
	// @param order: In case there are several textures of the same type,
	// order retrieves the nth texture of that type
	// @returns: A texture reference if it can be found
	ITexture* GetTexture( TextureType type, int order = 0 ) const override;

	// Adds a texture reference to this material.
	// If the same texture exists, then it isn't added.
	// @param texture: The texture to be added
	// @param force: Adds the texture anyway, even if it's already in there
	void AddTexture( ITexture* texture, bool force = false ) override;

private:
	std::string name{ "Default" };
	std::string fileName{ "#built-in" }; // Only gets set if this material was parsed from some material definition file

	IShader* shader{ nullptr };

	std::vector<ITexture*> textures;
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
