#include <stdio.h>

#include <math.h>


void to_binary  ( int  n, int w, int *x, int *o ) ;
void from_binary( int *x, int w, int *n ) ;
void adder( int *x, int *y, int *z, int *o, int w ) ;
void mult ( int *x, int *y, int *z, int *o, int w ) ;

void main()
{
    int a,b,c,d,w,n,o ;
    int x[32], y[32], z[32] ;
    
    o = 0 ;
    w = 9 ; /* word size in bits */
    /* note that x, y, and z must be at least as large as w and may be larger */
    
    b = 1 ; /* add one to each */
    for( c=250;c<260;c++ )
    {
        if ( c > 255 )  /* exceeds 8 bits */
            d = c-512 ;
        else
            d =  c ;
        printf( "%d %d ", d,b ) ;
        to_binary( d,w,x,&o ) ;
        if ( o )
            printf( "%d Overflow in to_binary! ",d ) ;
        to_binary( b,w,y,&o ) ;
        if ( o )
            printf( "%d Overflow in to_binary! ",b ) ;
        adder( x,y,z,&o,w ) ;
        from_binary( z,w,&a ) ;
        printf( "%d ", a ) ;
        if ( o )
            printf( "Overflow in adder! " ) ;
        printf( "\n" ) ;
    }
    
    b = -1 ; /* add -1 to each */
    for( c=250;c<260;c++ )
    {
        if ( c > 255 )
            d = c-512 ;
        else
            d =  c ;
        printf( "%d %d ", d,b ) ;
        to_binary( d,w,x,&o ) ;
        if ( o )
            printf( "%d Overflow in to_binary! ",d ) ;
        to_binary( b,w,y,&o ) ;
        if ( o )
            printf( "%d Overflow in to_binary! ",d ) ;
        adder( x,y,z,&o,w ) ;
        from_binary( z,w,&a ) ;
        printf( "%d ", a ) ;
        if ( o )
            printf( "Overflow in adder! " ) ;
        printf( "\n" ) ;
    }
    
    
    for( w=8; w<13; w=w+4 )          /* word sizes 8 and 12 bits */
        for( a=99; a >-100; a=a-99 )  /* 99, 0, -99 */
            for( b=4; b>-5; b=b-4 )    /* 4, 0, -4 */
            {
                to_binary( a,w,x,&o ) ;
                if ( o )
                    printf( "%d Overflow in to_binary! ",a ) ;
                to_binary( b,w,y,&o ) ;
                if ( o )
                    printf( "%d Overflow in to_binary! ",b ) ;
                mult( x,y,z,&o,w ) ;
                printf( "%d %d ", a,b ) ;
                from_binary( z,w,&n ) ;
                printf( "%d ", n ) ;
                if ( o )
                    printf( "Overflow in multiplier! " ) ;
                printf( "\n" ) ;
            }
    
    
}

void adder( int *x, int *y, int *z, int *o, int w )
{
    /* x is an input array of w ints, either 0 or 1 representing the first addend */
    /* y is an input array of w ints, either 0 or 1 representing the second addend */
    /* z is an output array of w ints, either 0 or 1 representing the sum of x and y */
    /* o is an output = 1 if an overflow occurred */
    /* w is an input = to the width in bits of x, y, z */
    int carry = 0;
    *o = 0;
    for (int i = 0; i < w; i++)
    {
        if (carry + *(x + i) + *(y + i) >= 2)
        {
            *(z + i) = carry + *(x + i) + *(y + i) - 2;
            carry = 1;
        }
        else
        {
            *(z + i) = carry + *(x + i) + *(y + i);
            carry = 0;
        }
    }
    if (*(x + w - 1) == 1 && *(y + w - 1) == 1 && *(z + w - 1) == 0)
    {
        *o = 1;
        return;
    }
    else if (*(x + w - 1) == 0 && *(y + w - 1) == 0 && *(z + w - 1) == 1)
    {
        *o = 1;
        return;
    }
        
}
void mult ( int *x, int *y, int *z, int *o, int w )
{
    /* x is an input array of w ints, either 0 or 1 representing the first multiplicand */
    /* y is an input array of w ints, either 0 or 1 representing the second multicand */
    /* z is an output array of w ints, either 0 or 1 representing the product of x and y */
    /* o is an output = 1 if an overflow occurred */
    /* w is an input = to the width in bits of x, y, z */
    *o = 0;
    for (int i = 0; i < w; i++)
    {
        *(z + i) = 0;
    }
    for (int i = 0; i < w; i++)
    {
        if (*(y + i) == 1)
        {
            int temp[w];
            for (int j = 0; j < i; j++)
            {
                *(temp + j) = 0;
            }
            for (int j = i; j < w; j++)
            {
                *(temp + j) = *(x + j - i);
            }
            adder(z, temp, z, o, w);
        }
    }
    int n = 0;
    for (int k = 0; k < w; k++)
    {
        if (*(z + k) == 0)
        {
            n++;
        }
    }
    if (n == w)
    {
        *o = 0;
        return;
    }
    
    if (*(x + w - 1) == 0 && *(y + w - 1) == 0 && *(z + w - 1) == 1)
    {
        *o = 1;
    }
    else if (*(x + w - 1) == 0 && *(y + w - 1) == 1 && *(z + w - 1) == 0)
    {
        *o = 1;
    }
    else if (*(x + w - 1) == 1 && *(y + w - 1) == 0 && *(z + w - 1) == 0)
    {
        *o = 1;
    }
    else if (*(x + w - 1) == 1 && *(y + w - 1) == 1 && *(z + w - 1) == 1)
    {
        *o = 1;
    }
    
}
void to_binary( int n, int w, int *x, int *o )
{
    /* n is an input and is the number to be converted to binary */
    /* w is an input = to the width in bits of x */
    /* x is an output array of w ints, either 0 or 1: the binary representation of n */
    /*           x[0] is the least significant bit   */
    /*           x[w-1] is the most signifcant bit, for a signed number, it is the sign */
    /* o is an output = 1 if an overflow occurred ( n is too large for w bits ) */
    *o = 0;
    if (n >= 0 && n < pow(2, w - 1))
    {
        for (int i = 0; i < w; i++)
        {
            *(x + i) = n % 2;
            n = (n - *(x + i)) / 2;
        }
        if (*(x + w - 1) == 1)
        {
            *o = 1;
        }
    }
    else if (n < 0 && -n <= pow(2, w - 1))
    {
        n = pow(2, w) + n;
        for (int i = 0; i < w; i++)
        {
            *(x + i) = n % 2;
            n = (n - *(x + i)) / 2;
        }
        if (*(x + w - 1) == 0)
        {
            *o = 1;
        }
    }
    else
    {
        *o = 1;
        return;
    }
    
}
void from_binary( int *x, int w, int *n )
{
    /* x is an input array of w ints, either 0 or 1: the binary representation of n */
    /*           x[0] is the least significant bit   */
    /*           x[w-1] is the most signifcant bit, for a signed number, it is the sign */
    /* w is an input = to the width in bits of x */
    /* n is an output and will be equal to the value stored in binary in x */
    *n = 0;
    if (*(x + w - 1) == 1)
    {
        for (int j = 0; j < w; j++)
        {
            *n += *(x + j) * pow(2, j);
        }
        *n = *n - pow(2, w);
    }
    else if (*(x + w - 1) == 0)
    {
        for (int j = 0; j < w - 1; j++)
        {
            *n += *(x + j) * pow(2, j);
        }
    }
    
}