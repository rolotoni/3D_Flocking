// ---------------------------------------------------------------------------
// Project Name		:	Simple Framework
// File Name		:	deafult.fs
// Author			:	Sun Tjen Fam
// Creation Date	:	2009/10/05
// Purpose			:	basic vertex shader
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// constants

#define PI			3.1415926535897932384626433832795

#define WAVE_NUM	3.0

// ---------------------------------------------------------------------------
// uniforms

// current 't' for the wave
uniform float uTime;

// ---------------------------------------------------------------------------
// per-vertex attributes

// the 't' offset for the current vertex
attribute vec3 aTimeOffset;

// ---------------------------------------------------------------------------
// varyings

varying vec3 vNormal;
varying vec3 vPos;

// ---------------------------------------------------------------------------

void main()
{
	// get the position in model space
	vec4  pos   = gl_Vertex;
	// calculate the wave phase at the current vertex
	float phase = aTimeOffset.y * aTimeOffset.y * WAVE_NUM * PI;
	// calculate the wave 't' (angle)
	float t     = uTime + phase;
	
	// calculate the height of the wave at the current vertex
	pos.y = sin(t) * 0.25;
	
	// calculate the normal for the current vertex
	vec3 normal = vec3(0.0, 1.0, 0.0) - vec3(aTimeOffset.x, 0.0, aTimeOffset.z) * cos(t) * 0.25 * WAVE_NUM * PI * 2.0 * aTimeOffset.y;
	normal      = normalize(normal);
	
	// calculate the position in projection space
	gl_Position		=	gl_ModelViewProjectionMatrix * pos;
	
	// calculate the normal in camera space
	vNormal			=	gl_NormalMatrix * normal;
	
	// calculate the position in camera space
	vPos			=	(gl_ModelViewMatrix * pos).xyz;

	// pass along the vertex color unmodified
	gl_FrontColor	=	gl_Color;
	
	// multiply the incoming texture coordinate with the texture matrix
	// associated with each texture unit to generate 2 texture coordinates
	gl_TexCoord[0]	=	gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_TexCoord[1]	=	gl_TextureMatrix[1] * gl_MultiTexCoord0;
}



