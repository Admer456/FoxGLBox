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
