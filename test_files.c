#include "files.h"
#include "test.h"
#include <unistd.h>
#include "log.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#define FILENAME_NOEXIST "/garbage"
#define FILENAME_EXIST "/"
#define FILENAME_NOPERM "/var/cache/ldconfig/aux-cache"
#define FILENAME_DIRECTORY "/"
#define FILENAME_FILE1 "/etc/passwd"
#define FILENAME_FILE2 "/etc/fstab"

int test_does_file_exist()
{
    begin_subtest("does file exist");
    expect_ok(1, does_file_exist(FILENAME_EXIST));
    expect_ok(0, does_file_exist(FILENAME_NOEXIST));
    expect_ok(-1, does_file_exist(FILENAME_NOPERM));
    end_subtest("does file exist");
}

int test_is_directory()
{
    begin_subtest("is directory");
    expect_ok(1, is_directory(FILENAME_DIRECTORY));
    expect_ok(0, is_directory(FILENAME_FILE1));
    expect_ok(0, is_directory(FILENAME_FILE2));
    expect_ok(0, is_directory(FILENAME_NOEXIST));
    end_subtest("is directory");

}

#define TESTFILE "./test.txt"
#define NBYTES 100
int test_file_bytesize()
{
   FILE *fp = fopen(TESTFILE, "w");
   char buf[NBYTES];
   begin_subtest("file bytesize");
   fwrite(fp, NBYTES, 1, fp);
   fclose(fp);


   expect_ok(NBYTES, file_bytes_size(TESTFILE));
   unlink(TESTFILE);
   expect_ok(-1, file_bytes_size(FILENAME_NOEXIST));
   end_subtest("file bytesize");

}

#define READ_FILENAME  "./test.txt"
#define HELLO_STR "hello\n world\n"
#define SOURCE_BYTESIZE 2048
int test_read_file()
{
    FILE *fp;
    char *bytes, *sourcebytes;
    int i;
    begin_subtest("read file");
    if ((fp = fopen(READ_FILENAME, "w")) == NULL) {
	LOG_ERR("Test can't continue, opening file failed");
	return -1;
    }
    fwrite(HELLO_STR, strlen(HELLO_STR)+1,  1, fp);
    fclose(fp);
    expect_ok(strlen(HELLO_STR)+1,
	      readfile(READ_FILENAME, &bytes));
    expect_ok(0, strcmp(bytes, HELLO_STR));
    free(bytes);

    sourcebytes = malloc(SOURCE_BYTESIZE);
    for(i = 0; i<SOURCE_BYTESIZE; i++) {
	sourcebytes[i] = rand()%CHAR_MAX;
    }

    if ((fp = fopen(READ_FILENAME, "w")) == NULL) {
	LOG_ERR("test can't continue, open file failed");
	return -1;
    }
    if (fwrite(sourcebytes, 1, SOURCE_BYTESIZE, fp) != SOURCE_BYTESIZE) {
	LOG_ERR("fwrite failed\n");
	return -1;
    }
    fclose(fp);

    expect_ok(SOURCE_BYTESIZE, readfile(READ_FILENAME, &bytes));
    for(i=0; i<SOURCE_BYTESIZE; i++) {
	expect_ok_noprint(sourcebytes[i], bytes[i]);
    }

    free(sourcebytes);
    free(bytes);
    unlink(READ_FILENAME);
    end_subtest("read file");
}
#define WRITEFILENAME "./test.txt"
#define WRITEFILEBUF "hello\nhello\nhello\nhello\n"
#define WRITERANDBUF_SZ 2048
int test_write_file()
{
    char *readbuf= NULL, *writebuf = NULL;
    int i;
    begin_subtest("write file");
    writefile(WRITEFILENAME, WRITEFILEBUF, strlen(WRITEFILEBUF)+1, "w", 0);
    expect_ok(strlen(WRITEFILEBUF), readfile(WRITEFILENAME, &readbuf)-1);
    expect_ok(0, strcmp(WRITEFILEBUF, readbuf));
    free(readbuf);

    if (!(writebuf = malloc(WRITERANDBUF_SZ))) {
	LOG_ERR("test can't allocate memory");
	return -1;
    }
    for(i = 0; i<WRITERANDBUF_SZ; i++) {
	writebuf[i] = rand()%CHAR_MAX;
    }

    expect_ok(
	WRITERANDBUF_SZ,
	writefile(WRITEFILENAME, writebuf, WRITERANDBUF_SZ, "w", 1));
    expect_ok(
	WRITERANDBUF_SZ,
	readfile(WRITEFILENAME, &readbuf));
    for(i=0; i<WRITERANDBUF_SZ; i++) {
	expect_ok_noprint(writebuf[i], readbuf[i]);
    }

    free(readbuf);
    free(writebuf);

    unlink(WRITEFILENAME);
    end_subtest("write file");
	      
}

int main()
{
    test_module_begin("file i/o");
    test_does_file_exist();
    test_is_directory();
    test_file_bytesize();
    test_read_file();
    test_write_file();
    test_module_end("file i/o");

}
