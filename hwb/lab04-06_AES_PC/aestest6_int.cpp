#include <stdio.h>
#include <immintrin.h>
#include <chrono>

/* AES-128 simple implementation template and testing */

/*
Author: Lukas Dolansky, dolanluk@fit.cvut.cz
Template: Jiri Bucek 2017
AES specification:
http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
*/

/* AES Constants */

/* Helper functions */
void hexprint16(uint8_t *p) {
	for (int i = 0; i < 16; i++)
		printf("%02hhx ", p[i]);
	puts("");
}

// **************** AES  functions ****************

inline __m128i shift(__m128i tempRSX, __m128i previousRound)
{
    __m128i newRound, tempXOR;
    // put RSX to every byte
    tempRSX = _mm_shuffle_epi32(tempRSX, 0xff);
    // shift and xor previous state to temp
    // XOR w3,w2,w1,w0 ^ w2,w1,w0,0x0
    tempXOR = _mm_slli_si128(previousRound, 0x4);
    newRound = _mm_xor_si128(tempXOR, previousRound);
    // XOR w3^w2,w2^w1,w1^w0,w0 ^ w1,w0,0x0,0x0
    tempXOR = _mm_slli_si128(tempXOR, 0x4);
    newRound = _mm_xor_si128(tempXOR, newRound);
    // XOR w3^w2^w1,w2^w1^w0,w1^w0,w0 ^ w0,0x0,0x0,0x0
    tempXOR = _mm_slli_si128(tempXOR, 0x4);
    newRound = _mm_xor_si128(tempXOR, newRound);
    // XOR w3^w2^w1^w0,w2^w1^w0,w1^w0,w0 ^ RSX,RSX,RSX,RSX
    newRound = _mm_xor_si128(tempRSX, newRound);
    return newRound;
}

//const uint8_t rCon[12] = {
//        0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36,
//};

inline void expandKeyI(uint8_t originalKey[16], uint32_t expandedKey[44])
{
    __m128i newRound, tempRSX;
    __m128i * key = (__m128i*)expandedKey;

    key[0] = newRound = _mm_loadu_si128((__m128i *)originalKey);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x01);
    key[1] = newRound = shift(tempRSX, newRound);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x02);
    key[2] = newRound = shift(tempRSX, newRound);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x04);
    key[3] = newRound = shift(tempRSX, newRound);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x08);
    key[4] = newRound = shift(tempRSX, newRound);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x10);
    key[5] = newRound = shift(tempRSX, newRound);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x20);
    key[6] = newRound = shift(tempRSX, newRound);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x40);
    key[7] = newRound = shift(tempRSX, newRound);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x80);
    key[8] = newRound = shift(tempRSX, newRound);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x1b);
    key[9] = newRound = shift(tempRSX, newRound);
    tempRSX = _mm_aeskeygenassist_si128(newRound, 0x36);
    key[10] = shift(tempRSX, newRound);
}

inline void aes(uint8_t *in, uint8_t *out, uint32_t * expKey)
{
	//... Initialize ...

    // first AddKey
    __m128i stateI = _mm_loadu_si128((__m128i *) in);
    stateI = _mm_xor_si128(stateI, ((__m128i *)expKey)[0]);

    // Inner AES rounds
    stateI = _mm_aesenc_si128(stateI, ((__m128i *)expKey)[1]);
    stateI = _mm_aesenc_si128(stateI, ((__m128i *)expKey)[2]);
    stateI = _mm_aesenc_si128(stateI, ((__m128i *)expKey)[3]);
    stateI = _mm_aesenc_si128(stateI, ((__m128i *)expKey)[4]);
    stateI = _mm_aesenc_si128(stateI, ((__m128i *)expKey)[5]);
    stateI = _mm_aesenc_si128(stateI, ((__m128i *)expKey)[6]);
    stateI = _mm_aesenc_si128(stateI, ((__m128i *)expKey)[7]);
    stateI = _mm_aesenc_si128(stateI, ((__m128i *)expKey)[8]);
    stateI = _mm_aesenc_si128(stateI, ((__m128i *)expKey)[9]);

    // Last round
    stateI = _mm_aesenclast_si128(stateI, ((__m128i *)expKey)[10]);

    // store result
    _mm_storeu_si128((__m128i *)out, stateI);
}

//****************************
// MAIN function: AES testing
//****************************
int main(int argc, char* argv[])
{

    int retry = 100000;

    if(argc > 1)
    {
        retry = atoi(argv[1]);
    }
    // test aes encryption
    uint8_t key[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    uint8_t in[16] =  { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89};
    //uint8_t out[16] = { 0, /*...*/ };
    uint32_t expKey[11 * 4];

//    printf("Key: ");
//    hexprint16(key);
//    printf("In:  ");
//    hexprint16(in);



    const auto start = std::chrono::high_resolution_clock::now();

    expandKeyI(key, expKey);
    for(int i = 0; i < retry; i++)
    {
        aes(in, in, expKey);
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> fp_ms = end - start;

//    printf("Out: ");
//    hexprint16(in);
//    puts("");

//    printf("Elapsed time for %d iterations of AES: %f ms\n",retry, fp_ms);
    printf("%.3f", fp_ms);

    return  in[0];
}
