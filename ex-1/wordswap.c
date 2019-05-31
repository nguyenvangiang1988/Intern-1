#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "define.h"

char* ltrim(char* p){
	char* res = (char*)malloc(STRING_LENGTH);
	strcpy(res, "");

	int length = strlen(p);

	for(int i=0; i<length; i++){
		if(!isspace(p[i])){
			int j = 0;
			while(i<length){
				res[j++] = p[i++];
			}
			res[i] = 0;
			break;
		}
	}

	return res;
}

char* rtrim(char* p){
	char* res = (char*)malloc(STRING_LENGTH);
	strcpy(res, "");

	int length = strlen(p), ei, i;

	for(int i=length - 1; i>=0; i--){
		if(!isspace(p[i])){
			ei = i;
			break;
		}
	}

	for(i=0; i<=ei; i++){
		res[i] = p[i];
	}

	res[i] = 0;

	return res;
}

char* trim(char* p){
	char* res = (char*)malloc(STRING_LENGTH);
	int length = strlen(p);
	int space = 1, i, j;

	for(i=0, j=0; i<length; i++){
		if(!space || !isspace(p[i])){
			res[j++] = p[i];
		}

		space = isspace(p[i]);
	}

	if(isspace(res[j-1])){
		res[j-1] = 0;
	}else{
		res[j] = 0;
	}

	return res;
}


char* strstrpos(char* p, int si, int ei){
	char* rs = malloc(ei - si + 2);
	strcpy(rs, "");

	int length = strlen(p);
	if(si <= length && ei <= length){
		for(int i=si, j=0; i<=ei; i++, j++){
			rs[j] = p[i];
		}
	}

	return rs;
}

char* strsubstr(char* p, int si, int l){
	int ei = si + l - 1;

	return strstrpos(p, si, ei);
}


int indexofspace(char* p, int s, int dir){
	int length = strlen(p);

	if(dir == 1){
		for(int i=s; i<length; i++){
			if(isspace(p[i])){
				return i;
			}
		}
	}else{
		for(int i=s; i>=0; i--){
			if(isspace(p[i])){
				return i;
			}
		}
	}

	return -1;
}

int indexofnonspace(char* p, int s, int dir){
	int length = strlen(p);

	if(dir == 1){
		for(int i=s; i<length; i++){
			if(!isspace(p[i])){
				return i;
			}
		}
	}else{
		for(int i=s; i>=0; i--){
			if(!isspace(p[i])){
				return i;
			}
		}
	}

	return -1;
}

int CountWords(char* ps){
	char* p = trim(ps);
	int c = 0;

	int length = strlen(p);
	int bs = 0;
	if(length){
		for(int i=0; i<length; i++){
			if(p[i] == 32 && !bs){
				c += 1;
				bs = 1;
			}else{
				if(p[i] != 32){
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
	if(CountWords(str) < 4){
		return "Invalid";
	}
	char* res = (char*)malloc(STRING_LENGTH);
	char* p = trim(str);
	char** wordsarray = (char**)malloc(STRING_WORDS_NUMBER * sizeof(char*));

	int length = strlen(p);

	int wordindex = 0;

	int ei = 0, si = 0;

	while(ei < length){
		si = indexofnonspace(p, ei, 1);
		ei = indexofspace(p, si, 1);

		if(ei == -1){
			ei = length;
			wordsarray[wordindex++] = strstrpos(p, si, ei - 1);
			break;
		}else{
			wordsarray[wordindex++] = strstrpos(p, si, ei - 1);
		}
	}


	char* im = wordsarray[1];
	wordsarray[1] = wordsarray[wordindex - 2];
	wordsarray[wordindex - 2] = im;

	strcpy(res, "");

	for(int i = 0; i<wordindex - 1; i++){
		strcat(res, wordsarray[i]);
		strcat(res, " ");
	}

	strcat(res, wordsarray[wordindex - 1]);

	return res;
}

char* WordSwap2(char* str){
	if(CountWords(str) < 4){
		return "Invalid";
	}

	int length = strlen(str);
	char* res = (char*)malloc(length + 1);
	strcpy(res, "");

	int fsi, fei, lsi, lei;

	fsi = indexofnonspace(str, indexofspace(str, indexofnonspace(str, 0, 1), 1), 1);
//	fsi = indexofspace(str, indexofnonspace(str, 0, 1), 1) + 1;
	fei = indexofspace(str, fsi, 1) - 1;
	lei = indexofnonspace(str, indexofspace(str, indexofnonspace(str, length - 1, 0), 0), 0);
//	lei = indexofspace(str, indexofnonspace(str, length - 1, 0), 0) - 1;
	lsi = indexofspace(str, lei, 0) + 1;

	char* s1 = strstrpos(str, 0, fsi - 1);
	char* s2 = strstrpos(str, fsi, fei);
	char* s3 = strstrpos(str, fei + 1, lsi - 1);
	char* s4 = strstrpos(str, lsi, lei);
	char* s5 = strstrpos(str, lei + 1, length - 1);

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
