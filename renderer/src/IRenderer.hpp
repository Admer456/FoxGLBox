#pragma once

#include "Backends/BackendRegistry.hpp"

class IRenderer
{
public:
    // Initialises the renderer
    virtual bool                Init( const RenderInitParams& params ) = 0;
    // Shuts down the renderer and frees all resources
    virtual void                Shutdown() = 0;
    // Returns the renderer API name, e.g. OpenGL 4.5 or DirectX 11
    virtual const char*         GetAPIName() const = 0;
    // Is this renderer using the GPU or the CPU?
    virtual bool                IsHardware() const = 0;

    // Clears some buffers, resets the far/near ranges etc.
    virtual void                BeginFrame() = 0;
    // Unbinds things and sets others up for the next frame
    virtual void                EndFrame() = 0;

    // Creates & compiles a shader from a shader file
    virtual IShader*            CreateShader( const char* shaderFile ) = 0;
    // Gets the default shader
    virtual IShader*            GetDefaultShader() = 0;
    // Reloads all shaders
    virtual void                ReloadShaders() = 0;

    // Renders multiple DrawSurface instances, is still equivalent to 1 drawcall (depending on the backend)
    // @param params: render entity parameters, to position and rotate the model in space, among other things
    // @param surface: surface ID, must not be bigger than the number of surfaces in a model
    // @param batchHandle: handle to the backend batch object; draws single instance if invalid
    // @param batchSize: how many instances to draw; draws single instances if 0 or 1
    virtual void                RenderSurfaceBatch( const RenderEntityParams& params, const int& surface,
                                                    const BatchHandle& batchHandle, const int& batchSize ) = 0;

    // Set the render view, update the viewport etc.
    virtual void                SetRenderView( const RenderView* view ) = 0;
    // Get the current render view
    virtual const RenderView*   GetRenderView() = 0;

    // Clears the screen
    virtual void                Clear() = 0;
    // Copies the framebuffer into a texture
    virtual void                CopyFrameToTexture( ITexture* texture ) = 0;

    // Creates a model from given parameters
    virtual RenderModelHandle   CreateModel( const RenderModelParams& params, const DrawMesh* model ) = 0;
    // Updates a model, only for dynamic models
    virtual void                UpdateModel( const RenderModelHandle& handle, const DrawMesh* model ) = 0;

    // Creates a texture from given data
    virtual ITexture*           AllocateTexture( const char* name ) = 0;
    // Updates a texture with new data
    virtual void                UpdateTexture( ITexture* texture, byte* data ) = 0;

    // Registers a render batch so a render entity can be rendered in multiple instances
    virtual BatchHandle         CreateBatch( RenderBatchParam* params, const int& batchSize ) = 0;
    // Updates data for this render batch
    virtual void                UpdateBatch( const BatchHandle& handle, RenderBatchParam* params, const int& batchSize ) = 0;
    // Checks if the handle and the batch at the handle are valid
    virtual bool                IsBatchValid( const BatchHandle& handle ) = 0;

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
