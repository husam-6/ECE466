/* You'll need to comment out the next 4 lines and the final closing brace */
/* to test for assignment 2.  These declarations are here to create a */
/* syntactically valid pile of C code should you care to test this */
/* with gcc or clang */

// int *z; 
// static int *f[3];
// int ****a; 

// static long int *** main(int a, int b){
//     long int *f; 
// }

static int a(); 
// static int a();
// int b();
int main(){
    extern int a; 
}

volatile signed short s3;

// static unsigned char f; 
// // long long fibonacci(int n);
// // int a;

extern long int *** m(long long ***a, short int *b){
    long int *f; 
    {
        int a;
        int b;
        {
            int f();
        } 
    }
    int f();
}

// int main(){
//     int extern a;   // still minor bug where storage specifier goes after... this is technically valid
// }

// int a; 
// long long int *(**f[])[10];
// int **p;
// int *p[10];
// int ap[5][10][11];
// int ***ap[5][10][3];
// int *(* bar)[5][6];
// int (*(**(* bar)))[5][6];
// int *(* bar)[5][5];
// int (**(* bar)[5])[5];
// long int **op;
// long double (**(* bar)[5])[4];
// int *(*p)[5];
// int a[5];
// int a, b, *c, d; 
// int a[10][10]; 
// int y; 
// int f(){
//     int z; 
//     int *p; 
//     int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,*p,s;
//     struct tag {int m;} *x,q;

//     a=b,c+=-10/d;
//     "X"!="Y"?--e:f++;
//     g-="ABC"[3];
//     h%=+i;
//     i/=15.3;
//     j<<=3;
//     j>>=k,l;
//     m&=n&&o;
//     n|=n^o;
//     fn(a&&b||c,d&e|f^g,10==20!=0) >= (~a<<2);
//     o^='A';
//     p=&x->m;
//     *q.m++;
//     s=sizeof(++a);
// }
