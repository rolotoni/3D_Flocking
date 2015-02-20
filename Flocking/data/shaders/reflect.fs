// --------------------------------------------------------------------------
// File Name		:	reflect.fs
// Author			:	Jesse Harrison
// Creation Date	:	6/18/2011
// Purpose			:	reflection/refraction fs
// ---------------------------------------------------------------------------

#define REFLECTION 0
#define REFRACTION 1
#define FRESNEL    2

// ---------------------------------------------------------------------------
// uniforms

   uniform mat4 InvCam;
   uniform sampler2D cube_back_tex;
   uniform sampler2D cube_front_tex;
   uniform sampler2D cube_bottom_tex;
   uniform sampler2D cube_top_tex;
   uniform sampler2D cube_left_tex;
   uniform sampler2D cube_right_tex;
   uniform sampler2D normal_tex;

   uniform int  uFlag;
   uniform bool normal_map;

// ---------------------------------------------------------------------------
// varyings
   
   varying vec3 norm;
   varying vec3 pos;
   varying vec3 tan;
   varying vec3 bitan;

// ---------------------------------------------------------------------------
vec4 CubeMapSample(vec3 r)
{
  float x = abs(r.x);
  float y = abs(r.y);
  float z = abs(r.z);
  vec2 tc;

  if(x > y && x > z)      //x side
  {
    //left x
    if(r.x < 0.0)
	{
	  r.x /= x;
	  r.y /= x;
	  r.z /= x;
	  
	  r.y = (r.y + 1.0) / 2.0;
	  r.z = (r.z + 1.0) / 2.0;

	  tc.x = -r.z;
	  tc.y = r.y;

	  return texture2D(cube_left_tex, tc);
	}
	else
	{
	  r.x /= x;
	  r.y /= x;
	  r.z /= x;
	  
	  r.y = (r.y + 1.0) / 2.0;
	  r.z = (r.z + 1.0) / 2.0;

	  tc.x = r.z;
	  tc.y = r.y;
	  return texture2D(cube_right_tex, tc);
	}
  }
  else if(y > x && y > z) //y side
  {
    //bottom y
    if(r.y < 0.0)
	{
	  r.x /= y;
	  r.y /= y;
	  r.z /= y;
	  
	  r.x = (r.x + 1.0) / 2.0;
	  r.z = (r.z + 1.0) / 2.0;

	  tc.x = r.x;
	  tc.y = -r.z;

	  return texture2D(cube_bottom_tex, tc);
	}
	else
	{
	  r.x /= y;
	  r.y /= y;
	  r.z /= y;
	  
	  r.x = (r.x + 1.0) / 2.0;
	  r.z = (r.z + 1.0) / 2.0;

	  tc.x = r.x;
	  tc.y = r.z;
	  return texture2D(cube_top_tex, tc);
	}
  }
  else                    //z side
  {
    //back z
    if(r.z < 0.0)
	{
	  r.x /= z;
	  r.y /= z;
	  r.z /= z;
	  
	  r.x = (r.x + 1.0) / 2.0;
	  r.y = (r.y + 1.0) / 2.0;

	  tc.x = r.x;
	  tc.y = r.y;

	  return texture2D(cube_back_tex, tc);
	}
	else
	{
	  r.x /= z;
	  r.y /= z;
	  r.z /= z;
	  
	  r.x = (r.x + 1.0) / 2.0;
	  r.y = (r.y + 1.0) / 2.0;

	  tc.x = -r.x;
	  tc.y = r.y;
	  return texture2D(cube_front_tex, tc);
	}
  }

  return vec4(0.0);
}

vec3 ReflectionVector(vec3 L, vec3 N)
{
   L = normalize(L);
   vec3 rv = 2.0 * (dot(N,L)) * N - L;
   return rv;
}

vec3 RefractionVector(vec3 L, vec3 N)
{
  float ni = 1.0;
  float nt = 1.33;
  float K  = ni/nt;

  L = normalize(L);

  vec3 T = (K *dot(N,L) - sqrt(1.0 - (K*K) * (1.0 - dot(N,L)*dot(N,L)))) * N - K*L;
  return T;
}

float Fresnel(vec3 L, vec3 N)
{

  L = normalize(L);

  float ni = 1.0;
  float nt = 1.33;
  float K  = ni/nt;
  
  float NL   = dot(N,L);
  float cosi = NL;
  float sini = length(cosi * N - L);

  float Rs = ((ni * cosi - nt * sqrt(1.0-(K*sini)*(K*sini)))/((ni * cosi + nt * sqrt(1.0-(K*sini)*(K*sini)))))
	         * ((ni * cosi - nt * sqrt(1.0-(K*sini)*(K*sini)))/((ni * cosi + nt * sqrt(1.0-(K*sini)*(K*sini)))));

  float Rp = ((ni*sqrt(1.0-(K*sini)*(K*sini))-nt*cosi)/(ni*sqrt(1.0-(K*sini)*(K*sini))+nt*cosi))
            * ((ni*sqrt(1.0-(K*sini)*(K*sini))-nt*cosi)/(ni*sqrt(1.0-(K*sini)*(K*sini))+nt*cosi));

  float R = (Rs + Rp) * 0.5;
  return R;

}

void main()
{
  vec3 N = normalize(norm);
  if(normal_map)
  {
    vec3 Tan   = normalize(tan);
    vec3 BiTan = normalize(bitan);
    mat3 TBNMat = mat3(Tan,BiTan,N.xyz);
    N     = vec3(texture2D(normal_tex, gl_TexCoord[7].st)).xyz;
    N = (N * 2.0) - 1.0;//[-1,1] range
    N = TBNMat * N;
    N = normalize(N);
  }

  if( uFlag == REFLECTION )
  {
    vec3 rv = ReflectionVector(-pos, N);
    rv = (InvCam * vec4(rv, 0.0)).xyz;
    gl_FragColor = CubeMapSample(rv);
    return;
  }
  else if( uFlag == REFRACTION)
  {
	vec3 T = RefractionVector(-pos, N);
	T = (InvCam * vec4(T, 0.0)).xyz;
    gl_FragColor = CubeMapSample(T);
    return;
  }
  else if( uFlag == FRESNEL)
  {
    float R = Fresnel(-pos, N);
    float T = 1.0 - R;

	vec3 reflection = ReflectionVector(-pos, N);
	vec3 refraction = RefractionVector(-pos, N);

	reflection = (InvCam * vec4(reflection, 0.0)).xyz;
	refraction = (InvCam * vec4(refraction, 0.0)).xyz;

	gl_FragColor = T * CubeMapSample(refraction) + R * CubeMapSample(reflection);
	return;
  }

  gl_FragColor = gl_Color;
}