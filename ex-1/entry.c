#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"

void PrintLine(char* s){
	printf("%s\n", s);
}

void Print(char* s){
	printf("%s", s);
}

int main(){
	PrintLine("Choose Problem");
	PrintLine("1. Sum Odd");
	PrintLine("2. Word Swap");
	PrintLine("3. Check Student Attendance");


	int probid;
	Print("Problem number: ");
	scanf("%d", &probid);

	if(probid==1){
		int n;
		PrintLine("> SumOdd Problem");
		Print("n = ");
		scanf("%u", &n);
		printf("sumOdd(%u) = %d\n", n, sumOdd(n));
	}

	if(probid==2){
		char* s = (char*)malloc(STRING_LENGTH);
		PrintLine("> WordSwap Problem");
		Print("string = ");
		gets(s);
		gets(s);
		printf("WordSwap(\"%s\") = \"%s\"", s, WordSwap2(s));
	}

	if(probid==3){
		int n, ids[100];
		PrintLine("> Check Student Attendance");
		Print("n = ");
		scanf("%d", &n);
		printf("Number of students: %d\n", n);
		for(int i=0; i < n; i++){
			printf("ID of student %d = ", i + 1);
			scanf("%d", ids+i);
		}

		Print("Result: ");
		findAbsence(ids, n);
	}

	if(probid>3 || probid<1){
		PrintLine("Out of scope");
	}

	return 0x0;
}
