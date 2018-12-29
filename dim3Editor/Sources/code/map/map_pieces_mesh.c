/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Mesh

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;
extern app_state_type			state;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Add OBJ Mesh
      
======================================================= */

void piece_add_obj_mesh(void)
{
	char					path[1024],err_str[256];
	
	if (!piece_create_texture_ok()) return;
	
		// import the file
		
	if (!os_load_file("Select an OBJ file",path,"obj")) return;
	
	if (!map_import_obj(path,err_str)) {
		os_dialog_alert("Import Failed",err_str);
		return;
    }
}

/* =======================================================

      Add OBJ Mesh UV
      
======================================================= */

void piece_add_obj_mesh_uv(void)
{
	int						n,k,type,nline,nvertex,npoly,nuv,npt,uv_idx,
							mesh_idx,poly_idx;
	char					*c,txt[256],vstr[256],uvstr[256],path[1024];
	float					fx,fy,fz;
	float					*uvs,*uv;
	d3fpnt					min,max;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_mesh_poly_uv_type	*uv_ptr;
	
		// get mesh to replace UVs on
		
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=item_map_mesh) {
		os_dialog_alert("No mesh selected","You need to select a mesh to replace the UVs on.");
		return;
	}
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// import the file
		
	if (!os_load_file("Select an OBJ file",path,"obj")) return;

	if (!textdecode_open(path,0x20)) {
		os_dialog_alert("Import Failed","Could not open OBJ file.");
		return;
    }
	
    nline=textdecode_count();
	
		// count vertexes, uvs, and faces
		
	nvertex=0;
	npoly=0;
	nuv=0;
	
	min.x=min.y=min.z=0.0f;
	max.x=max.y=max.z=0.0f;
	
    for (n=0;n!=nline;n++) {
        textdecode_get_piece(n,0,txt);
        
        if (strcmp(txt,"v")==0) {
			nvertex++;
			
			textdecode_get_piece(n,1,txt);
			fx=(float)strtod(txt,NULL);
			textdecode_get_piece(n,2,txt);
			fy=(float)strtod(txt,NULL);
			textdecode_get_piece(n,3,txt);
			fz=(float)strtod(txt,NULL);
			
			if (fx<min.x) min.x=fx;
			if (fx>max.x) max.x=fx;
			if (fy<min.y) min.y=fy;
			if (fy>max.y) max.y=fy;
			if (fz<min.z) min.z=fz;
			if (fz>max.z) max.z=fz;
		}
		else {
			if (strcmp(txt,"f")==0) {
				npoly++;
			}
			else {
				if (strcmp(txt,"vt")==0) {
					nuv++;
				}
			}
		}
	}
	
		// does this OBJ match OBJ importing over?
		
	if (nuv==0) {
		textdecode_close();
		os_dialog_alert("Import Failed","The imported OBJ has no UVs.");
		return;
    }
		
	if (npoly!=mesh->npoly) {
		sprintf(txt,"The imported OBJ has %d polys and the selected mesh has %d polys.\nDo you want to continue?",npoly,mesh->npoly);
		if (os_dialog_confirm("Import Warning",txt,FALSE)!=0) {
			textdecode_close();
			return;
		}
    }
	
		// get the UVs
		
	uvs=(float*)malloc(sizeof(float)*(2*nuv));
	if (uvs==NULL) {
		textdecode_close();
		os_dialog_alert("Import Failed","Out of Memory.");
		return;
	}

	uv=uvs;

	for (n=0;n!=nline;n++) {
		textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"vt")!=0) continue;
				
		textdecode_get_piece(n,1,uvstr);
		*uv++=(float)strtod(uvstr,NULL);
		textdecode_get_piece(n,2,uvstr);
		*uv++=1.0f-(float)strtod(uvstr,NULL);
	}

		// replace the UVs
		
	poly_idx=0;
	
    for (n=0;n!=nline;n++) {
	
			// a face?
			
		textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"f")!=0) continue;
		
			// out of polys?
			
		if (poly_idx>=mesh->npoly) break;
		
		poly=&mesh->polys[poly_idx];
		poly_idx++;
		
            // get the face points
        
        npt=0;
		
		if (view_get_uv_layer()==uv_layer_normal) {
			uv_ptr=&poly->main_uv;
		}
		else {
			uv_ptr=&poly->lmap_uv;
		}
        
        for (k=0;k!=8;k++) {
            textdecode_get_piece(n,(k+1),txt);
            if (txt[0]==0x0) break;
            
            uvstr[0]=0x0;
            
            strcpy(vstr,txt);
            c=strchr(vstr,'/');
            if (c!=NULL) {
                strcpy(uvstr,(c+1));
                *c=0x0;
            }
            c=strchr(uvstr,'/');
            if (c!=NULL) *c=0x0;
            
			if (uvstr[0]==0x0) {
				uv_ptr->uvs[npt].x=uv_ptr->uvs[npt].y=0.0f;
			}
			else {
				uv_idx=atoi(uvstr)-1;
				
				uv=uvs+(uv_idx*2);
                uv_ptr->uvs[npt].x=*uv++;
                uv_ptr->uvs[npt].y=*uv;
            }
			
            npt++;
        }
	}
	
	free(uvs);
	
	textdecode_close();
	
		// finish up
		
	main_wind_draw();
}

/* =======================================================

      Add Height Map Mesh
      
======================================================= */

float piece_add_height_map_mesh_get_height(unsigned char *data,int wid,int high,bool alpha_channel,int x,int z,int div_cnt)
{
	int				y;
	unsigned char	*p;
	
		// find offset in portal
		
	x=(int)(((float)x/(float)div_cnt)*(float)wid);
	y=(int)(((float)z/(float)div_cnt)*(float)high);
	
	if (x<0) x=0;
	if (x>=wid) x=wid-1;
	if (y<0) y=0;
	if (y>=high) y=high-1;
	
	if (alpha_channel) {
		p=data+((y*(wid<<2))+(x<<2));
	}
	else {
		p=data+((y*(wid*3))+(x*3));
	}
	
	return(((float)*p)/255.0f);
}

void piece_add_height_map_mesh(void)
{
	int					n,x,z,px,pz,div_cnt,div_sz,total_sz,bwid,bhigh,high,
						kx[4],ky[4],kz[4],y[4],
						mesh_idx,txt_idx;
	float				f_portal_y_sz,gx[4],gy[4];
	char				path[1024];
	unsigned char		*data;
	bool				alpha_channel;
	d3pnt				pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
	if (!piece_create_texture_ok()) return;
	
		// get the png
		
	if (!os_load_file("Select an PNG file",path,"png")) return;
	data=bitmap_load_png_data(path,&bwid,&bhigh,&alpha_channel);
	if (data==NULL) return;
	
		// division and sizes
		
	if (!dialog_height_import_run(&div_cnt,&total_sz,&high)) {
		free(data);
		return;
	}
		
	div_sz=total_sz/div_cnt;
	f_portal_y_sz=(float)high;
	
	piece_create_get_spot(&pnt);
	
		// texture
		
	txt_idx=map_texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
		// polygons
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) {
		free(data);
		return;
	}
	
	view_vbo_mesh_initialize(mesh_idx);
	
	os_set_wait_cursor();

	progress_start("Height Map Import: Importing PNG",(div_cnt+3));

	gx[0]=gx[1]=gx[2]=gx[3]=0.0f;
	gy[0]=gy[1]=gy[2]=gy[3]=0.0f;
	
	for (z=0;z!=div_cnt;z++) {
		for (x=0;x!=div_cnt;x++) {
			
			px=(pnt.x-(total_sz/2))+(x*div_sz);
			pz=(pnt.z-(total_sz/2))+(z*div_sz);
			
				// get the Ys
				
			y[0]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,x,z,div_cnt));
			y[1]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,(x+1),z,div_cnt));
			y[2]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,(x+1),(z+1),div_cnt));
			y[3]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,x,(z+1),div_cnt));
			
				// if all Ys are the same, add as a quad
				
			if ((y[0]==y[1]) && (y[0]==y[2]) && (y[0]==y[3])) {
			
				kx[0]=kx[3]=px;
				kx[1]=kx[2]=px+div_sz;
				kz[0]=kz[1]=pz;
				kz[2]=kz[3]=pz+div_sz;
				ky[0]=ky[1]=ky[2]=ky[3]=y[0];
				
				map_mesh_add_poly(&map,mesh_idx,4,kx,ky,kz,gx,gy,txt_idx);
			}
			
				// otherwise add as two triangles
				// face the triangle in opposite directions
				// on each pass
				
			else {
					
				if (((x+z)&0x1)!=0) {
					kx[0]=px;
					kx[1]=kx[2]=px+div_sz;
					kz[0]=kz[1]=pz;
					kz[2]=pz+div_sz;
					ky[0]=y[0];
					ky[1]=y[1];
					ky[2]=y[2];
					
					map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
				
					kx[0]=kx[2]=px;
					kx[1]=px+div_sz;
					kz[0]=pz;
					kz[1]=kz[2]=pz+div_sz;
					ky[0]=y[0];
					ky[1]=y[2];
					ky[2]=y[3];
					
					map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
				}
				else {
					kx[0]=kx[2]=px;
					kx[1]=px+div_sz;
					kz[0]=kz[1]=pz;
					kz[2]=pz+div_sz;
					ky[0]=y[0];
					ky[1]=y[1];
					ky[2]=y[3];
					
					map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
				
					kx[0]=kx[1]=px+div_sz;
					kx[2]=px;
					kz[0]=pz;
					kz[1]=kz[2]=pz+div_sz;
					ky[0]=y[1];
					ky[1]=y[2];
					ky[2]=y[3];
					
					map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
				}
			}
		}

		view_vbo_mesh_rebuild(mesh_idx);
		progress_next();
	}
	
		// reset UVs and normals

	mesh=&map.mesh.meshes[mesh_idx];
	
	progress_next_title("Height Map Import: Building Normals");
	map_recalc_normals_mesh(&map,mesh,normal_mode_none,FALSE);
	
	progress_next_title("Height Map Import: Creating UVs");			
	map_mesh_reset_uv(&map,mesh_idx);

		// on height maps, make sure all
		// Ys face up

	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
		if (poly->tangent_space.normal.y>0) poly->tangent_space.normal.y=-poly->tangent_space.normal.y;
		poly++;
	}

	view_vbo_mesh_rebuild(mesh_idx);
		
	progress_next();
	free(data);
	
	progress_end();
	os_set_arrow_cursor();
	
		// finish up

	select_clear();
	select_add(item_map_mesh,mesh_idx,0);

	main_wind_draw();
}

/* =======================================================

      Add Grid Mesh
      
======================================================= */

void piece_add_grid_mesh(void)
{
	int				mesh_idx,
					x,y,z,sz,txt_idx,
					px[4],py[4],pz[4];
	float			gx[4],gy[4];
	bool			sides[6];
	d3pnt			pnt,divs;
	
	if (!piece_create_texture_ok()) return;

		// get mesh divisions
		
	if (!dialog_grid_run(&divs,sides)) return;
	
		// texture, position, and size
		
	txt_idx=map_texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
	sz=500;		// medium grid

	piece_create_get_spot(&pnt);
	pnt.x-=((divs.x*sz)/2);
	pnt.y-=((divs.y*sz)/2);
	pnt.z-=((divs.z*sz)/2);

	view_click_grid(&pnt);

		// create mesh
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) return;
	
		// default UVs
		
	gx[0]=gx[1]=gx[2]=gx[3]=0.0f;
	gy[0]=gy[1]=gy[2]=gy[3]=0.0f;

	view_vbo_mesh_initialize(mesh_idx);
	
	os_set_wait_cursor();

	progress_start("Create Grid",(divs.x+divs.y+divs.z));
	
		// x sides

	for (z=0;z!=divs.z;z++) {
		for (y=0;y!=divs.y;y++) {
			pz[0]=pz[1]=(z*sz)+pnt.z;
			pz[2]=pz[3]=((z+1)*sz)+pnt.z;
			py[0]=py[3]=(y*sz)+pnt.y;
			py[1]=py[2]=((y+1)*sz)+pnt.y;
			
			if (sides[0]) {
				px[0]=px[1]=px[2]=px[3]=pnt.x;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}

			if ((divs.x!=0) && (sides[1])) {
				px[0]=px[1]=px[2]=px[3]=(divs.x*sz)+pnt.x;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}

		}

		view_vbo_mesh_rebuild(mesh_idx);
		progress_next();
	}
	
		// y sides
		
	for (z=0;z!=divs.z;z++) {
		for (x=0;x!=divs.x;x++) {
			px[0]=px[3]=(x*sz)+pnt.x;
			px[1]=px[2]=((x+1)*sz)+pnt.x;
			pz[0]=pz[1]=(z*sz)+pnt.z;
			pz[2]=pz[3]=((z+1)*sz)+pnt.z;

			if (sides[2]) {
				py[0]=py[1]=py[2]=py[3]=pnt.y;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}

			if ((divs.y!=0) && (sides[3])) {
				py[0]=py[1]=py[2]=py[3]=(divs.y*sz)+pnt.y;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}
		}
		
		view_vbo_mesh_rebuild(mesh_idx);
		progress_next();
	}
	
		// z sides

	for (x=0;x!=divs.x;x++) {
		for (y=0;y!=divs.y;y++) {
			px[0]=px[3]=(x*sz)+pnt.x;
			px[1]=px[2]=((x+1)*sz)+pnt.x;
			py[0]=py[1]=(y*sz)+pnt.y;
			py[2]=py[3]=((y+1)*sz)+pnt.y;
			
			if (sides[4]) {
				pz[0]=pz[1]=pz[2]=pz[3]=pnt.z;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}

			if ((divs.z!=0) && (sides[5])) {
				pz[0]=pz[1]=pz[2]=pz[3]=(divs.z*sz)+pnt.z;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}

		}

		view_vbo_mesh_rebuild(mesh_idx);
		progress_next();
	}
	
		// reset UVs
		
	map_mesh_reset_uv(&map,mesh_idx);
	
	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_none,FALSE);
	
	progress_end();
	os_set_arrow_cursor();
	
		// finish up
		
	select_clear();
	select_add(item_map_mesh,mesh_idx,0);

	main_wind_draw();
}

/* =======================================================

      Add Polygon Mesh
      
======================================================= */

void piece_add_polygon_mesh(void)
{
	int				mesh_idx,sz,txt_idx,
					px[4],py[4],pz[4];
	float			gx[4],gy[4];
	d3pnt			pnt;
	
	if (!piece_create_texture_ok()) return;
	
		// texture and size
		
	txt_idx=map_texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
		// create mesh
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) return;
	
		// create polygon
		
	piece_create_get_spot(&pnt);

	sz=720;

	px[0]=px[3]=pnt.x-sz;
	px[1]=px[2]=pnt.x+sz;
	pz[0]=pz[1]=pnt.z-sz;
	pz[2]=pz[3]=pnt.z+sz;
	py[0]=py[1]=py[2]=py[3]=pnt.y;
	
	gx[0]=gx[1]=gx[2]=gx[3]=0.0f;
	gy[0]=gy[1]=gy[2]=gy[3]=0.0f;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
	
		// reset UVs
		
	map_mesh_reset_uv(&map,mesh_idx);

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_none,FALSE);
	
		// finish up
		
	view_vbo_mesh_initialize(mesh_idx);

	select_clear();
	select_add(item_map_mesh,mesh_idx,0);

	main_wind_draw();
}

/* =======================================================

      Combine Meshes
      
======================================================= */

void piece_combine_mesh(void)
{
	int				n,k,nsel,org_idx,mesh_combine_idx,new_mesh_combine_idx,
					type,mesh_idx,poly_idx;
	int				sel_mesh_idx[select_max_item];
	bool			skip;
	
		// get all selected meshes
		// select will change as meshes are deleted so
		// we need our own list.  we also have to check
		// for duplicates as multiple polygons can be in list
		
	nsel=0;
	
	for (n=0;n!=select_count();n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=item_map_mesh) continue;
		
		skip=FALSE;
		
		for (k=0;k!=nsel;k++) {
			if (sel_mesh_idx[k]==mesh_idx) {
				skip=TRUE;
				break;
			}
		}
		
		if (!skip) sel_mesh_idx[nsel++]=mesh_idx;
	}
	
	if (nsel<2) return;
	
		// clear all VBOs

	view_vbo_map_free();

		// clear selection
		
	select_clear();
	
		// combine all meshes
		
	mesh_combine_idx=sel_mesh_idx[0];
	
	for (n=1;n<nsel;n++) {
	
			// combine
			
		mesh_idx=sel_mesh_idx[n];
		new_mesh_combine_idx=map_mesh_combine(&map,mesh_combine_idx,mesh_idx);
		if (new_mesh_combine_idx==-1) return;
		
			// move other meshes in select as combine deleted them
			
		for (k=(n+1);k<nsel;k++) {
			org_idx=sel_mesh_idx[k];
			if (org_idx>mesh_idx) sel_mesh_idx[k]--;
			if (org_idx>mesh_combine_idx) sel_mesh_idx[k]--;
		}
		
		mesh_combine_idx=new_mesh_combine_idx;
	}
	
	if (mesh_combine_idx!=-1) select_add(item_map_mesh,mesh_combine_idx,0);

		// rebuild the VBOs

	view_vbo_map_initialize();
}

/* =======================================================

      Split Meshes
      
======================================================= */

void piece_split_mesh(void)
{
	int					n,k,mesh_idx,x[8],y[8],z[8],
						type,add_mesh_idx,add_poly_idx,poly_idx;
	float				gx[8],gy[8];
	bool				first_mesh;
	d3pnt				*pt;
	map_mesh_type		*mesh,*copy_mesh;
	map_mesh_poly_type	*poly,*copy_poly;
	
		// create new mesh
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) return;

		// clear all VBOs

	view_vbo_map_free();
	
		// add in the selected polygons
		
	first_mesh=TRUE;
	mesh=&map.mesh.meshes[mesh_idx];
		
	for (n=0;n!=select_count();n++) {
		select_get(n,&type,&add_mesh_idx,&add_poly_idx);
		if (type!=item_map_mesh) continue;
		
			// if first mesh hit, copy mesh settings
			
		copy_mesh=&map.mesh.meshes[add_mesh_idx];
		
		if (first_mesh) {
			mesh->group_idx=copy_mesh->group_idx;
			memmove(&mesh->flag,&copy_mesh->flag,sizeof(map_mesh_flag_type));
			memmove(&mesh->msg,&copy_mesh->msg,sizeof(map_mesh_message_type));
			
			first_mesh=FALSE;
		}
		
			// add the polygon
		
		poly=&map.mesh.meshes[add_mesh_idx].polys[add_poly_idx];
		
		for (k=0;k!=poly->ptsz;k++) {
			pt=&copy_mesh->vertexes[poly->v[k]];
			x[k]=pt->x;
			y[k]=pt->y;
			z[k]=pt->z;
			gx[k]=poly->main_uv.uvs[k].x;
			gy[k]=poly->main_uv.uvs[k].y;
		}
		
		poly_idx=map_mesh_add_poly(&map,mesh_idx,poly->ptsz,x,y,z,gx,gy,poly->txt_idx);
		
			// dupe some polygon settings
			// get original polygon against as memory could
			// have moved
			
		poly=&map.mesh.meshes[add_mesh_idx].polys[add_poly_idx];
		copy_poly=&map.mesh.meshes[mesh_idx].polys[poly_idx];
		
		memmove(&copy_poly->tangent_space,&poly->tangent_space,sizeof(tangent_space_type));
		memmove(&copy_poly->flag,&poly->flag,sizeof(map_mesh_poly_flag_type));
		
		copy_poly->shift.x=poly->shift.x;
		copy_poly->shift.y=poly->shift.y;
		strcpy(copy_poly->camera,poly->camera);
		
			// delete poly from mesh
			
		select_delete_move_index(item_map_mesh,add_mesh_idx,add_poly_idx);
		map_mesh_delete_poly(&map,add_mesh_idx,add_poly_idx);
	}
	
		// if no polygons, delete mesh
		
	if (map.mesh.meshes[mesh_idx].npoly==0) {
		map_mesh_delete(&map,mesh_idx);
		view_vbo_map_initialize();
		return;
	}

		// rebuild the VBOs

	view_vbo_map_initialize();
	
		// make new mesh selection
		
	select_clear();
	select_add(item_map_mesh,mesh_idx,0);
	
		// auto-switch to mesh drag mode
		
	state.map.select_mode=select_mode_mesh;

	main_wind_draw();
}

/* =======================================================

      Map Changes
      
======================================================= */

void map_mesh_move_all(int x,int y,int z)
{
	int				n;
	d3pnt			pt;
	
	pt.x=x;
	pt.y=y;
	pt.z=z;

	map_move(&map,&pt);
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		view_vbo_mesh_rebuild(n);
	}
}

void map_mesh_center_all(void)
{
	int				n;

	map_center(&map);

	for (n=0;n!=map.mesh.nmesh;n++) {
		view_vbo_mesh_rebuild(n);
	}
}

void map_mesh_reset_uv_all(void)
{
	int				n;

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (!map.mesh.meshes[n].flag.lock_uv) {
			map_mesh_reset_uv(&map,n);
			view_vbo_mesh_rebuild(n);
		}
	}
}

