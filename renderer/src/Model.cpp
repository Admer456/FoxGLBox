#include <fstream>
#include <array>
#include <filesystem>
#include <iostream>

#include "IRenderWorld.hpp"
#include "Model.hpp"

#include <sstream>
#include <map>

namespace fs = std::filesystem;
using InFile = std::ifstream;

#ifndef FOX_USE_ASSIMP

class OBJParser final
{
public:
	// v x y z
	using objVertexPosition = std::array<float, 3>;
	// vn x y z
	using objVertexNormal = std::array<float, 3>;
	// vt x y
	using objVertexTexCoord = std::array<float, 3>;
	// v/vn/vt
	using objVertex = std::pair<std::string, DrawVertex>;
	// f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
	using objTriangle = std::array<vertexid_t, 3>;

public:
	DrawMesh ParseOBJ( InFile& file, const char* filePath )
	{
		DrawMesh dmesh;

		// Parse the file
		std::string token;
		while ( file >> token )
		{
			if ( token[0] == '#' )
			{
				std::getline( file, token );
				continue;
			}

			else if ( token == "v" )
			{
				objVertexPosition pos;
				file >> pos[0] >> pos[1] >> pos[2];
				objVertexPositions.push_back( std::move( pos ) );
				continue;
			}

			else if ( token == "vn" )
			{
				objVertexNormal normal;
				file >> normal[0] >> normal[1] >> normal[2];
				objVertexNormals.push_back( std::move( normal ) );
				continue;
			}

			else if ( token == "vt" )
			{
				objVertexTexCoord texCoord;
				file >> texCoord[0] >> texCoord[1];
				texCoord[1] = 1.0f - texCoord[1]; // Gotta be reversed cuz' OpenGL
				objVertexTexCoords.push_back( std::move( texCoord ) );
				continue;
			}

			else if ( token == "f" )
			{
				objTriangle triangle = ParseOBJTriangle( file );
				objTriangles.push_back( std::move( triangle ) );
				continue;
			}

			else
			{
				std::getline( file, token );
				continue;
			}
		}

		// Fill dmesh with data
		// Vertices go first
		for ( auto& vert : objVertices )
		{
			dmesh.AddVertex( vert.second );
		}

		// Triangles and vertex indices are already sorted, just add them all
		DrawSurface surf = DrawSurface(); // use default material
		for ( auto& tri : objTriangles )
		{	
			surf.vertexIndices.push_back( tri[0] );
			surf.vertexIndices.push_back( tri[1] );
			surf.vertexIndices.push_back( tri[2] );
		}

		dmesh.AddSurface( std::move( surf ) );

		return dmesh;
	}

	const std::vector<objVertex>& GetVertices()
	{
		return objVertices;
	}

	const std::vector<objTriangle>& GetTriangles()
	{
		return objTriangles;
	}

private:
	static constexpr int NotFound = -1;

	int FindVertex( const std::string_view& token )
	{
		int i = 0;
		for ( auto& vertex : objVertices )
		{
			if ( vertex.first == token )
			{
				return i;
			}

			i++;
		}

		return NotFound;
	}

	int ParseOBJVertex( const std::string& token )
	{
		constexpr int Position = 0;
		constexpr int TextureCoordinate = 1;
		constexpr int Normal = 2;

		objVertex vertex;
		DrawVertex dvert;
		int indices[3];

		// Parse the string
		std::istringstream stream( token );
		//        vertexPos     vertexTexCoord vertexNormal
		stream >> indices[0] >> indices[1] >> indices[2];

		indices[0] -= 1;
		indices[1] -= 1;
		indices[2] -= 1;

		// Get references to each thing of concern
		objVertexPosition& pos = objVertexPositions[indices[Position]];
		objVertexTexCoord& texCoord = objVertexTexCoords[indices[TextureCoordinate]];
		objVertexNormal& normal = objVertexNormals[indices[Normal]];

		dvert.position.x = pos[0];
		dvert.position.y = pos[1];
		dvert.position.z = pos[2];
		
		dvert.normal.x = normal[0] * 127.0f;
		dvert.normal.y = normal[1] * 127.0f;
		dvert.normal.z = normal[2] * 127.0f;

		dvert.texCoords.x = texCoord[0] * 32767.0f;
		dvert.texCoords.y = texCoord[1] * 32767.0f;
		
		vertex = std::make_pair( token, dvert );
		objVertices.push_back( vertex );

		return objVertices.size() - 1;
	}

	objTriangle ParseOBJTriangle( InFile& file )
	{
		objTriangle triangle;
	
		for ( int i = 0; i < 3; i++ )
		{
			std::string token;
			file >> token;
			for ( char& c : token )
			{
				if ( c == '/' )
				{
					c = ' ';
				}
			}

			triangle[i] = ParseOBJVertex( token );
		}

		return triangle;
	}

	std::vector<objVertexPosition> objVertexPositions;
	std::vector<objVertexNormal> objVertexNormals;
	std::vector<objVertexTexCoord> objVertexTexCoords;
	std::vector<objVertex> objVertices;
	std::vector<objTriangle> objTriangles;
};

#else 

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"

class OBJParser final
{
public:
	DrawMesh ParseOBJ( InFile& file, const char* filePath )
	{	using flags = aiPostProcessSteps;

		DrawMesh dmesh;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile( filePath, 
												  flags::aiProcess_GenNormals 
												  | flags::aiProcess_Triangulate 
												/*| flags::aiProcess_FlipWindingOrder*/ );

		for ( uint32_t i = 0; i < scene->mNumMeshes; i++ )
		{	aiMesh* mesh = scene->mMeshes[i];			
			for ( uint32_t v = 0; v < mesh->mNumVertices; v++ )
			{	DrawVertex dvertex;

				dvertex.position.x = mesh->mVertices[v].x;
				dvertex.position.y = mesh->mVertices[v].y;
				dvertex.position.z = mesh->mVertices[v].z;

				dvertex.normal.x = mesh->mNormals[v].x * 127.0f;
				dvertex.normal.y = mesh->mNormals[v].y * 127.0f;
				dvertex.normal.z = mesh->mNormals[v].z * 127.0f;

				dvertex.texCoords.x = mesh->mNormals[v].x * 32767.0f;
				dvertex.texCoords.y = mesh->mNormals[v].y * 32767.0f;

				dmesh.AddVertex( std::move( dvertex ) );
			}

			DrawSurface dsurface( nullptr );

			for ( uint32_t f = 0; f < mesh->mNumFaces; f++ )
			{	//DrawTriangle dtriangle;
				for ( uint32_t fv = 0; fv < mesh->mFaces[f].mNumIndices; fv++ )
				{
					dsurface.vertexIndices.push_back( mesh->mFaces[f].mIndices[fv] );
				}
			}

			dmesh.AddSurface( std::move( dsurface ) );
		}

		importer.FreeScene();

		return dmesh;
	}
};

#endif

// =====================================================================
// Model::LoadFromPath
// =====================================================================
void Model::LoadFromPath( const char* filePath )
{
	name = filePath;

	// Check if it exists first
	if ( !fs::exists( filePath ) )
	{
		std::cout << "Can't load model '" << filePath << "'" << std::endl
			<< "Absolute path: '" << fs::absolute( filePath ) << "'" << std::endl;
		
		okay = false;
		return;
	}

	// Open the file
	InFile objFile( filePath );
	if ( objFile.bad() )
	{
		std::cout << "Model '" << filePath << "' bad" << std::endl;

		okay = false;
		objFile.close();
		return;
	}

	// Let's hardcode an OBJ model loader right now, until we get a plugin system
	mesh = OBJParser().ParseOBJ( objFile, filePath );

	// Everything went well!
	okay = true;
	objFile.close();
}

// =====================================================================
// Model::Okay
// =====================================================================
bool Model::Okay() const
{
	return okay;
}
