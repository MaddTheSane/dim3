/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Vertex Transformations

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

d3vct							cur_set_normal={0.0f,-1.0f,0.0f};

extern model_type				model;
extern app_state_type			state;

/* =======================================================

      Vertex Center
      
======================================================= */

void model_vertex_find_center_sel_vertexes(int mesh_idx,int *p_cx,int *p_cy,int *p_cz)
{
	int					i,nt,cx,cz,cy,cnt;
	model_vertex_type	*vertex;
	
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	cx=cy=cz=0;
	cnt=0;
	
	for (i=0;i!=nt;i++) {
		if (model_vertex_mask_check_sel(mesh_idx,i)) {
			cx+=vertex->pnt.x;
			cy+=vertex->pnt.y;
			cz+=vertex->pnt.z;
			cnt++;
		}
		vertex++;
	}
	
	if (cnt==0) {
		*p_cx=*p_cy=*p_cz=0;
		return;
	}
	
	*p_cx=cx/cnt;
	*p_cy=cy/cnt;
	*p_cz=cz/cnt;
}

/* =======================================================

      Invert, Set, and Flip In/Out Normal Vertexes
      
======================================================= */

void model_vertex_invert_normals(int mesh_idx)
{
	int					n,nvertex;
	model_vertex_type	*vertex;
	
	nvertex=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nvertex;n++) {
		
		if (model_vertex_mask_check_sel(mesh_idx,n)) {
			vertex->tangent_space.normal.x=-vertex->tangent_space.normal.x;
			vertex->tangent_space.normal.y=-vertex->tangent_space.normal.y;
			vertex->tangent_space.normal.z=-vertex->tangent_space.normal.z;
		}
		
		vertex++;
	}
}

void model_vertex_set_normals(int mesh_idx)
{
	int					n,nvertex;
	d3vct				normal;
	model_vertex_type	*vertex;
	
		// run dialog

	memmove(&normal,&cur_set_normal,sizeof(d3vct));
	
	if (!dialog_set_normal_run(&normal)) return;
	
	memmove(&cur_set_normal,&normal,sizeof(d3vct));

		// set the normals

	nvertex=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nvertex;n++) {
		
		if (model_vertex_mask_check_sel(mesh_idx,n)) {
			vertex->tangent_space.normal.x=normal.x;
			vertex->tangent_space.normal.y=normal.y;
			vertex->tangent_space.normal.z=normal.z;
		}
		
		vertex++;
	}
}

void model_vertex_set_normals_in_out(int mesh_idx,bool out)
{
	int					n,count,nvertex;
	bool				is_out;
	d3pnt				center,*pnt;
	d3vct				face_vct;
	model_vertex_type	*vertex;

		// get the center

	center.x=center.y=center.z=0;
	count=0;

	nvertex=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nvertex;n++) {
				
		if (model_vertex_mask_check_sel(mesh_idx,n)) {
			pnt=&model.meshes[mesh_idx].vertexes[n].pnt;
			center.x+=pnt->x;
			center.y+=pnt->y;
			center.z+=pnt->z;
			count++;
		}

		vertex++;
	}

	if (count==0) return;

	center.x/=count;
	center.y/=count;
	center.z/=count;

		// flip the normals for in/out
		
	vertex=model.meshes[mesh_idx].vertexes;

	for (n=0;n!=nvertex;n++) {
				
		if (model_vertex_mask_check_sel(mesh_idx,n)) {

				// determine if vertex is facing 'out'
			
			vector_create(&face_vct,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,center.x,center.y,center.z);
			is_out=(vector_dot_product(&vertex->tangent_space.normal,&face_vct)>0.0f);

			if (is_out!=out) {
				vertex->tangent_space.normal.x=-vertex->tangent_space.normal.x;
				vertex->tangent_space.normal.y=-vertex->tangent_space.normal.y;
				vertex->tangent_space.normal.z=-vertex->tangent_space.normal.z;
			}
		}

		vertex++;
	}
}

/* =======================================================

      Clear Bones From Vertexes
      
======================================================= */

void model_vertex_clear_bone_attachments_sel_vertexes(int mesh_idx)
{
	int					n,nt;
	model_vertex_type	*vertex;
	
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nt;n++) {
		if (model_vertex_mask_check_sel(mesh_idx,n)) {
			vertex->major_bone_idx=-1;
			vertex->minor_bone_idx=-1;
		}
		vertex++;
	}
}

/* =======================================================

      Automatically Set Vertexes for Bones
      
======================================================= */

void model_vertex_auto_bone_attachments(int mesh_idx)
{
	int					n,k,nt,bone_idx,
						cur_dist,dist;
	model_bone_type		*bone;
	model_vertex_type	*vertex;
	
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nt;n++) {
	
		if (vertex->major_bone_idx!=-1) {
			vertex++;
			continue;
		}
		
		bone_idx=-1;
		cur_dist=0;
		
		bone=model.bones;
		
		for (k=0;k!=model.nbone;k++) {
			dist=distance_get(bone->pnt.x,bone->pnt.y,bone->pnt.z,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z);
			if ((bone_idx==-1) || (dist<cur_dist)) {
				bone_idx=k;
				cur_dist=dist;
			}
			
			bone++;
		}
		
		if (bone_idx!=-1) {
			vertex->major_bone_idx=bone_idx;
			vertex->minor_bone_idx=-1;
			vertex->bone_factor=1.0f;
		}
		
		vertex++;
	}
}

/* =======================================================

      Remove Vertexes
      
======================================================= */

void model_vertex_delete_sel_vertex(int mesh_idx)
{
	int					i,n,j,sz;
	char				*vertex_sel;
	bool				hit;
	model_poly_type		*poly;

		// convert sels to boolean
		
	vertex_sel=(char*)malloc(model.meshes[mesh_idx].nvertex);
		
	for (i=0;i!=model.meshes[mesh_idx].nvertex;i++) {
		vertex_sel[i]=(char)model_vertex_mask_check_sel(mesh_idx,i);
	}
	
		// delete all vertexes
		
	i=0;
	
	while (i<model.meshes[mesh_idx].nvertex) {
	
		if (vertex_sel[i]==0) {
			i++;
			continue;
		}
		
			// delete the vertex
			
		sz=((model.meshes[mesh_idx].nvertex-1)-i);
		
		if (sz>0) {
			memmove(&model.meshes[mesh_idx].vertexes[i],&model.meshes[mesh_idx].vertexes[i+1],(sz*sizeof(model_vertex_type)));
			memmove(&vertex_sel[i],&vertex_sel[i+1],sz);
		}
		
		model.meshes[mesh_idx].nvertex--;
		
			// delete all polys with vertex
			
		n=0;
		
		while (n<model.meshes[mesh_idx].npoly) {
		
			hit=FALSE;

			poly=&model.meshes[mesh_idx].polys[n];
			
			for (j=0;j!=poly->ptsz;j++) {
				hit=hit||(poly->v[j]==i);
				if (poly->v[j]>i) poly->v[j]--;
			}
		
			if (!hit) {
				n++;
				continue;
			}
			
				// delete poly
					
			sz=((model.meshes[mesh_idx].npoly-1)-n);
			
			if (sz>0) {
				memmove(&model.meshes[mesh_idx].polys[n],&model.meshes[mesh_idx].polys[n+1],(sz*sizeof(model_poly_type)));
			}
				
			model.meshes[mesh_idx].npoly--;
		}
	}
	
	free(vertex_sel);
	
		// clear sels
		
	model_vertex_mask_clear_sel(mesh_idx);
	model_vertex_mask_clear_hide(mesh_idx);
}

/* =======================================================

      Delete Unused Vertexes
      
======================================================= */

void model_vertex_delete_unused_vertexes(int mesh_idx)
{
	int					n,k,i,t,nvertex,npoly,sz;
	unsigned char		*v_ok;
    model_poly_type		*poly;
	
		// vertex hit list
			
	nvertex=model.meshes[mesh_idx].nvertex;
	
	v_ok=(unsigned char*)malloc(nvertex);
	bzero(v_ok,nvertex);
	
		// find vertexes hit
		
	npoly=model.meshes[mesh_idx].npoly;
	poly=model.meshes[mesh_idx].polys;
	
	for (n=0;n!=npoly;n++) {
		for (k=0;k!=poly->ptsz;k++) {
			v_ok[poly->v[k]]=0x1;
		}
		poly++;
	}
	
		// delete unused vertexes
	
	for (n=(nvertex-1);n>=0;n--) {
	
		if (v_ok[n]==0x1) continue;
		
			// change all poly vertex pointers
	
		poly=model.meshes[mesh_idx].polys;
		
		for (i=0;i!=npoly;i++) {
			for (t=0;t!=poly->ptsz;t++) {
				if (poly->v[t]>n) poly->v[t]--;
			}
			poly++;
		}
		
			// delete vertex
			
		sz=(nvertex-n)*sizeof(model_vertex_type);
		if (sz>0) memmove(&model.meshes[mesh_idx].vertexes[n],&model.meshes[mesh_idx].vertexes[n+1],sz);
		
		nvertex--;
	}
	
	model.meshes[mesh_idx].nvertex=nvertex;

	free(v_ok);
}

/* =======================================================

      Remove Polygon
      
======================================================= */

void model_vertex_delete_sel_poly(int mesh_idx)
{
	int					n,sz;
	char				*poly_sel;

		// convert sels to boolean
		
	poly_sel=(char*)malloc(model.meshes[mesh_idx].npoly);
		
	for (n=0;n!=model.meshes[mesh_idx].npoly;n++) {
		poly_sel[n]=(char)model_poly_mask_check_sel(mesh_idx,n);
	}
	
		// delete all polys
		
	n=0;
	
	while (n<model.meshes[mesh_idx].npoly) {
	
		if (poly_sel[n]==0) {
			n++;
			continue;
		}
		
			// delete the poly
			
		sz=((model.meshes[mesh_idx].npoly-1)-n);
		
		if (sz>0) {
			memmove(&model.meshes[mesh_idx].polys[n],&model.meshes[mesh_idx].polys[n+1],(sz*sizeof(model_poly_type)));
			memmove(&poly_sel[n],&poly_sel[n+1],sz);
		}
		
		model.meshes[mesh_idx].npoly--;
	}
	
	free(poly_sel);
	
		// clear sels
		
	model_poly_mask_clear_sel(mesh_idx);
	model_poly_mask_clear_hide(mesh_idx);
	
		// any remove any vertexes
		// that are now unattached
		
	model_vertex_delete_unused_vertexes(mesh_idx);
}

/* =======================================================

      Collapse Vertexes
      
======================================================= */

void model_vertex_collapse_selected(int mesh_idx)
{
	int					n,k,idx;
	model_mesh_type		*mesh;
	model_poly_type		*poly;

	mesh=&model.meshes[mesh_idx];
	
		// find fist vertex to collapse to
		
	idx=-1;
	
	for (n=0;n!=mesh->nvertex;n++) {
		if ((model_vertex_mask_check_sel(mesh_idx,n)) && (!model_vertex_mask_check_hide(mesh_idx,n))) {
			if (idx==-1) {
				idx=n;
				break;
			}
		}
	}
	
	if (idx==-1) return;
	
		// collapse

	poly=mesh->polys;
		
	for (n=0;n!=mesh->npoly;n++) {
	
		for (k=0;k!=poly->ptsz;k++) {
			if (poly->v[k]==idx) continue;
			if (!model_vertex_mask_check_sel(mesh_idx,poly->v[k])) continue;
			if (model_vertex_mask_check_hide(mesh_idx,poly->v[k])) continue;

			poly->v[k]=idx;
		}

		poly++;
	}

		// clear selection

	model_vertex_mask_clear_sel(mesh_idx);
	
		// finish by cleaning all the
		// now collapses and unconnected
		// vertexes
		
	model_vertex_delete_unused_vertexes(mesh_idx);
}

void model_vertex_collapse_similar(int mesh_idx)
{
	int					n,k,t;
	model_mesh_type		*mesh;
	model_vertex_type	*vertex,*chk_vertex;
	model_poly_type		*poly;

	mesh=&model.meshes[mesh_idx];

		// run through all the vertexes

	vertex=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {

		poly=mesh->polys;
			
		for (k=0;k!=mesh->npoly;k++) {
		
			for (t=0;t!=poly->ptsz;t++) {
				if (poly->v[t]==n) continue;

				chk_vertex=&mesh->vertexes[poly->v[t]];
				if ((chk_vertex->pnt.x==vertex->pnt.x) && (chk_vertex->pnt.y==vertex->pnt.y) && (chk_vertex->pnt.z==vertex->pnt.z)) poly->v[t]=n;
			}

			poly++;
		}

		vertex++;
	}

		// clear selection

	model_vertex_mask_clear_sel(mesh_idx);
	
		// finish by cleaning all the
		// now collapses and unconnected
		// vertexes
		
	model_vertex_delete_unused_vertexes(mesh_idx);
}

/* =======================================================

      Poly Shared Vertex Utility
      
======================================================= */

int polygon_find_vertex_offset(model_poly_type *poly,int v_idx)
{
	int			n;

	for (n=0;n!=poly->ptsz;n++) {
		if (poly->v[n]==v_idx) return(n);
	}

	return(-1);
}

/* =======================================================

      Poly Combine and Tesselate
      
======================================================= */

void model_polygon_make_quad(int mesh_idx)
{
	int					n,npoly,ptsz,idx,next_idx,dir,sz,
						poly_1_idx,poly_2_idx,start_idx,
						v[8];
	float				gx[8],gy[8];
	model_mesh_type		*mesh;
	model_poly_type		*poly_1,*poly_2,
						*cur_poly,*other_poly,*swap_poly;

	mesh=&model.meshes[mesh_idx];
	npoly=mesh->npoly;

		// find two triangles

	poly_1_idx=poly_2_idx=-1;

	for (n=0;n!=npoly;n++) {
		if ((model_poly_mask_check_sel(mesh_idx,n)) && (!model_poly_mask_check_hide(mesh_idx,n))) {
			if (poly_1_idx==-1) {
				poly_1_idx=n;
				continue;
			}
			if (poly_2_idx==-1) {
				poly_2_idx=n;
				continue;
			}
			os_dialog_alert("Make Quad","There are more than two polygons selected");
			return;
		}
	}

	if ((poly_1_idx==-1) || (poly_2_idx==-1)) {
		os_dialog_alert("Make Quad","Need to select two triangle polygons");
		return;
	}

		// make sure they are correct

	poly_1=&mesh->polys[poly_1_idx];
	poly_2=&mesh->polys[poly_2_idx];

	if ((poly_1->ptsz!=3) || (poly_2->ptsz!=3)) {
		os_dialog_alert("Make Quad","Both polygons need to be triangles");
		return;
	}

		// find a non shared vertex on first
		// polygon to start on

	start_idx=0;

	while (start_idx<poly_1->ptsz) {
		if (polygon_find_vertex_offset(poly_2,poly_1->v[start_idx])==-1) break;
		start_idx++;
	}

	if (start_idx==poly_1->ptsz) {
		os_dialog_alert("Make Quad","Unable to merge these triangle polygons into a single quad");
		return;
	}

		// run around the first polygon
		// and switch when to the second
		// when we hit a shared vertex

	ptsz=0;

	idx=start_idx;
	dir=1;

	cur_poly=poly_1;
	other_poly=poly_2;

	while (TRUE) {

		v[ptsz]=cur_poly->v[idx];
		gx[ptsz]=cur_poly->gx[idx];
		gy[ptsz]=cur_poly->gy[idx];

		ptsz++;

			// if these vertexes are
			// shared, switch to other poly
			// and go in the direction
			// of the next unshared vertex

		next_idx=polygon_find_vertex_offset(other_poly,cur_poly->v[idx]);

		if (next_idx!=-1) {
			idx=next_idx;

			swap_poly=cur_poly;
			cur_poly=other_poly;
			other_poly=swap_poly;

			next_idx=idx+1;
			if (next_idx==cur_poly->ptsz) next_idx=0;

			if (polygon_find_vertex_offset(other_poly,cur_poly->v[next_idx])==-1) {
				dir=1;
			}
			else {
				dir=-1;
			}
		}

			// next vertex

		idx+=dir;
		if (idx==cur_poly->ptsz) idx=0;
		if (idx<0) idx=cur_poly->ptsz-1;

			// we are at end if next vertex
			// is equal to the index where
			// we started

		if ((cur_poly==poly_1) && (idx==start_idx)) break;
	}

		// rebuild first polygon into quad

	for (n=0;n!=ptsz;n++) {
		poly_1->v[n]=v[n];
		poly_1->gx[n]=gx[n];
		poly_1->gy[n]=gy[n];
	}

	poly_1->ptsz=ptsz;

		// delete the second

	sz=(mesh->npoly-poly_2_idx)-1;
	if (sz>0) memmove(&mesh->polys[poly_2_idx],&mesh->polys[poly_2_idx+1],(sz*sizeof(model_poly_type)));

	mesh->npoly--;

		// rebuild the sel

	if (poly_1_idx>poly_2_idx) poly_1_idx--;

	model_poly_mask_clear_sel(mesh_idx);
	model_poly_mask_set_sel(mesh_idx,poly_1_idx,TRUE);
}

void model_polygon_tessellate(int mesh_idx,bool sel_only)
{
	int					n,k,t,npoly,ntrig,idx,txt_idx,
						trigs_v[6][3];
	float				trigs_gx[6][3],trigs_gy[6][3];
	model_mesh_type		*mesh;
	model_poly_type		*poly;

	mesh=&model.meshes[mesh_idx];
	npoly=mesh->npoly;

		// run through the polys

	for (n=0;n!=npoly;n++) {

			// if only tessellating selection

		if (sel_only) {
			if ((!model_poly_mask_check_sel(mesh_idx,n)) || (model_poly_mask_check_hide(mesh_idx,n))) continue;
		}

			// tessellate

		poly=&mesh->polys[n];
		if (poly->ptsz==3) continue;

		ntrig=poly->ptsz-2;

		for (k=0;k!=ntrig;k++) {
			trigs_v[k][0]=poly->v[0];
			trigs_gx[k][0]=poly->gx[0];
			trigs_gy[k][0]=poly->gy[0];

			trigs_v[k][1]=poly->v[k+1];
			trigs_gx[k][1]=poly->gx[k+1];
			trigs_gy[k][1]=poly->gy[k+1];

			trigs_v[k][2]=poly->v[k+2];
			trigs_gx[k][2]=poly->gx[k+2];
			trigs_gy[k][2]=poly->gy[k+2];
		}

		txt_idx=poly->txt_idx;

			// fix the original poly

		poly->ptsz=3;

			// add the new polys

		idx=mesh->npoly;

		model_mesh_set_poly_count(&model,mesh_idx,(mesh->npoly+(ntrig-1)));

		for (k=1;k!=ntrig;k++) {
			poly=&mesh->polys[idx];

			poly->ptsz=3;
			poly->txt_idx=txt_idx;

			for (t=0;t!=3;t++) {
				poly->v[t]=trigs_v[k][t];
				poly->gx[t]=trigs_gx[k][t];
				poly->gy[t]=trigs_gy[k][t];
			}

			idx++;
		}
	}
}

/* =======================================================

      Duplicate Attachments
      
======================================================= */

void model_bone_attach_duplicate(int mesh_idx)
{
	int					n,k,from_bone_idx,to_bone_idx,
						vertex_slop;
	d3pnt				dist;
	model_mesh_type		*mesh;
	model_vertex_type	*vertex,*chk_vertex;
	model_bone_type		*from_bone,*to_bone;

		// get bones to duplicate

	if (!dialog_bone_attach_duplicate_run(&from_bone_idx,&to_bone_idx,&vertex_slop)) return;

	mesh=&model.meshes[mesh_idx];

	from_bone=&model.bones[from_bone_idx];
	to_bone=&model.bones[to_bone_idx];

		// clear any old attachments
		// on to bone

	vertex=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
		if (vertex->major_bone_idx==to_bone_idx) {
			vertex->major_bone_idx=-1;
			vertex->minor_bone_idx=-1;
		}
		if (vertex->minor_bone_idx==to_bone_idx) vertex->minor_bone_idx=-1;

		vertex++;
	}

		// now find new attachments

	for (n=0;n!=mesh->nvertex;n++) {
		vertex=&mesh->vertexes[n];
		if ((vertex->major_bone_idx!=from_bone_idx) && (vertex->minor_bone_idx!=from_bone_idx)) continue;

			// find vertex like this one
			// for other bone
	
		dist.x=abs(vertex->pnt.x-from_bone->pnt.x);
		dist.y=abs(vertex->pnt.y-from_bone->pnt.y);
		dist.z=abs(vertex->pnt.z-from_bone->pnt.z);

		for (k=0;k!=mesh->nvertex;k++) {
			if (k==n) continue;
			
			chk_vertex=&mesh->vertexes[k];
			
			if (abs(dist.x-abs(chk_vertex->pnt.x-to_bone->pnt.x))>vertex_slop) continue;
			if (abs(dist.y-abs(chk_vertex->pnt.y-to_bone->pnt.y))>vertex_slop) continue;
			if (abs(dist.z-abs(chk_vertex->pnt.z-to_bone->pnt.z))>vertex_slop) continue;

			if (vertex->major_bone_idx==from_bone_idx) chk_vertex->major_bone_idx=to_bone_idx;
			if (vertex->minor_bone_idx==from_bone_idx) {
				chk_vertex->minor_bone_idx=to_bone_idx;
				chk_vertex->bone_factor=vertex->bone_factor;
			}
		}
	}

	 model_calculate_parents(&model);

		// select the to bone

	state.model.cur_bone_idx=to_bone_idx;
	model_palette_scroll_into_view(item_model_bone,state.model.cur_bone_idx);
	model_vertex_mask_set_sel_bone(state.model.cur_mesh_idx,state.model.cur_bone_idx);
}

/* =======================================================

      Clean Up Model
      
======================================================= */

void model_clean_up(void)
{
	int					mesh_idx,n,k,t,i,npoly,nhit,sz,
						poly_count;
	char				str[256];
	bool				delete_poly;
	d3pnt				*p1,*p2;
	model_mesh_type		*mesh;
	model_poly_type		*poly,*chk_poly;
	
	if (os_dialog_confirm("Model Clean Up","Cleaning up a model will delete any overlapped duplicate polygons.\nAre you sure you want to clean up?",FALSE)!=0) return;

		// run through all the meshes
		
	poly_count=0;
		
	for (mesh_idx=0;mesh_idx!=model.nmesh;mesh_idx++) {
	
			// clear current selection
			
		model_vertex_mask_clear_sel(mesh_idx);
		model_vertex_mask_clear_hide(mesh_idx);

			// find all polys that are duplicates
			// and therefore can be deleted
			
		mesh=&model.meshes[mesh_idx];
		
		while (TRUE) {
		
			delete_poly=FALSE;
			npoly=mesh->npoly;

			for (n=0;n!=npoly;n++) {
				poly=&mesh->polys[n];
					
					// don't check ones already selected
						
				if (model_poly_mask_check_sel(mesh_idx,n)) continue;
				
					// compare with other polys
					
				for (k=0;k!=npoly;k++) {
					if (k==n) continue;
					
						// don't check ones already selected
						
					if (model_poly_mask_check_sel(mesh_idx,k)) continue;
					
					chk_poly=&mesh->polys[k];
					if (chk_poly->ptsz!=poly->ptsz) continue;
					
						// they can be in any order,
						// so check them all
					
					nhit=0;
					
					for (t=0;t!=chk_poly->ptsz;t++) {
						for (i=0;i!=poly->ptsz;i++) {
							p1=&mesh->vertexes[chk_poly->v[t]].pnt;
							p2=&mesh->vertexes[poly->v[i]].pnt;
							if ((p1->x==p2->x) && (p1->y==p2->y) && (p1->z==p2->z)) {
								nhit++;
								break;
							}
						}
					}
					
						// delete if equal
						
					if (nhit!=chk_poly->ptsz) continue;
					
					delete_poly=TRUE;
					poly_count++;
			
					sz=((model.meshes[mesh_idx].npoly-1)-n);
					if (sz>0) memmove(&model.meshes[mesh_idx].polys[n],&model.meshes[mesh_idx].polys[n+1],(sz*sizeof(model_poly_type)));
		
					model.meshes[mesh_idx].npoly--;
					
					break;
				}
				
				if (delete_poly) break;
				
			}
			
			if (!delete_poly) break;
		}
	}
	
		// alert
		
	sprintf(str,"Deleted %d polygons",poly_count);
	os_dialog_alert("Model Clean Up",str);
}

