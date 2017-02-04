#ifndef _FILES_H_
#define _FILES_H_

int does_file_exist(char *fname);
int is_directory(char *fname);
int readfile(char *fname, char **filebytes);
int writefile(char *fname, char *buffer, int bufsize, char *mode, int overwrite);
long file_bytes_size(char *fname);
#endif
