int SumOdd(unsigned int n){
    int result = 0;
    int p;
    while (n){
        p = n % 10;
        if (p % 2){
            result += p;
        }
        n /= 10;
    }

    return result;
}
