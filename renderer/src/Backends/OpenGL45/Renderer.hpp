#pragma once

class Model;
class VertexBuffer;

// =====================================================================
// OpenGL 4.5 render backend
// =====================================================================
class Renderer_OpenGL45 : public IRenderer
{
public:
    // Initialises the renderer
    bool                Init( const RenderInitParams& params ) override;
    // Shuts down the renderer and frees all resources
    void                Shutdown() override;
    // Returns the renderer API name, e.g. OpenGL 4.5 or DirectX 11
    const char*         GetAPIName() const override;
    // Is this renderer using the GPU or the CPU?
    bool                IsHardware() const override;

    void                BeginFrame() override;
    void                EndFrame() override;

    // Creates & compiles a shader from a shader file
    IShader*            CreateShader( const char* shaderFile ) override;
    // Gets the default shader
    IShader*            GetDefaultShader() override
    { return &defaultShader; }
    // Reloads all shaders
    void                ReloadShaders() override;

    // Renders a DrawSurface, is equivalent to 1 drawcall
    // @param params: render entity parameters, to position and rotate the model in space, among other things
    // @param surface: surface ID, must not be bigger than the number of surfaces in a model
    void                RenderSurface( const RenderEntityParams& params, const int& surface ) override;

    // Renders multiple DrawSurface instances, is still equivalent to 1 drawcall (depending on the backend)
    // @param params: render entity parameters, to position and rotate the model in space, among other things
    // @param surface: surface ID, must not be bigger than the number of surfaces in a model
    // @param batchParams: batch-specific parameters for each instance
    // @param batchSize: how many instances to draw
    void                RenderSurfaceBatch( const RenderModelHandle& params, const int& surface,
                                            const RenderBatchParams* batchParams, const int& batchSize ) override;

    // Set the render view, update the viewport etc.
    void                SetRenderView( const RenderView* view ) override;
    // Get the current render view
    const RenderView*   GetRenderView() override;

    // Clears the screen
    void                Clear() override;
    // Copies the framebuffer into a texture
    void                CopyFrameToTexture( ITexture* texture ) override;

    // Creates a model from given parameters
    RenderModelHandle   CreateModel( const RenderModelParams& params, const DrawMesh* model ) override;
    // Updates a model, only for dynamic models
    void                UpdateModel( const RenderModelHandle& handle, const DrawMesh* model ) override;

    // Creates a texture from given data
    ITexture*           AllocateTexture( const char* name ) override;
    // Updates a texture with new data
    void                UpdateTexture( ITexture* texture, byte* data ) override;

private:
    bool                InitDefaultShader();
    void                BindDefaultShader();

    void                SetupMatrices( const RenderEntityParams& params, IShader* shader );
    void                PerformDrawCall( VertexArray& va, const RenderBatchParams* batchParams = nullptr, 
                                         const uint32_t& batchSize = 0 );

private:
    using VertexArrayGroup = std::vector<VertexArray>;

    std::vector<VertexArrayGroup> vertexArrays;
    std::vector<VertexBuffer> vertexBuffers;

    RenderView          currentView;

    Shader              defaultShader;

private: // Statistics
    uint32_t            numDrawCalls;
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
