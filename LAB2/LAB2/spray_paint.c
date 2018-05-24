#include <stdio.h>
#include <stdlib.h>

char *min_ptr = NULL , *max_ptr, *stack_ptr;
void spray_paint( char *x, int sz, char tok, char *t )
{
    printf( "%p %3d %x %s\n", x,sz,(unsigned char) tok,t ) ;
    
    //
    // spray paint code
    //
    int i;
    for (i = 0; i < sz; i++)
    {
        *(x + i) = tok;
    }
    // update min_ptr
    if (min_ptr == NULL)
    {
        min_ptr = x;
    }
    if (min_ptr > x)
    {
        min_ptr = x;
    }
    // update max_ptr
    char *test_ptr = x + sz - 1;
    if (max_ptr == NULL)
    {
        max_ptr = test_ptr;
    }
    if (max_ptr < test_ptr)
    {
        max_ptr = test_ptr;
    }
}

void dumper( char *x, int n )
{
    //
    //   dumper code
    //
    printf("Address          +x00      +x04      +x08      +x0c\n");
    int i, j = 0;
    // round the address down to a multiple of 16
    unsigned char* rounded = (unsigned char*) (&x);
    unsigned char a;
    a = *rounded % 16;
    *rounded -= a;
    
    for (i = 0; i < n; i++)
    {
        printf("%p   ", x);
        // to make the output right endian
        for (j = 3; j >= 0; j--)
        {
            printf("%02x", (unsigned char)*(x + j));
        }
        printf("  ");
        for (j = 7; j >= 4; j--)
        {
            printf("%02x", (unsigned char)*(x + j));
        }
        printf("  ");
        for (j = 11; j >= 8; j--)
        {
            printf("%02x", (unsigned char)*(x + j));
        }
        printf("  ");
        for (j = 15; j >= 12; j--)
        {
            printf("%02x", (unsigned char)*(x + j));
        }
        printf("\n");
        x += 16;
    }
    printf("\n");

}

void sub2()
{
    int i ;
    char* a ;
    char x[20] ;
    
    spray_paint( (char *) &i, sizeof(i), 0xf1, "sub2.i" ) ;
    spray_paint( (char *) &a, sizeof(a), 0xf2, "sub2.a" ) ;
    spray_paint( (char *) &x, sizeof(x), 0xf3, "sub2.x" ) ;
    printf ( "Min= %p Max= %p\n", min_ptr, max_ptr ) ;
    dumper( min_ptr,(int) (max_ptr-min_ptr)/16+1 ) ;
    //
    //   save/destroy the stack here (don't forget to use an external save area)
    //
    // store the data to a global pointer which will not be easily changed and destroyed
    int num = (int) (max_ptr - min_ptr) + 1;
    stack_ptr = (char *)malloc(num * sizeof(char));
    int j;
    for (j = 0; j < num; j++)
    {
        *(stack_ptr + j) = *(min_ptr + j);
    }
    spray_paint(min_ptr, num, 0x00, "destroy");
    printf( " destroyed stack\n" ) ;
    dumper( min_ptr,(int) (max_ptr-min_ptr)/16+1 ) ;
    //
    //    restore the stack here
    //
    int k;
    for (k = 0; k < num; k++)
    {
        *(min_ptr + k) = *(stack_ptr + k);
    }
    printf( " restored stack\n" ) ;
    dumper( min_ptr,(int) (max_ptr-min_ptr)/16+1 ) ;
    free(stack_ptr);
}

void sub1()
{
    float i ;
    short a ;
    int x[20] ;
    
    spray_paint( (char *) &i, sizeof(i), 0xe1, "sub1.i" ) ;
    spray_paint( (char *) &a, sizeof(a), 0xe2, "sub1.a" ) ;
    spray_paint( (char *) &x, sizeof(x), 0xe3, "sub1.x" ) ;
    sub2() ;
}
int main()
{
    struct mine {
        char* a ;
        char x[20] ;
        float z ;
    } i;
    
    union crazy {
        float a ;
        char b ;
        int s ;
    } a ;
    
    char x[50] ;
    
    spray_paint( (char *) &i, sizeof(i), 0xd1, "main.i" ) ;
    spray_paint( (char *) &a, sizeof(a), 0xd2, "main.a" ) ;
    spray_paint( (char *) &x, sizeof(x), 0xd3, "main.x" ) ;
    
    sub1() ;
    
    return 0 ;
}