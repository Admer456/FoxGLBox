#section vertex
#version 450 core

// Render data
layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec3 vertexNormal;
layout ( location = 2 ) in vec2 vertexCoord;

// Uniforms
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

// Outputs for the fragment shader
out vec3 fragmentPosition;
out vec3 fragmentNormal;
out vec2 fragmentCoord;
out float fragmentVertexID;

void main()
{
    // Send the normal & texcoord to the fragment shader
    fragmentPosition = (modelMatrix * vec4( vertexPosition, 1.0 )).xyz;
    fragmentNormal = (modelMatrix * vec4( vertexNormal, 0.0 )).xyz;
    fragmentCoord = vertexCoord;

    // Calculate vertex position
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4( vertexPosition, 1.0 );

    fragmentVertexID = gl_VertexID;
}

#end // vertex

#section fragment
#version 450 core

// Inputs from the vertex shader
in vec3 fragmentPosition;
in vec3 fragmentNormal;
in vec2 fragmentCoord;
in float fragmentVertexID;

// Textures
uniform sampler2D albedoMap;

// Outputs for OpenGL
out vec4 outColour;

// Assumes that lightDir and normal are normalized
float HalfLambert( vec3 lightDir, vec3 normal )
{
    // We are transforming this dot from [-1.0, 1.0] to [0.0, 1.0]
    return (dot( lightDir, normal ) + 1.0) * 0.5;
}

vec3 LightInteraction( vec3 lightDir, vec3 lightColor, float diffusePower )
{
    float halfLambert = HalfLambert( lightDir, normalize( fragmentNormal ) );

    halfLambert = pow( halfLambert, diffusePower );

    return lightColor * halfLambert;
}

void main()
{
	// Hardcoded light direction
	const vec3 lightDirMain = normalize( vec3( -10.0, 20.0, 20.0 ) );
	const vec3 lightDirAmbient = vec3( 0.0, -1.0, 0.0 );

    const float mainPower = 2.0;
    const float ambientPower = 0.5;

    vec3 mainDiffuse = LightInteraction( lightDirMain, vec3( 1.0, 0.98, 0.9 ), mainPower ) * 2.0;
    mainDiffuse += LightInteraction( lightDirAmbient, vec3( 0.06, 0.11, 0.18 ), ambientPower );

    mainDiffuse = texture( albedoMap, fragmentCoord ).rgb * mainDiffuse;

    //mainDiffuse = texture( albedoMap, fragmentCoord ).rgb;
    //mainDiffuse.xy = fragmentCoord;
    outColour.rgb = mainDiffuse;
    outColour.a = 1.0;
}

#end // fragment
