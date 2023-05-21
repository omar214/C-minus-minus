#pragma once
// built in headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // for directory
#include <stdarg.h> // to accept variable number of arguments
#include <stdbool.h> // for boolean


FILE * read_file(char * path) {
    FILE * file = fopen(path, "r");
    if (file == NULL) {
        printf("Error: File %s not found\n", path);
        exit(0);
    }
    return file;
}

FILE * create_file(char * path) {
    FILE * file = fopen(path, "w");
    if (file == NULL) {
        printf("Error: File %s not found\n", path);
        exit(0);
    }
    return file;
}

DIR * open_dir(char * path) {
    DIR * dir = opendir(path);
    if (dir == NULL) {
        printf("Error: Directory not found\n");
        exit(0);
    }
    return dir;
}