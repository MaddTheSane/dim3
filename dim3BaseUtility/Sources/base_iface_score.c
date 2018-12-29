/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Score XML

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

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Read Score
      
======================================================= */

void score_xml_read(iface_type *iface)
{
	int						n,count,
							head_tag,tag;
	char					path[1024];
	iface_score_type		*score;
	
		// clear scores
		
	score=iface->score_list.scores;
		
	for (n=0;n!=max_score_count;n++) {
		score->name[0]=0x0;
		score->score=0;
		score++;
	}
	
		// any file to load
		
	if (!file_paths_app_data_exist(&file_path_setup,path,"Settings","Score","xml")) return;
	
		// read file
		
	if (!xml_open_file(path)) return;
    
		// decode the file
       
    head_tag=xml_findrootchild("Scores");
    if (head_tag!=-1) {

		score=iface->score_list.scores;
	
		count=xml_countchildren(head_tag);
		tag=xml_findfirstchild("Score",head_tag);
		
        for (n=0;n!=count;n++) {
			xml_get_attribute_text(tag,"name",score->name,64);
			score->score=xml_get_attribute_int(tag,"score");

			score++;
			tag=xml_findnextchild(tag);
        }
	}
  
	xml_close_file();
}

/* =======================================================

      Write Score
      
======================================================= */

void score_xml_write(iface_type *iface,char *err_str)
{
	int						n;
	char					path[1024];
	bool					ok;
	iface_score_type		*score;
	
		// start the file
		
    xml_new_file();
	
		// create the xml
    
    xml_add_tagstart("Scores");
    xml_add_tagend(FALSE);
	
	score=iface->score_list.scores;
	
	for (n=0;n!=max_score_count;n++) {
		xml_add_tagstart("Score");
		xml_add_attribute_text("name",score->name);
		xml_add_attribute_int("score",score->score);
		xml_add_tagend(TRUE);
		
		score++;
	}

    xml_add_tagclose("Scores");

        // save the file
		
	file_paths_app_data(&file_path_setup,path,"Settings","Score","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
}

/* =======================================================

      Add Score
      
======================================================= */

int score_add(iface_type *iface,char *name,int high_score)
{
	int					n,idx,sz;
	char				err_str[256];
	iface_score_type	*score;
	
		// read score file
		
	score_xml_read(iface);
	
		// find place for score
		
	idx=-1;
	score=iface->score_list.scores;
	
	for (n=0;n!=max_score_count;n++) {
		if (high_score>score->score) {
			idx=n;
			break;
		}
		score++;
	}
	
	if (idx==-1) return(-1);
	
		// add score
		
	sz=(max_score_count-idx)-1;
	if (sz>0) memmove(&score[idx+1],&score[idx],(sz*sizeof(iface_score_type)));
	
	strcpy(iface->score_list.scores[idx].name,name);
	iface->score_list.scores[idx].score=high_score;
		
		// write back score file
	
	score_xml_write(iface,err_str);
	
	return(idx);			// return the place
}
