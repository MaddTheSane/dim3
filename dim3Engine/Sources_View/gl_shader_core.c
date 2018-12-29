/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Core Shader Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern iface_type			iface;
extern map_type				map;
extern setup_type			setup;
extern view_type			view;
extern render_info_type		render_info;

extern shader_type			bitmap_shader;

shader_type					core_shaders[max_shader_light+1][max_core_shader];

/* =======================================================

      ES2 non-ES2 Precision Fixes
      
======================================================= */

void gl_core_shader_build_generic_precision_defines(char *buf)
{
	strcat(buf,"#ifndef GL_ES\n");
	strcat(buf," #define lowp\n");
	strcat(buf," #define mediump\n");
	strcat(buf," #define highp\n");
	strcat(buf,"#endif\n");
}

/* =======================================================

      Generic GLSL Light Structure
      
======================================================= */

void gl_core_shader_build_generic_light_struct(int nlight,char *buf)
{
	int			n;
	
	if (nlight==0) return;
	
	strcat(buf,"struct dim3LightType {\n");
	strcat(buf," highp vec3 position;\n");
	strcat(buf," lowp vec3 color;\n");
	strcat(buf," mediump float intensity;\n");
	strcat(buf," mediump float invertIntensity;\n");
	strcat(buf," mediump float exponent;\n");
	strcat(buf," mediump vec3 direction;\n");
	strcat(buf,"};\n");
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"uniform dim3LightType dim3Light_%d;\n",n);
	}
}

/* =======================================================

      Build Single Core Map Shaders
      
======================================================= */

char* gl_core_map_shader_build_vert(int nlight,bool fog,bool bump,bool spec)
{
	int				n;
	char			*buf;

		// no lights mean no spec

	if (nlight==0) spec=FALSE;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build vert shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);

	strcat(buf,"uniform highp mat4 dim3ProjectionMatrix;\n");
	strcat(buf,"uniform highp mat4 dim3ModelViewMatrix;\n");
	strcat(buf,"uniform highp mat3 dim3NormalMatrix;\n");
	strcat(buf,"uniform highp vec3 dim3CameraPosition;\n");
	if (fog) {
		strcat(buf,"uniform highp float dim3FogEnd;\n");
		strcat(buf,"uniform highp float dim3FogScale;\n");
	}

	strcat(buf,"attribute highp vec3 dim3Vertex;\n");
	strcat(buf,"attribute mediump vec3 dim3VertexNormal;\n");
	if ((bump) || (spec)) strcat(buf,"attribute mediump vec3 dim3VertexTangent;\n");
	strcat(buf,"attribute mediump vec2 dim3VertexUV,dim3VertexLightMapUV;\n");
	
	strcat(buf,"varying mediump vec2 uv,lightMapUV;\n");
	strcat(buf,"varying mediump vec3 dirNormal;\n");
	if (fog) strcat(buf,"varying mediump float fogFactor;\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying highp vec3 lightVector_%d;\n",n);
		if (bump) sprintf(strchr(buf,0),"varying highp vec3 lightVertexVector_%d;\n",n);
	}

	if (spec) strcat(buf,"varying mediump vec3 eyeVector;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	strcat(buf,"gl_Position=dim3ProjectionMatrix*dim3ModelViewMatrix*vec4(dim3Vertex,1.0);\n");
	strcat(buf,"uv=dim3VertexUV;\n");
	strcat(buf,"lightMapUV=dim3VertexLightMapUV;\n");
	
	strcat(buf,"dirNormal=normalize(dim3VertexNormal);\n");

	strcat(buf,"highp vec3 vtx=vec3(dim3ModelViewMatrix*vec4(dim3Vertex,1.0));\n");
	
	if ((bump) || (spec)) {
		strcat(buf,"vec3 tangentSpaceTangent=normalize(dim3NormalMatrix*dim3VertexTangent);\n");
		strcat(buf,"vec3 tangentSpaceBinormal=normalize(dim3NormalMatrix*cross(dim3VertexNormal,dim3VertexTangent));\n");
		strcat(buf,"vec3 tangentSpaceNormal=normalize(dim3NormalMatrix*dim3VertexNormal);\n");
	}
	
	if (spec) {
		strcat(buf,"eyeVector.x=dot(-vtx,tangentSpaceTangent);\n");
		strcat(buf,"eyeVector.y=dot(-vtx,tangentSpaceBinormal);\n");
		strcat(buf,"eyeVector.z=dot(-vtx,tangentSpaceNormal);\n");
	}
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector_%d=dim3Light_%d.position-vtx;\n",n,n);
		
		if (bump) {
			sprintf(strchr(buf,0),"lightVertexVector_%d.x=dot(lightVector_%d,tangentSpaceTangent);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector_%d.y=dot(lightVector_%d,tangentSpaceBinormal);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector_%d.z=dot(lightVector_%d,tangentSpaceNormal);\n",n,n);
		}
	}
	
	if (fog) strcat(buf,"fogFactor=clamp(((dim3FogEnd-distance(dim3Vertex,dim3CameraPosition))*dim3FogScale),0.0,1.0);\n");

	strcat(buf,"}\n");

	return(buf);
}

char* gl_core_map_shader_build_frag(int nlight,bool fog,bool bump,bool spec,bool glow)
{
	int				n;
	char			*buf;

		// no lights mean no spec

	if (nlight==0) spec=FALSE;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build frag shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);
	
	strcat(buf,"uniform lowp sampler2D dim3Tex;\n");
	if (bump) strcat(buf,"uniform lowp sampler2D dim3TexBump;\n");
	if (spec) strcat(buf,"uniform lowp sampler2D dim3TexSpecular;\n");
	if (glow) strcat(buf,"uniform lowp sampler2D dim3TexGlow;\n");
	strcat(buf,"uniform lowp sampler2D dim3TexLightMap;\n");

	strcat(buf,"uniform lowp float dim3Alpha;\n");
	if (spec) strcat(buf,"uniform lowp float dim3ShineFactor;\n");
	if (glow) strcat(buf,"uniform lowp float dim3GlowFactor;\n");
	
	strcat(buf,"uniform lowp vec3 dim3AmbientColor;\n");
	if (fog) strcat(buf,"uniform lowp vec3 dim3FogColor;\n");
	
	strcat(buf,"varying mediump vec2 uv,lightMapUV;\n");
	strcat(buf,"varying mediump vec3 dirNormal;\n");
	if (fog) strcat(buf,"varying mediump float fogFactor;\n");
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying highp vec3 lightVector_%d;\n",n);
		if (bump) sprintf(strchr(buf,0),"varying highp vec3 lightVertexVector_%d;\n",n);
	}

	if (spec) strcat(buf,"varying mediump vec3 eyeVector;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	
	strcat(buf,"highp float dist;\n");
	strcat(buf,"lowp float att;\n");

		// the ambient + light map
		// lightmap uv always 0..1

	strcat(buf,"lowp vec3 ambient=dim3AmbientColor+texture2D(dim3TexLightMap,lightMapUV).rgb;\n");
	
		// the texture map
		
	strcat(buf,"lowp vec4 tex=texture2D(dim3Tex,uv);\n");
		
		// the bump map
		// note we include an all over ambient (no directional) bump value first
		
	if (bump) {
		strcat(buf,"highp vec3 bumpLightVertexVector;\n");
		strcat(buf,"lowp vec3 bumpMap=normalize((texture2D(dim3TexBump,uv).rgb*2.0)-1.0);\n");
		strcat(buf,"lowp float bump=dot(vec3(0.33,0.33,0.33),bumpMap);\n");
	}
	
		// the spec map
		
	if (spec) {
		strcat(buf,"lowp vec3 spec=vec3(0.0,0.0,0.0),specHalfVector;\n");
		strcat(buf,"lowp vec3 specMap=texture2D(dim3TexSpecular,uv).rgb;\n");
		strcat(buf,"lowp float specFactor;\n");
	}
	
		// the texture lighting

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector_%d);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3Light_%d.intensity) {\n",n);
		sprintf(strchr(buf,0)," if (dot(dirNormal,dim3Light_%d.direction)>=0.0) {\n",n);
		sprintf(strchr(buf,0),"  att=1.0-(dist*dim3Light_%d.invertIntensity);\n",n);
		sprintf(strchr(buf,0),"  att+=pow(att,dim3Light_%d.exponent);\n",n);
		sprintf(strchr(buf,0),"  ambient+=(dim3Light_%d.color*att);\n",n);

			// per-light bump calc
			
		if (bump) {
			sprintf(strchr(buf,0),"  bumpLightVertexVector=normalize(lightVertexVector_%d);\n",n);
			sprintf(strchr(buf,0),"  bump+=(dot(bumpLightVertexVector,bumpMap)*att);\n");
		}
		
			// per-light spec count
		
		if (spec) {
			strcat(buf,"  specHalfVector=normalize(normalize(eyeVector)+bumpLightVertexVector);\n");
			strcat(buf,"  specFactor=max(dot(bumpMap,specHalfVector),0.0);\n");
			strcat(buf,"  spec+=((specMap*pow(specFactor,dim3ShineFactor))*att);\n");
		}
		
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}

		// finish the spec by making sure
		// it's dimmed in dark areas
		
	if (spec) strcat(buf,"spec=min(spec,1.0)*((ambient.r+ambient.g+ambient.b)*0.33);\n");

		// if there's a glow, calculate the
		// glow rgb

	if (glow) strcat(buf,"lowp vec3 glow=texture2D(dim3TexGlow,uv).rgb*dim3GlowFactor;\n");
	
		// if we have a bump, add it into
		// the ambient and make sure it's never
		// less than 10% of the map ambient

	if (bump) strcat(buf,"ambient=max((ambient*bump),(dim3AmbientColor*0.9));\n");

		// output the fragment
		// and add in the spec and glow
		
	if (!fog) {
		strcat(buf,"gl_FragColor.rgb=");
	}
	else {
		strcat(buf,"lowp vec3 frag=");
	}
	
	if (spec) strcat(buf,"(");
	if (glow) strcat(buf,"(");
	strcat(buf,"tex.rgb*ambient");
	if (spec) strcat(buf,")+spec");
	if (glow) strcat(buf,")+glow");
	strcat(buf,";\n");
	
	if (fog) strcat(buf,"gl_FragColor.rgb=mix(dim3FogColor,frag,fogFactor);\n");
	
	strcat(buf,"gl_FragColor.a=tex.a*dim3Alpha;\n");

	strcat(buf,"}\n");

	return(buf);
}

bool gl_core_map_shader_create(shader_type *shader,int nlight,bool fog,bool bump,bool spec,bool glow,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_core_map_shader_build_vert(nlight,fog,bump,spec);
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_core_map_shader_build_frag(nlight,fog,bump,spec,glow);
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	sprintf(shader->name,"core_map_light:%d",nlight);
	if (fog) strcat(shader->name,"_fog");
	if (bump) strcat(shader->name,"_bump");
	if (spec) strcat(shader->name,"_spec");
	if (glow) strcat(shader->name,"_glow");
	
	sprintf(shader->vertex_name,"%s_vert",shader->name);
	sprintf(shader->fragment_name,"%s_frag",shader->name);
	
		// compile the code

	ok=gl_shader_code_compile(shader,vertex_data,fragment_data,err_str);

		// free the code

	free(vertex_data);
	free(fragment_data);
	
	return(ok);
}

/* =======================================================

      Build Single Core Liquid Shaders
      
======================================================= */

char* gl_core_liquid_shader_build_vert(int nlight)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build vert shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);

	strcat(buf,"uniform highp mat4 dim3ProjectionMatrix;\n");
	strcat(buf,"uniform highp mat4 dim3ModelViewMatrix;\n");
	strcat(buf,"uniform highp vec3 dim3CameraPosition;\n");
	strcat(buf,"attribute highp vec3 dim3Vertex;\n");
	strcat(buf,"attribute mediump vec3 dim3VertexNormal;\n");
	strcat(buf,"attribute mediump vec2 dim3VertexUV,dim3VertexLightMapUV;\n");
	
	strcat(buf,"varying mediump vec2 uv,lightMapUV;\n");
	strcat(buf,"varying mediump vec3 dirNormal;\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying highp vec3 lightVector_%d;\n",n);
	}
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	strcat(buf,"gl_Position=dim3ProjectionMatrix*dim3ModelViewMatrix*vec4(dim3Vertex,1.0);\n");
	strcat(buf,"uv=dim3VertexUV;\n");
	strcat(buf,"lightMapUV=dim3VertexLightMapUV;\n");
	
	strcat(buf,"dirNormal=normalize(dim3VertexNormal);\n");

	strcat(buf,"highp vec3 vtx=vec3(dim3ModelViewMatrix*vec4(dim3Vertex,1.0));\n");
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector_%d=dim3Light_%d.position-vtx;\n",n,n);
	}

	strcat(buf,"}\n");

	return(buf);
}

char* gl_core_liquid_shader_build_frag(int nlight)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build frag shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);
	
	strcat(buf,"uniform lowp sampler2D dim3Tex;\n");
	strcat(buf,"uniform lowp sampler2D dim3TexLightMap;\n");
	strcat(buf,"uniform lowp float dim3Alpha;\n");
	strcat(buf,"uniform lowp vec3 dim3AmbientColor;\n");
	
	strcat(buf,"varying mediump vec2 uv,lightMapUV;\n");
	strcat(buf,"varying mediump vec3 dirNormal;\n");
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying highp vec3 lightVector_%d;\n",n);
	}
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	
	strcat(buf,"highp float att,dist;\n");

		// the ambient plus light map

	strcat(buf,"lowp vec3 ambient=dim3AmbientColor+texture2D(dim3TexLightMap,lightMapUV).rgb;\n");
	
		// the texture map
		
	strcat(buf,"lowp vec4 tex=texture2D(dim3Tex,uv);\n");
	
		// the texture lighting
		
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector_%d);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3Light_%d.intensity) {\n",n);
		sprintf(strchr(buf,0)," if (dot(dirNormal,dim3Light_%d.direction)>=0.0) {\n",n);
		sprintf(strchr(buf,0),"  att=1.0-(dist*dim3Light_%d.invertIntensity);\n",n);
		sprintf(strchr(buf,0),"  att+=pow(att,dim3Light_%d.exponent);\n",n);
		sprintf(strchr(buf,0),"  ambient+=(dim3Light_%d.color*att);\n",n);
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}

		// output the fragment

	strcat(buf,"gl_FragColor.rgb=(tex.rgb*ambient);\n");
	strcat(buf,"gl_FragColor.a=tex.a*dim3Alpha;\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_core_liquid_shader_create(shader_type *shader,int nlight,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_core_liquid_shader_build_vert(nlight);
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_core_liquid_shader_build_frag(nlight);
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	sprintf(shader->name,"core_liquid_light:%d",nlight);
	sprintf(shader->vertex_name,"%s_vert",shader->name);
	sprintf(shader->fragment_name,"%s_frag",shader->name);
	
		// compile the code

	ok=gl_shader_code_compile(shader,vertex_data,fragment_data,err_str);

		// free the code

	free(vertex_data);
	free(fragment_data);

	return(ok);
}

/* =======================================================

      Build Single Core Model Shaders
      
======================================================= */

char* gl_core_model_shader_build_vert(int nlight,bool fog,bool bump,bool spec)
{
	int				n;
	char			*buf;

		// no lights mean no spec

	if (nlight==0) spec=FALSE;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build vert shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);

	strcat(buf,"uniform highp mat4 dim3ProjectionMatrix;\n");
	strcat(buf,"uniform highp mat4 dim3ModelViewMatrix;\n");
	strcat(buf,"uniform highp mat3 dim3NormalMatrix;\n");
	strcat(buf,"uniform highp vec3 dim3CameraPosition;\n");
	if (fog) {
		strcat(buf,"uniform highp float dim3FogEnd;\n");
		strcat(buf,"uniform highp float dim3FogScale;\n");
	}

	strcat(buf,"attribute highp vec3 dim3Vertex;\n");
	strcat(buf,"attribute highp vec3 dim3VertexNormal;\n");
	if ((bump) || (spec)) strcat(buf,"attribute highp vec3 dim3VertexTangent;\n");
	strcat(buf,"attribute mediump vec2 dim3VertexUV;\n");
	
	strcat(buf,"varying mediump vec2 uv;\n");
	strcat(buf,"varying mediump vec3 dirNormal;\n");
	strcat(buf,"varying mediump vec3 tangentSpaceNormal;\n");
	if (fog) strcat(buf,"varying mediump float fogFactor;\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying highp vec3 lightVector_%d;\n",n);
		if (bump) sprintf(strchr(buf,0),"varying highp vec3 lightVertexVector_%d;\n",n);
	}

	if (spec) strcat(buf,"varying mediump vec3 eyeVector;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	strcat(buf,"gl_Position=dim3ProjectionMatrix*dim3ModelViewMatrix*vec4(dim3Vertex,1.0);\n");
	strcat(buf,"uv=dim3VertexUV;\n");

	strcat(buf,"dirNormal=normalize(dim3VertexNormal);\n");
	
	strcat(buf,"highp vec3 vtx=vec3(dim3ModelViewMatrix*vec4(dim3Vertex,1.0));\n");
	
	if ((bump) || (spec)) {
		strcat(buf,"mediump vec3 tangentSpaceTangent=normalize(dim3NormalMatrix*dim3VertexTangent);\n");
		strcat(buf,"mediump vec3 tangentSpaceBinormal=normalize(dim3NormalMatrix*cross(dim3VertexNormal,dim3VertexTangent));\n");
	}
	
	strcat(buf,"tangentSpaceNormal=normalize(dim3NormalMatrix*dim3VertexNormal);\n");	// always need normal for diffuse
	
	if (spec) {
		strcat(buf,"eyeVector.x=dot(-vtx,tangentSpaceTangent);\n");
		strcat(buf,"eyeVector.y=dot(-vtx,tangentSpaceBinormal);\n");
		strcat(buf,"eyeVector.z=dot(-vtx,tangentSpaceNormal);\n");
	}
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector_%d=dim3Light_%d.position-vtx;\n",n,n);
		
		if (bump) {
			sprintf(strchr(buf,0),"lightVertexVector_%d.x=dot(lightVector_%d,tangentSpaceTangent);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector_%d.y=dot(lightVector_%d,tangentSpaceBinormal);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector_%d.z=dot(lightVector_%d,tangentSpaceNormal);\n",n,n);
		}
	}
	
	if (fog) strcat(buf,"fogFactor=clamp(((dim3FogEnd-distance(dim3Vertex,dim3CameraPosition))*dim3FogScale),0.0,1.0);\n");
	
	strcat(buf,"}\n");

	return(buf);
}

char* gl_core_model_shader_build_frag(int nlight,bool fog,bool bump,bool spec,bool glow)
{
	int				n;
	char			*buf;

		// no lights mean no spec

	if (nlight==0) spec=FALSE;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// need to define out lowp/mediump/highp
		// for non ES2 shaders

	gl_core_shader_build_generic_precision_defines(buf);

		// build frag shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);
	
	strcat(buf,"uniform lowp sampler2D dim3Tex;\n");
	if (bump) strcat(buf,"uniform lowp sampler2D dim3TexBump;\n");
	if (spec) strcat(buf,"uniform lowp sampler2D dim3TexSpecular;\n");
	if (glow) strcat(buf,"uniform lowp sampler2D dim3TexGlow;\n");

	strcat(buf,"uniform lowp float dim3Alpha;\n");
	if (spec) strcat(buf,"uniform lowp float dim3ShineFactor;\n");
	if (glow) strcat(buf,"uniform lowp float dim3GlowFactor;\n");
	
	strcat(buf,"uniform lowp vec3 dim3AmbientColor;\n");
	if (fog) strcat(buf,"uniform lowp vec3 dim3FogColor;\n");
	strcat(buf,"uniform mediump vec3 dim3DiffuseVector;\n");
	
	strcat(buf,"varying mediump vec2 uv;\n");
	strcat(buf,"varying mediump vec3 dirNormal;\n");
	strcat(buf,"varying mediump vec3 tangentSpaceNormal;\n");
	if (fog) strcat(buf,"varying mediump float fogFactor;\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying highp vec3 lightVector_%d;\n",n);
		if (bump) sprintf(strchr(buf,0),"varying highp vec3 lightVertexVector_%d;\n",n);
	}

	if (spec) strcat(buf,"varying mediump vec3 eyeVector;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	
	strcat(buf,"highp float att,dist;\n");
	strcat(buf,"lowp vec3 ambient=dim3AmbientColor;\n");
	
		// the texture map
		
	strcat(buf,"lowp vec4 tex=texture2D(dim3Tex,uv);\n");
	
		// the bump map
		// note we include an all over ambient (no directional) bump value first
		
	if (bump) {
		strcat(buf,"highp vec3 bumpLightVertexVector;\n");
		strcat(buf,"mediump vec3 bumpMap=normalize((texture2D(dim3TexBump,uv).rgb*2.0)-1.0);\n");
		strcat(buf,"mediump float bump=dot(vec3(0.33,0.33,0.33),bumpMap);\n");
	}
	
		// the spec map
		
	if (spec) {
		strcat(buf,"mediump vec3 spec=vec3(0.0,0.0,0.0),specHalfVector;\n");
		strcat(buf,"lowp vec3 specMap=texture2D(dim3TexSpecular,uv).rgb;\n");
		strcat(buf,"mediump float specFactor;\n");
	}
			
		// diffuse
		
	strcat(buf,"lowp float diffuse=(dot(dim3DiffuseVector,tangentSpaceNormal)+1.0)*0.5;\n");
	
		// the texture lighting
		
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector_%d);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3Light_%d.intensity) {\n",n);
		sprintf(strchr(buf,0)," if (dot(dirNormal,dim3Light_%d.direction)>=0.0) {\n",n);
		sprintf(strchr(buf,0),"  att=1.0-(dist*dim3Light_%d.invertIntensity);\n",n);
		sprintf(strchr(buf,0),"  att+=pow(att,dim3Light_%d.exponent);\n",n);
		sprintf(strchr(buf,0),"  ambient+=(dim3Light_%d.color*att);\n",n);
		
			// per light bump calc
			
		if (bump) {
			sprintf(strchr(buf,0),"  bumpLightVertexVector=normalize(lightVertexVector_%d);\n",n);
			strcat(buf,"  bump+=(dot(bumpLightVertexVector,bumpMap)*att);\n");
		}
		
			// per light spec calc
			
		if (spec) {
			strcat(buf,"  specHalfVector=normalize(normalize(eyeVector)+bumpLightVertexVector);\n");
			strcat(buf,"  specFactor=max(dot(bumpMap,specHalfVector),0.0);\n");
			strcat(buf,"  spec+=((specMap*pow(specFactor,dim3ShineFactor))*att);\n");
		}
		
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}

		// if there's a glow, calculate the
		// glow rgb

	if (glow) strcat(buf,"lowp vec3 glow=texture2D(dim3TexGlow,uv).rgb*dim3GlowFactor;\n");

		// create the ambient with ambient*diffuse*bump
		// and make sure it's never
		// less than 10% of the map ambient

	strcat(buf,"ambient=max((ambient*diffuse");
	if (bump) strcat(buf,"*bump");
	strcat(buf,"),(dim3AmbientColor*0.9));\n");

		// output the fragment
		// and add in the spec and glow

	if (!fog) {
		strcat(buf,"gl_FragColor.rgb=");
	}
	else {
		strcat(buf,"lowp vec3 frag=");
	}

	if (spec) strcat(buf,"(");
	if (glow) strcat(buf,"(");
	strcat(buf,"tex.rgb*ambient");
	if (spec) strcat(buf,")+spec");
	if (glow) strcat(buf,")+glow");
	strcat(buf,";\n");
	
	if (fog) strcat(buf,"gl_FragColor.rgb=mix(dim3FogColor,frag,fogFactor);\n");

	strcat(buf,"gl_FragColor.a=tex.a*dim3Alpha;\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_core_model_shader_create(shader_type *shader,int nlight,bool fog,bool bump,bool spec,bool glow,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_core_model_shader_build_vert(nlight,fog,bump,spec);
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_core_model_shader_build_frag(nlight,fog,bump,spec,glow);
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	sprintf(shader->name,"core_model_light:%d",nlight);
	if (fog) strcat(shader->name,"_fog");
	if (bump) strcat(shader->name,"_bump");
	if (spec) strcat(shader->name,"_spec");
	if (glow) strcat(shader->name,"_glow");
	
	sprintf(shader->vertex_name,"%s_vert",shader->name);
	sprintf(shader->fragment_name,"%s_frag",shader->name);
	
		// compile the code

	ok=gl_shader_code_compile(shader,vertex_data,fragment_data,err_str);

		// free the code

	free(vertex_data);
	free(fragment_data);

	return(ok);
}

/* =======================================================

      Core Shader Initialize/Shutdown
      
======================================================= */

bool gl_core_shader_initialize_per_light(int nlight,char *err_str)
{
		//
		// core model shaders
		//

		// gl_core_model_shader_light

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_light],nlight,FALSE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_model_shader_light_bump

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_light_bump],nlight,FALSE,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_light_bump_spec

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_light_bump_spec],nlight,FALSE,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_light_glow

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_light_glow],nlight,FALSE,FALSE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_model_shader_light_bump_glow

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_light_bump_glow],nlight,FALSE,TRUE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_light_bump_spec_glow

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_light_bump_spec_glow],nlight,FALSE,TRUE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_fog_light

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_fog_light],nlight,TRUE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_fog_light_bump

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_fog_light_bump],nlight,TRUE,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_fog_light_bump_spec

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_fog_light_bump_spec],nlight,TRUE,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_fog_light_glow

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_fog_light_glow],nlight,TRUE,FALSE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_fog_light_bump_glow

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_fog_light_bump_glow],nlight,TRUE,TRUE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_fog_light_bump_spec_glow

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_fog_light_bump_spec_glow],nlight,TRUE,TRUE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		//
		// core map shaders
		//

		// gl_core_map_shader_light_map

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map],nlight,FALSE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_light_map_bump

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map_bump],nlight,FALSE,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_light_map_bump_spec

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map_bump_spec],nlight,FALSE,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_light_map_glow

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map_glow],nlight,FALSE,FALSE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_light_map_bump_glow

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map_bump_glow],nlight,FALSE,TRUE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_light_map_bump_spec_glow

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map_bump_spec_glow],nlight,FALSE,TRUE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_fog_light_map

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map],nlight,TRUE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_fog_light_map_bump

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map_bump],nlight,TRUE,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_fog_light_map_bump_spec

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map_bump_spec],nlight,TRUE,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_fog_light_map_glow

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map_glow],nlight,TRUE,FALSE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_fog_light_map_bump_glow

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map_bump_glow],nlight,TRUE,TRUE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_fog_light_map_bump_spec_glow

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map_bump_spec_glow],nlight,TRUE,TRUE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		//
		// core liquid shaders
		//

		// gl_core_map_shader_liquid

	if (!gl_core_liquid_shader_create(&core_shaders[nlight][gl_core_map_shader_liquid],nlight,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
	return(TRUE);
}

bool gl_core_shader_initialize(char *err_str)
{
	int					n,k;
	
		// note: we use max_shader_light+1 as
		// the first spot is 0 lights, and then 1
		// through max_shader_light

		// clear core shaders

	for (k=0;k!=(max_shader_light+1);k++) {
		for (n=0;n!=max_core_shader;n++) {
			gl_shader_code_clear(&core_shaders[k][n]);
		}
	}

		// initialize light varients of shaders		
		
	for (k=0;k!=(max_shader_light+1);k++) {
		if (!gl_core_shader_initialize_per_light(k,err_str)) return(FALSE);
	}	
	
	return(TRUE);
}

void gl_core_shader_shutdown(void)
{
	int					n,k;

		// shutdown shaders

	for (k=0;k!=(max_shader_light+1);k++) {
		for (n=0;n!=max_core_shader;n++) {
			gl_shader_code_shutdown(&core_shaders[k][n]);
		}
	}
}

/* =======================================================

      Core Shader Utilities
      
======================================================= */

static inline int gl_core_shader_find_for_mode(int core_shader_group,texture_type *texture)
{
	bool				bump,spec,glow;

		// liquid shaders

	if (core_shader_group==core_shader_group_liquid) return(gl_core_map_shader_liquid);
	
		// are bump, spec, and glow on?
		
	bump=(texture->frames[0].bumpmap.gl_id!=-1);
	spec=(texture->frames[0].specularmap.gl_id!=-1);
	glow=(texture->frames[0].glowmap.gl_id!=-1);

		// spec requires a bump, so turn
		// it off if no bump
		
	if ((spec) && (!bump)) bump=spec=FALSE;

		// model shaders

	if (core_shader_group==core_shader_group_model) {
		
		if (!fog_solid_on()) {
			if (!glow) {
				if ((bump) && (spec)) return(gl_core_model_shader_light_bump_spec);
				if ((bump) && (!spec)) return(gl_core_model_shader_light_bump);
				return(gl_core_model_shader_light);
			}
			else {
				if ((bump) && (spec)) return(gl_core_model_shader_light_bump_spec_glow);
				if ((bump) && (!spec)) return(gl_core_model_shader_light_bump_glow);
				return(gl_core_model_shader_light_glow);
			}
		}

		if (!glow) {
			if ((bump) && (spec)) return(gl_core_model_shader_fog_light_bump_spec);
			if ((bump) && (!spec)) return(gl_core_model_shader_fog_light_bump);
			return(gl_core_model_shader_fog_light);
		}
		else {
			if ((bump) && (spec)) return(gl_core_model_shader_fog_light_bump_spec_glow);
			if ((bump) && (!spec)) return(gl_core_model_shader_fog_light_bump_glow);
			return(gl_core_model_shader_fog_light_glow);
		}
	}
	
		// map shaders
		
	if (!fog_solid_on()) {
		if (!glow) {
			if ((bump) && (spec)) return(gl_core_map_shader_light_map_bump_spec);
			if ((bump) && (!spec)) return(gl_core_map_shader_light_map_bump);
			return(gl_core_map_shader_light_map);
		}
		else {
			if ((bump) && (spec)) return(gl_core_map_shader_light_map_bump_spec_glow);
			if ((bump) && (!spec)) return(gl_core_map_shader_light_map_bump_glow);
			return(gl_core_map_shader_light_map_glow);
		}
	}
	
	if (!glow) {
		if ((bump) && (spec)) return(gl_core_map_shader_fog_light_map_bump_spec);
		if ((bump) && (!spec)) return(gl_core_map_shader_fog_light_map_bump);
		return(gl_core_map_shader_fog_light_map);
	}
	else {
		if ((bump) && (spec)) return(gl_core_map_shader_fog_light_map_bump_spec_glow);
		if ((bump) && (!spec)) return(gl_core_map_shader_fog_light_map_bump_glow);
		return(gl_core_map_shader_fog_light_map_glow);
	}
}

shader_type* gl_core_shader_find_ptr(int nlight,int core_shader_group,texture_type *texture)
{
	int				which_varient;

	which_varient=gl_core_shader_find_for_mode(core_shader_group,texture);
	return(&core_shaders[nlight][which_varient]);
}

