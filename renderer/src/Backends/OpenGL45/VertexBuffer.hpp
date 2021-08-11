#pragma once

struct VertexAttributes
{
    static constexpr int Positions = 0;
    static constexpr int Normals = 1;
    static constexpr int TexCoords = 2;
};

struct VertexAttribOffsets
{
    static constexpr size_t FloatSize = sizeof( float );
    static constexpr size_t Int32Size = sizeof( int32_t );
    static constexpr size_t Int16Size = sizeof( int16_t );

    static constexpr int Positions = 0;
    static constexpr int Normals = Positions + 3*FloatSize;
    static constexpr int TexCoords = Normals + 3*FloatSize;
};

class DrawMesh;
class VertexBuffer;

enum
{
    VB_Static = 0x88E4,
    VB_Dynamic = 0x88E8,
    VB_Stream = 0x88E0
};

extern bool GLError( const char* why );

// =====================================================================
// VertexArray
// 
// Manages VAOs and EBOs
// Is used to draw stuff
// 
// Every draw surface is equivalent to one vertex array
// When initialising a model, you first generate & bind a VAO&EBO,
// then generate, bind & buffer a VBO, then set up vertex attributes
// =====================================================================
class VertexArray
{
public:
    VertexArray( VertexBuffer* buffer )
        : vertexBuffer( buffer )
    {
        indices.clear();
        Init();
    }

    VertexArray( VertexBuffer* buffer, const DrawSurface* surf )
        : vertexBuffer( buffer ), material( surf->material )
    {
        indices.clear();
        SetupIndices( surf );
        // Generate element indices
        Init();
        GLError( "initialised the vertex array" );
        PrintDebugInfo();
        // Buffer the indices
        BufferData( VB_Static );
        GLError( "buffered data for the vertex array's element buffer" );
        PrintDebugInfo();
        // Set up the vertex attributes
        SetupVertexAttributes();
        GLError( "set up the vertex array's attributes" );
        PrintDebugInfo();
    }

    void Init();
    void Bind( bool arrayOnly = true ) const;
    void BufferData( int type );
    // Sets up vertex attributes
    void SetupVertexAttributes() const;

    inline const void* GetIndexData() const
    {
        return indices.data();
    }

    inline size_t GetNumIndices() const
    {
        return indices.size();
    }

    inline IMaterial* GetMaterial() const
    {
        return material;
    }

    static constexpr size_t Stride = 8U * sizeof( float );
    
    void PrintDebugInfo() const;

private:
    void SetupIndices( const DrawSurface* surf );


    static void* VBOffset( const int& num )
    {
        return reinterpret_cast<void*>(num);
    }

    static void SetupSingleVertexAttrib( const int& attribType, const int& size = 3, const int& offset = 0 );

    GLuint vertexArrayHandle{ 0 };
    GLuint elementBufferHandle{ 0 };
    std::vector<vertexid_t> indices;
    VertexBuffer* vertexBuffer{ nullptr };
    IMaterial* material{ nullptr };
};

// =====================================================================
// VertexBuffer
// 
// Manages VBOs
// Holds render data
// =====================================================================
class VertexBuffer
{
public:
    VertexBuffer()
    {
        Init();
    }

    VertexBuffer( const DrawMesh* mesh )
    {
        Init( mesh );
    }

    // Generates buffers
    void Init();
    // Generates a vertex buffer + array buffers from the mesh
    // @param mesh: the mesh to generate this buffer from
    void Init( const DrawMesh* mesh );
    // Binds the whole thing
    void Bind() const;
    // Loads data
    void BufferData( const float* data, size_t size, int type = VB_Static );
    
    inline const size_t& GetNumVertices() const
    {
        return numVertices;
    }

private:
    GLuint vertexBufferHandle{ 0 };
    size_t numVertices{ 0 };

    std::vector<float> data;
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
