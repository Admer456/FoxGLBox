#pragma once

class IRenderWorld;
class IRenderer;

#include "RenderEntity.hpp"
#include <array>
#include <vector>
#include <unordered_map>

class RenderWorld : public IRenderWorld
{
public:
    bool                    Init( const RenderInitParams& params ) override;
    void                    Shutdown() override;
    // Returns the renderer API name, e.g. OpenGL 4.5 or DirectX 11
    const char*             GetAPIName() const override;
    // Is this renderer using the GPU or the CPU?
    bool                    IsHardware() const override;
    
    // ========================================
    // Render entity manipulation

    // Allocates a render entity and retrieves a handle
    // Once created, this entity will be rendered in the renderworld
    RenderEntityHandle      CreateEntity( const RenderEntityParams& params ) override;
    // Updates the render entity
    // @returns true on success, false if the handle is invalid, or the params are invalid
    bool                    UpdateEntity( const RenderEntityHandle& handle, const RenderEntityParams& params ) override;
    // Submits the entity to the renderworld immediately, to be drawn this frame
    // @returns true on success, false if the params are invalid
    bool                    CreateImmediateEntity( const RenderEntityParams& params ) override;
    // Frees the render entity from the renderworld, no longer to be rendered again
    // If you wish to just hide entities, update render entity params instead
    void                    DestroyEntity( const RenderEntityHandle& handle ) override;

    // ========================================
    // Render model manipulation

    // Creates a model from given parameters
    RenderModelHandle       CreateModel( const RenderModelParams& params ) override;
    // Updates a model dynamically, only for dynamic models
    void                    UpdateModel( const RenderModelHandle& handle, const RenderModelParams& params ) override;

    // ========================================
    // Material, texture and shader business

    // Creates a simple material with a single diffuse texture
    IMaterial*              CreateMaterialSimple( ITexture* diffuseImage ) override;
    // Gets an existing material
    // If the name ends with a file extension like .png, it'll assume
    // that's an albedo map and create a new material based on it
    IMaterial*              LoadMaterial( const char* materialName ) override;
    // Reloads all materials
    void                    ReloadMaterials() override;
    
    // Loads a texture
    ITexture*               LoadTexture( const char* path, TextureType type, uint16_t flags ) override;
    // Creates a custom texture
    ITexture*               CreateTexture( const char* name, int width, int height, 
                                       TextureType type, uint16_t flags, byte* data ) override;
    // Updates a texture
    void                    UpdateTexture( ITexture* texture, byte* data ) override;

    // Loads and compiles a shader
    IShader*                LoadShader( const char* path ) override;
    // Reloads all shaders
    void                    ReloadShaders() override;

    // ========================================
    // View rendering

    // Renders the view into a frame
    void                    RenderFrame( const RenderView& view ) override;

    // ========================================
    // Utilities

    // Calculates a model matrix from the given parameters
    // Useful to calculate model matrices for render batching
    glm::mat4               CalculateModelMatrix( const glm::vec3& position, const glm::mat4& orientation ) override;

private:
    // @param handle: a valid handle to a model
    // @returns the number of surfaces a model has
    uint32_t                GetNumSurfacesForModel( const RenderModelHandle& handle );
    // Utility for obtaining the batchID from the render backend
    // @returns: BatchInvalid if there's no batch data; a valid batchID otherwise
    BatchHandle             GetBatchIndex( const RenderEntityParams& params );
private:
    using                   BatchMap = std::unordered_map<const RenderEntityParams*, BatchHandle>;
    struct                  RenderEntitySlot
    {
        RenderEntity    re;
        bool            active{ false };
        bool            temporary{ false };
    };

    IRenderer*              backend{ nullptr };
    std::array<RenderEntitySlot, 16384U> entities;

    std::vector<Model>      models;
    std::vector<IShader*>   shaders;
    std::vector<ITexture*>  textures;
    std::vector<IMaterial*> materials;
    BatchMap                batches;

    static constexpr size_t EntityArraySize = sizeof( entities );
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
