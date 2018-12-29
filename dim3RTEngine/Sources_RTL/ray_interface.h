//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

	// types

#ifndef _ray_types
	#include "ray_types.h"
#endif

	// scene targets

#define RL_SCENE_TARGET_MEMORY								0
#define RL_SCENE_TARGET_OPENGL_TEXTURE						1

	// scene formats

#define RL_SCENE_FORMAT_32_RGBA								0

	// scene states

#define RL_SCENE_STATE_IDLE									0
#define RL_SCENE_STATE_RENDERING							1

	// material alpha types

#define RL_MATERIAL_ALPHA_PASS_THROUGH						0
#define RL_MATERIAL_ALPHA_REFLECT							1
#define RL_MATERIAL_ALPHA_REFRACT							2
#define RL_MATERIAL_ALPHA_ADDITIVE							3

	// material buffer targets

#define RL_MATERIAL_TARGET_COLOR							0
#define RL_MATERIAL_TARGET_NORMAL							1
#define RL_MATERIAL_TARGET_SPECULAR							2
#define RL_MATERIAL_TARGET_GLOW								3

	// material formats

#define RL_MATERIAL_FORMAT_32_RGBA							0
#define RL_MATERIAL_FORMAT_24_RGB							1

	// mesh array formats

#define RL_MESH_FORMAT_VERTEX_3_FLOAT						0
#define RL_MESH_FORMAT_UV_2_FLOAT							0
#define RL_MESH_FORMAT_NORMAL_3_FLOAT						0
#define RL_MESH_FORMAT_TANGENT_3_FLOAT						0

#define RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL_TANGENT	0
#define RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV					1

	// mesh flags

#define RL_MESH_FLAG_NONE									0x00000000
#define RL_MESH_FLAG_NON_RAY_TRACE_BLOCKING					0x00000001
#define RL_MESH_FLAG_NON_BOUNCE_TRACE_BLOCKING				0x00000002
#define RL_MESH_FLAG_NON_LIGHT_TRACE_BLOCKING				0x00000004
#define RL_MESH_FLAG_HIGHLIGHT								0x00000008

	// errors

#define RL_ERROR_OK											0
#define RL_ERROR_UNKNOWN_SCENE_ID							-1
#define RL_ERROR_UNKNOWN_MATERIAL_ID						-2
#define RL_ERROR_UNKNOWN_LIGHT_ID							-3
#define RL_ERROR_UNKNOWN_MESH_ID							-4
#define RL_ERROR_MESH_POLY_INDEX_OUT_OF_BOUNDS				-5
#define RL_ERROR_OUT_OF_MEMORY								-6
#define RL_ERROR_UNKNOWN_TARGET								-7
#define RL_ERROR_UNKNOWN_FORMAT								-8
#define RL_ERROR_UNKNOWN_ALPHA_TYPE							-9
#define RL_ERROR_MATERIAL_ATTACHED_TO_MESH					-10
#define RL_ERROR_THREADING_ERROR							-11
#define RL_ERROR_POINT_BEHIND_PLANE							-12
#define RL_ERROR_TOO_MANY_MATERIALS							-13

//
// functions
//

	// initialize and shutdown
	
extern int rtlInitialize(void);
extern int rtlShutdown(void);

	// materials

extern int rtlMaterialAdd(int wid,int high,int alphaType,unsigned long flags);
extern int rtlMaterialCount(void);
extern int rtlMaterialDelete(int materialId);
extern int rtlMaterialDeleteAll(void);
extern int rtlMaterialAttachBufferData(int materialId,int target,int format,unsigned char* data);
extern int rtlMaterialAttachBufferColor(int materialId,int target,rtlColor *col);
extern int rtlMaterialSetShineFactor(int materialId,float shineFactor);
extern int rtlMaterialSetGlowFactor(int materialId,float glowFactor);
extern int rtlMaterialSetRefractionFactor(int materialId,float refractionFactor);
extern int rtlMaterialBuildMipMaps(int materialId);

	// scenes

extern int rtlSceneAdd(rtl2DPoint *size,int target,int format,void *attachment,unsigned long flags);
extern int rtlSceneCount(void);
extern int rtlSceneDelete(int sceneId);
extern int rtlSceneClearBuffer(int sceneId,rtlColor *col);
extern int rtlSceneGetBuffer(int sceneId,void **buffer);
extern int rtlSceneRender(int sceneId);
extern int rtlSceneRenderState(int sceneId);
extern int rtlSceneRenderFinish(int sceneId);

	// scene eye position
	
extern int rtlSceneEyePositionSet(int sceneId,rtlPoint *pnt,float fovy,rtlMatrix *rotMatrix,float eyeMaxDist);
extern int rtlSceneEyeTranslatePoint(int sceneId,ray_point_type *pnt3d,ray_2d_point_type *pnt2d);

	// scene lights

extern int rtlSceneAmbient(int sceneId,rtlColor *col);
extern int rtlSceneLightAdd(int sceneId);
extern int rtlSceneLightCount(int sceneId);
extern int rtlSceneLightDelete(int sceneId,int lightId);
extern int rtlSceneLightDeleteAll(int scenedId);

extern int rtlSceneLightSetHidden(int sceneId,int lightId,bool hidden);

extern int rtlSceneLightSetPosition(int sceneId,int lightId,rtlPoint *pnt);
extern int rtlSceneLightSetColor(int sceneId,int lightId,rtlColor *col);
extern int rtlSceneLightSetIntensity(int sceneId,int lightId,float intensity,float exponent);
extern int rtlSceneLightSetDirection(int sceneId,int lightId,rtlVector *vector,float angle,bool active);

	// scene meshes

extern int rtlSceneMeshAdd(int sceneId,unsigned long flags);
extern int rtlSceneMeshCount(int sceneId);
extern int rtlSceneMeshDelete(int sceneId,int meshId);
extern int rtlSceneMeshDeleteAll(int scenedId);

extern int rtlSceneMeshSetHidden(int sceneId,int meshId,bool hidden);

extern int rtlSceneMeshSetVertex(int sceneId,int meshId,int format,int count,void *vertex_data);
extern int rtlSceneMeshMapVertexPointer(int sceneId,int meshId,void **vertex_data);
extern int rtlSceneMeshUnMapVertexPointer(int sceneId,int meshId);

extern int rtlSceneMeshSetUV(int sceneId,int meshId,int format,int count,void *uv_data);
extern int rtlSceneMeshMapUVPointer(int sceneId,int meshId,void **uv_data);
extern int rtlSceneMeshUnMapUVPointer(int sceneId,int meshId);

extern int rtlSceneMeshSetNormal(int sceneId,int meshId,int format,int count,void *normal_data);
extern int rtlSceneMeshMapNormalPointer(int sceneId,int meshId,void **normal_data);
extern int rtlSceneMeshUnMapNormalPointer(int sceneId,int meshId);

extern int rtlSceneMeshSetTangent(int sceneId,int meshId,int format,int count,void *tangent_data);
extern int rtlSceneMeshMapTangentPointer(int sceneId,int meshId,void **tangent_data);
extern int rtlSceneMeshUnMapTangentPointer(int sceneId,int meshId);

extern int rtlSceneMeshSetPoly(int sceneId,int meshId,int format,int count,void *poly_data);

extern int rtlSceneMeshSetPolyColor(int sceneId,int meshId,int poly_idx,rtlColor *col);
extern int rtlSceneMeshSetPolyColorAll(int sceneId,int meshId,rtlColor *col);

	// math utilities

extern void rtlMatrixIdentity(rtlMatrix *mat);
extern void rtlMatrixMultiply(rtlMatrix *mat_1,rtlMatrix *mat_2);
extern void rtlMatrixVectorMultiply(rtlMatrix *mat,rtlVector *v);
extern void rtlMatrixRotateX(rtlMatrix *mat,float ang);
extern void rtlMatrixRotateY(rtlMatrix *mat,float ang);
extern void rtlMatrixRotateZ(rtlMatrix *mat,float ang);
extern void rtlMatrixScale(rtlMatrix *mat,rtlVector *v);
extern void rlMatrixTranpose(rtlMatrix *mat);
