int printf(); 
int main(){
    int b, i; 
    b = 1; 
    for(i = 0; i<5; i++){
        if (i == 1){
            i++; 
            break;
        }
        b *=10; 
    }
    printf("b=%d\n", b);
}