#include <stdio.h>
#include <stdlib.h>

#define HEAPSIZE 32768

#define hdr_payload(i)  ((struct HDR *) &heap[i])->payload
#define hdr_freeall(i)  ((struct HDR *) &heap[i])->freeall
#define hdr_succesr(i)  ((struct HDR *) &heap[i])->succesr
#define hdr_previus(i)  ((struct HDR *) &heap[i])->previus

char *heap ;
int  heapsize ;

int sizehdr ;

struct HDR       // free block header/footer/linked list
{
    int  payload ;    // size of block (excluding headers)
    char freeall ;    // is this block allocated? 0=free/1=allocated
    int  succesr ;    // successor free block
    int  previus ;    // previous free block
} anchor ;

void dump_heap( char *t )   // prints all of the headers/footers on the heap
{
    int i ;
    
    int j=0 ;
    printf( "%s anchor= %d\n", t, anchor.succesr ) ;
    for( i=0; i<heapsize; i=i+sizehdr  )
        if ( hdr_payload(i) != 0 )
        {
            printf( "%6d %6d %6d %6d %6d\n", i, hdr_payload(i), hdr_freeall(i), hdr_previus(i), hdr_succesr(i) ) ;
            if ( j++%2==1 ) printf( "\n" ) ;
        }
}

void init_heap()   // initialize the heap
{
    heap = (char *) calloc( HEAPSIZE, 1 ) ;
    sizehdr =  16 ;
    heapsize = HEAPSIZE ;
    
    anchor.payload =  0 ;   // anchor
    anchor.freeall =  1 ;
    anchor.succesr =  0 ;
    anchor.previus = -1 ;
    
    hdr_payload(0) = heapsize-2*sizehdr ;   // header
    hdr_freeall(0) = 0 ;
    hdr_succesr(0) = heapsize ;
    hdr_previus(0) = -1 ;
    hdr_payload(heapsize-sizehdr) = heapsize-2*sizehdr ; // footer
    hdr_freeall(heapsize-sizehdr) = 0 ;
}

void lfree( int *aa )   // frees the block at address aa
{
    // lfree code here
    sizehdr = 16;
    if (!hdr_freeall(*aa - sizehdr))
    {
        return;
    }
    int thishead = *aa - sizehdr;
    int thisfoot = *aa + hdr_payload(*aa - sizehdr);
    int lastfoot = *aa - 2 * sizehdr;
    int lasthead = *aa - 3 * sizehdr - hdr_payload(*aa - 2 * sizehdr);
    int nexthead = *aa + sizehdr + hdr_payload(*aa - sizehdr);
    int nextfoot = *aa + 2 * sizehdr + hdr_payload(*aa - sizehdr) + hdr_payload(*aa + sizehdr + hdr_payload(*aa - sizehdr));
    
    if ((nextfoot <= heapsize) && (lasthead >= 0) && (!hdr_freeall(lasthead)) && (!hdr_freeall(nexthead)))
    {
        if (hdr_previus(lasthead) == -1 && hdr_succesr(nexthead) == heapsize)
        {
            if (hdr_succesr(lasthead) == nexthead)
            {
                hdr_succesr(lasthead) = heapsize;
            }
            else
            {
                hdr_succesr(hdr_previus(nexthead)) = heapsize;
            }
            
        }
        else if (hdr_previus(nexthead) == -1 && hdr_succesr(lasthead) == heapsize)
        {
            if (hdr_succesr(nexthead) == lasthead)
            {
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = heapsize;
                anchor.succesr = lasthead;
            }
            else
            {
                hdr_previus(hdr_succesr(nexthead)) = lasthead;
                hdr_succesr(hdr_previus(lasthead)) = heapsize;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = hdr_succesr(nexthead);
                anchor.succesr = lasthead;
            }
        }
        else if (hdr_previus(lasthead) == -1)
        {
            if (hdr_succesr(lasthead) == nexthead)
            {
                hdr_previus(hdr_succesr(nexthead)) = lasthead;
                hdr_succesr(lasthead) = hdr_succesr(nexthead);
            }
            else
            {
                hdr_succesr(hdr_previus(nexthead)) = hdr_succesr(nexthead);
                hdr_previus(hdr_succesr(nexthead)) = hdr_previus(nexthead);
            }
        }
        else if (hdr_previus(nexthead) == -1)
        {
            if (hdr_succesr(nexthead) == lasthead)
            {
                hdr_previus(lasthead) = -1;
                anchor.succesr = lasthead;
            }
            else
            {
                hdr_previus(hdr_succesr(lasthead)) = hdr_previus(lasthead);
                hdr_succesr(hdr_previus(lasthead)) = hdr_succesr(lasthead);
                hdr_previus(hdr_succesr(nexthead)) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = hdr_succesr(nexthead);
                anchor.succesr = lasthead;
            }
        }
        else if (hdr_succesr(lasthead) == heapsize)
        {
            if (hdr_succesr(nexthead) == lasthead)
            {
                hdr_succesr(hdr_previus(nexthead)) = heapsize;
                hdr_previus(anchor.succesr) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = anchor.succesr;
                anchor.succesr = lasthead;
            }
            else
            {
                hdr_succesr(hdr_previus(lasthead)) = heapsize;
                hdr_succesr(hdr_previus(nexthead)) = hdr_succesr(nexthead);
                hdr_previus(hdr_succesr(nexthead)) = hdr_previus(nexthead);
                hdr_previus(anchor.succesr) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = anchor.succesr;
                anchor.succesr = lasthead;
            }
        }
        else if (hdr_succesr(nexthead) == heapsize)
        {
            if (hdr_succesr(lasthead) == nexthead)
            {
                hdr_succesr(hdr_previus(lasthead)) = heapsize;
                hdr_previus(anchor.succesr) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = anchor.succesr;
                anchor.succesr = lasthead;
            }
            else
            {
                hdr_succesr(hdr_previus(nexthead)) = heapsize;
                hdr_previus(hdr_succesr(lasthead)) = hdr_previus(lasthead);
                hdr_succesr(hdr_previus(lasthead)) = hdr_succesr(lasthead);
                hdr_previus(anchor.succesr) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = anchor.succesr;
                anchor.succesr = lasthead;
            }
        }
        else
        {
            if (hdr_succesr(nexthead) == lasthead)
            {
                hdr_previus(hdr_succesr(lasthead)) = hdr_previus(nexthead);
                hdr_succesr(hdr_previus(nexthead)) = hdr_succesr(lasthead);
                hdr_previus(anchor.succesr) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = anchor.succesr;
                anchor.succesr = lasthead;
            }
            else if (hdr_succesr(lasthead) == nexthead)
            {
                hdr_succesr(hdr_previus(lasthead)) = hdr_succesr(nexthead);
                hdr_previus(hdr_succesr(nexthead)) = hdr_previus(lasthead);
                hdr_previus(anchor.succesr) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = anchor.succesr;
                anchor.succesr = lasthead;
            }
            else
            {
                hdr_succesr(hdr_previus(lasthead)) = hdr_succesr(lasthead);
                hdr_previus(hdr_succesr(lasthead)) = hdr_previus(lasthead);
                hdr_succesr(hdr_previus(nexthead)) = hdr_succesr(nexthead);
                hdr_previus(hdr_succesr(nexthead)) = hdr_previus(nexthead);
                hdr_previus(anchor.succesr) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = anchor.succesr;
                anchor.succesr = lasthead;
            }
        }
        
        //update payload
        hdr_payload(lasthead) += hdr_payload(thishead) + hdr_payload(nexthead) + 4 * sizehdr;
        hdr_payload(lastfoot) = 0;
        hdr_payload(thishead) = 0;
        hdr_payload(thisfoot) = 0;
        hdr_payload(nexthead) = 0;
        hdr_payload(nextfoot) = hdr_payload(lasthead);
        
        //update freeall
        hdr_freeall(lastfoot) = 0;
        hdr_freeall(thishead) = 0;
        hdr_freeall(thisfoot) = 0;
        hdr_freeall(nextfoot) = 0;
        
        //update previus
        hdr_previus(lastfoot) = 0;
        hdr_previus(thishead) = 0;
        hdr_previus(thisfoot) = 0;
        hdr_previus(nexthead) = 0;
        hdr_previus(nextfoot) = 0;
        
        //update succesr
        hdr_succesr(lastfoot) = 0;
        hdr_succesr(thishead) = 0;
        hdr_succesr(thisfoot) = 0;
        hdr_succesr(nexthead) = 0;
        hdr_succesr(nextfoot) = 0;
    }
    else if ((nextfoot <= heapsize - sizehdr) && (thishead >= 0) && (!hdr_freeall(nexthead)))
    {
        //update previous and successing free block nexthead
        if (hdr_previus(nexthead) == -1 && hdr_succesr(nexthead) == heapsize)
        {
            hdr_succesr(thishead) = heapsize;
            hdr_previus(thishead) = -1;
            anchor.succesr = thishead;
        }
        else if (hdr_previus(nexthead) == -1)
        {
            hdr_previus(hdr_succesr(nexthead)) = thishead;
            hdr_previus(thishead) = -1;
            hdr_succesr(thishead) = hdr_succesr(nexthead);
            anchor.succesr = thishead;
        }
        else if (hdr_succesr(nexthead) == heapsize)
        {
            hdr_succesr(hdr_previus(nexthead)) = heapsize;
            hdr_previus(thishead) = -1;
            hdr_succesr(thishead) = anchor.succesr;
            hdr_previus(anchor.succesr) = thishead;
            anchor.succesr = thishead;
        }
        else
        {
            hdr_previus(hdr_succesr(nexthead)) = hdr_previus(nexthead);
            hdr_succesr(hdr_previus(nexthead)) = hdr_succesr(nexthead);
            hdr_previus(thishead) = -1;
            hdr_succesr(thishead) = anchor.succesr;
            hdr_previus(anchor.succesr) = thishead;
            anchor.succesr = thishead;
        }
        
        //update payload
        hdr_payload(thishead) += hdr_payload(nexthead) + 2 * sizehdr;
        hdr_payload(thisfoot) = 0;
        hdr_payload(nexthead) = 0;
        hdr_payload(nextfoot) = hdr_payload(thishead);
        
        //update freeall
        hdr_freeall(thishead) = 0;
        hdr_freeall(thisfoot) = 0;
        hdr_freeall(nextfoot) = 0;
        
        //update previus
        hdr_previus(thisfoot) = 0;
        hdr_previus(nexthead) = 0;
        hdr_previus(nextfoot) = 0;
        
        //update succesr
        hdr_succesr(thisfoot) = 0;
        hdr_succesr(nexthead) = 0;
        hdr_succesr(nextfoot) = 0;
    }
    else if ((lasthead >= 0) && (thisfoot < heapsize) && (!hdr_freeall(lasthead)))
    {
        //update previous and successing free block lasthead
        if (hdr_previus(lasthead) != -1)
        {
            if (hdr_succesr(lasthead) == heapsize)
            {
                hdr_succesr(hdr_previus(lasthead)) = heapsize;
                hdr_previus(anchor.succesr) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = anchor.succesr;
                anchor.succesr = lasthead;
            }
            else
            {
                hdr_previus(hdr_succesr(lasthead)) = hdr_previus(lasthead);
                hdr_succesr(hdr_previus(lasthead)) = hdr_succesr(lasthead);
                hdr_previus(anchor.succesr) = lasthead;
                hdr_previus(lasthead) = -1;
                hdr_succesr(lasthead) = anchor.succesr;
                anchor.succesr = lasthead;
            }
        }
        
        hdr_previus(lasthead) = -1;
        anchor.succesr = lasthead;
        
        //update payload
        hdr_payload(lasthead) += hdr_payload(thishead) + 2 * sizehdr;
        hdr_payload(lastfoot) = 0;
        hdr_payload(thishead) = 0;
        hdr_payload(thisfoot) = hdr_payload(lasthead);
        
        //update freeall
        hdr_freeall(thishead) = 0;
        hdr_freeall(thisfoot) = 0;
        hdr_freeall(lastfoot) = 0;
        
        //update previus
        hdr_previus(lastfoot) = 0;
        hdr_previus(thishead) = 0;
        hdr_previus(thisfoot) = 0;
        
        //update succesr
        hdr_succesr(lastfoot) = 0;
        hdr_succesr(thishead) = 0;
        hdr_succesr(thisfoot) = 0;
    }
    //if no coalescing
    else
    {
        hdr_freeall(thishead) = 0;
        hdr_freeall(thisfoot) = 0;
        hdr_previus(thisfoot) = 0;
        hdr_succesr(thisfoot) = 0;
        
        if (anchor.succesr == 0)
        {
            hdr_previus(thishead) = -1;
            hdr_succesr(thishead) = heapsize;
            anchor.succesr = thishead;
        }
        else
        {
            hdr_previus(anchor.succesr) = thishead;
            hdr_previus(thishead) = -1;
            hdr_succesr(thishead) = anchor.succesr;
            anchor.succesr = thishead;
        }
    }

    *aa = 0;
}

int lalloc( int amt )   // allocates a block of size int
{
    sizehdr = 16;
    int size;
    if (amt % sizehdr == 0)
    {
        size = amt + 2 * sizehdr;
    }
    else
    {
        size = amt - amt % sizehdr + 3 * sizehdr;
    }
    int i = anchor.succesr;
    while (hdr_payload(i) < size && hdr_succesr(i) != heapsize)
    {
        i = hdr_succesr(i);
    }
    if (hdr_payload(i) > size)
    {
        //test if the allocated block used to be succesr of anchor
        if (hdr_previus(i) == -1)
        {
            anchor.succesr = i + size;
            hdr_previus(i + size) = -1;
            hdr_succesr(i + size) = hdr_succesr(i);
            hdr_previus(i) = 0;
            hdr_succesr(i) = 0;
            
        }
        else
        {
            hdr_previus(hdr_succesr(i)) = i + size;
            hdr_succesr(hdr_previus(i)) = i + size;
            hdr_previus(i + size) = hdr_previus(i);
            hdr_succesr(i + size) = hdr_succesr(i);
            hdr_previus(i) = 0;
            hdr_succesr(i) = 0;
        }
        
        //update footer of newly allocated block
        hdr_freeall(i + size - sizehdr) = 1;
        hdr_payload(i + size - sizehdr) = size - 2 * sizehdr;
        hdr_previus(i + size - sizehdr) = 0;
        hdr_succesr(i + size - sizehdr) = 0;
        
        //update header of free fragment
        hdr_freeall(i + size) = 0;
        hdr_payload(i + size) = hdr_payload(i) - hdr_payload(i + size - sizehdr) - 2 * sizehdr;
        
        //update footer of free fragment
        hdr_freeall(i + sizehdr + hdr_payload(i)) = 0;
        hdr_payload(i + sizehdr + hdr_payload(i)) = hdr_payload(i + size);
        hdr_previus(i + sizehdr + hdr_payload(i)) = 0;
        hdr_succesr(i + sizehdr + hdr_payload(i)) = 0;
        //update header of newly allocated block
        hdr_freeall(i) = 1;
        hdr_payload(i) = size - 2 * sizehdr;
        
        return i + sizehdr;
    }
    else if (hdr_payload(i) == size)
    {
        //test if the allocated block used to be succesr of anchor
        if (hdr_previus(i) == -1)
        {
            anchor.succesr = hdr_succesr(i);
            hdr_previus(anchor.succesr) = -1;
            hdr_previus(i) = 0;
            hdr_succesr(i) = 0;
        }
        else
        {
            hdr_previus(hdr_succesr(i)) = hdr_previus(i);
            hdr_succesr(hdr_previus(i)) = hdr_succesr(i);
            hdr_previus(i) = 0;
            hdr_succesr(i) = 0;
        }
        
        //update footer of newly allocated block
        hdr_freeall(i + size - sizehdr) = 1;
        hdr_payload(i + size - sizehdr) = size - 2 * sizehdr;
        hdr_previus(i + size - sizehdr) = 0;
        hdr_succesr(i + size - sizehdr) = 0;
        
        //update header of newly allocated block
        hdr_freeall(i) = 1;
        hdr_payload(i) = size - 2 * sizehdr;
        
        return i + sizehdr;
    }
    
    //allocate new blocks
    int freeFragment = hdr_payload(heapsize - sizehdr);
    int originalHeapsize = heapsize;
    int newhead = heapsize - 2 * sizehdr - freeFragment;
    int extraSpace = size - freeFragment - 2 * sizehdr;
    int tempSize = size - 2 * sizehdr - freeFragment;
    do {
        heap = (char * )realloc(heap, heapsize + HEAPSIZE);
        heapsize += HEAPSIZE;
        tempSize -= HEAPSIZE;
    } while (tempSize > 0);
    hdr_succesr(i) = heapsize;
    
    //to use lfree, we need to allocate the block
    hdr_freeall(originalHeapsize) = 1;
    hdr_payload(originalHeapsize) = heapsize - originalHeapsize - 2 * sizehdr;
    hdr_previus(originalHeapsize) = 0;
    hdr_succesr(originalHeapsize) = 0;
    
    hdr_freeall(heapsize - sizehdr) = 1;
    hdr_payload(heapsize - sizehdr) = hdr_payload(originalHeapsize);
    hdr_previus(heapsize - sizehdr) = 0;
    hdr_succesr(heapsize - sizehdr) = 0;
    int freetag = originalHeapsize + sizehdr;
    lfree(&freetag);
    
    //update header of free fragment
    hdr_payload(newhead + size) = heapsize - originalHeapsize - extraSpace - 2 * sizehdr;
    hdr_freeall(newhead + size) = 0;
    hdr_previus(newhead + size) = hdr_previus(newhead);
    hdr_succesr(newhead + size) = hdr_succesr(newhead);
    if(hdr_succesr(newhead) != heapsize)
    {
        hdr_previus(hdr_succesr(newhead)) = newhead + size;
    }
    
    //update header of newly allocated block
    hdr_payload(newhead) = size - 2 * sizehdr;
    hdr_freeall(newhead) = 1;
    hdr_previus(newhead) = 0;
    hdr_succesr(newhead) = 0;
    
    //update footer of newly allocated block
    hdr_payload(newhead + size - sizehdr) = hdr_payload(newhead);
    hdr_freeall(newhead + size - sizehdr) = 1;
    hdr_previus(newhead + size - sizehdr) = 0;
    hdr_succesr(newhead + size - sizehdr) = 0;
    
    //update footer of free fragment
    hdr_payload(heapsize - sizehdr) = hdr_payload(newhead + size);
    hdr_freeall(heapsize - sizehdr) = 0;
    hdr_previus(heapsize - sizehdr) = 0;
    
    hdr_succesr(newhead + size) = hdr_succesr(newhead + size);
    
    anchor.succesr = newhead + size;
    
    return newhead + sizehdr;
}


int main()
{
    int i ;
    int j = 0  ;
    int blks[100] ;
    
    init_heap() ;
    dump_heap( "init heap" ) ;
    
    for( i=0; i<20; i++ )
        blks[j++] = lalloc( rand()%500 ) ;
    dump_heap( "first allocs" ) ;
    
    lfree( &blks[10] ) ;
    lfree( &blks[11] ) ;
    dump_heap( "coalesce with upper" ) ;
    
    lfree( &blks[14] ) ;
    lfree( &blks[13] ) ;
    dump_heap( "coalesce with lower" ) ;
    
    lfree( &blks[5] ) ;
    lfree( &blks[7] ) ;
    lfree( &blks[6] ) ;
    dump_heap( "coalesce with both" ) ;
    
    blks[5] = lalloc( 80000 ) ;
    dump_heap( "blew the top off" ) ;
    
    for( i=0; i<20; i++ )
        if (blks[i] != 0 )
        {
            lfree( &blks[i] ) ;
        }
    dump_heap( "free everything " ) ;
    
    return 0 ;
}