int do_while_test(b){
    do{
        // continue;
        if (b == 10){
            b /= 100;
            continue;
        }
        b*=10;
    } while(b && b<100);
}

int main(){
    int *f, b, c; 
    while(f){
        if (f == 5){
            f+=10; 
            b = b * 100 * c; 
            break;
        }
        b *= 10;
        b++;
    }

}