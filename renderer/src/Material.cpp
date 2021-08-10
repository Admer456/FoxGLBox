#include "IRenderWorld.hpp"
#include "Material.hpp"

// =====================================================================
// Material::GetTexture
// =====================================================================
ITexture* Material::GetTexture( TextureType type, int order ) const
{
	int number = 0;
	for ( ITexture* texture : textures )
	{
		if ( texture->GetTextureType() == type )
		{
			if ( number == order )
			{
				return texture;
			}

			number++;
		}
	}

	return nullptr;
}

// =====================================================================
// Material::AddTexture
// =====================================================================
void Material::AddTexture( ITexture* texture, bool force )
{
	for ( ITexture* texture : textures )
	{
		if ( texture == texture && !force )
		{
			return;
		}
	}

	textures.push_back( texture );
}
