#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "log.h"

/*
 * file_bytes_size : string -> number
 * get the number of bytes in the file fname.
 * returns:
 *  -1 if the file doesn't exist.
 *  file size in bytes if it does. 
 */
long file_bytes_size(char *fname)
{
    struct stat statbuf; 

    memset(&statbuf, 0, sizeof statbuf);
    if (stat(fname, &statbuf) != 0)
	return -1;
    else
	return statbuf.st_size;
}

/*
 * is_directory : string -> number
 * return whether or not the file is a directory.
 * returns:
 *   0 if the file doesn't exist or isn't a directory.
 *   1 if the file is a directory.
 *   -1 for an error returned by stat that isn't ENOENT.
 */
int is_directory(char *fname)
{
    struct stat statbuf;

    memset(&statbuf, 0, sizeof statbuf);

    if (stat(fname, &statbuf) != 0) {
	if (errno == ENOENT)
	    return 0;
	else
	    return -1;
    }
    return S_ISDIR(statbuf.st_mode);
}

/*
 * does_file_exist: string -> number
 * tests if the file fname exists or not.
 * returns:
 *   1 if it does exist in some form.
 *   0 if it doesn't
 *   -1 for some other error that stat returned, like EPERM.
 */
int does_file_exist(char *fname)
{
    struct stat statbuf;

    memset(&statbuf, 0, sizeof statbuf);
    if (stat(fname, &statbuf) != 0) {
	if (errno == ENOENT) {
	    return 0;
	} else {
	    return -1;
	}
    }
    return 1;
}

/*
 * readfile : string, pointer to a string -> number
 * read the contents of file fname into a buffer which is allocated here, by readfile. 
 * returns:
 *    -1 on some error (file DNE, failed malloc, etc.)
 *    file size in bytes otherwise
 */
int readfile(char *fname, char **filebytes)
{
    long to_malloc = 0;
    FILE *fp = NULL;
    if (!does_file_exist(fname)) {
	LOG_INFO("file doesn't exist: %s", fname);
	return -1;
    }

    if((to_malloc = file_bytes_size(fname)) <= 0) {
	LOG_ERR("couldn't get file bytes size of %s", fname);
	return -1;
    }
    *filebytes = NULL;
    if ((*filebytes = malloc(to_malloc)) == NULL) {
	LOG_ERR("failed malloc");
	return -1;
    }
    memset(*filebytes, 0, to_malloc);

    if ((fp = fopen(fname, "r")) == NULL) {
	LOG_ERR("failed fopen of %s", fname);
	goto err;
    }

    if (fread(*filebytes, 1, to_malloc, fp) != to_malloc) {
	LOG_ERR("failed fread");
	goto err;
    }
    
    fclose(fp);
    return to_malloc;
err:
    if (fp)
	fclose(fp);
    if (*filebytes)
	free(*filebytes);
    return -1;
}

/*
 * writefile: string, buffer, number, number -> number
 * write contents of buffer, which is bufsize long, into file fname.
 * If the file exists and overwrite is set, exit, returning -2. 
 * If the file doesn't exist, or overwrite is specified, open it for writing, 
 * and write the buffer. return number of bytes written.
 */
int writefile(char *fname, char *buffer, int bufsize, char *mode, int overwrite)
{
    FILE *fp = NULL;
    
    if (does_file_exist(fname) && !overwrite)
	return -2;

    if ((fp = fopen(fname, mode)) == NULL) {
	LOG_ERR("can't open %s: %s\n", fname, strerror(errno));
	return -1;
    }

    if (fwrite(buffer, 1, bufsize, fp) != bufsize) {
	LOG_ERR("didn't write all %d bytes to %s\n", bufsize, fname);
	goto err;
    }

    fclose(fp);
    return bufsize;
err:
    fclose(fp);
    return -1;
}


int copyfile(char *src, char *dst, int careful)
{
    char buf[COPY_BUFSIZE];
    struct stat statbuf;
    int nread = 0, writesize=0, iter_written=0, total_written=0;
    
    if (careful && does_file_exist(dst)) {
	return -2;
    }
    memset(&statbuf, 0, sizeof statbuf);
    if (stat(src, &statbuf) != 0) {
	LOG_ERR("can't stat file %s: %s", src, strerror(errno));
	return -1;
    }
    if ((srcfd = open(src, O_RDONLY)) < 0) {
	LOG_ERR("can't open %s for reading: %s", src, strerror(errno));
	return -1;
    }
    if ((dstfd = open(dst, O_WRONLY|O_CREAT|O_TRUNC, statbuf.st_mode)) < 0) {
	LOG_ERR("can't open %s for writing: %s", src, strerror(errno));
	return -1;
    }

    written = 0;
    while ((nread = read(srcfd, buf, COPY_BUFSIZE)) >= 0) {
	while(written != nread) {
	    if ((iter_written = write(dstfd, buf+written, nread-written)) < 0) {
		LOG_ERROR("write error on %s: %s", dst, strerror(errno));
		goto err;
	    }
	    written += iter_written;
	}
	total_written += written;
	written = 0;
    }

    if (nread < 0) {
	LOG_ERROR("read error on %s: %s", src, strerror(errno));
	goto err;
    }

    close(src);
    close(dst);
    return total_written;
err:
    close(src);
    close(dst);
    return -1;
}
