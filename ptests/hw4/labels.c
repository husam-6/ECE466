int i, a;

int main(){
    a++;
    goto future;
}

int f(){
    future:
        while(i<100){
            i++;
        }
    
    goto past;
    past:
        ++i;
}