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
void VertexArray::SetupSingleVertexAttrib( const int& attribType, const int& size, const int& offset )
{
	glEnableVertexAttribArray( attribType );
	GLError( "VertexArray::SetupSingleVertexAttrib: called glEnableVertexAttribArray" );
	glVertexAttribPointer( attribType, size, GL_FLOAT, GL_FALSE, Stride, VBOffset( offset ) );
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
	if ( vertexData == nullptr )
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
