/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: File Searches

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
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Add files to Path Directory List
            
======================================================= */

int file_paths_add_file(file_path_directory_type *fpd,int parent_idx,char *name,char *sub_path,bool is_dir,bool remove_subfile)
{
	int				n,k,idx,sz;
	char			*c,file_name[file_str_len];
	
		// any more spots?
		
	if (fpd->nfile>=file_paths_max_directory_file) return(-1);
	
		// copy the name and
		// remove the extension
		
	strncpy(file_name,name,file_str_len);
	file_name[file_str_len-1]=0x0;
	
	if (sub_path!=NULL) {
		string_safe_strcat(file_name,"/",file_str_len);
		file_name[file_str_len-1]=0x0;
		
		string_safe_strcat(file_name,sub_path,file_str_len);
		file_name[file_str_len-1]=0x0;
	}
	else {	
		c=strrchr(file_name,'.');
		if (c!=NULL) *c=0x0;
	}
	
		// check for sub file removal
		
	if (remove_subfile) {
		k=strlen(file_name);
		if (k>2) {
			if (file_name[k-2]=='_') {
				if ((file_name[k-1]=='n') || (file_name[k-1]=='s') || (file_name[k-1]=='g')) return(-1);
			}
		}
	}
	
		// see if we need to override a file
		// already in the list
		
	idx=-1;
	
	for (n=0;n!=fpd->nfile;n++) {
		if ((strcasecmp(fpd->files[n].file_name,file_name)==0) && (fpd->files[n].parent_idx==parent_idx)) {
			idx=n;
			break;
		}
	}
	
		// remove old one
		
	if (idx!=-1) {
		sz=(fpd->nfile-(idx+1))*sizeof(file_path_directory_file_type);
		if (sz>0) memmove(&fpd->files[idx],&fpd->files[idx+1],sz);
		fpd->nfile--;
	}
	
		// add new one
		
	fpd->files[fpd->nfile].parent_idx=parent_idx;
	fpd->files[fpd->nfile].is_dir=is_dir;
	strcpy(fpd->files[fpd->nfile].file_name,file_name);
	
	fpd->nfile++;
	
	return(fpd->nfile-1);
}

/* =======================================================

      Determine if Directory has Required File
            
======================================================= */

bool file_paths_check_required_file(char *path,char *path_add,char *dir_name,char *required_file_name)
{
	char					str[256],path2[1024];
	char					*c,*c2;
	struct stat				sb;

		// loop through multiple required file names

	c=required_file_name;

	while (TRUE) {
		if (*c==0x0) break;

		strcpy(str,c);
		c2=strchr(c,';');
		if (c2!=NULL) str[c2-c]=0x0;

		if (path_add!=NULL) {
			sprintf(path2,"%s/%s/%s/%s",path,path_add,dir_name,str);
		}
		else {
			sprintf(path2,"%s/%s/%s",path,dir_name,str);
		}
				
		if (stat(path2,&sb)==0) return(TRUE);

		if (c2==NULL) break;
		c=c2+1;
	}

	return(FALSE);
}

/* =======================================================

      Read Directories OS X/Linux
            
======================================================= */

#ifndef D3_OS_WINDOWS

void file_paths_read_directory_files(file_path_directory_type *fpd,int parent_idx,char *path,char *path_add,char *ext_name)
{
	int						next_parent_idx;
	char					*c,path2[1024],path_add2[1024];
	DIR						*dir;
	struct dirent			*de;
	
	if (path_add!=NULL) {
		sprintf(path2,"%s/%s",path,path_add);
	}
	else {
		strcpy(path2,path);
	}

	dir=opendir(path2);
	if (dir==NULL) return;
	
	while (TRUE) {
		de=readdir(dir);
		if (de==NULL) break;
		
		if (de->d_name[0]=='.') continue;
		
			// check extension
			
		c=strrchr(de->d_name,'.');
			
			// go deeper into directories

		if (c==NULL) {
		
				// add directory to file list
				
			next_parent_idx=file_paths_add_file(fpd,parent_idx,de->d_name,NULL,TRUE,FALSE);
			if (next_parent_idx==-1) break;
		
				// into next path
				
			if (path_add!=NULL) {
				sprintf(path_add2,"%s/%s",path_add,de->d_name);
			}
			else {
				strcpy(path_add2,de->d_name);
			}
			
			file_paths_read_directory_files(fpd,next_parent_idx,path,path_add2,ext_name);
			continue;
		}
		
			// looking for extension
			
		if (strcasecmp((c+1),ext_name)!=0) continue;
		
			// add file
			
		file_paths_add_file(fpd,parent_idx,de->d_name,NULL,FALSE,TRUE);
	}
	
	closedir(dir);
}

void file_paths_read_directory_dirs(file_path_directory_type *fpd,int parent_idx,char *path,char *path_add,char *required_file_name)
{
	int						next_parent_idx;
	char					*c,path2[1024],path_add2[1024];
	DIR						*dir;
	struct dirent			*de;
	
	if (path_add!=NULL) {
		sprintf(path2,"%s/%s",path,path_add);
	}
	else {
		strcpy(path2,path);
	}

	dir=opendir(path2);
	if (dir==NULL) return;
	
	while (TRUE) {
		de=readdir(dir);
		if (de==NULL) break;
		
		if (de->d_name[0]=='.') continue;
		
			// check for directory
			
		c=strrchr(de->d_name,'.');
		if (c!=NULL) continue;
		
			// check for required file name
			// inside of directory, if it exists, add directory
			// otherwise recurse further
			
		if (!file_paths_check_required_file(path,path_add,de->d_name,required_file_name)) {
		
				// add directory to file list
				
			next_parent_idx=file_paths_add_file(fpd,parent_idx,de->d_name,NULL,TRUE,FALSE);
			if (next_parent_idx==-1) break;
		
				// into next path
			
			if (path_add!=NULL) {
				sprintf(path_add2,"%s/%s",path_add,de->d_name);
			}
			else {
				strcpy(path_add2,de->d_name);
			}
			
			file_paths_read_directory_dirs(fpd,next_parent_idx,path,path_add2,required_file_name);
			continue;
		}
		
			// add file
			
		file_paths_add_file(fpd,parent_idx,de->d_name,NULL,FALSE,FALSE);
	}
	
	closedir(dir);
}

#endif

/* =======================================================

      Read Directories Windows
            
======================================================= */

#ifdef D3_OS_WINDOWS

void file_paths_read_directory_files(file_path_directory_type *fpd,int parent_idx,char *path,char *path_add,char *ext_name)
{
	int					next_parent_idx;
	char				*c,path2[1024],path_add2[1024];
	bool				find_first;
	WIN32_FIND_DATA		find_data;
	HANDLE				find_hand;

	if (path_add!=NULL) {
		sprintf(path2,"%s\\%s\\*",path,path_add);
	}
	else {
		sprintf(path2,"%s\\*",path);
	}

	find_hand=FindFirstFile(path2,&find_data);
	if (find_hand==INVALID_HANDLE_VALUE) return;
	
	find_first=TRUE;

	while (TRUE) {

		if (!find_first) {
			if (!FindNextFile(find_hand,&find_data)) break;
		}
		find_first=FALSE;

			// skip dot paths

		if (find_data.cFileName[0]=='.') continue;
		
			// go deeper into directories

		if ((find_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0) {
			
				// add directory to file list
				
			next_parent_idx=file_paths_add_file(fpd,parent_idx,find_data.cFileName,NULL,TRUE,FALSE);
			if (next_parent_idx==-1) break;
		
				// into next path
				
			if (path_add!=NULL) {
				sprintf(path_add2,"%s\\%s",path_add,find_data.cFileName);
			}
			else {
				strcpy(path_add2,find_data.cFileName);
			}

			file_paths_read_directory_files(fpd,next_parent_idx,path,path_add2,ext_name);
			continue;
		}

			// check extension

		c=strrchr(find_data.cFileName,'.');
		if ((c==NULL) || (ext_name==NULL)) continue;

		if (strcasecmp((c+1),ext_name)!=0) continue;

			// add file to list
		
		file_paths_add_file(fpd,parent_idx,find_data.cFileName,NULL,FALSE,TRUE);
	}

	FindClose(find_hand);
}

void file_paths_read_directory_dirs(file_path_directory_type *fpd,int parent_idx,char *path,char *path_add,char *required_file_name)
{
	int					next_parent_idx;
	char				path2[1024],path_add2[1024];
	bool				find_first;
	WIN32_FIND_DATA		find_data;
	HANDLE				find_hand;

	if (path_add!=NULL) {
		sprintf(path2,"%s\\%s\\*",path,path_add);
	}
	else {
		sprintf(path2,"%s\\*",path);
	}

	find_hand=FindFirstFile(path2,&find_data);
	if (find_hand==INVALID_HANDLE_VALUE) return;
	
	find_first=TRUE;

	while (TRUE) {

		if (!find_first) {
			if (!FindNextFile(find_hand,&find_data)) break;
		}
		find_first=FALSE;

			// skip dot paths

		if (find_data.cFileName[0]=='.') continue;
		
			// check for directory

		if ((find_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) continue;
		
			// check for required file name
			// inside of directory, if it exists, add directory
			// otherwise recurse further
			
		if (!file_paths_check_required_file(path,path_add,find_data.cFileName,required_file_name)) {
		
				// add directory to file list
				
			next_parent_idx=file_paths_add_file(fpd,parent_idx,find_data.cFileName,NULL,TRUE,FALSE);
			if (next_parent_idx==-1) break;
		
				// into next path
		
			if (path_add!=NULL) {
				sprintf(path_add2,"%s\\%s",path_add,find_data.cFileName);
			}
			else {
				strcpy(path_add2,find_data.cFileName);
			}

			file_paths_read_directory_dirs(fpd,next_parent_idx,path,path_add2,required_file_name);
			continue;
		}

			// add file to list
		
		file_paths_add_file(fpd,parent_idx,find_data.cFileName,NULL,FALSE,FALSE);
	}

	FindClose(find_hand);
}

#endif

/* =======================================================

      Generic Routines
            
======================================================= */

file_path_directory_type* file_paths_read_directory_data(file_path_setup_type *file_path_setup,char *sub_path,char *ext_name)
{
	int							parent_idx;
	char						path[1024];
	file_path_directory_type	*fpd;
	
		// create memory for file list
		
	fpd=malloc(sizeof(file_path_directory_type));
	memset(fpd,0x0,sizeof(file_path_directory_type));
	
	fpd->nfile=0;

		// run through the possible dim3
		// data directories (inside app, data, data2)

	if (file_path_setup->path_app[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_app);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		parent_idx=file_paths_add_file(fpd,-1,"App",sub_path,TRUE,FALSE);
		file_paths_read_directory_files(fpd,parent_idx,path,NULL,ext_name);
	}
	
	if (file_path_setup->path_data[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_data);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		parent_idx=file_paths_add_file(fpd,-1,"Data",sub_path,TRUE,FALSE);
		file_paths_read_directory_files(fpd,parent_idx,path,NULL,ext_name);
	}
	
	if (file_path_setup->path_data_2[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_data_2);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		parent_idx=file_paths_add_file(fpd,-1,"Data2",sub_path,TRUE,FALSE);
		file_paths_read_directory_files(fpd,parent_idx,path,NULL,ext_name);
	}

	return(fpd);
}

file_path_directory_type* file_paths_read_directory_data_dir(file_path_setup_type *file_path_setup,char *sub_path,char *required_file_name)
{
	int							parent_idx;
	char						path[1024];
	file_path_directory_type	*fpd;
	
		// create memory for file list
		
	fpd=malloc(sizeof(file_path_directory_type));
	memset(fpd,0x0,sizeof(file_path_directory_type));
	
	fpd->nfile=0;

		// run through the dim3 directories

	if (file_path_setup->path_app[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_app);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		parent_idx=file_paths_add_file(fpd,-1,"App",sub_path,TRUE,FALSE);
		file_paths_read_directory_dirs(fpd,parent_idx,path,NULL,required_file_name);
	}
	
	if (file_path_setup->path_data[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_data);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		parent_idx=file_paths_add_file(fpd,-1,"Data",sub_path,TRUE,FALSE);
		file_paths_read_directory_dirs(fpd,parent_idx,path,NULL,required_file_name);
	}
	
	if (file_path_setup->path_data_2[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_data_2);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		parent_idx=file_paths_add_file(fpd,-1,"Data2",sub_path,TRUE,FALSE);
		file_paths_read_directory_dirs(fpd,parent_idx,path,NULL,required_file_name);
	}

	return(fpd);
}

file_path_directory_type* file_paths_read_directory_document(file_path_setup_type *file_path_setup,char *sub_path,char *ext_name)
{
	int							parent_idx;
	char						path[1024];
	file_path_directory_type	*fpd;
	
		// create memory for file list
		
	fpd=malloc(sizeof(file_path_directory_type));
	memset(fpd,0x0,sizeof(file_path_directory_type));
	
	fpd->nfile=0;

		// read the files
		
	file_paths_app_data(file_path_setup,path,sub_path,NULL,NULL);
	parent_idx=file_paths_add_file(fpd,-1,"Documents",NULL,TRUE,FALSE);
	file_paths_read_directory_files(fpd,parent_idx,path,NULL,ext_name);

	return(fpd);
}

void file_paths_close_directory(file_path_directory_type *fpd)
{
	if (fpd!=NULL) free(fpd);
}

/* =======================================================

      File Path Utilities
            
======================================================= */

int file_paths_count_children(file_path_directory_type *fpd,int index)
{
	int				n,count;

	count=0;

	for (n=0;n!=fpd->nfile;n++) {
		if (fpd->files[n].parent_idx==index) count++;
	}

	return(count);
}

void file_paths_get_complete_path_from_index_recurse(file_path_directory_type *fpd,int idx,char *file_name)
{
		// never add top levels
		
	if (fpd->files[idx].parent_idx==-1) return;
	
		// add higher level first
		
	file_paths_get_complete_path_from_index_recurse(fpd,fpd->files[idx].parent_idx,file_name);
	
		// add this level
		
	if (file_name[0]!=0x0) {
		string_safe_strcat(file_name,"/",file_str_len);
		file_name[file_str_len-1]=0x0;
	}
	
	string_safe_strcat(file_name,fpd->files[idx].file_name,file_str_len);
	file_name[file_str_len-1]=0x0;
}
	
void file_paths_get_complete_path_from_index(file_path_directory_type *fpd,int idx,char *file_name)
{
	file_name[0]=0x0;
	file_paths_get_complete_path_from_index_recurse(fpd,idx,file_name);
}

/* =======================================================

      Project Hash
            
======================================================= */

int file_paths_file_hash(char *path,char *file_name)
{
	int				n,hash,fsz;
	char			*c,path2[1024];
	unsigned char	*hp,*data;
	FILE			*file;
	struct stat		sb;

		// only hash js and xml files

	c=strchr(file_name,'.');
	if (c==NULL) return(0);

	c++;
	if ((strcasecmp(c,"js")!=0) && (strcasecmp(c,"xml")!=0)) return(0);

		// get file size

	sprintf(path2,"%s/%s",path,file_name);
	if (stat(path2,&sb)!=0) return(0);

	fsz=(int)sb.st_size;

		// read file

	data=(unsigned char*)malloc(fsz);
	if (data==NULL) return(0);

	file=fopen(path2,"rb");
	if (file==NULL) {
		free(data);
		return(0);
	}

	fread(data,1,fsz,file);
	fclose(file);

		// get the hash

	hash=0;
	hp=data;

	for (n=0;n!=fsz;n++) {
		hash+=(int)*hp++;
	}

	free(data);

	return(hash+fsz);
}

#ifndef D3_OS_WINDOWS

int file_paths_project_hash(char *path)
{
	int					hash;
	char				*c,path2[1024];
	DIR					*dir;
	struct dirent		*de;
	
	dir=opendir(path);
	if (dir==NULL) return(0);
	
	hash=0;

	while (TRUE) {
		de=readdir(dir);
		if (de==NULL) break;

			// skip dot paths
		
		if (de->d_name[0]=='.') continue;
		
			// recurse into directories
			
		c=strrchr(de->d_name,'.');
		if (c==NULL) {
			sprintf(path2,"%s/%s",path,de->d_name);
			hash+=file_paths_project_hash(path2);
			continue;
		}

			// get file hash

		hash+=file_paths_file_hash(path,de->d_name);
	}
	
	closedir(dir);

	return(hash);
}

#else

int file_paths_project_hash(char *path)
{
	int					hash;
	char				find_path[1024];
	bool				find_first;
	WIN32_FIND_DATA		find_data;
	HANDLE				find_hand;

	sprintf(find_path,"%s\\*",path);

	find_hand=FindFirstFile(find_path,&find_data);
	if (find_hand==INVALID_HANDLE_VALUE) return(0);
	
	hash=0;
	find_first=TRUE;

	while (TRUE) {

		if (!find_first) {
			if (!FindNextFile(find_hand,&find_data)) break;
		}
		find_first=FALSE;

			// skip dot paths

		if (find_data.cFileName[0]=='.') continue;
		
			// recurse into directories

		if ((find_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0) {
			sprintf(find_path,"%s\\%s",path,find_data.cFileName);
			hash+=file_paths_project_hash(find_path);
			continue;
		}

			// get file hash

		hash+=file_paths_file_hash(path,find_data.cFileName);
	}

	FindClose(find_hand);

	return(hash);
}

#endif