#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define S       16     //  (16 cache sets)
#define E       4      //  (4-way associative cache)
#define B       32     //  (32 elements in each block)
#define T       7      //  (7 tag bits)
#define M       65536  //  (65536 location memory)
#define READ    1
#define WRITE   0

int s ;
int b ;
int m ;

int rhits = 0 ; int rmiss = 0 ; int whits = 0 ; int wmiss = 0 ; int dwrit = 0 ;

struct cache_t
{
    char dirty ;
    char valid ;
    int  tag   ;
    int 	last  ;
    int  *block ;
} cache[S][E] ;

int *memory ;

int callno = 0 ;

int x  =     0 ;
int y  = 16384 ;
int z  = 32768 ;
int ni =    20 ;
int nj =    20 ;

void stats( char *t )
{
    int i,j,k,A ;
    
    for( i=0; i<S; i++ )
        for( j=0; j<E; j++ )
        {
            if( cache[i][j].valid & cache[i][j].dirty )
            {
                A = cache[i][j].tag*exp2(m-T)+i*B ;
                for( k=0; k<B; k++ )
                    memory[A+k] = cache[i][j].block[k] ;
                dwrit = dwrit+1 ;
            }
            cache[i][j].valid = 0 ;
            cache[i][j].dirty = 0 ;
        }
    
    printf( "%8s y=%5d ni=%5d nj=%5d rh=%5d rm=%5d wh=%5d wm= %5d dw=%5d\n", t, y,ni,nj,
           rhits, rmiss, whits, wmiss, dwrit ) ;
    rhits = 0 ; rmiss = 0 ;
    whits = 0 ; wmiss = 0 ;
    dwrit = 0 ;
    
}

void initcache()
{
    //
    // put code to calculate s, b, m here
    //
    s = log(S) / log(2);
    b = log(B) / log(2);
    m = log(M) / log(2);
    printf( "S= %d E= %d B= %d T= %d M= %d s= %d b= %d m= %d\n", S,E,B,T,M,s,b,m ) ;
    
    //
    // put code to initialize cache and memory here
    //
    //initialize cache
    int i, j;
    for (i = 0; i < S; i++)
    {
        for (j = 0; j < E; j++)
        {
            cache[i][j].block = (int *)malloc(B * (sizeof(int)));
            cache[i][j].dirty = 0;
            cache[i][j].valid = 0;
            cache[i][j].last = 0;
        }
    }
    
    //initialize memory
    memory = (int *)malloc(M * (sizeof(int)));
    for (i = 0; i < M; i++)
    {
        *(memory + i) = i;
    }
}

void readwritecache( int readwrite, int a, int *value, int *hitmiss, int voice )
{
    
    //
    // readwrite = READ (1) for read, WRITE (0) for write
    // a = binary memory address ( 0 <= a < 65535 ) to read or write
    // *value is memory value returned when readwrite = READ
    //           memory value to store when readwrite = WRITE
    // hitmiss is set to 0 for a miss, 1 for a hit, in either read or write case
    // voice is a debugging switch
    //
    //
    //   compute si, ta, and bo from the address a
    int si, ta, bo, li = -1;
    //	si = stack, index
    //	ta = tag
    //	bo = block offset
    //      increment callno
    //
    si = (a >> b) % S;
    ta = (a >> (s + b));
    bo = a % B;
    callno++;
    if( voice )
        printf( "%5d rw= %d a= %5d bo= %3d si= %3d ta= %3d", callno,readwrite,a,bo,si,ta ) ;
    
    //
    //   check each line of the set:
    //	if( cache[si][line#].valid && cache[si][line#].tag = ta )
    //	to find a hit
    //
    *hitmiss = 0;
    int i, j;
    for (i = 0; i < E; i++)
    {
        if (cache[si][i].valid && cache[si][i].tag == ta)
        {
            *hitmiss = 1;
            li = i;
            if (readwrite == WRITE)
            {
                whits++;
            }
            else if (readwrite == READ)
            {
                rhits++;
            }
            break;
        }
    }
    
    //
    //   if no hit, choose a line (not valid or LRU )
    //
    if (*hitmiss == 0)
    {
        if (readwrite == WRITE)
        {
            wmiss++;
        }
        else if (readwrite == READ)
        {
            rmiss++;
        }
        for (i = 0; i < E; i++)
        {
            if (!cache[si][i].valid)
            {
                li = i;
                break;
            }
        }
        if (li == -1)
        {
            int lastCall = cache[si][0].last;
            li = 0;
            for (i = 1; i < E; i++)
            {
                if (cache[si][i].last < lastCall)
                {
                    li = i;
                    lastCall = cache[si][i].last;
                }
            }
        }
    }
    
    //
    //   if chosen line dirty, copy to memory
    //
    if (cache[si][li].valid && cache[si][li].dirty && *hitmiss == 0)
    {
        for (i = ((cache[si][li].tag >> b) << b), j = 0; j < B; i++, j++)
        {
            memory[i] = cache[si][li].block[j];
        }
        dwrit++;
    }
    if( voice )
        printf( "li= %d", li ) ;
    
    //
    //    copy from memory to chosen line
    //
    if (*hitmiss == 0)
    {
        cache[si][li].valid = 1;
        cache[si][li].tag = ta;
        for (i = a - a % B, j = 0; j < B; i++, j++)
        {
            cache[si][li].block[j] = memory[i];
        }
    }
    
    //
    //    if write, copy value into line, set dirty
    //
    if (readwrite == WRITE)
    {
        cache[si][li].block[bo] = *value;
        cache[si][li].dirty = 1;
    }
    //    else copy value from line, not dirty
    else if (readwrite == READ)
    {
        *value = cache[si][li].block[bo];
        cache[si][li].dirty = 0;
    }
    
    //
    //    set last for line
    //
    cache[si][li].last = callno;
    if( voice )
        printf( " %d %d %d\n", *value, cache[si][li].valid, cache[si][li].dirty ) ;
}

void locationexample()
{
    int i,j,k,hm, r;
    
    for( y=16374; y< 16395; y=y+1 )
    {
        for( i=0; i<nj;i++ )
        {
            readwritecache( READ,   x+i, &r, &hm, 0 ) ;
            readwritecache( WRITE,  y+i, &r, &hm, 0 ) ;
        }
        stats( "loc copy" ) ;
    }
    
    
    //
    // code for row wise transponse
    //
    for (y = 16374; y < 16395; y++)
    {
        for (j = 0; j < ni; j++)
        {
            for (k = 0; k < nj; k++)
            {
                readwritecache(READ, y + j * nj + k, &r, &hm, 0);
                readwritecache(WRITE, x + k * nj + j, &r, &hm, 0);
            }
        }
        stats( "loc rows" ) ;
    }
    
    //
    // code for col wise transponse
    //
    for (y = 16374; y < 16395; y++)
    {
        for (k = 0; k < nj; k++)
        {
            for (j = 0; j < ni; j++)
            {
                readwritecache(READ, y + j * nj + k, &r, &hm, 0);
                readwritecache(WRITE, x + k * nj + j, &r, &hm, 0);
            }
        }
        stats( "loc cols" ) ;
    }
}

void wsexample()
{
    int i,j,ii,jj,hm,r ;
    
    y = 20000 ;
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for row wise transpose
        //
        for (i = 0; i < ni; i++)
        {
            for (j = 0; j < nj; j++)
            {
                readwritecache(READ, y + i * nj + j, &r, &hm, 0);
                readwritecache(WRITE, x + j * nj + i, &r, &hm, 0);
            }
        }
        stats( "ws rows" ) ;
    }
    
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for row wise transpose with 8x blocking
        //
        for (ii = 0; ii < ni; ii = ii + 8)
        {
            for (jj = 0; jj < nj; jj = jj + 8)
            {
                for (i = ii; i < ii + 8; i++)
                {
                    for (j = jj; j < jj + 8; j++)
                    {
                        readwritecache(READ, y + i * nj + j, &r, &hm, 0);
                        readwritecache(WRITE, x + j * nj + i, &r, &hm, 0);
                    }
                }
            }
        }
        stats( "wsbrows" ) ;
    }
    
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for col wise transpose 
        //
        for (j = 0; j < nj; j++)
        {
            for (i = 0; i < ni; i++)
            {
                readwritecache(READ, y + i * nj + j, &r, &hm, 0);
                readwritecache(WRITE, x + j * nj + i, &r, &hm, 0);
            }
        }
        stats( "ws cols" ) ;
    }
    
    for( ni=88; ni<128; ni=ni+8 )
    {
        nj = ni ;
        //
        // code for col wise transpose with 8x blocking
        //
        for (jj = 0; jj < nj; jj = jj + 8)
        {
            for (ii = 0; ii < ni; ii = ii + 8)
            {
                for (j = jj; j < jj + 8; j++)
                {
                    for (i = ii; i < ii + 8; i++)
                    {
                        readwritecache(READ, y + i * nj + j, &r, &hm, 0);
                        readwritecache(WRITE, x + j * nj + i, &r, &hm, 0);
                    }
                }
            }
        }
        stats( "wsbcols" ) ;
    }
    
}

int main()
{
    
    initcache() ;
    locationexample() ;
    wsexample() ;
    
    return 0 ;
}
