/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model OBJ importer

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern model_type				model;
extern app_state_type			state;

/* =======================================================

      Import OBJ Re-Rig Vertexes
      
======================================================= */

void model_import_obj_rerig_vertexes(model_mesh_type *mesh,int old_nvertex,model_vertex_type *old_vertex)
{
	int						n,k,v_idx,
							cur_dist,dist;
	model_vertex_type		*v1,*v2;
		
		// run through and re-rig the
		// new vertexes

	v1=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
	
			// find the closest (by
			// distance) older vertex

		v_idx=-1;
		cur_dist=10000;

		v2=old_vertex;
	
		for (k=0;k!=old_nvertex;k++) {

			dist=distance_get(v1->pnt.x,v1->pnt.y,v1->pnt.z,v2->pnt.x,v2->pnt.y,v2->pnt.z);
			if (dist<cur_dist) {
				cur_dist=dist;
				v_idx=k;
			}

			v2++;
		}

			// set it like closest
			// vertex

		if (v_idx!=-1) {
			v2=&old_vertex[v_idx];
			v1->major_bone_idx=v2->major_bone_idx;
			v1->minor_bone_idx=v2->minor_bone_idx;
			v1->bone_factor=v2->bone_factor;
		}
		
		v1++;
	}
}

/* =======================================================

      Import OBJ File Replace
      
======================================================= */

float model_import_obj_replace_factor(float f_ty,float f_by)
{
	int			lx,rx,lz,rz,ty,by;
	
	model_get_vertex_extent(&model,state.model.cur_mesh_idx,&lx,&rx,&lz,&rz,&ty,&by);
	return(((float)(by-ty))/fabsf(f_by-f_ty));
}

/* =======================================================

      Import OBJ File
      
======================================================= */

bool model_import_obj(char *path,bool replace,bool *found_normals,char *err_str)
{
	int						n,k,i,idx,nvertex,npoly,nuv,nnormal,nline,nmaterial,
							texture_idx,high,ptsz,old_nvertex,sz;
	int						*poly_normal_count;
	float					f_count,fy,f_ty,f_by,import_factor;
	char					txt[256],*c,vstr[256],vtstr[256],vnstr[256],
							material_name[256],
							material_list[max_model_texture][name_str_len];
	bool					hit_material,in_material,all_material_exists,face_error;
	d3uv					*uv,*uv_ptr;
	d3vct					*normal,*normal_ptr;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex,*old_vertex;
    model_poly_type			*poly;
	model_poly_type			temp_poly;
	
	mesh=&model.meshes[state.model.cur_mesh_idx];
	
		// load the file
		
	if (!textdecode_open(path,0x20)) {
		strcpy(err_str,"Couldn't open file.");
		return(FALSE);
    }
	
	progress_start("OBJ Import",(replace?4:3));
	progress_next_title("Obj Import: Parsing OBJ");
	
    nline=textdecode_count();
	
		// count materials and get a list
		// and see if we already have a material
		// for all the textures
		
	nmaterial=0;
	all_material_exists=TRUE;
	
    for (n=0;n!=nline;n++) {
        textdecode_get_piece(n,0,txt);
        if (strcmp(txt,"usemtl")==0) {
		
            textdecode_get_piece(n,1,material_name);
			material_name[name_str_len-1]=0x0;

				// already doing this material?

			hit_material=FALSE;

			for (i=0;i!=nmaterial;i++) {
				if (strcasecmp(material_list[i],material_name)==0) {
					hit_material=TRUE;
					break;
				}
			}

			if (!hit_material) {
				strcpy(material_list[nmaterial],material_name);
				nmaterial++;
			}

			all_material_exists&=model_texture_exists(material_name);
		}
	}

		// can't import if no textures
		
	if (nmaterial==0) {
		progress_end();
		textdecode_close();
		strcpy(err_str,"OBJ contains no materials.  Animator can only import texture-mapped OBJs.");
		return(FALSE);
	}
	
		// can't import if too many materials?
		
	if ((nmaterial+model_texture_count())>=max_model_texture) {
		progress_end();
		textdecode_close();
		sprintf(err_str,"Too many materials, models can have a maximum of %d materials.",max_model_texture);
		return(FALSE);
	}
	
		// count the vertexes, uvs, normals, and polys
		// and find the total height
		
	nvertex=0;
	nuv=0;
	nnormal=0;
	npoly=0;

	face_error=FALSE;
	
	f_ty=f_by=0;

    for (n=0;n!=nline;n++) {
        textdecode_get_piece(n,0,txt);
		
        if (strcmp(txt,"v")==0) {
			
			textdecode_get_piece(n,2,txt);
			fy=(float)strtod(txt,NULL);
			
			if (nvertex==0) {
				f_ty=f_by=fy;
			}
			else {
				if (fy<f_ty) f_ty=fy;
				if (fy>f_by) f_by=fy;
			}
			
			nvertex++;
			continue;
		}
		
        if (strcmp(txt,"vt")==0) {
			nuv++;
			continue;
		}
		
        if (strcmp(txt,"vn")==0) {
			nnormal++;
			continue;
		}

		if (strcmp(txt,"f")==0) {

				// count vertexes
				// to find bad faces

			ptsz=0;
	        
			for (k=0;k!=8;k++) {
				textdecode_get_piece(n,(k+1),txt);
				if (txt[0]==0x0) break;
				ptsz++;
			}

			if (ptsz<3) {
				face_error=TRUE;
				continue;
			}

			npoly++;
			continue;
		}
	}
	
		// can't import if no UVs
		
	if (nuv==0) {
		progress_end();
		textdecode_close();
		sprintf(err_str,"There are no UVs in this OBJ, please texture map the model before importing.");
		return(FALSE);
	}

		// face errors

	if (face_error) os_dialog_alert("OBJ Warning","There were polygons with less than 3 vertexes in this OBJ.  Those polygons were ignored.");
	
		// get the scale if not a replace
		
	if (!replace) {
		high=dialog_import_finish_run();
		import_factor=((float)high)/fabsf(f_by-f_ty);
	}

		// if a replacement, remember the old
		// vertexes so we can reattach bones
		
	else {
		import_factor=model_import_obj_replace_factor(f_ty,f_by);
		
		old_nvertex=mesh->nvertex;
		sz=sizeof(model_vertex_type)*old_nvertex;
		old_vertex=(model_vertex_type*)malloc(sz);
		memmove(old_vertex,mesh->vertexes,sz);
	}
	
		// model memory
		
	progress_next_title("Obj Import: Building New Model");
		
	model_mesh_set_vertex_count(&model,state.model.cur_mesh_idx,nvertex);
	model_mesh_set_poly_count(&model,state.model.cur_mesh_idx,npoly);
	uv_ptr=(d3uv*)malloc(nuv*sizeof(d3uv));
	
	if (nnormal==0) {
		normal_ptr=(d3vct*)malloc(sizeof(d3vct));
	}
	else {
		normal_ptr=(d3vct*)malloc(nnormal*sizeof(d3vct));
	}
	
		// get the vertexes, uv, and normals

	vertex=mesh->vertexes;
    
    uv=uv_ptr;
	normal=normal_ptr;
   
    for (n=0;n!=nline;n++) {

        textdecode_get_piece(n,0,txt);
      
            // a vertex
            
        if (strcmp(txt,"v")==0) {
			textdecode_get_piece(n,1,txt);
			vertex->pnt.x=-(int)(strtod(txt,NULL)*import_factor);
			textdecode_get_piece(n,2,txt);
			vertex->pnt.y=-(int)(strtod(txt,NULL)*import_factor);
			textdecode_get_piece(n,3,txt);
			vertex->pnt.z=-(int)(strtod(txt,NULL)*import_factor);
            
            vertex->major_bone_idx=vertex->minor_bone_idx=-1;
            vertex->bone_factor=1;
			
			vertex->tangent_space.normal.x=vertex->tangent_space.normal.y=vertex->tangent_space.normal.z=0.0f;
 			vertex->tangent_space.tangent.x=vertex->tangent_space.tangent.y=vertex->tangent_space.tangent.z=0.0f;
      
            vertex++;
        }
        else {
        
            // a uv
            
            if (strcmp(txt,"vt")==0) {
                textdecode_get_piece(n,1,txt);
                uv->x=(float)strtod(txt,NULL);
                textdecode_get_piece(n,2,txt);
                uv->y=(float)strtod(txt,NULL);
                
 				uv++;
            }
			else {
			
				// a normal
				
				if (strcmp(txt,"vn")==0) {
					textdecode_get_piece(n,1,txt);
					normal->x=-(float)strtod(txt,NULL);
					textdecode_get_piece(n,2,txt);
					normal->y=-(float)strtod(txt,NULL);
					textdecode_get_piece(n,3,txt);
					normal->z=-(float)strtod(txt,NULL);
					
					normal++;
				}
			}
        }
    }

	*found_normals=(nnormal!=0);
	
		// a count for averaging the
		// normals from the polys into
		// the vertexes
		
	poly_normal_count=(int*)malloc(sizeof(int)*nvertex);
	bzero(poly_normal_count,(sizeof(int)*nvertex));

		// get the polygons

	poly=mesh->polys;

	for (i=0;i!=nmaterial;i++) {

            // get texture
  
		texture_idx=model_texture_pick(material_list[i],err_str);
		if (texture_idx==-1) {
			progress_end();
			textdecode_close();
			return(FALSE);
		}

		in_material=FALSE;

			// run through the polys

		for (n=0;n!=nline;n++) {

			textdecode_get_piece(n,0,txt);
	        
				// material change
	            
			if (strcmp(txt,"usemtl")==0) {
				textdecode_get_piece(n,1,material_name);
				material_name[name_str_len-1]=0x0;

				in_material=(strcasecmp(material_name,material_list[i])==0);
				continue;
			}

			if (!in_material) continue;

				// a face
	            
			if (strcmp(txt,"f")!=0) continue;
	        
				// create the poly

			temp_poly.txt_idx=texture_idx;
			
			ptsz=0;
	        
			for (k=0;k!=8;k++) {
				textdecode_get_piece(n,(k+1),txt);
				if (txt[0]==0x0) break;
	            
					// seperate into vertex, UV, and normal
					
				vtstr[0]=0x0;
				vnstr[0]=0x0;
	            
				strcpy(vstr,txt);
				c=strchr(vstr,'/');
				if (c!=NULL) {
					strcpy(vtstr,(c+1));
					*c=0x0;
				}
				c=strchr(vtstr,'/');
				if (c!=NULL) {
					strcpy(vnstr,(c+1));
					*c=0x0;
				}
				
				temp_poly.v[ptsz]=atoi(vstr);
				if (temp_poly.v[ptsz]>0) {
					temp_poly.v[ptsz]--;
				}
				else {
					temp_poly.v[ptsz]=nvertex+temp_poly.v[ptsz];
				}
	            
				if (vtstr[0]==0x0) {
					temp_poly.gx[ptsz]=temp_poly.gy[ptsz]=0.0f;
				}
				else {
					idx=atoi(vtstr);
					if (idx>0) {
						idx--;
					}
					else {
						idx=nuv+idx;
					}
					
					temp_poly.gx[ptsz]=uv_ptr[idx].x;
					temp_poly.gy[ptsz]=1.0f-uv_ptr[idx].y;
				}
					
				if (vnstr[0]!=0x0) {
					idx=atoi(vnstr);
					if (idx>0) {
						idx--;
					}
					else {
						idx=nnormal+idx;
					}

					vertex=&mesh->vertexes[temp_poly.v[ptsz]];
					
					vertex->tangent_space.normal.x+=normal_ptr[idx].x;
					vertex->tangent_space.normal.y+=normal_ptr[idx].y;
					vertex->tangent_space.normal.z+=normal_ptr[idx].z;
					
					poly_normal_count[temp_poly.v[ptsz]]++;
				}
				
				ptsz++;
			}

				// skip faces with <3 vertexes

			if (ptsz<3) continue;

				// create the new polygon

			temp_poly.ptsz=ptsz;
			memmove(poly,&temp_poly,sizeof(model_poly_type));

			poly++;
		}
	}
		
		// average the vertex normals
		
	if (nnormal!=0) {
	
		vertex=mesh->vertexes;
		
		for (n=0;n!=mesh->nvertex;n++) {

			if (poly_normal_count[n]!=0) {
				f_count=(float)poly_normal_count[n];
				
				vertex->tangent_space.normal.x/=f_count;
				vertex->tangent_space.normal.y/=f_count;
				vertex->tangent_space.normal.z/=f_count;
			}
			
			vector_normalize(&vertex->tangent_space.normal);

			vertex++;
		}
		
	}

	free(poly_normal_count);
	
	free(uv_ptr);
	free(normal_ptr);
	
	textdecode_close();
	
		// finish up
		
	model_vertex_delete_unused_vertexes(state.model.cur_mesh_idx);

	model_center_xz(&model,state.model.cur_mesh_idx);
	model_floor(&model,state.model.cur_mesh_idx);
	model_recalc_boxes(&model);
    model_recalc_normals(&model,(*found_normals));

		// if replace, fix by moving
		// any user mesh moves

	if (replace) {
		vertex=mesh->vertexes;

		for (n=0;n!=mesh->nvertex;n++) {
			vertex->pnt.x+=mesh->import_move.x;
 			vertex->pnt.y+=mesh->import_move.y;
			vertex->pnt.z+=mesh->import_move.z;
       
            vertex++;
        }
	}
	else {
		mesh->import_move.x=0;
		mesh->import_move.y=0;
		mesh->import_move.z=0;
	}

		// if replacement, fix all the
		// bone attachments
		
	if (replace) {
		progress_next_title("Obj Import: Fixing Bone Rigging");
		model_import_obj_rerig_vertexes(mesh,old_nvertex,old_vertex);
		free(old_vertex);
	}
	
	progress_end();

	return(TRUE);
}

