// ---------------------------------------------------------------------------
// File Name		:	phong.fs
// Author			:	Jesse Harrison
// Creation Date	:	2/6/12
// Purpose			:	phong fragment shader
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// uniforms
uniform bool Textured;
uniform vec3 light_pos;
uniform vec3 light_dir;
uniform vec4 color;
uniform sampler2D emissive_tex;
uniform sampler2D diffuse_tex;
uniform sampler2D ambient_tex;
uniform sampler2D specular_tex;

// ---------------------------------------------------------------------------
// varyings

varying vec3	normal;
varying vec3    pos;

// ---------------------------------------------------------------------------

vec4 Gambient  = vec4(0.2,0.2,0.2,1.0);
vec4 Kambient  = vec4(0.2);
vec4 Kdiffuse  = vec4(0.8,0.8,0.8,1.0);
vec4 Kspecular = vec4(1.0);
vec4 emissive  = vec4(0.0);
vec3 d_att     = vec3(1.0,0.1,0.0);

float ns = 1.0; //shiny


void main()
{
    vec3 Normal = normalize(normal);

    if(Textured)
    {
      // average the 2 textures color and mutiply the vertex color
	  Kambient  = texture2D(ambient_tex, gl_TexCoord[0].st);
	  Kdiffuse  = texture2D(diffuse_tex, gl_TexCoord[1].st);
	  emissive  = texture2D(emissive_tex, gl_TexCoord[2].st);
	  Kspecular = texture2D(specular_tex, gl_TexCoord[3].st);

	  Normal = (Normal * 2.0) - 1.0;//[-1,1] range
	  Normal = normalize(Normal);

	  ns = Kspecular.w * 256.0;
	  Kspecular.w = 1.0;
      gl_FragColor = emissive + Gambient * Kambient;
	  vec3 L = light_pos - pos;

	  vec3 R = 2.0 * dot(Normal, normalize(L)) * Normal - normalize(L);
	  float d_sq = dot(L,L);
	  float d = sqrt(d_sq);
	  L = normalize(L);
	  float Att = min(1.0/(d_att.x + d_att.y * d + d_att.z * d_sq), 1.0);

      //suppose to be Idiffuse which is the lights properties
      //but for now the same as object's
	  vec4 Ambient  = Gambient * Kambient;
	  vec4 Diffuse  = Kdiffuse * Kdiffuse * max(dot(Normal,L), 0.0);
	  vec4 Specular = Kspecular * Kspecular * pow((max(dot(R, -normalize(pos)),0.0)), ns);
      

	  gl_FragColor += Att * Ambient + Att *(Diffuse + Specular);
      return;
    }

	vec3 L = normalize(light_pos - pos);

	vec4 Diffuse  = 100 * color + Kambient  * color + Kdiffuse * color * max(dot(Normal,L), 0.0);
      
	gl_FragColor = Diffuse;
}
