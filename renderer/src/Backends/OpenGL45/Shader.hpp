#pragma once

#include <string>
#include <fstream>

class ShaderObject final
{
public:
	uint32_t		vertexShader;
	uint32_t		fragmentShader;
	uint32_t		shaderHandle;

	uint32_t		uniformProjectionMatrix;
	uint32_t		uniformModelMatrix;
	uint32_t		uniformViewMatrix;

	uint16_t		shaderFlags;
};

// =====================================================================
//	OpenGL 4.5 shader
// 
//	Backend-specific shader class, handles shader-related OpenGL calls
// =====================================================================
class Shader : public IShader
{
public:
	// @returns The full path to the shader file, unless it's built-in or generated
	const char*			GetName() const override { return name.c_str(); }

	// Load the shader code
	// @param shaderPath: path to the shader
	// @returns: false if the file cannot be found
	bool				Load( const char* shaderPath ) override;
	// Build the shader
	// @returns true on success, false if there was an error
	bool				Compile() override;
	// Reloads the shader
	void				Reload();
	// Binds the shader to be used for rendering
	void				Bind( uint16_t shaderFlags ) override;
	// @returns The error message, in case there was one while compiling
	const char*			GetErrorMessage() const override;
	// @returns The linker error message done during glLinkProgram
	const char*			GetLinkerErrorMessage() const;

	// Uniforms //
	uint32_t			GetUniformHandle( const char* uniformName ) const override;
	void				SetUniform1i( const uint32_t& uniformHandle, const int& value ) override;
	void				SetUniform1f( const uint32_t& uniformHandle, const float& value ) override;
	void				SetUniform2f( const uint32_t& uniformHandle, const float& x, const float& y ) override;
	void				SetUniform3f( const uint32_t& uniformHandle, const float& x, const float& y, const float& z ) override;
	void				SetUniform3fv( const uint32_t& uniformHandle, const glm::vec3& v ) override;
	void				SetUniformmat3( const uint32_t& uniformHandle, const glm::mat3& m ) override;
	void				SetUniformmat4( const uint32_t& uniformHandle, const glm::mat4& m ) override;

	void				SetProjectionMatrix( const glm::mat4& m ) override
	{
		SetUniformmat4( currentObject->uniformProjectionMatrix, m );
	}

	void				SetModelMatrix( const glm::mat4& m ) override
	{
		SetUniformmat4( currentObject->uniformModelMatrix, m );
	}
	
	void				SetViewMatrix( const glm::mat4& m ) override
	{
		SetUniformmat4( currentObject->uniformViewMatrix, m );
	}

private:
	// Populates apiObjects with ShaderObjects
	// The resulting number of apiObjects will be the number of
	// unique shader flag combinations
	void				PopulateShaderObjects();

	// Parses shader metadata, sets versionText
	// @returns value for supportedShaderFlags
	static uint16_t		ExtractShaderMeta( std::ifstream& file, std::string& versionText );
	static std::string	ExtractShaderText( const char* sectionName, std::ifstream& file );
	static std::string	DeterminePreprocessorFlags( uint16_t shaderFlags );

private:
	std::string			name{ "Default" };
	std::string			fileName{ "" };
	std::string			errorMessage{ "OK" };

	std::string			versionText{ "#version 450 core\n" };
	std::string			vertexText;
	std::string			fragmentText;

	// ShaderObject stores permutated shader handles
	std::vector<ShaderObject> apiObjects;
	ShaderObject*		currentObject;

	uint16_t			supportedShaderFlags{ ShaderFlag_Normal };
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
