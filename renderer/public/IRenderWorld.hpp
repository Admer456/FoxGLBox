#pragma once

#include <stdint.h>
#include <array>
#include <vector>
#include <string>

#ifndef byte
using byte = uint8_t;
#endif

using RenderEntityHandle = unsigned int;
using RenderModelHandle = unsigned int;
constexpr RenderEntityHandle RenderHandleInvalid = ~0;

// Includes for basic stuff
#include "glm/glm.hpp"
#include "IMaterial.hpp"
#include "DrawGeometry.hpp"
#include "RenderEntityParams.hpp"
#include "RenderModelParams.hpp"
#include "RenderView.hpp"

// It is important to differentiate several concepts:
// Render entity        -> an object in the renderworld that gets drawn by its parameters and model
// Render entity params -> a bunch of properties that can be controlled by the end user, to affect the render entity
// Render entity handle -> a handle to a render entity, exposed to the end user
// Render model         -> an object that is referred to by a render entity, via a handle, to determine its appearance, exposed
// Render model handle  -> a handle to a render model, also exposed

// Also, on the higher level:
// Render system        -> what brings us the renderer frontend
// Render world         -> the renderer frontend, exposed to the user
// Renderer             -> the renderer backend, the user can only choose which one to use

struct RenderInitParams
{
    static constexpr size_t OpenGLRange = 0U;
    static constexpr size_t VulkanRange = 100U;
    static constexpr size_t Direct3DRange = 200U;
    static constexpr size_t SoftwareRange = 400U;

    enum
    {   // OpenGL backends
        Renderer_OpenGL21 = OpenGLRange,
        Renderer_OpenGL33,
        Renderer_OpenGL45,
        // Vulkan backends
        Renderer_Vulkan = VulkanRange,
        // Direct3D backends
        Renderer_Direct3D9 = Direct3DRange,
        Renderer_Direct3D11,
        // Software renderer backends
        Renderer_SoftwareGeneric = SoftwareRange
    };

    enum
    {
        Windowing_SDL2 = 0,
        Windowing_GLFW,
        Windowing_Custom
    };

    int windowWidth{ 1280 };
    int windowHeight{ 720 };
    int renderBackend{ Renderer_OpenGL45 };
    int windowingFramework{ Windowing_SDL2 };
    void* context{ nullptr }; // e.g. SDL2 OpenGL context
};

class IRenderWorld
{
public:
    virtual bool                Init( const RenderInitParams& params ) = 0;
    virtual void                Shutdown() = 0;
    // Returns the renderer API name, e.g. OpenGL 4.5 or DirectX 11
    virtual const char*         GetAPIName() const = 0;
    // Is this renderer using the GPU or the CPU?
    virtual bool                IsHardware() const = 0;

    // ========================================
    // Render entity manipulation

    // Allocates a render entity and retrieves a handle
    // Once created, this entity will be rendered in the renderworld
    virtual RenderEntityHandle  CreateEntity( const RenderEntityParams& params ) = 0;
    // Updates the render entity
    // @returns true on success, false if the handle is invalid, or the params are invalid
    virtual bool                UpdateEntity( const RenderEntityHandle& handle, const RenderEntityParams& params ) = 0;
    // Submits the entity to the renderworld immediately, to be drawn this frame
    // @returns true on success, false if the params are invalid
    virtual bool                CreateImmediateEntity( const RenderEntityParams& params ) = 0;
    // Frees the render entity from the renderworld, no longer to be rendered again
    // If you wish to just hide entities, update render entity params instead
    virtual void                DestroyEntity( const RenderEntityHandle& handle ) = 0;

    // ========================================
    // Render model manipulation

    // Creates a model from given parameters
    virtual RenderModelHandle   CreateModel( const RenderModelParams& params ) = 0;
    // Updates a model dynamically, only for dynamic models
    virtual void                UpdateModel( const RenderModelHandle& handle, const RenderModelParams& params ) = 0;

    // ========================================
    // Material, texture and shader business

    // Creates a simple material from a diffuse texture
    virtual IMaterial*          CreateMaterialSimple( ITexture* diffuseImage ) = 0;
    // Gets an existing material
    // If the name ends with a file extension like .png, it'll assume
    // that's an albedo map and create a new material based on it
    virtual IMaterial*          LoadMaterial( const char* materialName ) = 0;
    // Reloads all materials
    virtual void                ReloadMaterials() = 0;

    // Loads a texture
    virtual ITexture*           LoadTexture( const char* path, TextureType type = TextureType_Albedo, uint16_t flags = DefaultTextureFlags ) = 0;
    // Creates a custom texture
    virtual ITexture*           CreateTexture( const char* name, int width = 64, int height = 64, 
                                               TextureType type = TextureType_Albedo, 
                                               uint16_t flags   = DefaultTextureFlags, byte* data = nullptr ) = 0;
    // Updates a texture
    virtual void                UpdateTexture( ITexture* texture, byte* data ) = 0;

    // Loads and compiles a shader
    virtual IShader*            LoadShader( const char* path ) = 0;
    // Reloads all shaders
    virtual void                ReloadShaders() = 0;

    // ========================================
    // View rendering

    // Renders the view into a frame
    virtual void                RenderFrame( const RenderView& view ) = 0;
};

class IRenderSystem
{
public:
    virtual IRenderWorld* InitRenderer( const RenderInitParams& params ) = 0;
};

// API import stuff
// If FGL_DYNAMIC_LIBRARY is defined, then load the foxglbox DLL, 
// find the GetAPI function, and import to your application whatever
// it exports, in this case, the render system
namespace foxglbox
{
#ifdef FGL_DYNAMIC_LIBRARY
    // What the library exports
    struct Exports
    {
        IRenderSystem* system;
    };

    using FunctionType = Exports* (void);
    constexpr const char* FunctionName = "GetAPI";
#else
    IRenderSystem* GetRenderSystem();
#endif
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
