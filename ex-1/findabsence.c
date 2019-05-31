
//void Sort(int* a, int n){
//	for(int i = 0; i < n; i++){
//		for(int j=i + 1; j < n; j++){
//			if(a[j] < a[i]){
////				int t = a[i];
////				a[i] = a[j];
////				a[j] = t;
//				a[i]^=a[j]^=a[i]^=a[j];
//			}
//		}
//	}
//}

void DumpArray(int* a, int n){
	for(int i=0; i<n; i++){
		printf("%d ", a[i]);
	}
	printf("\012");
}


int inarray(int e, int* a, int n){
	for(int i = 0; i < n; i++){
		if(a[i] == e){
			return 1;
		}
	}

	return 0;
}

void findAbsence(int* ids, int length){
	for(int i=0; i<length; i++){
		if(ids[i] > length || ids[i] < 1){
			printf("Invalid");
			return;
		}
	}
	int result[100];
	int nabsence = 0;

	for(int i = 1; i <= length; i++){
		if(!inarray(i, ids, length)){
			result[nabsence++] = i;
		}
	}

//	Sort(result, nabsence);
	DumpArray(result, nabsence);
}