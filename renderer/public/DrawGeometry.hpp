#pragma once

class IMaterial;

// Vertex IDs have 32 bits
// Previously they were 16 bits, but aman.obj wouldn't
// load correctly...
using vertexid_t = uint32_t;

class DrawVertex final
{
public:
    glm::vec3       position;   // 12 bytes
    // Shading
    glm::i8vec4     normal;     // 4 bytes
    glm::i8vec4     tangent;    // 4 bytes
    // Color & texture
    glm::i16vec2    texCoords;  // 4 bytes
    glm::i8vec4     color;      // 4 bytes
    // Bone weights
    glm::i8vec4     weights;    // 4 bytes

    bool operator == ( const DrawVertex& rhs ) const
    {
        return position == rhs.position
            && normal == rhs.normal
            && texCoords == rhs.texCoords;
    }
};

// All adds up to 32 bytes, should be kinda compact
constexpr size_t DrawVertSize = sizeof( DrawVertex );
static_assert(DrawVertSize == 32U, "DrawVertex must be 32 bytes big!");

class DrawTriangle final
{
public:
    DrawTriangle()
    {
        vertexIndices[0] = 0;
        vertexIndices[1] = 0;
        vertexIndices[2] = 0;
    }

    DrawTriangle( int a, int b, int c )
    {
        vertexIndices[0] = a;
        vertexIndices[1] = b;
        vertexIndices[2] = c;
    }

    vertexid_t vertexIndices[3]; // into DrawMesh::vertices
};

class DrawSurface final
{
public:
    DrawSurface()
    {
        material = nullptr;
        vertexIndices.clear();
    }

    DrawSurface( IMaterial* surfaceMaterial )
        : material( surfaceMaterial ), materialName( surfaceMaterial->GetName() )
    {
        vertexIndices.clear();
    }

    DrawSurface( const char* name )
        : materialName( name )
    {
        vertexIndices.clear();
    }

    std::string materialName{ "default" };
    IMaterial* material;
    //std::vector<uint16_t> triangleIndices; // into DrawMesh::triangles
    std::vector<vertexid_t> vertexIndices; // into DrawMesh::vertices
};

class DrawMesh final
{
public:
    DrawMesh() = default;
    ~DrawMesh() = default;

    void AddVertex( DrawVertex& vertex )
    {
        vertices.push_back( vertex );
    }

    void AddTriangles( DrawTriangle& triangle )
    {
        triangles.push_back( triangle );
    }

    void AddSurface( DrawSurface& surface )
    {
        if ( surfaces.empty() && surface.vertexIndices.empty() )
        {
            surface.vertexIndices.reserve( vertices.size() );
            for ( int i = 0; i < triangles.size(); i++ )
            {
                surface.vertexIndices.push_back( i );
            }
        }

        surfaces.push_back( surface );
    }

//private:
    std::vector<DrawSurface>    surfaces;
    std::vector<DrawTriangle>   triangles;
    std::vector<DrawVertex>     vertices;
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
