/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Obj-C Dialog Outline DataSource

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#import "AppDelegate.h"
#import "DialogOutlineDataSource.h"
#include "interface.h"

extern file_path_directory_type		*diag_fpd;

int									nfile_root;
bool								initialized;
NSString							*file_ns_strs[file_paths_max_directory_file];

@implementation DialogOutlineDataSource

/* =======================================================

      Delegate Setup
	        
======================================================= */

-(id)init
{
	int			n;
	
	self=[super init];
	
	initialized=false;
	
	for (n=0;n!=file_paths_max_directory_file;n++) {
		file_ns_strs[n]=nil;
	}
	
	return(self);
}

-(void)setupStrings
{
	int			n;

	nfile_root=0;
	
	for (n=0;n!=diag_fpd->nfile;n++) {
		file_ns_strs[n]=[[NSString alloc] initWithUTF8String:diag_fpd->files[n].file_name];
		if (diag_fpd->files[n].is_dir) nfile_root++;
	}

	initialized=TRUE;
}

-(void)freeStrings
{
	int			n;
	
	if (!initialized) return;
	
	for (n=0;n!=file_paths_max_directory_file;n++) {
		if (file_ns_strs[n]!=nil) [file_ns_strs[n] release];
	}
	
	initialized=FALSE;
}

/* =======================================================

      Item to Index
	        
======================================================= */

NSString* diag_datasource_item_for_index(int parent_idx,int idx)
{
	int				n,child_idx;
	
	child_idx=0;
	
	for (n=0;n!=diag_fpd->nfile;n++) {
		if (diag_fpd->files[n].parent_idx==parent_idx) {
			if (child_idx==idx) return(file_ns_strs[n]);
			child_idx++;
		}
	}

	return(nil);
}

int diag_datasource_index_count_children(int parent_idx)
{
	int				n,child_count;
	
	child_count=0;
	
	for (n=0;n!=diag_fpd->nfile;n++) {
		if (diag_fpd->files[n].parent_idx==parent_idx) child_count++;
	}

	return(child_count);
}

int diag_datasource_index_for_item(NSString *item)
{
	int				n;
	
	for (n=0;n!=diag_fpd->nfile;n++) {
		if (file_ns_strs[n]==item) return(n);
	}

	return(-1);
}

/* =======================================================

      Item - Index Functions
	        
======================================================= */

-(NSObject*)indexToItem:(int)idx
{
	return(file_ns_strs[idx]);
}

-(int)itemToIndex:(NSObject*)item
{
	return(diag_datasource_index_for_item((NSString*)item));
}

/* =======================================================

      Delegate Messages
	        
======================================================= */

-(BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
	int				idx;
	
	if (!initialized) return(NO);
	
	idx=diag_datasource_index_for_item(item);
	if (idx==-1) return(NO);
	
	return(diag_datasource_index_count_children(idx)!=0);
}

-(NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
	int				idx;
	
	if (!initialized) return(0);
	
		// root item
		
	if (item==nil) return(nfile_root);
	
		// find index for item
		
	idx=diag_datasource_index_for_item(item);
	if (idx==-1) return(0);
	
	return(diag_datasource_index_count_children(idx));
}

-(id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
	int				parent_idx;
	
	if (!initialized) return(nil);
	
		// item id for root items
		// we just use the NSString for this, as it's unique
		
	if (item==nil) return(diag_datasource_item_for_index(-1,index));
	
		// item id for children items
		
	parent_idx=diag_datasource_index_for_item(item);
	if (parent_idx==-1) return(nil);
	
	return(diag_datasource_item_for_index(parent_idx,index));
}

-(id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)theColumn byItem:(id)item
{
	int				idx;
	
	if (!initialized) return(nil);
	
	idx=diag_datasource_index_for_item(item);
	if (idx==-1) return(nil);
	
	return(file_ns_strs[idx]);
}


@end
