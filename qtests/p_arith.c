int main(){
    int x, *p; 
    int e[10];
    x = e[3];
    p = &e[5];

    (*p) *= *p + 1;
    x = sizeof(p);
}