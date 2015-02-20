// --------------------------------------------------------------------------
// File Name		:	reflect.vs
// Author			:	Jesse Harrison
// Creation Date	:	6/18/2011
// Purpose			:	reflection/refraction vs
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// varyings

   varying vec3 norm;
   varying vec3 pos;
   varying vec3 tan;
   varying vec3 bitan;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// attributes

  attribute vec3 Tan;
  attribute vec3 BiTan;

// ---------------------------------------------------------------------------

void main()
{
	//
	tan   = (gl_ModelViewMatrix * vec4(Tan.xyz,0.0)).xyz;
	bitan = (gl_ModelViewMatrix * vec4(BiTan.xyz,0.0)).xyz;

	// calculate the position in projection space
	gl_Position		=	ftransform();
	
	// calculate the normal in camera space
	norm		=	gl_NormalMatrix * gl_Normal;

	// pass along the vertex color unmodified
	gl_FrontColor	=	gl_Color;

	pos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	
	// multiply the incoming texture coordinate with the texture matrix
	// associated with each texture unit to generate 2 texture coordinates
	gl_TexCoord[0]	=	gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_TexCoord[7]	=	gl_TextureMatrix[7] * gl_MultiTexCoord0;
}
