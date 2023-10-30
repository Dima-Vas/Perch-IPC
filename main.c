// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//#include <iostream>
//#include "options_parser.h"

//#include <string>
//#include <stack>

#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>

int if_exists(const char *path){
    if (path == NULL){
        printf("Error: \"int if_exists(const char *path)\" got NULL pointer (path == NULL)\n");
        return 0; // a.k.a. false
    }
    struct stat path_stat;
    if (stat(path, &path_stat) != 0){
        return 0;
    };
    return 0;
}

int is_regular_file(const char *path) {
    if (path == NULL){
        printf("Error: \"int is_regular_file(const char *path)\" got NULL pointer (path == NULL)\n");
        return 0; // a.k.a. false
    }
    struct stat path_stat;
    if (stat(path, &path_stat) != 0){
        return 0;
    };
    return S_ISREG(path_stat.st_mode);
}

char* LastModDateTime(long lastModificationTime) {
    time_t modificationTime = (time_t)lastModificationTime;
    struct tm *localTime = localtime(&modificationTime);
    if (localTime != NULL) {
        char dateTimeStr[100];
        strftime(dateTimeStr, sizeof(dateTimeStr), "%Y-%m-%d %H:%M:%S", localTime);
        char* formattedDateTime = (char*)malloc(strlen(dateTimeStr) + 1);
        strcpy(formattedDateTime, dateTimeStr);
        return formattedDateTime;
    } else {
        return NULL;
    }
}

static int display_info(const char *fpath,
                         const struct stat *sb,
                         int tflag,
                         struct FTW *ftwbuf) {
    printf( (sb->st_mode & S_IRUSR) ? "r" : "-");
    printf( (sb->st_mode & S_IWUSR) ? "w" : "-");
    printf( (sb->st_mode & S_IXUSR) ? "x" : "-");
    printf( (sb->st_mode & S_IRGRP) ? "r" : "-");
    printf( (sb->st_mode & S_IWGRP) ? "w" : "-");
    printf( (sb->st_mode & S_IXGRP) ? "x" : "-");
    printf( (sb->st_mode & S_IROTH) ? "r" : "-");
    printf( (sb->st_mode & S_IWOTH) ? "w" : "-");
    printf( (sb->st_mode & S_IXOTH) ? "x" : "-");
    struct passwd *pws;
    printf("%16s %8jd %19s %s%s\n",
           getpwuid(sb->st_uid)->pw_name,
           (intmax_t) sb->st_size,
           LastModDateTime(sb->st_mtime),
           (tflag == FTW_D) ?   "/"   :
           (tflag == FTW_SL) ? "@" :
           (S_ISSOCK(sb->st_mode)) ? "=" :
           (S_ISFIFO(sb->st_mode)) ? "|" :
           (sb->st_mode & S_IXUSR) ? "*" :
           (tflag == FTW_F) ? " " : "?",
           fpath + ftwbuf->base);
    if (tflag == FTW_SL){

    }
    if (tflag == FTW_D){
        printf("\n%s:\n", fpath);
    }
    return 0;           /* To tell nftw() to continue */
}



int main(int argc, char *argv[]) {
    int flags = 0;
    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags)
        == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}