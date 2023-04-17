#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cmath>
using namespace std;
#endif /* __PROGTEST__ */


typedef uintptr_t * POINTER;
#define HEADER_SIZE (sizeof(uintptr_t))
#define POINTER_SIZE (sizeof(POINTER))
// 4 bytes: unsigned int for size of the allocated block <---- forward header
// 8 bytes: pointer to prevoius free block
// 8 bytes: pointer to next free block
// 4 bytes: unsigned int for size of the allocated block <---- backward header
#define BLOCK_SIZE (HEADER_SIZE + POINTER_SIZE + POINTER_SIZE + HEADER_SIZE) // size of a single memory block

/// @brief Structure for keeping track of allocated blocks
struct t_bitset
{
  uint8_t * pointerAtStart = 0;
  unsigned int bitsetSize = 0;  // size of bitset <--- maybe will be unnecessary
  
  void setBitset();
  bool findInbitset(void * blk, bool free);
  void bitsetDone(int * pendingBlk);
  void clean();
};

uintptr_t memorySize = 0;  // size of the given memory space
uintptr_t allocableMemorySize = 0; // size of the allocable memory

t_bitset bitset; // bitset to keepe record of starters od allocated headers

uintptr_t heapStart = 0;  // pointer given by the system -> but it points to servis information
uintptr_t shiftedHeapStart = 0; // pointer behind servis information -> here the real space for user data starts
uintptr_t heapEnd = 0;  // pointer to the end of the memory space given by the system

POINTER buddyList[32] = {0};  // list of pointers to list of free memory blocks  <=== BUDDY SYSTEM (Segregated free list)
unsigned int largestExp = 0; // largest exponent for buddy blocks


//------------------------- BITSET vv---------------------------------

/// @brief Allocate memory for bitset at the beginning of the memory space, clear the bitset and set pointer behind the bitset
void t_bitset::setBitset()
{
  // (memorySize / BLOCK_SIZE) = get number of blocks that fit in the memory space
  // /8 = get number of bits, not bytes
  bitsetSize = ((memorySize / BLOCK_SIZE) / 8); 
  // +1 = to get free bits for the reminder from ((memSize / BLOCK_SIZE) / 8)
  if(((memorySize / BLOCK_SIZE) % 8) > 0)
  {
    bitsetSize += 1;
  }
 
  uint8_t * ptr = pointerAtStart = (uint8_t *) heapStart;
  // clear the bitset + set the shiftedHeapStart pointer behind the bitset 
  for(unsigned int byte = 0; byte < bitsetSize; byte++, ptr++)
  {
    *ptr = 0;
  }

  // allign the shiftedHeapStart's address to BLOCK_SIZE
  uintptr_t offset = ((uintptr_t)ptr) % BLOCK_SIZE;
  if(offset > 0)
  {
    offset = 32 - offset;
  }
  shiftedHeapStart = (uintptr_t)ptr + offset;

  // save how much memory is usable
  allocableMemorySize = heapEnd - shiftedHeapStart;
  return;
}

/// @brief Check if given pointer to a block is start of an allocated chunk
/// @param blk pointer to a block
/// @param free true for free; false for allocation
/// @return true - start of allocated chunk; false otherwise; if free == false return !return values <<---
bool t_bitset::findInbitset(void * blk, bool free)
{
  uintptr_t bit = (((uintptr_t) blk) - shiftedHeapStart) / BLOCK_SIZE; // get the order of the bit
  unsigned int byte = bit / 8; // get in which byte is this bit
  if(byte > bitsetSize) return false; // error -> byte is larger than bitsize (should not happen)
  
  uint8_t mask = 0b1 << (bit % 8); // set mask for the position of the bit in the byte
  bool result = (pointerAtStart[byte] & mask); // mask the bit
  if(free)
  {
    pointerAtStart[byte] &= (~mask); // set 0 in the bitset  
  }
  else
  {
    pointerAtStart[byte] |= mask; // set 1 in the bitset
    return !result;
  }
  return result;
}

/// @brief Go through bitset bits and look for '1'  = allocated bulks 
/// @param pendingBlk number of still allocated bulks
void t_bitset::bitsetDone(int * pendingBlk)
{
  *pendingBlk = 0;
  uint8_t mask;
  uint8_t * ptr = pointerAtStart;
  for(unsigned int byte = 0; byte < bitsetSize; byte++)
  {
    mask = 0b1;
    for(int8_t bit = 0; bit < 8; bit++)
    {
      if(((*ptr) & mask) != 0)
      {
        (*pendingBlk)++;
      }
      mask <<= 1;
    }
    *ptr = 0; // cleaning
    ptr++;
  }
  clean();
}

/// @brief zero out variables
void t_bitset::clean()
{
  pointerAtStart = 0;
  bitsetSize = 0;
}
//------------------------- BITSET ^^---------------------------------

void clean()
{
  memorySize = 0;
  allocableMemorySize = 0;
  heapStart = 0;
  shiftedHeapStart = 0;
  heapEnd = 0;
  for(int i = 0; i < 32; i++) buddyList[i] = 0;
  largestExp = 0;
}

/// @brief Connect chunk to the start of the buddylist
/// @param exponent class in the buddyList
/// @param chunk pointer to the soon-to-be-connected chunk
void bind(unsigned int exponent, POINTER chunk)
{
  if(buddyList[exponent] == 0) // if the list is empty
  {
    chunk[2] = 0; // null pointer to the next chunk
  }
  else
  {
    chunk[2] = (uintptr_t)(buddyList[exponent]); // set pointer to the next chunk
    (buddyList[exponent])[1] = (uintptr_t)chunk; // set pointer from the next chunk to this chunk
  }
  chunk[1] = 0; // null pointer to the prevoius (nonexistent chunk)
  buddyList[exponent] = chunk; // set root pointer to the new chunk
  return;
}

/// @brief Put blockSize in the headers
/// @param blockStart pointer to start of the block
/// @param blockSize size of the block
/// @param allocated 0 for free block; 1 for allocated block
void prepareHeaders(POINTER blockStart, uintptr_t blockSize, uintptr_t allocated)
{
  blockStart[(blockSize - HEADER_SIZE)/POINTER_SIZE] = *blockStart = blockSize + allocated; // set blockSize in headers
}

/// @brief Connect the chunk in two, prepare the following chunk and bind is to buddyList
/// @param chunk pointer to chunk to be connected
/// @param exponent exponent of the newly created chunk
/// @param size size of the newly created chunk
void connect(POINTER chunk, unsigned int exponent, uintptr_t size)
{
  prepareHeaders(chunk, size, 0);
  bind(exponent, chunk);
}

/// @brief Fill BuddyList with initial blocks
void prepareBuddySystem()
{
  uintptr_t buddyBlockSize = BLOCK_SIZE;
  // find out how large block can fit in the allocable memory space
  while(buddyBlockSize < allocableMemorySize)
  {
    buddyBlockSize*=2;
    largestExp++;
  }
  // now is buddyBlockSize larger than allocable memorySpace -> it must be decreased again
  buddyBlockSize /= 2;
  largestExp--;

  // put the largest continuous block to the buddyList
  connect((POINTER)shiftedHeapStart, largestExp, buddyBlockSize);

  uintptr_t pointerToNewBlock = shiftedHeapStart + buddyBlockSize; // pointer behind the allocated buddyBlock
  uintptr_t leftOverMemory = allocableMemorySize - buddyBlockSize; // size of memory unasigned to buddyLists
  unsigned int currentExp = largestExp; // current size of BuddyBlock in terms of exponent
  // put small left over blocks to their buddyLists
  while(leftOverMemory >= BLOCK_SIZE)
  {
    buddyBlockSize /= 2; // decrease possible size of chunk
    currentExp--;
    if(buddyBlockSize <= leftOverMemory) // if chunk fits the leftover memory, put it in buddyList
    {
      leftOverMemory -= buddyBlockSize; // use leftovermemory for the buddyBlock
      connect((POINTER)pointerToNewBlock, currentExp, buddyBlockSize); // connect the buddyBlock to its list
      pointerToNewBlock += buddyBlockSize; // move pointer behind the solved chunk
    }
  }
  allocableMemorySize -= leftOverMemory; // decrease by the space left unused (smaller than BLOCK_SIZE)
  return;
}

/// @brief Disconnect free chunk from buddy list
/// @param exponent class in the buddy list
/// @param chunk pointer to the soon-to-be-disconnected chunk
void unbind(unsigned int exponent, POINTER chunk)
{
  POINTER ptrPrev = (POINTER) chunk[1];
  POINTER ptrNext = (POINTER) chunk[2];
  if(ptrNext != 0)
  {
    ptrNext[1] = (ptrPrev ? ptrPrev[2] : 0); // change PREV_POINTER in the following chunk
  }
  
  if(ptrPrev != 0)
  {
    ptrPrev[2]= (ptrNext ? ptrNext[1] : 0); // change NEXT_POINTER in the previous chunk
  }

  if(buddyList[exponent] == chunk)
  {
    buddyList[exponent] = ptrNext; // change root pointer to the next chunk
  }
}

/// @brief Prepare chunk - set allocation bit, set size and unbind from buddy list
/// @param exponent class in the buddylists
/// @param size minimal necessarry size for the chunk
/// @param chunk pointer for the chunk
bool allocate(unsigned int exponent, uintptr_t size, POINTER & chunk)
{
  chunk = buddyList[exponent];  // appoint pointer to the allocated chunk
  if(!bitset.findInbitset((void*)chunk, false)) return false; // set allocated bit in bitset
  prepareHeaders(chunk, size, 1); // prepare headers
  unbind(exponent, chunk); // unbind from buddyList
  return true;
}

/// @brief Looks at neighbours if they are free and merges blocks with the same size together or if both neighbours have half the size
/// @param chunk pointer to block to be merged (unbinded)
/// @param exponent current class of the block
void merge(POINTER chunk, unsigned int exponent)
{
  uintptr_t size = chunk[0];
  uintptr_t predecessorSize;
  uintptr_t successorSize;
  bool predecessorFree = ((uintptr_t)chunk > shiftedHeapStart && ((predecessorSize = chunk[-1]) & 1) == 0); // check if the previous block is free
  bool successorFree = ((uintptr_t)chunk < (shiftedHeapStart + allocableMemorySize) && ((successorSize = chunk[size/POINTER_SIZE]) & 1) == 0); // check if the following chunk is free
  if(predecessorFree && (size == predecessorSize)) // merge with predecessor
  {
    unbind(exponent, chunk = (&(chunk[predecessorSize/POINTER_SIZE]))); // disconnect from buddyList
    connect(chunk, ++exponent, size*2); // connect whole chunk to buddyList
    merge(chunk, exponent); // try merge again
  }
  else if(successorFree && (size == successorSize)) // merge with successor
  {
    unbind(exponent, &(chunk[size/POINTER_SIZE]));
    connect(&(chunk[size/POINTER_SIZE]), ++exponent, size*2);
    merge(chunk, exponent);
  }
  else if(successorFree && predecessorFree && (successorSize == predecessorSize) && (successorSize == size/2)) // predecessor and successor are half of chunk and free -> merge together
  {
    unbind(exponent-1, chunk = (&(chunk[predecessorSize/POINTER_SIZE])));
    unbind(exponent-1, &(chunk[size/POINTER_SIZE]));
    connect(chunk, ++exponent, size*2);
    merge(chunk, exponent);
  }
  else // connect the chunk to buddyList if there are no free budies
  {
    connect(chunk, exponent, size);
  }
  return;
}

/// @brief Divided allocated block to needed memory and free space. Connect free memory chunks to buddyList
/// @param chunk pointer to allocated memory
/// @param neededExponent minimal necessary exponent for
/// @param neededSize minimial necessary chunkSize
/// @param allocatedExponent real allocated exponent
void divide(POINTER chunk, unsigned int neededExponent, uintptr_t neededSize, unsigned int allocatedExponent)
{
  POINTER newChunk = nullptr;
  // divide allocated chunk into blocks and connect them to buddies
  while(neededExponent < allocatedExponent)
  {
    newChunk = chunk + (neededSize/POINTER_SIZE);
    connect(newChunk, neededExponent, neededSize);
    neededExponent++;
    neededSize *= 2;
  }
  // merge with neighbouring buddies
  if(newChunk)
  {
    merge(newChunk, neededExponent-1);
  }
}

/// @brief Find exponent to given size
/// @param size size of the chunk
/// @return class in the buddyList
unsigned int findExponent(uintptr_t size)
{
  unsigned int exponent = 0;
  while(size > BLOCK_SIZE)
  {
    size /= 2;
    exponent++;
  }
  return exponent;
}

// PROGTEST INTERFACE -----------------------------------------------VVVVV

void   HeapInit    ( void * memPool, int memSize )
{
  /* todo */

  // save the given memory space
  heapStart = (uintptr_t) memPool;
  memorySize = (uintptr_t) memSize;
  heapEnd = heapStart + memorySize;
  // calculate bits needed for the bitset of memory blocks origins
  // clear biteset
  bitset.setBitset();
  // set first allocable memory blocks to buddy system
  prepareBuddySystem();
  return;
}
void * HeapAlloc   ( int    size )
{
  /* todo */
  POINTER resultPtr = NULL;
  // fast check
  if(size < 0 || (uintptr_t)size > allocableMemorySize) return NULL;
  // calculate minimal size with the headers
  uintptr_t necessarySize = size + (2*HEADER_SIZE);
  uintptr_t roundedSize = BLOCK_SIZE;
  unsigned int roundedExp = 0;
  // find the first applicable size
  while(roundedSize < necessarySize)
  {
    roundedExp++;
    roundedSize *= 2;
  }

  // go through buddy lists (from smaller to larger blocks)
  uintptr_t chunkSize = roundedSize;
  unsigned int newExp = roundedExp;
  for(; newExp <= largestExp; newExp++, chunkSize *= 2)
  {
    // try to allocace ->  check if there are free chunks
    if(buddyList[newExp] != 0) 
    {
      if(!allocate(newExp, roundedSize, resultPtr)) return NULL; // allocate block
      divide(resultPtr, roundedExp, roundedSize, newExp); // divide the block + merge the other block with buddies if possible
      return (void *)(resultPtr + (HEADER_SIZE/POINTER_SIZE)); // return pointer pointing behind the header
    }
  }
  // if no suitable block is found return NULL
  return NULL;
}
bool   HeapFree    ( void * blk )
{
  /* todo */
  // check if the address is in the given memory space
  if((uintptr_t)blk < shiftedHeapStart || (uintptr_t)blk >= heapEnd) return false;
  // check if the address is aligned to BLOCK_SIZE
  if(((uintptr_t)blk) % BLOCK_SIZE != HEADER_SIZE) return false;
  // check if the block on the address is allocated (in bitset) and set 0 in bitset
  POINTER ptr = ((POINTER)blk) - HEADER_SIZE/POINTER_SIZE;
  if(!bitset.findInbitset((void*)ptr, true)) return false;
  // set headers
  *ptr -= 1;
  // try to merge block with neighbouring buddies to create larger block + put the block at the start of the buddy list
  merge(ptr, findExponent(ptr[0]));
  return true;
}
void   HeapDone    ( int  * pendingBlk )
{
  /* todo */
  // go through bitset to find out how many blocks are still allocated
  bitset.bitsetDone(pendingBlk);
  // clean variables and memory
  clean();
  return;
}

// PROGTEST INTERFACE -----------------------------------------------^^^^^

#ifndef __PROGTEST__
int main ( void )
{
  uint8_t       * p0, *p1, *p2, *p3, *p4;
  int             pendingBlk;
  static uint8_t  memPool[3 * 1048576];

  HeapInit ( memPool, 2097152 );
  assert ( ( p0 = (uint8_t*) HeapAlloc ( 512000 ) ) != NULL );
  memset ( p0, 0, 512000 );
  assert ( ( p1 = (uint8_t*) HeapAlloc ( 511000 ) ) != NULL );
  memset ( p1, 0, 511000 );
  assert ( ( p2 = (uint8_t*) HeapAlloc ( 26000 ) ) != NULL );
  memset ( p2, 0, 26000 );
  HeapDone ( &pendingBlk );
  assert ( pendingBlk == 3 );
  printf("1. TEST DONE\n");

  HeapInit ( memPool, 2097152 );
  assert ( ( p0 = (uint8_t*) HeapAlloc ( 1000000 ) ) != NULL );
  memset ( p0, 0, 1000000 );
  assert ( ( p1 = (uint8_t*) HeapAlloc ( 250000 ) ) != NULL );
  memset ( p1, 0, 250000 );
  assert ( ( p2 = (uint8_t*) HeapAlloc ( 250000 ) ) != NULL );
  memset ( p2, 0, 250000 );
  assert ( ( p3 = (uint8_t*) HeapAlloc ( 250000 ) ) != NULL );
  memset ( p3, 0, 250000 );
  assert ( ( p4 = (uint8_t*) HeapAlloc ( 50000 ) ) != NULL );
  memset ( p4, 0, 50000 );
  assert ( HeapFree ( p2 ) );
  assert ( HeapFree ( p4 ) );
  assert ( HeapFree ( p3 ) );
  assert ( HeapFree ( p1 ) );
  assert ( ( p1 = (uint8_t*) HeapAlloc ( 500000 ) ) != NULL );
  memset ( p1, 0, 500000 );
  assert ( HeapFree ( p0 ) );
  assert ( HeapFree ( p1 ) );
  HeapDone ( &pendingBlk );
  assert ( pendingBlk == 0 );
  printf("2. TEST DONE\n");


  HeapInit ( memPool, 2359296 );
  assert ( ( p0 = (uint8_t*) HeapAlloc ( 1000000 ) ) != NULL );
  memset ( p0, 0, 1000000 );
  assert ( ( p1 = (uint8_t*) HeapAlloc ( 500000 ) ) != NULL );
  memset ( p1, 0, 500000 );
  assert ( ( p2 = (uint8_t*) HeapAlloc ( 500000 ) ) != NULL );
  memset ( p2, 0, 500000 );
  assert ( ( p3 = (uint8_t*) HeapAlloc ( 500000 ) ) == NULL );
  assert ( HeapFree ( p2 ) );
  assert ( ( p2 = (uint8_t*) HeapAlloc ( 300000 ) ) != NULL );
  memset ( p2, 0, 300000 );
  assert ( HeapFree ( p0 ) );
  assert ( HeapFree ( p1 ) );
  HeapDone ( &pendingBlk );
  assert ( pendingBlk == 1 );
  printf("3. TEST DONE\n");


  HeapInit ( memPool, 2359296 );
  assert ( ( p0 = (uint8_t*) HeapAlloc ( 1000000 ) ) != NULL );
  memset ( p0, 0, 1000000 );
  assert ( ! HeapFree ( p0 + 1000 ) );
  HeapDone ( &pendingBlk );
  assert ( pendingBlk == 1 );
  printf("4. TEST DONE\n");

  printf("SUCCESS\n");
  return 0;
}
#endif /* __PROGTEST__ */

