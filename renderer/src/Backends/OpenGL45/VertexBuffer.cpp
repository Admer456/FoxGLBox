#include "IRenderWorld.hpp"

#define GLEW_STATIC 1
#include <GL/glew.h>

#include "VertexBuffer.hpp"

// =====================================================================
// VertexArray::Init
// =====================================================================
void VertexArray::Init()
{
	glGenVertexArrays( 1, &vertexArrayHandle );
	GLError( "VertexArray::Init: generated a VAO" );

	glGenBuffers( 1, &elementBufferHandle );
	GLError( "VertexArray::Init: generated an EBO" );

	Bind();
	GLError( "VertexArray::Init: bound the VAO" );

	vertexBuffer->Bind();
	GLError( "VertexArray::Init: bound the VBO" );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle );
	GLError( "VertexArray::Init: bound the EBO" );
}

// =====================================================================
// VertexArray::Bind
// =====================================================================
void VertexArray::Bind( bool arrayOnly ) const
{
	glBindVertexArray( vertexArrayHandle );
	GLError( "VertexArray::Bind: bound the VAO" );

	if ( !arrayOnly )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle );
		GLError( "VertexArray::Bind: bound the EBO" );
	}
}

// =====================================================================
// VertexArray::SetupIndices
// =====================================================================
void VertexArray::BufferData( int type )
{
	Bind( false );
	GLError( "VertexArray::BufferData: bound the VAO and EBO" );

	vertexBuffer->Bind();
	GLError( "VertexArray::BufferData: bound the VBO" );

	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( vertexid_t ), indices.data(), type );
	GLError( "VertexArray::BufferData: buffered GL_ELEMENT_ARRAY_BUFFER with indices.data()" );
}

// =====================================================================
// VertexArray::SetupIndices
// =====================================================================
void VertexArray::SetupIndices( const DrawSurface* surf )
{
	indices = surf->vertexIndices;
}

// =====================================================================
// VertexArray::SetupVertexAttributes
// =====================================================================
void VertexArray::SetupVertexAttributes() const
{
	using attribs = VertexAttributes;
	using offsets = VertexAttribOffsets;

	SetupSingleVertexAttrib( attribs::Positions, 3, offsets::Positions );
	GLError( "VertexArray::SetupVertexAttributes: set up Positions" );
	SetupSingleVertexAttrib( attribs::Normals,   3, offsets::Normals );
	GLError( "VertexArray::SetupVertexAttributes: set up Normals" );
	SetupSingleVertexAttrib( attribs::TexCoords, 2, offsets::TexCoords );
	GLError( "VertexArray::SetupVertexAttributes: set up TexCoords" );
}

// =====================================================================
// VertexArray::SetupSingleVertexAttrib
// =====================================================================
void VertexArray::SetupSingleVertexAttrib( const int& attribType, const int& size, const int& offset, const int& stride )
{
	glEnableVertexAttribArray( attribType );
	GLError( "VertexArray::SetupSingleVertexAttrib: called glEnableVertexAttribArray" );
	glVertexAttribPointer( attribType, size, GL_FLOAT, GL_FALSE, stride, VBOffset( offset ) );
	GLError( "VertexArray::SetupSingleVertexAttrib: called glVertexAttribPointer" );
}

// =====================================================================
// VertexArray::PrintDebugInfo
// =====================================================================
void VertexArray::PrintDebugInfo() const
{
	return;

	int info;
	glGetVertexArrayiv( vertexArrayHandle, GL_ELEMENT_ARRAY_BUFFER_BINDING, &info );

	printf( "VertexArray::PrintDebugInfo: VA's VBO handle is %i\n", info );
}

// =====================================================================
// InstancedArray::ctor
// =====================================================================
InstancedArray::InstancedArray( RenderBatchParam* params, const uint32_t& size )
	: batchParams( params ), batchSize( size )
{
	Init();
	BufferData();
}

// =====================================================================
// InstancedArray::Init
// =====================================================================
void InstancedArray::Init()
{
	glGenBuffers( 1, &instancedBufferHandle );
}

// =====================================================================
// InstancedArray::Update
// =====================================================================
void InstancedArray::Update( RenderBatchParam* params, const uint32_t& size )
{
	if ( nullptr == params || !size )
	{
		return;
	}

	batchParams = params;
	batchSize = size;

	// Bigger new batch size -> call glBufferData
	// Smaller new batch size or equal -> only call glBufferSubData cuz' cheaper
	BufferData( size > batchSize );
}

// =====================================================================
// InstancedArray::Bind
// =====================================================================
void InstancedArray::Bind()
{
	glBindBuffer( GL_ARRAY_BUFFER, instancedBufferHandle );
}

// =====================================================================
// InstancedArray::Unbind
// =====================================================================
void InstancedArray::Unbind()
{
	using attribs = VertexAttributes;

	glDisableVertexAttribArray( attribs::BatchModelMatrix + 0 );
	glDisableVertexAttribArray( attribs::BatchModelMatrix + 1 );
	glDisableVertexAttribArray( attribs::BatchModelMatrix + 2 );
	glDisableVertexAttribArray( attribs::BatchModelMatrix + 3 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

// =====================================================================
// InstancedArray::BufferData
// =====================================================================
void InstancedArray::BufferData( bool resize )
{
	if ( nullptr == batchParams )
	{
		return;
	}

	Bind();

	// Fill the IBO with transform data
	if ( resize )
	{	// GL_DYNAMIC_DRAW cuz' this can change often
		glBufferData( GL_ARRAY_BUFFER, sizeof( RenderBatchParam ) * batchSize, batchParams, GL_DYNAMIC_DRAW );
		printf( "InstancedArray::BufferData: batchSize = %i\n", (int)batchSize );
	}
	else
	{
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( RenderBatchParam ) * batchSize, batchParams );
		printf( "InstancedArray::BufferSubData: batchSize = %i\n", (int)batchSize );
	}

}

// =====================================================================
// InstancedArray::SetupVertexAttributes
// 
// Lots of things can go wrong here lol
// Assumes this IBO is already bound with a bound VAO
// =====================================================================
void InstancedArray::SetupVertexAttributes()
{
	// sizeof(vec3) + sizeof(mat4)
	constexpr int InstancedArrayStride = 16 * sizeof( float );
	using attribs = VertexAttributes;
	using offsets = VertexAttribOffsets;

	// Model matrices
	VertexArray::SetupSingleVertexAttrib( attribs::BatchModelMatrix + 0, 4, offsets::BatchModelMatrix + offsets::Vec4Size * 0, InstancedArrayStride );
	VertexArray::SetupSingleVertexAttrib( attribs::BatchModelMatrix + 1, 4, offsets::BatchModelMatrix + offsets::Vec4Size * 1, InstancedArrayStride );
	VertexArray::SetupSingleVertexAttrib( attribs::BatchModelMatrix + 2, 4, offsets::BatchModelMatrix + offsets::Vec4Size * 2, InstancedArrayStride );
	VertexArray::SetupSingleVertexAttrib( attribs::BatchModelMatrix + 3, 4, offsets::BatchModelMatrix + offsets::Vec4Size * 3, InstancedArrayStride );
	GLError( "InstancedArray::SetupVertexAttributes: set up BatchModelMatrix" );
	
	// Reset the current array buffer
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	// Update batch data every 1 instance
	glVertexAttribDivisor( attribs::BatchModelMatrix + 0, 1 );
	glVertexAttribDivisor( attribs::BatchModelMatrix + 1, 1 );
	glVertexAttribDivisor( attribs::BatchModelMatrix + 2, 1 );
	glVertexAttribDivisor( attribs::BatchModelMatrix + 3, 1 );
	GLError( "InstancedArray::SetupVertexAttributes: called glVertexAttribDivisor on BatchModelMatrix" );
}

// =====================================================================
// VertexBuffer::Init
// =====================================================================
void VertexBuffer::Init()
{
	// Generate a vertex buffer and the array buffer
	glGenBuffers( 1, &vertexBufferHandle );
}

// =====================================================================
// VertexBuffer::Init
// =====================================================================
void VertexBuffer::Init( const DrawMesh* mesh )
{
	data.clear();

	// Fill our local vertex buffer
	for ( const DrawVertex& vert : mesh->vertices )
	{
		data.push_back( vert.position.x );
		data.push_back( vert.position.y );
		data.push_back( vert.position.z );

		data.push_back( (float)vert.normal.x / 127.0f );
		data.push_back( (float)vert.normal.y / 127.0f );
		data.push_back( (float)vert.normal.z / 127.0f );
		
		data.push_back( (float)vert.texCoords.x / 32767.0f );
		data.push_back( (float)vert.texCoords.y / 32767.0f );
	}

	// Init, bind and buffer the stuff
	Init();
	BufferData( data.data(), data.size() * sizeof(float), VB_Static );
}

// ===============================================================================================
// VertexBuffer::Bind
// ===============================================================================================
void VertexBuffer::Bind() const
{
	// Bind the vertex buffer
	glBindBuffer( GL_ARRAY_BUFFER, vertexBufferHandle );
}

// ===============================================================================================
// VertexBuffer::BufferData
// ===============================================================================================
void VertexBuffer::BufferData( const float* vertexData, size_t size, int type )
{
	if ( nullptr == vertexData )
	{
		vertexData = data.data();
		size = data.size() * sizeof(float);
	}

	Bind();
	// Fill the VBO with vertex data
	glBufferData( GL_ARRAY_BUFFER, size, vertexData, type );
	// Determine the number of vertices
	numVertices = size / VertexArray::Stride;

	printf( "VertexBuffer::BufferData: numVertices = %i\n", (int)numVertices );
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
