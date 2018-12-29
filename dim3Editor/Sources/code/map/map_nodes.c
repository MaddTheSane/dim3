/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Node Link Routines

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

extern map_type					map;
extern app_state_type			state;

typedef struct					{
									int					win_link_idx,win_dist,
														link_dist[max_node_link];
								} node_scan_type;

node_scan_type					node_scan[max_node];

/* =======================================================

      Node Links Lookup
      
======================================================= */

int node_link_is_node_link_selected(void)
{
	int			type,main_idx,sub_idx;
	
	if (select_count()!=1) return(-1);
	
	select_get(0,&type,&main_idx,&sub_idx);
	if (type!=item_map_node) return(-1);
	
	return(main_idx);
}

void node_link_select(int node_idx)
{
	select_clear();
	
	select_add(item_map_node,node_idx,-1);
}

/* =======================================================

      Node Links Add/Delete
      
======================================================= */

int node_link_get_free_link(int node_idx)
{
	int			n;
	
	for (n=0;n!=max_node_link;n++) {
		if (map.nodes[node_idx].link[n]==-1) return(n);
	}
	
	return(-1);
}

bool node_link_has_link(int node_idx,int link_node_idx)
{
	int			n;
	
	for (n=0;n!=max_node_link;n++) {
		if (map.nodes[node_idx].link[n]==link_node_idx) return(TRUE);
	}
	
	return(FALSE);
}

bool node_link_click(int node_idx)
{
	int			n,org_node_idx,k1,k2,sz;
	
		// get original node
		
	org_node_idx=node_link_is_node_link_selected();
	if (org_node_idx==-1) return(FALSE);
	
		// remove link mode
		
	if (state.map.node_mode==node_mode_remove_link) {
			
		for (n=0;n!=max_node_link;n++) {
		
			if (map.nodes[org_node_idx].link[n]==node_idx) {
				map_undo_push();
				sz=(max_node_link-n)-1;
				if (sz>0) memmove(&map.nodes[org_node_idx].link[n],&map.nodes[org_node_idx].link[n+1],(sz*sizeof(short)));
				map.nodes[org_node_idx].link[max_node_link-1]=-1;
				return(TRUE);
			}
			
			if (map.nodes[node_idx].link[n]==org_node_idx) {
				map_undo_push();
				sz=(max_node_link-n)-1;
				if (sz>0) memmove(&map.nodes[node_idx].link[n],&map.nodes[node_idx].link[n+1],(sz*sizeof(short)));
				map.nodes[node_idx].link[max_node_link-1]=-1;
				return(TRUE);
			}
		}
		
		return(FALSE);
	}
	
		// add link mode
		
	if (state.map.node_mode==node_mode_link) {
		
		k1=node_link_get_free_link(org_node_idx);
		k2=node_link_get_free_link(node_idx);
		
		if ((k1==-1) || (k2==-1)) {
			os_dialog_alert("Can not connect nodes","You've reached the maximum number of connected nodes for this node.");
			return(FALSE);
		}
		
		map_undo_push();
		
		if (!node_link_has_link(org_node_idx,node_idx)) map.nodes[org_node_idx].link[k1]=node_idx;
		if (!node_link_has_link(node_idx,org_node_idx)) map.nodes[node_idx].link[k2]=org_node_idx;
		
		node_link_select(node_idx);
		
		return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Duplicate and Drag Nodes
      
======================================================= */

int node_duplicate_and_drag(int org_node_idx)
{
	int				n,node_idx,k1,k2;
	
		// no more nodes
		
	if (map.nnode==max_node) {
		os_dialog_alert("Can Not Create Node","You've reached the maximum number of nodes for a map.");
		return(-1);
	}
	
		// duplicate current node
		
	node_idx=map.nnode;

	memmove(&map.nodes[map.nnode],&map.nodes[org_node_idx],sizeof(node_type));

	for (n=0;n!=max_node_link;n++) {
		map.nodes[map.nnode].link[n]=-1;
	}

	map.nnode++;
	
		// link them
		
	k1=node_link_get_free_link(org_node_idx);
	k2=node_link_get_free_link(node_idx);
	
	map.nodes[org_node_idx].link[k1]=node_idx;
	map.nodes[node_idx].link[k2]=org_node_idx;

	return(node_idx);
}

/* =======================================================

      Rebuild Node Paths
      
======================================================= */

void node_path_trace_all(int org_node_idx,int org_link_idx,int node_idx,int cur_dist,unsigned char *node_hit,int level)
{
	int				n,nxt_node_idx,dist;
	node_type		*node;
	
		// if at the top, node = original node
		// and clear the hit list and win distance
		// lists.
		
	if (level==0) {
		node_idx=org_node_idx;
		cur_dist=0;
		
		memset(node_hit,0x0,map.nnode);
		
		for (n=0;n!=map.nnode;n++) {
			node_scan[n].win_link_idx=-1;
		}
	}
	
	node_hit[node_idx]=0x1;
		
		// trace all paths from this node
		
	node=&map.nodes[node_idx];
		
	for (n=0;n!=max_node_link;n++) {
	
			// if we are at the top, set the original
			// link index for distance writes
			
		if (level==0) org_link_idx=n;
		
			// get next node
			
		nxt_node_idx=node->link[n];
		if (nxt_node_idx==-1) break;
		
			// already hit?
			
		if (node_hit[nxt_node_idx]!=0x0) continue;
		
			// see if this node's distance is
			// already greater than the current winning distance
			// or is the winning distance
			
		dist=cur_dist+node_scan[node_idx].link_dist[n];
		
		if ((node_scan[nxt_node_idx].win_link_idx!=-1) && (dist>node_scan[nxt_node_idx].win_dist)) continue;
		
		if ((node_scan[nxt_node_idx].win_link_idx==-1) || (dist<node_scan[nxt_node_idx].win_dist)) {
			node_scan[nxt_node_idx].win_link_idx=org_link_idx;
			node_scan[nxt_node_idx].win_dist=dist;
		}
		
			// continue down the path
		
		node_path_trace_all(org_node_idx,org_link_idx,nxt_node_idx,dist,node_hit,(level+1));
	}
	
		// backing up, free this node for additional searches
		
	node_hit[node_idx]=0x0;
	
		// if we are at the first level, then
		// we can set all the path wins as hints
		
	if (level!=0) return;
	
	for (n=0;n!=map.nnode;n++) {
		if (n!=node_idx) {
			node->path_hint[n]=node->link[node_scan[n].win_link_idx];
		}
	}
}

void node_path_rebuild(void)
{
    int				i,x,z,k,n;
	unsigned char	node_hit[max_node];
	node_type		*node,*to_node;
	
        // precalc the distance between each
		// directly linked node
        
	node=map.nodes;
		
    for (n=0;n!=map.nnode;n++) {
    
        x=node->pnt.x;
        z=node->pnt.z;
		
			// get distance to each directly linked node
			
        for (i=0;i!=max_node_link;i++) {
            k=node->link[i];
            if ((k==-1) || (k==n)) {
                node_scan[n].link_dist[i]=0;
            }
            else {
				to_node=&map.nodes[k];
            	node_scan[n].link_dist[i]=distance_2D_get(x,z,to_node->pnt.x,to_node->pnt.z);
            }
        }
        
            // clear out path hints

        for (k=0;k!=map.nnode;k++) {
            node->path_hint[k]=-1;
        }
        
        node++;
    }
    
        // find the best path between all nodes
	
    for (n=0;n!=map.nnode;n++) {
		node_path_trace_all(n,0,0,0,node_hit,0);
    }
}

