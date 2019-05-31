#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "define.h"

char* LTrim(char* p){
    char* res = (char*)malloc(STRING_LENGTH);
    strcpy(res, "");

    int length = strlen(p);

    for (int i=0; i<length; i++){
        if (!isspace(p[i])){
            int j = 0;
            while (i<length){
                res[j++] = p[i++];
            }
            res[i] = 0;
            break;
        }
    }

    return res;
}

char* RTrim(char* p){
    char* res = (char*)malloc(STRING_LENGTH);
    strcpy(res, "");

    int length = strlen(p), ei, i;

    for (int i=length - 1; i>=0; i--){
        if (!isspace(p[i])){
            ei = i;
            break;
        }
    }

    for (i=0; i<=ei; i++){
        res[i] = p[i];
    }

    res[i] = 0;

    return res;
}

char* Trim(char* p){
    char* res = (char*)malloc(STRING_LENGTH);
    int length = strlen(p);
    int space = 1, i, j;

    for (i=0, j=0; i<length; i++){
        if (!space || !isspace(p[i])){
            res[j++] = p[i];
        }

        space = isspace(p[i]);
    }

    if (isspace(res[j-1])){
        res[j-1] = 0;
    }else{
        res[j] = 0;
    }

    return res;
}

char* StrStrPos(char* p, int si, int ei){
    char* rs = malloc(ei - si + 2);
    strcpy(rs, "");

    int length = strlen(p);
    if (si <= length && ei <= length){
        for (int i=si, j=0; i<=ei; i++, j++){
            rs[j] = p[i];
        }
    }

    return rs;
}

char* StrSubStr(char* p, int si, int l){
    int ei = si + l - 1;

    return StrStrPos(p, si, ei);
}

int IndexOfSpace(char* p, int s, int dir){
    int length = strlen(p);

    if (dir == 1){
        for (int i=s; i<length; i++){
            if (isspace(p[i])){
                return i;
            }
        }
    }else{
        for (int i=s; i>=0; i--){
            if (isspace(p[i])){
                return i;
            }
        }
    }

    return -1;
}

int IndexOfNonSpace(char* p, int s, int dir){
    int length = strlen(p);

    if (dir == 1){
        for (int i=s; i<length; i++){
            if (!isspace(p[i])){
                return i;
            }
        }
    }else{
        for (int i=s; i>=0; i--){
            if (!isspace(p[i])){
                return i;
            }
        }
    }

    return -1;
}

int CountWords(char* ps){
    char* p = Trim(ps);
    int c = 0;

    int length = strlen(p);
    int bs = 0;
    if (length){
        for (int i=0; i<length; i++){
            if (p[i] == 32 && !bs){
                c += 1;
                bs = 1;
            }else{
                if (p[i] != 32){
                    bs = 0;
                }
            }
        }

        return c + 1;
    }else{
        return 0;
    }
}

char* WordSwap(char* str){
    if (CountWords(str) < 4){
        return "Invalid";
    }

    int length = strlen(str);
    char* res = (char*)malloc(length + 1);
    strcpy(res, "");

    int fsi, fei, lsi, lei;

    fsi = IndexOfNonSpace(str, IndexOfSpace(str, IndexOfNonSpace(str, 0, 1), 1), 1);
    fei = IndexOfSpace(str, fsi, 1) - 1;
    lei = IndexOfNonSpace(str, IndexOfSpace(str, IndexOfNonSpace(str, length - 1, 0), 0), 0);
    lsi = IndexOfSpace(str, lei, 0) + 1;

    char* s1 = StrStrPos(str, 0, fsi - 1);
    char* s2 = StrStrPos(str, fsi, fei);
    char* s3 = StrStrPos(str, fei + 1, lsi - 1);
    char* s4 = StrStrPos(str, lsi, lei);
    char* s5 = StrStrPos(str, lei + 1, length - 1);

    strcat(res, s1);
    strcat(res, s4);
    strcat(res, s3);
    strcat(res, s2);
    strcat(res, s5);

    free(s1);
    free(s2);
    free(s3);
    free(s4);
    free(s5);

    res[length] = 0;

    return res;
}
