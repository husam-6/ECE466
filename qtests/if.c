int printf(); 

int logic_test(int b, int c){
    c = c < -5 || b > 3;
}

int main(){
    int *f, b;
    if (f && *f != 5){
        b++;
        f = f + b; 
    }
    else{
        b--; 
        f = f - b; 
    }
    printf("B=%d\n", b);
}