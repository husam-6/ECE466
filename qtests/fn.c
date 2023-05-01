int printf();
int fn(int a, int * b, int c){
    a++;
    a = a + (*b) * c; 
    return a; 
}

int main(){
    int i, b, c; 
    i = 0; 
    b = 10; 
    c = 100;
    b = fn(i, &b, c) + 1;

    int *f; 
    int *(fnp());
    int (*fun_ptr)(int);
    fun_ptr = &fn;
    (*fun_ptr)(10);

    return b; 
}