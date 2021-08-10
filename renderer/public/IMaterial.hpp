#pragma once

enum TextureType : uint8_t
{
    // RGB -> RGB
    // Alpha -> opacity
    TextureType_Albedo,
    
    // RGB -> XYZ, 
    // Alpha -> heightmap
    TextureType_Normal, 
    
    // Red -> roughness, 
    // green -> metallic, 
    // blue -> baked AO
    TextureType_Physical,

    // RGB -> RGB
    TextureType_Lightmap,
};

enum TextureFlags : uint16_t
{
    TextureFlag_Static = 1 << 0,
    // For render targets
    TextureFlag_Dynamic = 1 << 1,
    // Linear filtering
    TextureFlag_Linear = 1 << 2,
    // Nearest filtering
    TextureFlag_Nearest = 1 << 3,
    // No mipmapping
    TextureFlag_NoMip = 1 << 4,
    // Normal repeating
    TextureFlag_Repeat = 1 << 5,
    // Mirror repeating
    TextureFlag_RepeatMirror = 1 << 6,
    // Clamp
    TextureFlag_RepeatClampToEdge = 1 << 7,
    // Greyscale
    TextureFlag_Greyscale = 1 << 8,
    // RGB
    TextureFlag_RGB = 1 << 9,
    // RGBA
    TextureFlag_RGBA = 1 << 10,
    // 8 bits per channel
    TextureFlag_ByteSized = 1 << 11,
    // 32 bits per channel
    TextureFlag_FloatSized = 1 << 12,
};

constexpr uint16_t DefaultTextureFlags = 
    TextureFlag_Static | 
    TextureFlag_Linear | 
    TextureFlag_Repeat | 
    TextureFlag_RGB | 
    TextureFlag_ByteSized;

// The render backend may implement things relevant to this
// e.g. a GL texture number
class ITexture
{
public:
    // Binds the texture to be used by the shader
    virtual void Bind( uint8_t textureUnit ) = 0;

    // Loads the texture from a file, autodetermines width, height etc.
    virtual bool LoadFromFile( const char* path ) = 0;

    // Loads the texture directly from raw data
    virtual void LoadDirect( int textureWidth, int textureHeight, TextureType textureType, uint16_t textureFlags, byte* data ) = 0;

    // @returns The full path to the texture, or a custom name if this texture is generated
    virtual const char* GetName() const = 0;

    // @returns The associated texture type
    virtual TextureType GetTextureType() const = 0;
    // Sets the texture type
    virtual void SetTextureType( const TextureType& newType ) = 0;

    // @returns The associated texture flags
    virtual uint16_t GetTextureFlags() const = 0;
    // Sets the texture flags
    virtual void SetTextureFlags( const uint16_t& newFlags ) = 0;
};

class IShader
{
public:
    // @returns The full path to the shader file, unless it's built-in or generated
    virtual const char* GetName() const = 0;
    
    // Load the shader code
    // @param shaderPath: path to the shader
    virtual bool Load( const char* shaderPath ) = 0;
    
    // Build the shader
    // @returns true on success, false if there was an error
    virtual bool Compile() = 0;
    
    // Reloads the shader
    virtual void Reload() = 0;
    
    // Binds the shader to be used for rendering
    virtual void Bind() = 0;
    
    // @returns The error message, in case there was one while compiling
    virtual const char* GetErrorMessage() const = 0;

    /*
    // Ideas for instanced rendering & skinned rendering 

    // @returns Whether or not the shader can be used for instanced rendering
    // This is true if the shader script contains #instanced
    virtual bool CanInstance() const = 0;
    
    // Bind the shader for instanced rendering
    virtual void BindInstanced() = 0;
    
    // @returns If the shader can be used for skinned meshes
    virtual bool CanSkin() const = 0;
    */

    virtual uint32_t GetUniformHandle( const char* uniformName ) const = 0;
    virtual void SetUniform1i( const uint32_t& uniformHandle, const int& value ) = 0;
    virtual void SetUniform1f( const uint32_t& uniformHandle, const float& value ) = 0;
    virtual void SetUniform2f( const uint32_t& uniformHandle, const float& x, const float& y ) = 0;
    virtual void SetUniform3f( const uint32_t& uniformHandle, const float& x, const float& y, const float& z ) = 0;
    virtual void SetUniform3fv( const uint32_t& uniformHandle, const glm::vec3& v ) = 0;
    virtual void SetUniformmat3( const uint32_t& uniformHandle, const glm::mat3& m ) = 0;
    virtual void SetUniformmat4( const uint32_t& uniformHandle, const glm::mat4& m ) = 0;

    virtual void SetProjectionMatrix( const glm::mat4& m ) = 0;
    virtual void SetModelMatrix( const glm::mat4& m ) = 0;
    virtual void SetViewMatrix( const glm::mat4& m ) = 0;
};

// TODO for materials:
// - transparency settings
// - tinting
// - shadowing options
// - fog settings
// - "shader expectations"
// - material parsing

// =====================================================================
// IMaterial - the material interface
// Materials in FoxGLBox are very simple. A material can reference only
// a single shader, but it can bundle multiple textures and send them
// to said shader.
// =====================================================================
class IMaterial
{
public:
    // @returns The full name of the material
    virtual const char* GetName() const = 0;

    // Sets the full name of the material
    virtual void SetName( const char* newName ) = 0;

    // Get the material's shader
    virtual IShader* GetShader() const = 0;
    
    // Changes this material's shader
    virtual void SetShader( IShader* newShader ) = 0;

    // Returns a texture associated with this material
    // @param type: The type of the texture (TextureType_Albedo etc.)
    // @param order: In case there are several textures of the same type,
    // order retrieves the nth texture of that type
    // @returns: A texture reference if it can be found
    virtual ITexture* GetTexture( TextureType type, int order = 0 ) const = 0;

    // Adds a texture reference to this material.
    // If the same texture exists, then it isn't added.
    // @param texture: The texture to be added
    // @param force: Adds the texture anyway, even if it's already in there
    virtual void AddTexture( ITexture* texture, bool force = false ) = 0;
};

/*
Copyright © 2021 Admer456

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the “Software”), to deal in the Software without 
restriction, including without limitation the rights to use, copy, modify, merge, publish, 
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
