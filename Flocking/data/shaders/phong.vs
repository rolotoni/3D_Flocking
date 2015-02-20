// ---------------------------------------------------------------------------
// File Name		:	phong.vs
// Author			:	Jesse Harrison
// Creation Date	:	2/6/12
// Purpose			:	phong fragment shader
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// varyings

varying vec3 normal;
varying vec3 pos;
// ---------------------------------------------------------------------------

void main()
{
	// calculate the position in projection space
	gl_Position		=	ftransform();
	
	// calculate the normal in camera space
	normal			=	gl_NormalMatrix * gl_Normal;
	normal         /=   dot(normal, normal);
	
	pos             = (gl_ModelViewMatrix * gl_Vertex).xyz;

	// pass along the vertex color unmodified
	gl_FrontColor	=	gl_Color;

    // multiply the incoming texture coordinate with the texture matrix
	// associated with each texture unit to generate 2 texture coordinates
	gl_TexCoord[0]	=	gl_TextureMatrix[0]  * gl_MultiTexCoord0;
	gl_TexCoord[1]	=	gl_TextureMatrix[1]  * gl_MultiTexCoord0;
	gl_TexCoord[2]	=	gl_TextureMatrix[2]  * gl_MultiTexCoord0;
	gl_TexCoord[3]	=	gl_TextureMatrix[3]  * gl_MultiTexCoord0;
	gl_TexCoord[5]	=	gl_TextureMatrix[5]  * gl_MultiTexCoord0;
	gl_TexCoord[6]	=	gl_TextureMatrix[6]  * gl_MultiTexCoord0;
	
}

