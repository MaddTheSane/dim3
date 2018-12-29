/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Line

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
	#include "dim3engine.h"
#endif

#include "interface.h"

extern iface_type			iface;
extern app_type				app;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

extern SDL_Window					*sdl_wind;

extern bool dim3_osx_appstore_main(void);

/* =======================================================

      Check for Editor Launch
      
======================================================= */

#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
void app_check_editor_link(void)
{
	app.editor_override.on=FALSE;
}
#else
void app_check_editor_link(void)
{
	int				len;
	char			path[1024];
	unsigned char	uc_len;
	FILE			*file;
	
	app.editor_override.on=FALSE;

		// attempt to open editor link file
		
	file_paths_dim3_app_data(&file_path_setup,path,"dim3EditorLink","tmp");
	
	file=fopen(path,"rb");
	if (file==NULL) return;
	
		// name

	fread(&uc_len,1,1,file);
	len=(int)uc_len;
	fread(app.editor_override.map,1,len,file);

		// position

	fread(&app.editor_override.pt,1,sizeof(d3pnt),file);
	fread(&app.editor_override.ang,1,sizeof(d3ang),file);

	fclose(file);
	
		// get rid of the file
		
	remove(path);

		// mark editor override

	app.editor_override.on=TRUE;
}
#endif

/* =======================================================

      Check for Dedicated Hosting Launch
      
======================================================= */

#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
void app_check_dedicated_host(int argc,char *argv[])
{
	app.dedicated_host=FALSE;
}
#else
void app_check_dedicated_host(int argc,char *argv[])
{
	int				n;

	app.dedicated_host=FALSE;

	for (n=0;n!=argc;n++) {
		if (strcasecmp(argv[n],"-dedicated")==0) {
			app.dedicated_host=TRUE;
			break;
		}
	}
}
#endif

/* =======================================================

      Dedicated Host Kill Signals
      
======================================================= */

#ifdef D3_OS_WINDOWS

	// windows signals

BOOL WINAPI app_win32_signal(DWORD ctrl_type) 
{
	switch(ctrl_type) {
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			app.loop_quit=TRUE;
			return(TRUE);
	}

	return(FALSE);
} 

void app_register_kill_signals(void)
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)app_win32_signal,TRUE);
}

#else

	// OS X and linux signals

void app_osx_signal(int sig)
{
	app.loop_quit=TRUE;
}

void app_register_kill_signals(void)
{
	signal(SIGINT,app_osx_signal);
}
	
#endif

/* =======================================================

      Report Errors
      
======================================================= */

void app_report_error(char *err_str)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"dim3 Error",err_str,NULL);
}

/* =======================================================

      Infamous Main Code
      
======================================================= */

int main(int argc,char *argv[])
{
	bool			app_start_ok,app_run_ok;
	char			err_str[256];

		// setup paths

	if (!file_paths_setup(&file_path_setup)) {
		app_report_error("dim3 requires a data folder with project files.");
		return(0);
	}
	
		// get the project name
		
	iface_read_settings_project_name(&file_path_setup);

		// check if editor is launching engine and
		// if a map needs to be auto-loaded
	
	app_check_editor_link();

		// check if we are in dedicated
		// hosting mode

	app_check_dedicated_host(argc,argv);

		// setup app state and if dedicated,
		// then setup kill signals

	app.state=as_active;
	app.loop_quit=FALSE;

	if (app.dedicated_host) app_register_kill_signals();

		// start the app
		
	err_str[0]=0x0;
	
	app_start_ok=app_start(err_str);
	if (app_start_ok) {

			// pick starting run

		if (app.dedicated_host) {
			app_run_ok=app_run_dedicated_host(err_str);
		}
		else {
			if (app.editor_override.on) {
				app_run_ok=app_run_editor_launch(err_str);
			}
			else {
				app_run_ok=app_run_intro(err_str);
			}
		}

			// main loop

		if (app_run_ok) {

			while (!app.loop_quit) {

				if (app.dedicated_host) {
					if (!loop_main_dedicated(err_str)) break;
				}
				else {
					if (!loop_main(err_str)) break;
				}

			}
			
		}

		app_end();
	}
	
		// report any errors
		
	if (err_str[0]!=0x0) {
		if (app.dedicated_host) {
			fprintf(stdout,"Fatal Error: %s\n",err_str);
		}
		else {
			app_report_error(err_str);
		}
	}
	
	return(0);
}

