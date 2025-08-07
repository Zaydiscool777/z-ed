// #include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* bufferf(int form);

int main(){
    FILE* buff = bufferf(0);
    printf("first char: %c\n", fgetc(buff));
    return 0;
}

char filename[256] = "a";
/** Syncs (reads/writes) from a file to a file pointer.
If reading fails, will create a new file instead.
@param form 0 to read (create on fail)
*/
FILE* bufferf(int form){
    if(form){
        FILE* fp = fopen(filename, "w");
        return fp;
    } else {
        FILE* fp = fopen(filename, "r");
        if(fp == NULL){
            fp = fopen(filename, "w");
            fp = freopen(filename, "r", fp);
            return fp;
            // TODO: POSIX requires errno here?
        } else {
            return fp;
        }
    }
}