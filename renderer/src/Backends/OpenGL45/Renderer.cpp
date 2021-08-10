#include "IRenderWorld.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "FrontendTexture.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "IRenderer.hpp"

#define GLEW_STATIC 1
#include <GL/glew.h>
#include "VertexBuffer.hpp"

#include "Renderer.hpp"

#include "glm/gtc/matrix_transform.hpp"

IRenderer* AllocateRenderer45()
{
	return new Renderer_OpenGL45();
}

bool CanErrorPrint = true;

#define glErrorInfo( glEnum ) { glEnum, #glEnum },
const char* glTranslateError( GLenum error )
{
	struct GLErrorInfo { int glEnum; const char* glString; };
	constexpr GLErrorInfo errorEnumStrings[] =
	{
		glErrorInfo( GL_INVALID_ENUM )
		glErrorInfo( GL_INVALID_FRAMEBUFFER_OPERATION )
		glErrorInfo( GL_INVALID_INDEX )
		glErrorInfo( GL_INVALID_VALUE )
		glErrorInfo( GL_INVALID_OPERATION )
		glErrorInfo( GL_STACK_OVERFLOW )
		glErrorInfo( GL_STACK_UNDERFLOW )
		glErrorInfo( GL_OUT_OF_MEMORY )
	};

	for ( const auto& errorInfo : errorEnumStrings )
	{
		if ( error == errorInfo.glEnum )
			return errorInfo.glString;
	}

	return (const char*)glewGetErrorString( error );
}
#undef glErrorInfo

bool GLError( const char* why = nullptr )
{
	if ( !CanErrorPrint )
		return false;

	printf( "OpenGL: %s", why );

	auto error = glGetError();
	if ( error != GL_NO_ERROR )
	{
		printf( "\ngot an error: 0x%x | %i (%s)\n_________________________\n", 
				error, error, glTranslateError( error ) );
		return true;
	}

	printf( ", went OK\n" );

	return false;
}

// =====================================================================
// Renderer_OpenGL45::Init
// =====================================================================
bool Renderer_OpenGL45::Init( const RenderInitParams& params )
{	
	// Init GLEW
	if ( glewInit() != GLEW_OK )
	{
		Shutdown();
		return false;
	}

	// Build the default shader
	if ( !InitDefaultShader() )
	{
		Shutdown();
		return false;
	}

	GLError( "built the default shader" );

	vertexBuffers.clear();

	return true;
}

// =====================================================================
// Renderer_OpenGL45::Shutdown
// =====================================================================
void Renderer_OpenGL45::Shutdown()
{
	vertexArrays.clear();
	vertexBuffers.clear();
}

// =====================================================================
// Renderer_OpenGL45::GetAPIName
// =====================================================================
const char* Renderer_OpenGL45::GetAPIName() const
{
	return "OpenGL 4.5";
}

// =====================================================================
// Renderer_OpenGL45::IsHardware
// =====================================================================
bool Renderer_OpenGL45::IsHardware() const
{
	return true;
}

// =====================================================================
// Renderer_OpenGL45::BeginFrame
// =====================================================================
void Renderer_OpenGL45::BeginFrame()
{
	numDrawCalls = 0;
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glDepthRange( 0.01f, 8192.0f );
}

// =====================================================================
// Renderer_OpenGL45::EndFrame
// =====================================================================
void Renderer_OpenGL45::EndFrame()
{
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glUseProgram( 0 );
}

// =====================================================================
// Renderer_OpenGL45::CreateShader
// =====================================================================
IShader* Renderer_OpenGL45::CreateShader( const char* shaderFile )
{
	Shader* newShader = new Shader();
	if ( newShader->Load( shaderFile ) )
	{
		newShader->Compile();
		GLError( std::string( "CreateShader: attempted to compile shader " ).append( shaderFile ).c_str() );
		return newShader;
	}

	delete newShader;
	return nullptr;
}

// =====================================================================
// Renderer_OpenGL45::ReloadShaders
// =====================================================================
void Renderer_OpenGL45::ReloadShaders()
{
	defaultShader.Reload();
	GLError( "ReloadShaders: reloaded default shader" );
}

// =====================================================================
// Renderer_OpenGL45::RenderSurface
// =====================================================================
void Renderer_OpenGL45::RenderSurface( const RenderEntityParams& params, const int& surface )
{	
	CanErrorPrint = false;
	
	// Get the render data stuff
	VertexArray& va = vertexArrays[params.model].at( surface );
	ITexture* tex = va.GetMaterial()->GetTexture( TextureType_Albedo, 0 );
	IShader* shader = va.GetMaterial()->GetShader();

	// Bind the shader
	shader->Bind();

	// -------------------------------------
	// TODO:
	// Move this stuff to the Material class
	tex->Bind( 0 );

	static uint32_t textureUniformHandle = 0;
	if ( !textureUniformHandle )
	{
		textureUniformHandle = shader->GetUniformHandle( "albedoMap" );
	}
	shader->SetUniform1i( textureUniformHandle, 0 );
	// -------------------------------------

	// Set up the matrices
	SetupMatrices( params, shader );

	// GPU, RENDER NOW!
	PerformDrawCall( va );

	CanErrorPrint = true;
}

// =====================================================================
// Renderer_OpenGL45::RenderSurfaceBatch
// =====================================================================
void Renderer_OpenGL45::RenderSurfaceBatch( const RenderModelHandle& params, const int& surface, 
											const RenderBatchParams* batchParams, const int& batchSize )
{

}

// =====================================================================
// Renderer_OpenGL45::SetRenderView
// =====================================================================
void Renderer_OpenGL45::SetRenderView( const RenderView* view )
{
	currentView = *view;
}

// =====================================================================
// Renderer_OpenGL45::GetRenderView
// =====================================================================
const RenderView* Renderer_OpenGL45::GetRenderView()
{
	return &currentView;
}

// =====================================================================
// Renderer_OpenGL45::Clear
// =====================================================================
void Renderer_OpenGL45::Clear()
{
	glClearColor( 0.0f, 0.0f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

// =====================================================================
// Renderer_OpenGL45::CopyFrameToTexture
// =====================================================================
void Renderer_OpenGL45::CopyFrameToTexture( ITexture* texture )
{

}

// =====================================================================
// Renderer_OpenGL45::CreateModel
// =====================================================================
RenderModelHandle Renderer_OpenGL45::CreateModel( const RenderModelParams& params, const DrawMesh* model )
{
	// Generate a vertex buffer from the mesh
	vertexBuffers.push_back( VertexBuffer() );
	VertexBuffer& vb = vertexBuffers.back();
	
	// Step 1: populate the vertex buffer with data
	vb.Init( model );
	GLError( "generated a vertex buffer" );

	// Step 2: for every surface in the model, create a vertex array
	// with this vertex buffer
	vertexArrays.push_back( VertexArrayGroup() );
	VertexArrayGroup& vag = vertexArrays.back();
	for ( const DrawSurface& surface : model->surfaces )
	{
		vag.push_back( VertexArray( &vb, &surface ) );
		GLError( "generated a vertex array" );
	}

	return RenderHandleInvalid;
}

// =====================================================================
// Renderer_OpenGL45::UpdateModel
// =====================================================================
void Renderer_OpenGL45::UpdateModel( const RenderModelHandle& handle, const DrawMesh* model )
{

}

// =====================================================================
// Renderer_OpenGL45::CreateTexture
// =====================================================================
ITexture* Renderer_OpenGL45::AllocateTexture( const char* name )
{
	Texture* texture = new Texture();
	texture->Init();
	texture->SetName( name );
	return texture;
}

// =====================================================================
// Renderer_OpenGL45::UpdateTexture
// =====================================================================
void Renderer_OpenGL45::UpdateTexture( ITexture* texture, byte* data )
{

}

// =====================================================================
// Renderer_OpenGL45::InitDefaultShader
// =====================================================================
bool Renderer_OpenGL45::InitDefaultShader()
{	// Load the shader text
	if ( !defaultShader.Load( "shaders/default.glsl" ) )
	{
		printf( "Error in filesystem: %s\n", defaultShader.GetErrorMessage() );
		return false;
	}
	// Compile the shader
	if ( !defaultShader.Compile() )
	{
		printf( "Error in compilation: %s\n", defaultShader.GetErrorMessage() );
		return false;
	}

	return true;
}

// =====================================================================
// Renderer_OpenGL45::BindDefaultShader
// =====================================================================
void Renderer_OpenGL45::BindDefaultShader()
{
	defaultShader.Bind();
}

// =====================================================================
// Renderer_OpenGL45::SetupMatrices
// =====================================================================
void Renderer_OpenGL45::SetupMatrices( const RenderEntityParams& params, IShader* shader )
{
	// Get the projection matrix going
	float width = currentView.viewportWidth, height = currentView.viewportHeight;
	const glm::mat4 projectionMatrix = glm::perspective(
		glm::radians( currentView.cameraFov ), // camera FOV
		width / height, // aspect ratio
		0.01f, // zMin
		8192.0f ); // zFar

	shader->SetProjectionMatrix( projectionMatrix );

	// Calculate the model matrix
	const glm::vec3& position = params.position;
	const glm::mat4& axis = params.orientation; // orientation contains angles & scale

	glm::mat4 modelMatrix = glm::translate( glm::identity<glm::mat4>(), position ); // move the model
	modelMatrix *= axis;

	// Set the model matrix
	shader->SetModelMatrix( modelMatrix );

	// Set the view matrix, ultimately
	glm::mat4 viewMatrix = currentView.cameraOrientation;
	viewMatrix = glm::translate( viewMatrix, currentView.cameraPosition );

	shader->SetViewMatrix( viewMatrix );
}

// =====================================================================
// Renderer_OpenGL45::PerformDrawCall
// =====================================================================
void Renderer_OpenGL45::PerformDrawCall( VertexArray& va, const RenderBatchParams* batchParams, const uint32_t& batchSize )
{
	va.Bind();

	if ( nullptr == batchParams )
	{
		glDrawElements( GL_TRIANGLES, va.GetNumIndices(), GL_UNSIGNED_INT, nullptr );
	}
	else
	{
		if ( batchSize > 0 )
		{
			glDrawElementsInstanced( GL_TRIANGLES, va.GetNumIndices(), GL_UNSIGNED_INT, nullptr, batchSize );
		}
	}

	numDrawCalls++;
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
