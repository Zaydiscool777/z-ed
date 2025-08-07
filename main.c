// #include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    return 0;
}

char filename[256] = "a";
FILE* buffer(int form){
    if(form){
        FILE* fp = fopen(filename, "w");
        return fp;
    } else {
        FILE* fp = fopen(filename, "r");
        if(fp == NULL){
            fp = fopen(filename, "w");
            return fp;
            // TODO: POSIX requires errno here
        } else {
            return fp;
        }
    }
}