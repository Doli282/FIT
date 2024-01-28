#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

#include "aesfunctions.cpp"
#include "TBOX.h"

/* AES-128 simple implementation template and testing */

/*
Author: Lukas Dolansky, dolanluk@fit.cvut.cz
Template: Jiri Bucek 2017
AES specification:
http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
*/


void aesS(uint8_t *in, uint8_t *out, uint32_t * expKey)
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
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
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

//****************************
// MAIN function: AES testing
//****************************

int mainA(int retry)
{

    uint8_t key[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    uint8_t in[16] =  { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89};
    //uint8_t out[16] = { 0, /*...*/ };

    uint32_t expKey[11 * 4];
    expandKey(key, expKey);

    for(int i = 0; i < retry; i++)
    {
        aesS(in, in, expKey);

    }
	return  in[0];
}


/*
 * TBOX AES
 *
 */
void TboxOperation(t_state state)
{
    t_state newState;
    newState[0] = T0[wbyte(state[0], 0)] ^ T1[wbyte(state[1], 1)] ^ T2[wbyte(state[2], 2)] ^ T3[wbyte(state[3], 3)];
    newState[1] = T0[wbyte(state[1], 0)] ^ T1[wbyte(state[2], 1)] ^ T2[wbyte(state[3], 2)] ^ T3[wbyte(state[0], 3)];
    newState[2] = T0[wbyte(state[2], 0)] ^ T1[wbyte(state[3], 1)] ^ T2[wbyte(state[0], 2)] ^ T3[wbyte(state[1], 3)];
    newState[3] = T0[wbyte(state[3], 0)] ^ T1[wbyte(state[0], 1)] ^ T2[wbyte(state[1], 2)] ^ T3[wbyte(state[2], 3)];
    memcpy(state, newState, sizeof(newState));
}

void aesT(uint8_t *in, uint8_t *out, uint32_t * expKey)
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

//****************************
// MAIN function: AES testing
//****************************
int mainT(int retry)
{

    uint8_t key[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    uint8_t in[16] =  { 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89};
    //uint8_t out[16] = { 0, /*...*/ };

    uint32_t expKey[11 * 4];
    expandKey(key, expKey);

    for(int i = 0; i < retry; i++)
    {
        aesT(in, in, expKey);

    }

	return  in[0];
}
