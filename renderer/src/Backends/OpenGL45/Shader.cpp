#include "IRenderWorld.hpp"
#include "Shader.hpp"

#include <filesystem>

#define GLEW_STATIC 1
#include <GL/glew.h>

namespace fs = std::filesystem;

bool Shader::Load( const char* shaderPath )
{
	if ( !fs::exists( shaderPath ) )
	{
		errorMessage = 
			std::string( "Shader '" )
			.append( shaderPath )
			.append( "' does not exist" );
		return false;
	}

	fileName = shaderPath;
	name = shaderPath;

	std::ifstream shaderFile( shaderPath );
	if ( shaderFile.bad() || shaderFile.eof() )
	{
		errorMessage = 
			std::string( "Shader '" )
			.append( shaderPath )
			.append( "' is bad or empty" );
		return false;
	}

	supportedShaderFlags = ExtractShaderMeta( shaderFile, versionText );
	PopulateShaderObjects();

	vertexText = ExtractShaderText( "vertex", shaderFile );
	fragmentText = ExtractShaderText( "fragment", shaderFile );

	return true;
}

bool Shader::Compile()
{
	for ( ShaderObject& object : apiObjects )
	{
		// Create everything
		object.shaderHandle = glCreateProgram();
		object.vertexShader = glCreateShader( GL_VERTEX_SHADER );
		object.fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Determine GLSL preprocessor defines for shader permutations
		// versionText goes FIRST and foremost, then defines, then the
		// actual shader code
		std::string preprocessorText = versionText + DeterminePreprocessorFlags( object.shaderFlags );
		std::string finalVertexText = preprocessorText + vertexText;
		std::string finalFragmentText = preprocessorText + fragmentText;

		// Load the shaders with code
		const char* vertexString = finalVertexText.c_str();
		const char* fragmentString = finalFragmentText.c_str();

		printf( "## VERTEX SHADER STRING:\n%s\n", vertexString );

		glShaderSource( object.vertexShader, 1, &vertexString, nullptr );
		glShaderSource( object.fragmentShader, 1, &fragmentString, nullptr );

		// Compile the shaders
		glCompileShader( object.vertexShader );
		glCompileShader( object.fragmentShader );

		currentObject = &object;
		const char* errorMessage = GetErrorMessage();
		if ( nullptr != errorMessage )
		{
			printf( "Error while compiling: %s\n", errorMessage );
			return false;
		}

		// Attach shaders & link
		glAttachShader( object.shaderHandle, object.vertexShader );
		glAttachShader( object.shaderHandle, object.fragmentShader );
		glLinkProgram( object.shaderHandle );

		errorMessage = GetLinkerErrorMessage();
		if ( nullptr != errorMessage )
		{
			printf( "Error while linking: %s\n", errorMessage );
			return false;
		}

		// Delete these shader objects, we dun need them any more
		glDeleteShader( object.vertexShader );
		glDeleteShader( object.fragmentShader );

		// Get some uniform handles
		object.uniformProjectionMatrix = GetUniformHandle( "projMatrix" );
		object.uniformModelMatrix = GetUniformHandle( "modelMatrix" );
		object.uniformViewMatrix = GetUniformHandle( "viewMatrix" );
	}

	return true;
}

void Shader::Reload()
{
	for ( ShaderObject& object : apiObjects )
	{
		glDeleteProgram( object.shaderHandle );
	}

	Load( fileName.c_str() );
	Compile();
}

void Shader::Bind( uint16_t shaderFlags )
{
	if ( !((supportedShaderFlags & shaderFlags) ^ shaderFlags) )
	{
		for ( ShaderObject& object : apiObjects )
		{
			if ( object.shaderFlags == shaderFlags )
			{
				glUseProgram( object.shaderHandle );
				currentObject = &object;
				return;
			}
		}
	}
	
	glUseProgram( 0 );
}

const char* Shader::GetErrorMessage() const
{
	int success;
	static char infoLog[512];

	// Check the vertex shader
	glGetShaderiv( currentObject->vertexShader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( currentObject->vertexShader, 512, nullptr, infoLog );
		return infoLog;
	}
	// Then the fragment shader
	glGetShaderiv( currentObject->fragmentShader, GL_COMPILE_STATUS, &success );
	if ( !success )
	{
		glGetShaderInfoLog( currentObject->fragmentShader, 512, nullptr, infoLog );
		return infoLog;
	}

	return nullptr;
}

const char* Shader::GetLinkerErrorMessage() const
{
	int success;
	static char infoLog[512];

	glGetProgramiv( currentObject->shaderHandle, GL_LINK_STATUS, &success );
	if ( !success )
	{
		glGetProgramInfoLog( currentObject->shaderHandle, 512, nullptr, infoLog );
		return infoLog;
	}

	return nullptr;
}

uint32_t Shader::GetUniformHandle( const char* uniformName ) const
{
	return glGetUniformLocation( currentObject->shaderHandle, uniformName );
}

void Shader::SetUniform1i( const uint32_t& uniformHandle, const int& value )
{
	glUniform1i( uniformHandle, value );
}

void Shader::SetUniform1f( const uint32_t& uniformHandle, const float& value )
{
	glUniform1f( uniformHandle, value );
}

void Shader::SetUniform2f( const uint32_t& uniformHandle, const float& x, const float& y )
{
	glUniform2f( uniformHandle, x, y );
}

void Shader::SetUniform3f( const uint32_t& uniformHandle, const float& x, const float& y, const float& z )
{
	glUniform3f( uniformHandle, x, y, z );
}

void Shader::SetUniform3fv( const uint32_t& uniformHandle, const glm::vec3& v )
{
	glUniform3fv( uniformHandle, 1, &v.x );
}

void Shader::SetUniformmat3( const uint32_t& uniformHandle, const glm::mat3& m )
{
	glUniformMatrix3fv( uniformHandle, 1, GL_FALSE, &m[0].x );
}

void Shader::SetUniformmat4( const uint32_t& uniformHandle, const glm::mat4& m )
{
	glUniformMatrix4fv( uniformHandle, 1, GL_FALSE, &m[0].x );
}

constexpr uint16_t ShaderFlagCombinations[] =
{
	ShaderFlag_Normal,
	ShaderFlag_Normal | ShaderFlag_Instanced,
	ShaderFlag_Normal | ShaderFlag_CanSkin,
	ShaderFlag_Normal | ShaderFlag_Instanced | ShaderFlag_CanSkin
};

void Shader::PopulateShaderObjects()
{
	ShaderObject object;

	for ( const uint16_t& flagCombo : ShaderFlagCombinations )
	{
		// We gotta find out if our supportedShaderFlags
		// match the current shader flag combo
		// If supportedShaderFlags doesn't contain
		// ONE BIT of the combination, skip it
		uint16_t comparison = flagCombo & supportedShaderFlags;
		if ( comparison ^ flagCombo )
		{
			continue;
		}

		// Finally, register this shader variant
		object.shaderFlags = flagCombo;
		apiObjects.push_back( object );
	}
}

uint16_t Shader::ExtractShaderMeta( std::ifstream& file, std::string& versionText )
{
	uint16_t shaderFlags = ShaderFlag_Normal;
	std::string token;

	// Record where we were, so we can return to this once done
	auto lastSeekPoint = file.tellg();
	
	// Go to the start of the file
	file.clear();
	file.seekg( 0, std::ios::beg );

	while ( file >> token )
	{
		if ( token == "#supports" )
		{
			file >> token;
			if ( token == "instancing" || token == "batching" )
			{
				shaderFlags |= ShaderFlag_Instanced;
			}
		}

		if ( token == "#version" )
		{
			std::getline( file, token );
			versionText = "#version " + token + "\n";
		}
	}

	// Go back
	file.seekg( lastSeekPoint );
	return shaderFlags;
}

std::string Shader::ExtractShaderText( const char* sectionName, std::ifstream& file )
{
	std::ostringstream textStream; // all the shader text for this section
	bool entered = false; // whether we entered our section or not
	std::string line; // the current line in the text file

	// Record where we were
	auto lastSeekPoint = file.tellg();
	
	// Seek back to the start
	file.clear(); // clear EOF flag
	file.seekg( 0, std::ios::beg );
	
	while ( std::getline( file, line ) )
	{
		// Skip any FoxGLBox-specific keywords
		if ( line.find( "#supports" ) != std::string::npos )
		{
			continue;
		}

		// Find #section sectionName
		if ( !entered && (line.find( "#section" ) != std::string::npos) )
		{
			if ( line.find( sectionName, line.find( "#section" ) ) != std::string::npos )
			{
				entered = true;
			}

			continue;
		}
		// If we're in a section and we find an end, we get out
		if ( entered && (line.find( "#endsection" ) != std::string::npos) )
		{
			entered = false;
			break;
		}
		// Parse everything in there basically
		// TODO: add #include...
		if ( entered )
		{
			textStream << line << std::endl;
		}
	}

	// Put it back
	file.seekg( lastSeekPoint );

	return textStream.str();
}

std::string Shader::DeterminePreprocessorFlags( uint16_t shaderFlags )
{
	std::string result = "\n";

	if ( shaderFlags & ShaderFlag_Instanced )
		result += "#define SHADER_INSTANCED 1\n";

	return result;
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
