void DumpArray(int* a, int n){
    for (int i=0; i<n; i++){
        printf("%d ", a[i]);
    }
    printf("\012");
}


int InArray(int e, int* a, int n){
    for (int i = 0; i < n; i++){
        if (a[i] == e){
            return 1;
        }
    }

    return 0;
}

void FindAbsence(int* ids, int length){
    for (int i=0; i<length; i++){
        if (ids[i] > length || ids[i] < 1){
            printf("Invalid");
            return;
        }
    }
    int result[100];
    int nabsence = 0;

    for (int i = 1; i <= length; i++){
        if (!InArray(i, ids, length)){
            result[nabsence++] = i;
        }
    }

    DumpArray(result, nabsence);
}
