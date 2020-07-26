int loop(int a, int b, int c)
{
    int i = 0, ret = 0;

    do {
            c = b + 2;
            i++;
    } while (i < a);

    return ret + c;
}

void looper(int a, int b, int c,int d){
    int i=0;
    do {
            c = b + 2;
            i++;
        while(c){
            d=a+2;
            i++;
        }
    } while (i < a);
}