#include "IRenderWorld.hpp"
#include "Model.hpp"
#include "Material.hpp"
#include "RenderWorld.hpp"
#include "IRenderer.hpp"
#include "stb_image.h"

// =====================================================================
// RenderWorld::Init
// =====================================================================
bool RenderWorld::Init( const RenderInitParams& params )
{
    backend = GetRendererByBackend( params.renderBackend );
    if ( nullptr == backend )
    {
        Shutdown();
        return false;
    }

    if ( !backend->Init( params ) )
    {
        Shutdown();
        return false;
    }

    backend->Clear();
    return true;
}

// =====================================================================
// RenderWorld::Shutdown
// =====================================================================
void RenderWorld::Shutdown()
{
    shaders.clear();
    textures.clear();
    materials.clear();

    if ( nullptr != backend )
    {
        backend->Shutdown();

        delete backend;
        backend = nullptr;
    }
}

// =====================================================================
// RenderWorld::GetAPIName
// =====================================================================
const char* RenderWorld::GetAPIName() const
{
    return backend->GetAPIName();
}

// =====================================================================
// RenderWorld::GetAPIName
// =====================================================================
bool RenderWorld::IsHardware() const
{
    return backend->IsHardware();
}

// =====================================================================
// RenderWorld::CreateEntity
// =====================================================================
RenderEntityHandle RenderWorld::CreateEntity( const RenderEntityParams& params )
{
    RenderEntityHandle i = 0U;
    for ( auto& ent : entities )
    {
        if ( !ent.active )
        {
            ent.re = RenderEntity{ params };
            ent.active = true;
            return i;
        }

        i++;
    }

    return RenderHandleInvalid;
}

// =====================================================================
// RenderWorld::UpdateEntity
// =====================================================================
bool RenderWorld::UpdateEntity( const RenderEntityHandle& handle, const RenderEntityParams& params )
{
    if ( handle == RenderHandleInvalid )
    {
        return false;
    }

    if ( handle < 0 || handle >= entities.size() )
    {
        return false;
    }

    if ( !entities.at( handle ).active || entities.at( handle ).temporary )
    {
        return false;
    }

    entities.at( handle ).re.params = params;
    return true;
}

// =====================================================================
// RenderWorld::CreateImmediateEntity
// =====================================================================
bool RenderWorld::CreateImmediateEntity( const RenderEntityParams& params )
{
    RenderModelHandle handle = CreateEntity( params );

    if ( handle == RenderHandleInvalid )
    {
        return false;
    }

    entities.at( handle ).temporary = true;
    return true;
}

// =====================================================================
// RenderWorld::DestroyEntity
// =====================================================================
void RenderWorld::DestroyEntity( const RenderEntityHandle& handle )
{
    entities.at( handle ).active = false;
    entities.at( handle ).temporary = false;
}

// =====================================================================
// RenderWorld::CreateModel
// =====================================================================
RenderModelHandle RenderWorld::CreateModel( const RenderModelParams& params )
{
    // Check if we got existing ones
    RenderModelHandle handle = 0;
    for ( handle = 0; handle < models.size(); handle++ )
    {
        Model& model = models[handle];
        if ( model.name == params.modelPath )
        {
            return handle;
        }
    }

    // Create a new model
    models.push_back( Model() );
    Model& model = models.back();
    model.LoadFromPath( params.modelPath );

    if ( model.Okay() )
    {
        for ( auto& surf : model.mesh.surfaces )
        {
            // Hardcoded texture paths for now...
            ITexture* defaultTexture = LoadTexture( "metal1.png", TextureType_Albedo, DefaultTextureFlags );
            surf.material = CreateMaterialSimple( defaultTexture );
        }

        backend->CreateModel( params, &model.mesh );
        return handle;
    }

    models.pop_back();
    return RenderHandleInvalid;
}

// =====================================================================
// RenderWorld::UpdateModel
// =====================================================================
void RenderWorld::UpdateModel( const RenderModelHandle& handle, const RenderModelParams& params )
{


    return backend->UpdateModel( handle, params.mesh );
}

// =====================================================================
// RenderWorld::CreateMaterialSimple
// =====================================================================
IMaterial* RenderWorld::CreateMaterialSimple( ITexture* diffuseImage )
{
    for ( IMaterial* material : materials )
    {
        if ( !strcmp( material->GetName(), diffuseImage->GetName() ) )
        {
            return material;
        }
    }

    Material* material = new Material();
    material->SetName( diffuseImage->GetName() );
    material->SetShader( backend->GetDefaultShader() );
    material->AddTexture( diffuseImage );

    materials.push_back( material );
    return material;
}

// =====================================================================
// RenderWorld::LoadMaterial
// =====================================================================
IMaterial* RenderWorld::LoadMaterial( const char* materialName )
{
    return nullptr;
}

// =====================================================================
// RenderWorld::ReloadMaterials
// =====================================================================
void RenderWorld::ReloadMaterials()
{

}

// =====================================================================
// RenderWorld::LoadTexture
// =====================================================================
ITexture* RenderWorld::LoadTexture( const char* path, TextureType type, uint16_t flags )
{
    for ( auto& texture : textures )
    {
        if ( !strcmp( texture->GetName(), path ) )
        {
            return texture;
        }
    }

    ITexture* texture = backend->AllocateTexture( path );
    texture->SetTextureType( type );
    texture->SetTextureFlags( flags );

    if ( !texture->LoadFromFile( path ) )
    {
        delete texture;
        return nullptr;
    }

    textures.push_back( texture );
    return texture;
}

// =====================================================================
// RenderWorld::CreateTexture
// =====================================================================
ITexture* RenderWorld::CreateTexture( const char* name, int width, int height,
                         TextureType type, uint16_t flags, byte* data )
{
    for ( auto& texture : textures )
    {
        if ( !strcmp( texture->GetName(), name ) )
        {
            return texture;
        }
    }

    ITexture* texture = backend->AllocateTexture( name );
    texture->SetTextureType( type );
    texture->SetTextureFlags( flags );
    texture->LoadDirect( width, height, type, flags, data );

    textures.push_back( texture );
    return texture;
}

// =====================================================================
// RenderWorld::UpdateTexture
// =====================================================================
void RenderWorld::UpdateTexture( ITexture* texture, byte* data )
{
    
}

// =====================================================================
// RenderWorld::LoadShader
// =====================================================================
IShader* RenderWorld::LoadShader( const char* path )
{
    return nullptr;
}

// =====================================================================
// RenderWorld::RenderFrame
// =====================================================================
void RenderWorld::ReloadShaders()
{
    for ( IShader* shader : shaders )
    {
        shader->Reload();
        printf( "%s\n",
            std::string( "RenderWorld::ReloadShaders: reloaded shader '" )
            .append( shader->GetName() )
            .append( "'" )
            .c_str() );
    }

    backend->ReloadShaders();
}

// =====================================================================
// RenderWorld::RenderFrame
// =====================================================================
void RenderWorld::RenderFrame( const RenderView& view )
{
    backend->Clear();
    backend->BeginFrame();
    backend->SetRenderView( &view );

    for ( auto& e : entities )
    {
        if ( e.active )
        {
            // Render all surfaces of the render entity's model
            int numSurfaces = GetNumSurfacesForModel( e.re.params.model );
            if ( numSurfaces == RenderHandleInvalid )
            {
                continue;
            }

            for ( int i = 0; i < numSurfaces; i++ )
            {
                backend->RenderSurface( e.re.params, i );
            }

            // If this entity was temporary, then deactivate it
            if ( e.temporary )
            {
                e.active = false;
                e.temporary = false;
            }
        }
    }

    backend->EndFrame();
}

// =====================================================================
// RenderWorld::RenderSurface
// =====================================================================
uint32_t RenderWorld::GetNumSurfacesForModel( const RenderModelHandle& handle )
{
    if ( handle == RenderHandleInvalid || handle >= models.size() )
    {
        return RenderHandleInvalid;
    }

    return models.at( handle ).mesh.surfaces.size();
}

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
