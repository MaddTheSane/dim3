/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Music

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
	#include "dim3engine.h"
#endif

#include "interface.h"

extern int						audio_music_song_idx,audio_music_alt_song_idx,
								audio_music_global_volume;
extern bool						audio_music_paused,audio_music_loop;
extern audio_music_song_type	audio_music_song_cache[music_max_song_cache];

int								audio_music_fade_next_msec;
bool							audio_music_alt_stage_loop;
char							audio_music_fade_next_name[name_str_len];

extern map_type					map;
extern setup_type				setup;
extern file_path_setup_type		file_path_setup;

/* =======================================================

      Music Initialize and Shutdown
      
======================================================= */

void al_music_initialize_song(audio_music_song_type *song)
{
	song->fade_mode=music_fade_mode_none;
	song->name[0]=0x0;
	song->data=NULL;
	song->volume=600;
}

void al_music_initialize_cache(void)
{
	int						n;
	audio_music_song_type	*song;
	
	song=audio_music_song_cache;
	
	for (n=0;n!=music_max_song_cache;n++) {
		al_music_initialize_song(song);
		song++;
	}
}

void al_music_clear_cache(void)
{
	int						n;
	audio_music_song_type	*song;
	
	song=audio_music_song_cache;
		
	for (n=0;n!=music_max_song_cache;n++) {
		if (song->data!=NULL) free(song->data);
		al_music_initialize_song(song);
		song++;
	}
}

bool al_music_initialize(char *err_str)
{
		// initialize the mp3 decoder library
		
	if (mpg123_init()!=MPG123_OK) {
		strcpy(err_str,"Could not initialize mp3 decoder");
		return(FALSE);
	}
	
		// setup music
		
	audio_music_paused=FALSE;
	
	audio_music_song_idx=-1;
	audio_music_alt_song_idx=-1;
	
	audio_music_global_volume=600;

		// initialize the cache

	al_music_initialize_cache();

		// load in any pre-cached songs

	return(TRUE);
}

void al_music_shutdown(void)
{
		// free the cache
		
	al_music_clear_cache();
	
		// shut down the mp3 decoder library
		
	mpg123_exit();
}

/* =======================================================

      Load MP3
      
======================================================= */

short* al_music_load_mp3(char *name,float *f_sample_len,float *freq_factor,char *err_str)
{
	int						err,channels,encoding;
	size_t					sample_size,read_bytes;
	long					rate;
	char					path[1024];
	unsigned char			*data;
	mpg123_handle			*mh;

		// load mp3
		
	file_paths_data(&file_path_setup,path,"Music",name,"mp3");

	mh=mpg123_new(NULL,&err);
	if (mh==NULL) {
		strcpy(err_str,"Out of Memory");
		return(NULL);
	}
	
	if (mpg123_open(mh,path)!=MPG123_OK) {
		mpg123_delete(mh);
		sprintf(err_str,"Unable to open %s\n",path);
		return(NULL);
	}
	
		// we need stereo and 16 signed
		
	mpg123_getformat(mh,&rate,&channels,&encoding);
	
	if ((channels!=2) || (encoding!=MPG123_ENC_SIGNED_16)) {
		strcpy(err_str,"Music requires 16 bit stereo MP3");
		mpg123_close(mh);
		mpg123_delete(mh);
		return(NULL);
	}
	
		// make sure it's 16 bit and stereo
		
	mpg123_format_none(mh);
	mpg123_format(mh,rate,2,MPG123_ENC_SIGNED_16);
	
		// get size
		
	if (mpg123_scan(mh)!=MPG123_OK) {
		mpg123_close(mh);
		mpg123_delete(mh);
		sprintf(err_str,"Unable to read %s\n",path);
		return(NULL);
	}
	
		// get buffer size
		// each sample is 16 bits * 2 channels
		
	sample_size=mpg123_length(mh);
	sample_size*=(2*channels);
	data=(unsigned char*)malloc(sample_size);
	if (data==NULL) {
		mpg123_close(mh);
		mpg123_delete(mh);
		sprintf(err_str,"Unable to read %s\n",path);
		return(NULL);
	}

		// read it
	
	err=mpg123_read(mh,data,sample_size,&read_bytes);
	
		// end the file
		
	mpg123_close(mh);
	mpg123_delete(mh);
	
		// check for success of read
		
	if ((err!=MPG123_OK) && (err!=MPG123_DONE)) {
		sprintf(err_str,"Unable to read %s\n",path);
		return(NULL);
	}
	
		// pass back the data
		// we need to alter some of these factors
		// for stereo and 16-bit music

	*f_sample_len=(float)(read_bytes/2);
	*freq_factor=((float)rate)/((float)audio_frequency)*2.0f;

	return((short*)data);
}

/* =======================================================

      Music Cache
      
======================================================= */

int al_music_load(char *name,char *err_str)
{
	int						n,idx;
	audio_music_song_type	*song;

		// any music?

	if (name[0]==0x0) return(-1);

		// in cache?

	idx=-1;
	
	song=audio_music_song_cache;

	for (n=0;n!=music_max_song_cache;n++) {
		if (song->data==NULL) {
			if (idx==-1) idx=n;				// remember the first blank item
		}
		else {
			if (strcasecmp(name,song->name)==0) return(n);
		}
		song++;
	}

		// room to add to cache?

	if (idx==-1) {
		strcpy(err_str,"Music cache full");
		return(-1);
	}

		// add to cache

	song=&audio_music_song_cache[idx];

	strcpy(song->name,name);
	song->data=al_music_load_mp3(name,&song->f_sample_len,&song->freq_factor,err_str);
	if (song->data==NULL) return(-1);
	
	return(idx);
}

void al_music_map_pre_cache(void)
{
	int			n;
	char		err_str[256];
	
		// make sure everything is stopped
		
	SDL_LockAudio();

	audio_music_song_idx=-1;
	audio_music_alt_song_idx=-1;
	
		// clean the cache if we are
		// going into a new map
		
	al_music_clear_cache();

		// pre load anything map related
		
	al_music_load(map.music.name,err_str);

	for (n=0;n!=max_music_preload;n++) {
		al_music_load(map.music.preload_name[n],err_str);
	}
	
	SDL_UnlockAudio();
}

/* =======================================================

      Start and Stop Music
      
======================================================= */

int al_music_play_song(char *name,char *err_str)
{
	int						idx;
	audio_music_song_type	*song;
	
	SDL_LockAudio();
	
		// open music
		
	idx=al_music_load(name,err_str);
	if (idx==-1) {
		SDL_UnlockAudio();
		return(-1);
	}
	
		// play
		
	song=&audio_music_song_cache[idx];

	song->stream_pos=0.0f;
	song->volume=audio_music_global_volume;
	song->fade_mode=music_fade_mode_none;

	SDL_UnlockAudio();

	return(idx);
}

bool al_music_play(char *name,char *err_str)
{
	audio_music_song_idx=al_music_play_song(name,err_str);
	return(audio_music_song_idx!=-1);
}

void al_music_stop(void)
{
	SDL_LockAudio();

	audio_music_song_idx=-1;
	audio_music_alt_song_idx=-1;

	SDL_UnlockAudio();
}

void al_music_set_loop(bool loop)
{
	SDL_LockAudio();
	
		// if we have an alt music (during a cross fade)
		// then we stage the loop setting until the alt
		// music gets moved over
		
	if (audio_music_alt_song_idx==-1) {
		audio_music_loop=loop;
	}
	else {
		audio_music_alt_stage_loop=loop;
	}
	
	SDL_UnlockAudio();
}

void al_music_pause(void)
{
	SDL_LockAudio();
	audio_music_paused=TRUE;
	SDL_UnlockAudio();
}

void al_music_resume(void)
{
	SDL_LockAudio();
	audio_music_paused=FALSE;
	SDL_UnlockAudio();
}

bool al_music_playing(void)
{
	return((!audio_music_paused) && (audio_music_song_idx!=-1));
}

bool al_music_playing_is_name(char *name)
{
	if ((audio_music_paused) || (audio_music_song_idx==-1)) return(FALSE);
	return(strcasecmp(audio_music_song_cache[audio_music_song_idx].name,name)==0);
}

/* =======================================================

      Set Music Volume and State
      
======================================================= */

void al_music_set_volume(float music_volume)
{
	audio_music_global_volume=(int)(1024.0f*music_volume);
}

void al_music_set_state(bool music_on)
{
	if (audio_music_paused!=music_on) return;
	
	if (music_on) {
		al_music_resume();
	}
	else {
		al_music_pause();
	}
	
	audio_music_paused=!music_on;
}

/* =======================================================

      Swap Alternate Music
      
======================================================= */

void al_music_swap_alt_music(void)
{
	audio_music_song_type	*song;
	
	SDL_LockAudio();
	
		// switch the songs
		
	audio_music_song_idx=audio_music_alt_song_idx;
	audio_music_alt_song_idx=-1;
		
		// set all the flags
		
	song=&audio_music_song_cache[audio_music_song_idx];
	
	song->fade_mode=music_fade_mode_none;
	song->volume=audio_music_global_volume;
	
		// fix the loop which could have
		// been staged when alt music was playing
		
	audio_music_loop=audio_music_alt_stage_loop;
				
	SDL_UnlockAudio();
}

/* =======================================================

      Music Fades and Crosses
      
======================================================= */

bool al_music_fade_in(char *name,int msec,char *err_str)
{
	audio_music_song_type	*song;

		// start music

	audio_music_song_idx=al_music_play_song(name,err_str);
	if (audio_music_song_idx==-1) return(FALSE);

	song=&audio_music_song_cache[audio_music_song_idx];

		// if no msec, then just play music

	if (msec<=0) {
		song->fade_mode=music_fade_mode_none;
		song->volume=audio_music_global_volume;
		return(TRUE);
	}
	
		// start fade in

	song->fade_mode=music_fade_mode_in;
	song->fade_start_tick=game_time_get();
	song->fade_msec=msec;
	
	song->volume=0;

	return(TRUE);
}

void al_music_fade_out(int msec)
{
	audio_music_song_type	*song;

		// if no music playing, no fade out

	if (audio_music_song_idx==-1) return;
	
	song=&audio_music_song_cache[audio_music_song_idx];
	
		// if no msec, then just stop music

	if (msec<=0) {
		al_music_stop();
		song->fade_mode=music_fade_mode_none;
		return;
	}

		// start fade out

	song->fade_mode=music_fade_mode_out;
	song->fade_start_tick=game_time_get();
	song->fade_msec=msec;
	song->volume=audio_music_global_volume;
}

bool al_music_fade_out_fade_in(char *name,int fade_out_msec,int fade_in_msec,char *err_str)
{
	audio_music_song_type	*song;

		// if no fade out or no music playing, go directly to fade in

	if ((fade_out_msec<=0) || (audio_music_song_idx==-1)) {
		return(al_music_fade_in(name,fade_in_msec,err_str));
	}

		// setup next music for fade in

	song=&audio_music_song_cache[audio_music_song_idx];

	strcpy(audio_music_fade_next_name,name);
	audio_music_fade_next_msec=fade_in_msec;

		// start fade

	al_music_fade_out(fade_out_msec);

	song->fade_mode=music_fade_mode_out_fade_in;		// switch to fade out/fade in mode

	return(TRUE);
}

bool al_music_cross_fade(char *name,int cross_msec,char *err_str)
{
	int						tick;
	audio_music_song_type	*song,*alt_song;
	
		// if no fade out or no music playing, go directly to fade in
		
	if ((cross_msec<=0) || (audio_music_song_idx==-1)) {
		return(al_music_fade_in(name,cross_msec,err_str));
	}
		
		// start alt music

	audio_music_alt_song_idx=al_music_play_song(name,err_str);
	if (audio_music_alt_song_idx==-1) return(FALSE);
	
	song=&audio_music_song_cache[audio_music_song_idx];
	alt_song=&audio_music_song_cache[audio_music_alt_song_idx];

		// setup fade out for current song
		
	tick=game_time_get();
		
	song->fade_mode=music_fade_mode_cross_fade;
	song->fade_start_tick=tick;
	song->fade_msec=cross_msec;
	song->volume=audio_music_global_volume;

		// setup fade in for alt song
		
	alt_song->fade_mode=music_fade_mode_cross_fade;
	alt_song->fade_start_tick=tick;
	alt_song->fade_msec=cross_msec;
	alt_song->volume=0;
	
		// the staging looping in case looping
		// is changed when an alt song is playing
		
	audio_music_alt_stage_loop=audio_music_loop;

	return(TRUE);
}

/* =======================================================

      Run Music
      
======================================================= */

void al_music_run_song(int song_idx,bool alt_song)
{
	int						tick,dif;
	char					err_str[256];
	audio_music_song_type	*song;

		// any music?
		
	if (song_idx==-1) return;
	
		// is there a fade on?
		
	song=&audio_music_song_cache[song_idx];
	if (song->fade_mode==music_fade_mode_none) return;
	
		// time to stop fade?
		// if it's a cross fade, then make sure to
		// swap music

	tick=game_time_get();
		
	dif=tick-song->fade_start_tick;
	if (dif>=song->fade_msec) {
	
		switch (song->fade_mode) {

			case music_fade_mode_in:
				song->volume=audio_music_global_volume;
				song->fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out:
				al_music_stop();
				song->volume=audio_music_global_volume;
				song->fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out_fade_in:
				al_music_fade_in(audio_music_fade_next_name,audio_music_fade_next_msec,err_str);
				break;
				
			case music_fade_mode_cross_fade:
				if (alt_song) al_music_swap_alt_music();
				break;

		}

		return;
	}
	
		// set the fade volume
		
	switch (song->fade_mode) {
		
		case music_fade_mode_in:
			song->volume=(int)((float)audio_music_global_volume*(((float)dif)/(float)song->fade_msec));
			break;

		case music_fade_mode_out:
		case music_fade_mode_out_fade_in:
			song->volume=(int)((float)audio_music_global_volume*((float)(song->fade_msec-dif)/(float)song->fade_msec));
			break;
			
		case music_fade_mode_cross_fade:
			if (alt_song) {
				song->volume=(int)((float)audio_music_global_volume*(((float)dif)/(float)song->fade_msec));
			}
			else {
				song->volume=(int)((float)audio_music_global_volume*((float)(song->fade_msec-dif)/(float)song->fade_msec));
			}
			break;


	}

}

void al_music_run(void)
{
	al_music_run_song(audio_music_song_idx,FALSE);
	al_music_run_song(audio_music_alt_song_idx,TRUE);
}

