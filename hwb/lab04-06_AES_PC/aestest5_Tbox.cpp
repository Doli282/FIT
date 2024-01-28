#include <stdio.h>
#include <memory.h>
#include <chrono>
#include <stdlib.h>

#include "TBOX.h"
/* AES-128 simple implementation template and testing */

/*
Author: Lukas Dolansky, dolanluk@fit.cvut.cz
Template: Jiri Bucek 2017
AES specification:
http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
*/

/* AES Constants */

// forward sbox
const uint8_t SBOX[256] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
}; 

const uint8_t rCon[12] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36,
};

// TBOX
//uint32_t T0[256];
//uint32_t T1[256];
//uint32_t T2[256];
//uint32_t T3[256];

/* AES state type */
typedef uint32_t t_state[4];

/* Helper functions */
void hexprint16(uint8_t *p) {
	for (int i = 0; i < 16; i++)
		printf("%02hhx ", p[i]);
	puts("");
}

void hexprintw(uint32_t w) {
	for (int i = 0; i < 32; i += 8)
		printf("%02hhx ", (w >> i) & 0xffU);
}

void hexprintws(uint32_t * p, int cnt) {
	for (int i = 0; i < cnt; i++)
		hexprintw(p[i]);
	puts("");
}
void printstate(t_state s) {
	hexprintw(s[0]);
	hexprintw(s[1]);
	hexprintw(s[2]);
	hexprintw(s[3]);
	puts("");
}

uint32_t word(uint8_t a0, uint8_t a1, uint8_t a2, uint8_t a3) {
	return a0 | (uint32_t)a1 << 8 | (uint32_t)a2 << 16 | (uint32_t)a3 << 24;
}

uint8_t wbyte(uint32_t w, int pos) {
	return (w >> (pos * 8)) & 0xff;
}

// **************** AES  functions ****************
uint32_t subWord(uint32_t w) {
	return word(SBOX[wbyte(w, 0)], SBOX[wbyte(w, 1)], SBOX[wbyte(w, 2)], SBOX[wbyte(w, 3)]);
}

void subBytes(t_state s) {
	s[0] = subWord(s[0]);
    s[1] = subWord(s[1]);
    s[2] = subWord(s[2]);
    s[3] = subWord(s[3]);
}


void shiftRows(t_state s) {
	t_state newState;
	newState[0] = word(wbyte(s[0], 0), wbyte(s[1], 1), wbyte(s[2], 2), wbyte(s[3], 3));
	newState[1] = word(wbyte(s[1], 0), wbyte(s[2], 1), wbyte(s[3], 2), wbyte(s[0], 3));
	newState[2] = word(wbyte(s[2], 0), wbyte(s[3], 1), wbyte(s[0], 2), wbyte(s[1], 3));
	newState[3] = word(wbyte(s[3], 0), wbyte(s[0], 1), wbyte(s[1], 2), wbyte(s[2], 3));
	memcpy(s, newState, sizeof(newState));
}

uint8_t xtime(uint8_t a) {
	/*test*/
	bool carry = a & 0x80;
	/*shift*/
	a = a<<1;
	/*XOR*/
	uint8_t m_x = 0x1b;
	if(carry)
	{
		a = a ^ m_x;
	}
	return a;
}

// not mandatory - mix a single column
uint32_t mixColumn(uint32_t c) {
	uint8_t c0, c1, c2, c3;
	uint8_t s0, s1, s2, s3;
	c0 = wbyte(c, 0);
	c1 = wbyte(c, 1);
	c2 = wbyte(c, 2);
	c3 = wbyte(c, 3);

	s0 = xtime(c0) ^ xtime(c1) ^ c1 ^ c2 ^c3;
	s1 = c0 ^ xtime(c1) ^ xtime(c2) ^ c2 ^ c3;
    s2 = c0 ^ c1 ^ xtime(c2) ^ xtime(c3) ^ c3;
    s3 = xtime(c0) ^ c0 ^ c1 ^ c2 ^ xtime(c3);

	return word(s0, s1, s2, s3);
}


void mixColumns(t_state s) {
	s[0] = mixColumn(s[0]);
	s[1] = mixColumn(s[1]);
	s[2] = mixColumn(s[2]);
	s[3] = mixColumn(s[3]);
}

/*
 * Rotate word [a0,a1,a2,a3] => [a1,a2,a3,a0]
 */
uint32_t rotWord(uint32_t a)
{
    uint32_t a0 = a & 0xff;
    a = a >> 8;
    a = a | (a0 << 24);
    return a;
}

/*
* Key expansion from 128bits (4*32b)
* to 11 round keys (11*4*32b)
* each round key is 4*32b
*/
void expandKey(uint8_t k[16], uint32_t ek[44]) {
	ek [0] = word(k[0], k[1], k[2], k[3]);
    short Nk = 4; // number of 32-bit words making up Cipher Key
    short Nr = 10; // number of rounds
    short Nb = 4; // number of columns
    short i = 0;
    for(; i < Nk; i++)
    {
        ek[i] = word(k[4*i], k[4*i + 1], k[4*i + 2], k[4*i+3]);
    }
    uint32_t temp;
    for(; i < (Nb * (Nr + 1)); i++)
    {
        temp = ek[i-1];
        if(i % Nk == 0)
        {
            temp = subWord((rotWord(temp))) ^ word(rCon[i/Nk], 0x0, 0x0, 0x0);
        }
        ek[i] = ek[i-Nk] ^ temp;
    }
}


/* Adding expanded round key (prepared before) */
void addRoundKey(t_state s, uint32_t ek[], short index) {
	s[0] ^= ek[index];
    s[1] ^= ek[index+1];
    s[2] ^= ek[index+2];
    s[3] ^= ek[index+3];
}

void TboxOperation(t_state state)
{
    t_state newState;
    newState[0] = T0[wbyte(state[0], 0)] ^ T1[wbyte(state[1], 1)] ^ T2[wbyte(state[2], 2)] ^ T3[wbyte(state[3], 3)];
    newState[1] = T0[wbyte(state[1], 0)] ^ T1[wbyte(state[2], 1)] ^ T2[wbyte(state[3], 2)] ^ T3[wbyte(state[0], 3)];
    newState[2] = T0[wbyte(state[2], 0)] ^ T1[wbyte(state[3], 1)] ^ T2[wbyte(state[0], 2)] ^ T3[wbyte(state[1], 3)];
    newState[3] = T0[wbyte(state[3], 0)] ^ T1[wbyte(state[0], 1)] ^ T2[wbyte(state[1], 2)] ^ T3[wbyte(state[2], 3)];
    memcpy(state, newState, sizeof(newState));
}


void aes(uint8_t *in, uint8_t *out, uint32_t * expKey)
{
	//... Initialize ...
	unsigned short round = 0;

	t_state state;

	state[0] = word(in[0],  in[1],  in[2],  in[3]);
    state[1] = word(in[4],  in[5],  in[6],  in[7]);
    state[2] = word(in[8],  in[9],  in[10],  in[11]);
    state[3] = word(in[12],  in[13],  in[14],  in[15]);

	addRoundKey(state, expKey, 0);

    for(round = 1; round < 10; round++)
    {
        TboxOperation(state);
        addRoundKey(state, expKey, round*4);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, expKey, round*4);

	for (int i = 0; i < 16; i++) {
		if (i < 4) out[i] = wbyte(state[0], i % 4);
		else if (i < 8) out[i] = wbyte(state[1], i % 4);
		else if (i < 12) out[i] = wbyte(state[2], i % 4);
		else out[i] = wbyte(state[3], i % 4);
	}
}

void precalculateTBOX() {
    for (int i = 0; i < 256; i++) {
        T0[i] = word(xtime(SBOX[i]), SBOX[i], SBOX[i], xtime(SBOX[i]) ^ SBOX[i]);
        T1[i] = word(xtime(SBOX[i]) ^ SBOX[i], xtime(SBOX[i]), SBOX[i], SBOX[i]);
        T2[i] = word(SBOX[i], xtime(SBOX[i]) ^ SBOX[i], xtime(SBOX[i]), SBOX[i]);
        T3[i] = word(SBOX[i], SBOX[i], xtime(SBOX[i]) ^ SBOX[i], xtime(SBOX[i]));
    }
}

// Print TBOX in format of C array
// Must delete the last ',' comma behind the last element !!
void printTBOX()
{
    printf("#include <stdint.h>\n\n");
    printf("uint32_t T0[256] = {\n");
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            printf("%u, ", T0[i*16+j]);
        }
        printf("\n");
    }
    printf("};\n");
    printf("uint32_t T1[256] = {\n");
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            printf("%u, ", T1[i*16+j]);
        }
        printf("\n");
    }
    printf("};\n");
    printf("uint32_t T2[256] = {\n");
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            printf("%u, ", T2[i*16+j]);
        }
        printf("\n");
    }
    printf("};\n");
    printf("uint32_t T3[256] = {\n");
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            printf("%u, ", T3[i*16+j]);
        }
        printf("\n");
    }
    printf("};\n");
}

//****************************
// MAIN function: AES testing
//****************************
int main(int argc, char* argv[])
{
//    precalculateTBOX();

    int retry = 100000;

    if(argc > 1)
    {
        retry = atoi(argv[1]);
    }
    // test aes encryption
//    printf("Testing aes\n");
    uint8_t key[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    uint8_t in[16] =  { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89};
    //uint8_t out[16] = { 0, /*...*/ };
    uint32_t expKey[11 * 4];

//    printf("Key: ");
//    hexprint16(key);
//    printf("In:  ");
//    hexprint16(in);

    const auto start = std::chrono::high_resolution_clock::now();

    expandKey(key, expKey);
//    precalculateTBOX();

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
