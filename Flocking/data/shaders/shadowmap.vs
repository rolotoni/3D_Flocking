// ---------------------------------------------------------------------------
// File Name		:	phong.vs
// Author			:	Jesse Harrison
// Creation Date	:	5/5/11
// Purpose			:	phong fragment shader
// ---------------------------------------------------------------------------

#define CAM_FAR    70.0
#define CAM_NEAR  .01

// ---------------------------------------------------------------------------
// varyings
varying float depth;

// ---------------------------------------------------------------------------

void main()
{
	// calculate the position in projection space
	gl_Position	= gl_ModelViewMatrix * gl_Vertex;
	
	depth = (-gl_Position.z - CAM_NEAR) / (CAM_FAR - CAM_NEAR);

	depth += .003;

	gl_Position = ftransform();

}