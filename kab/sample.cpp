#ifndef __PROGTEST__
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>

#endif /* __PROGTEST__ */

// pseudo random generator of new texts
void changeText(unsigned char * text, int lengthOfByteMessage, unsigned char * byteHash, int length)
{
    for(int offset = 0; offset < length && offset < lengthOfByteMessage; offset++)
    {
        text[offset] ^= byteHash[offset];
    }
    return;
}

// transfer one hexa byte to two ascii chars
char byteToASCII (unsigned char c, int lowerHalf)
{
    char result;
    
    if(lowerHalf) c &= 0b00001111;
    else c = c >> 4;

    if(c < 10) result = '0' + c;
    else result = 'a' + c - 10;

    return result;
}

// transfrom hexa string to ascii string 
void toASCII(unsigned char * byteString, int byteStringLength, char ** message)
{
    *message = (char *) malloc(byteStringLength*2 + 1);
    int offsetNew = 0;
    for(int offset = 0; offset < byteStringLength; offset++, offsetNew++)
    {

        (*message)[offsetNew] = byteToASCII(byteString[offset], 0);
        offsetNew++;
        (*message)[offsetNew] = byteToASCII(byteString[offset], 1);
    }
    (*message)[offsetNew] = '\0';
    return;
}

// check if there are leading zeros
int myCheckHash(int bits, unsigned char byteHash[])
{
    int offset = 0;
    for(; offset < bits/8; offset++)
    {
        if(byteHash[offset] != 0x0) return 0;
    }
    
    int zeroBits = (bits%8);
    char mask = 0b10000000;
    unsigned char result = 0b10000000;
    mask = mask >> zeroBits;
    result = result >> zeroBits;
    if((byteHash[offset] & mask) != result) return 0;
    return 1;
}


int findHashEx (int bits, char ** message, char ** hash, const char * hashFunction) {
    /* TODO or use dummy implementation */
    
    //check for negative bits
    if(bits < 0) return 0;

    int correctFlag = 1; // check if error occured

//vvvvvvvvvvvvvvvvvvvvvvvvv from courses vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    EVP_MD_CTX * ctx;  // struktura kontextu
    const EVP_MD * type; // typ pouzite hashovaci funkce
    unsigned char byteHash[EVP_MAX_MD_SIZE]; // char pole pro hash - 64 bytu (max pro sha 512)
    unsigned int length;  // vysledna delka hashe

    /* Inicializace OpenSSL hash funkci */
    OpenSSL_add_all_digests();
    /* Zjisteni, jaka hashovaci funkce ma byt pouzita */
    type = EVP_get_digestbyname(hashFunction);

    /* Pokud predchozi prirazeni vratilo -1, tak nebyla zadana spravne hashovaci funkce */
    if (!type) {
 //       printf("Hash %s neexistuje.\n", hashFunction);
        correctFlag = 0;
    }

    ctx = EVP_MD_CTX_new(); // create context for hashing
    if (ctx == NULL)
        correctFlag = 0;
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^from coureses^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    int lengthOfByteMessage = 16;
    unsigned char * text = (unsigned char*) malloc(lengthOfByteMessage);
    
    if(RAND_bytes(text, lengthOfByteMessage) != 1) // generate random seed string
    {
        correctFlag = 0;
    }

    while(correctFlag)
    {

//vvvvvvvvvvvvvvvvvvvvvvvvv from courses vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
        /* Hash the text */
        if (!EVP_DigestInit_ex(ctx, type, NULL)) // context setup for our hash type
        {
            correctFlag = 0;
            break;
        }
        if (!EVP_DigestUpdate(ctx, text, lengthOfByteMessage)) // feed the message in
        {
            correctFlag = 0;
            break;
        }

        if (!EVP_DigestFinal_ex(ctx, byteHash, &length)) // get the hash
        {
            correctFlag = 0;
            break;
        }
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^from coureses^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        
        if((unsigned) bits > length) // check that bits are not longer than the hash itself
        {
            correctFlag = 0;
            break;
        }

        if(myCheckHash(bits, byteHash)) // check if '0' bits were found, otherwise change input
        {
            break;
        }
        else
        {
            changeText(text, lengthOfByteMessage, byteHash, length);
        }
    }

    if(correctFlag) // success -> tranfer to bytes and submit
    {
        toASCII(byteHash, length, hash);
        toASCII(text, lengthOfByteMessage, message);
    }
    else
    {
        free(text);
        EVP_MD_CTX_free(ctx);
        return 0;
    }
    free(text);
// ----------------------------from courses
    EVP_MD_CTX_free(ctx); // destroy the context
//-------------------------------------------
    return 1;
}

int findHash (int bits, char ** message, char ** hash) {
    /* TODO: Your code here */
    // run findHashEx with sha512
    char hashFunction[] = "sha512";
    return findHashEx(bits, message, hash, hashFunction);
}

#ifndef __PROGTEST__

// calculate number of leading zeros in hexa number
int zerosInHexa(char c)
{
    if(c >= '8') // 1xxx
    {
        return 0;
    }
    else if (c >= '4') // 01xx
    {
        return 1;
    }
    else if(c >= '2') // 001x
    {
        return 2;
    }
    else // 0001
    return 3;
}

int checkHash(int bits, char * hexString) {
    // DIY
    int offset = 0;
    for(; offset < bits/4; offset++)
    {
        if(hexString[offset] != '0') return 0;
    }
    
    int zeroBits = (bits%4);
    if(zeroBits != zerosInHexa(hexString[offset])) return 0;
    return 1;
}

int main (void) {
    char * message, * hash;
    assert(findHash(0, &message, &hash) == 1);
    assert(message && hash && checkHash(0, hash));
    free(message);
    free(hash);
    assert(findHash(1, &message, &hash) == 1);
    assert(message && hash && checkHash(1, hash));
    free(message);
    free(hash);
    assert(findHash(2, &message, &hash) == 1);
    assert(message && hash && checkHash(2, hash));
    free(message);
    free(hash);
    assert(findHash(3, &message, &hash) == 1);
    assert(message && hash && checkHash(3, hash));
    free(message);
    free(hash);
    assert(findHash(-1, &message, &hash) == 0);
    assert(findHashEx(3, &message, &hash, "sha512") == 1);
    assert(message && hash && checkHash(3, hash));
    free(message);
    free(hash);

    for( int i = 0; i < 20; i++)
    {
        assert(findHash(i, &message, &hash) == 1);
        assert(message && hash && checkHash(i, hash));
        free(message);
        free(hash);
    }
    printf("SUCCESS\n");
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

