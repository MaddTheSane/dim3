/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Setup Routines

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

int									audio_buffer_count,
									audio_global_sound_volume,audio_music_global_volume,
									audio_music_song_idx,audio_music_alt_song_idx;
float								audio_listener_ang_y;
bool								audio_music_paused,audio_music_loop;
unsigned char						audio_silence_byte;
d3pnt								audio_listener_pnt;
audio_buffer_type					audio_buffers[audio_max_buffer];
audio_play_type						audio_plays[audio_max_play];
audio_music_song_type				audio_music_song_cache[music_max_song_cache];

extern map_type						map;
extern camera_type					camera;

/* =======================================================

      Audio Callback Routine
      
======================================================= */

void audio_callback(void *userdata,Uint8 *stream,int len)
{
	int						n,k,pos,stream_len,dist,vol,
							data,left_channel,right_channel,
							last_play_idx;
	short					*s_stream;
	float					ang,f_ang;
	audio_buffer_type		*buffer;
	audio_play_type			*play;
	audio_music_song_type	*song;

		// calculate pitch and left/right channel multiplications
		// here.  We use a x/1024 factor to save on having to do
		// any int -> float conversions
		
		// we also keep track of last part of the array
		// that was playable so we can skip over when building wave

	last_play_idx=0;

	for (n=0;n!=audio_max_play;n++) {

		play=&audio_plays[n];
		if (!play->used) continue;

			// start with sound not in mix, only
			// use if a sound exists and it's within the
			// maximum distance

		play->mix=FALSE;

			// last good index

		last_play_idx=n+1;

			// get the buffer and stream add

		buffer=&audio_buffers[play->buffer_idx];
		play->stream_add=play->pitch*buffer->freq_factor;

			// is there positioning?

		if (play->no_position) {
			play->mix=TRUE;
			play->left_fact=play->right_fact=1024;
			continue;
		}

			// are we within the maximum distance?

		dist=distance_get(play->pnt.x,play->pnt.y,play->pnt.z,audio_listener_pnt.x,audio_listener_pnt.y,audio_listener_pnt.z);
		if (dist>=buffer->max_dist) continue;

			// calculate the volume

		play->mix=TRUE;

		if (dist<=buffer->min_dist) {
			vol=1024;
		}
		else {
			vol=1024-(int)(1024.0f*((float)(dist-buffer->min_dist)/(float)(buffer->max_dist-buffer->min_dist)));
		}
		
			// don't do left-right if not in fpp
			
		if (map.camera.camera_mode!=cv_fpp) {
			play->left_fact=vol>>1;
			play->right_fact=vol>>1;
			continue;
		}

			// get position to source
			// need to add in player's Y angle

		ang=angle_find(audio_listener_pnt.x,audio_listener_pnt.z,play->pnt.x,play->pnt.z);
		ang=angle_add(ang,-audio_listener_ang_y);

			// calculate left/right channels

		if (ang<180.0f) {
			f_ang=(ang/2.0f)*ANG_to_RAD;
		}
		else {
			f_ang=((350.0f-ang)/2.0f)*ANG_to_RAD;
		}
		
		play->left_fact=(int)(((float)vol)*fabsf(cosf(f_ang)));
		play->right_fact=(int)(((float)vol)*fabsf(sinf(f_ang)));
	}

		// if no plays or music, skip audio mix

	if (last_play_idx==0) {
		if ((audio_music_paused) || ((audio_music_song_idx==-1) && (audio_music_alt_song_idx==-1))) {
			memset(stream,audio_silence_byte,len);
			return;
		}
	}
	
		// mix the audio

	s_stream=(short*)stream;
	stream_len=len>>2;					// we are in stereo and 16 bit samples, so divide len by 4

	for (k=0;k<stream_len;k++) {

		left_channel=0;
		right_channel=0;

			// mix plays
			
		if (last_play_idx!=0) {

			for (n=0;n!=last_play_idx;n++) {

				play=&audio_plays[n];
				if (!play->used) continue;

					// get buffer for mixing
					
				buffer=&audio_buffers[play->buffer_idx];

					// do we mix this stream?
					
				if (play->mix) {

						// get stream data
						
					pos=(int)play->stream_pos;
					data=(int)(*(buffer->data+pos));
		
						// create the channels

					left_channel+=((data*play->left_fact)>>10);
					right_channel+=((data*play->right_fact)>>10);
				}
				
					// move onto next position in stream
					// or loop or stop sound

				play->stream_pos+=play->stream_add;

				if (play->stream_pos>=buffer->f_sample_len) {

					if (play->loop) {
						play->stream_pos=play->stream_pos-buffer->f_sample_len;
					}
					else {
						play->used=FALSE;
					}

				}
			}

				// global volume adjustment

			left_channel=((left_channel*audio_global_sound_volume)>>10);
			right_channel=((right_channel*audio_global_sound_volume)>>10);
		}
		
			// music
		
		if (!audio_music_paused) {
				
				// add in the music

			if (audio_music_song_idx!=-1) {
				song=&audio_music_song_cache[audio_music_song_idx];
				
				pos=(int)song->stream_pos;

				data=(int)(*(song->data+pos));
				left_channel+=((data*song->volume)>>10);
				
				data=(int)(*(song->data+(pos+1)));
				right_channel+=((data*song->volume)>>10);

				song->stream_pos+=song->freq_factor;		// in stereo

					// time to loop?

				if (song->stream_pos>=song->f_sample_len) {
					if (audio_music_loop) {
						song->stream_pos=song->stream_pos-song->f_sample_len;
					}
					else {
						song->stream_pos=0.0f;
						audio_music_song_idx=-1;
					}
				}
			}
			
				// add in the alt music (for cross fades)

			if (audio_music_alt_song_idx!=-1) {
				song=&audio_music_song_cache[audio_music_alt_song_idx];
				
				pos=(int)song->stream_pos;

				data=(int)(*(song->data+pos));
				left_channel+=((data*song->volume)>>10);
				
				data=(int)(*(song->data+(pos+1)));
				right_channel+=((data*song->volume)>>10);

				song->stream_pos+=song->freq_factor;		// in stereo

					// time to loop?

				if (song->stream_pos>=song->f_sample_len) {
					if (audio_music_loop) {
						song->stream_pos=song->stream_pos-song->f_sample_len;
					}
					else {
						song->stream_pos=0.0f;
						audio_music_alt_song_idx=-1;
					}
				}
			}
		}

			// fix any overflow
			
		left_channel=((left_channel+0x7FFF)&0xFFFF)-0x7FFF;
		right_channel=((right_channel+0x7FFF)&0xFFFF)-0x7FFF;

			// send channels to stream

		*s_stream++=(short)left_channel;
		*s_stream++=(short)right_channel;
	}
}

/* =======================================================

      Initialize and Shutdown SDL Sound
      
======================================================= */

bool al_initialize(char *err_str)
{
	int				n;
	SDL_AudioSpec	aspec,ospec;
	
		// initialize SDL audio

	aspec.freq=audio_frequency;
	aspec.format=audio_format;
	aspec.channels=audio_channels;
	aspec.samples=audio_sample_size;
	aspec.callback=audio_callback;
	aspec.userdata=NULL;

	if (SDL_OpenAudio(&aspec,&ospec)==-1) {
		sprintf(err_str,"Audio: Could not initialize: %s",SDL_GetError());
		return(FALSE);
	}

		// make sure we got the setup we need

	if ((ospec.freq!=audio_frequency) || (ospec.format!=audio_format) || (ospec.channels!=audio_channels)) {
		SDL_CloseAudio();
		sprintf(err_str,"Audio: Unable to get desired audio format");
		return(FALSE);

	}

	audio_silence_byte=ospec.silence;

		// no buffers loaded

	audio_buffer_count=0;

		// no sounds playing

	for (n=0;n!=audio_max_play;n++) {
		audio_plays[n].used=FALSE;
	}

		// default global volumes
		// sound volumes are factors of x/1024

	audio_global_sound_volume=512;
	
		// music setup
		
	if (!al_music_initialize(err_str)) return(FALSE);

		// start loop

	SDL_PauseAudio(0);

	return(TRUE);
}

void al_shutdown(void)
{
	SDL_PauseAudio(1);

	al_music_shutdown();
	
	SDL_CloseAudio();
}

/* =======================================================

      Set Sound Volume
      
======================================================= */

void al_set_volume(float sound_volume)
{
	audio_global_sound_volume=(int)(1024.0f*sound_volume);
}

