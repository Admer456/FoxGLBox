#pragma once

// =====================================================================
// Texture
// 
// API-specific texture implementation. 
// Look at FrontendTexture for more info
// =====================================================================
class Texture final : public FrontendTexture
{
public:
	void Init() override;
	void Bind( uint8_t textureUnit ) override;
	void LoadDirect( int textureWidth, int textureHeight,
					 TextureType textureType, uint16_t textureFlags, byte* data ) override;

	void SetName( const char* newName )
	{
		name = newName;
	}

private:
	void DetermineTextureRepeat();
	void DetermineTextureFilter();

	uint32_t textureHandle{ 0 };
	bool loaded{ false };
};
