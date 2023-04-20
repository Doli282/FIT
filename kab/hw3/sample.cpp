#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstring>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/pem.h>

using namespace std;

#endif /* __PROGTEST__ */

bool crypt_data(const char * inFileName, const char * outFileName, const char * keyFileName, const char * symmetricCipher = nullptr, bool functionOpen = true)
{
    const unsigned int BUFFER_SIZE = 1024;
    const unsigned int BLOCK_SIZE = EVP_MAX_BLOCK_LENGTH;
    const unsigned int READ_SIZE = BUFFER_SIZE;
    const size_t SIZE_OF_BLOCK = sizeof(unsigned char);

    bool correct = true;
    
    FILE * inFile;
    FILE * outFile;
    int read, written, cipherTextLength;
    unsigned char OT[BUFFER_SIZE];
    unsigned char ST[BUFFER_SIZE + BLOCK_SIZE];

    bool inOpened = false;
    bool outOpened = false;
    bool keyOpened = false;
    bool ekMalloced = false;

    FILE * keyFile;
    EVP_PKEY * keys;
    EVP_CIPHER_CTX * ctx;
    const EVP_CIPHER * cipherType;
    int cipherNID;

    int IVlen;
    unsigned char IV[EVP_MAX_IV_LENGTH];
    int ekl; // length of encrypted symmetric key
    unsigned char * ek; // buffer for symmetric key -> I will use only one

    //printf("Variables allocated\n");
    
    // create new context
	if(!(ctx = EVP_CIPHER_CTX_new()))
	{
		return false;
	}
    if(!(keys = EVP_PKEY_new()))
    {
        correct = false;
        goto end;
    }
    //printf("Context created\n");
    
    // open files
    if(!(keyFile = fopen(keyFileName, "rb")))
	{
        //printf("could not open KEY '%s'\n", keyFileName);
		correct = false;
        goto end;
	}
    keyOpened = true;
	if(!(inFile = fopen(inFileName, "rb")))
	{
        //printf("could not open IN '%s'\n", inFileName);
		correct = false;
        goto end;
	}
    inOpened= true;
	if(!(outFile = fopen(outFileName, "wb")))
	{
        //printf("could not open OUT '%s'\n", outFileName);
		correct = false;
        goto end;
	}
    outOpened = true;
    //printf("Files opened\n");

    if(functionOpen)
    { // for decryption
        //printf("decrypting...\n");
        // prepare private key
        if(!(PEM_read_PrivateKey(keyFile, &keys, NULL, NULL)))
        {
            correct = false;
            goto end;
        }

        // get cipher from the input file
        if((fread(&cipherNID, 4, 1, inFile) != 1) ||
            (fread(&ekl, 4, 1, inFile) != 1))
        {
            correct = false;
            goto end;
        }
        ek = (unsigned char *) malloc(ekl*sizeof(*ek));
        ekMalloced = true;

        if(fread(ek, 1, ekl, inFile) != (size_t)ekl)
        {
            correct = false;
            goto end;
        }
        // get info about the symmetric cipher
        if(!(cipherType = EVP_get_cipherbynid(cipherNID)))
        {
            correct = false;
            goto end;
        }

        IVlen = EVP_CIPHER_iv_length(cipherType);
        if(IVlen > 0)
        {
            if(fread(IV, 1, IVlen, inFile) != (size_t)IVlen)
            {
                correct = false;
                goto end;
            }
        }

        if(!(EVP_OpenInit(ctx, cipherType, ek, ekl, IV, keys)))
        {
            correct = false;
            goto end;
        }

    }
    else
    { // for encryption
        //printf("encrypting....\n");
        // get info about the symmetric cipher
        if(!(cipherType = EVP_get_cipherbyname(symmetricCipher)))
        {
            //printf("could not get cipher by name (%s)\n", symmetricCipher);
            correct = false;
            goto end;
        }
        cipherNID = EVP_CIPHER_nid(cipherType);
//        symCipherLen = EVP_CIPHER_key_length(cipherType);
        IVlen = EVP_CIPHER_iv_length(cipherType);
        
        // prepare public key
        if(!(PEM_read_PUBKEY(keyFile, &keys, NULL, NULL)))
        {
            //printf("could not read PUBKEY\n");
            correct = false;
            goto end;
        }
        if(!(ekl = EVP_PKEY_size(keys)))
        {
            //printf("could not determine key size\n");
            correct = false;
            goto end;
        }
        ek = (unsigned char *) malloc(ekl*sizeof(*ek));
        ekMalloced = true;
 
        if(!(EVP_SealInit(ctx, cipherType, &ek, &ekl, IV, &keys, 1)))
        {
            //printf("could not initialize SEAL\n");
            correct = false;
            goto end;
        }

        // write header in the outFile
        if((fwrite(&cipherNID, 4, 1, outFile) != 1) ||
            (fwrite(&ekl, 4, 1, outFile) != 1) ||
            (fwrite(ek, SIZE_OF_BLOCK, ekl, outFile) != (size_t)ekl))
        {
            //printf("could not write header\n");
            correct = false;
            goto end;
        }
        if(IVlen > 0)
        {
            if(fwrite(IV, SIZE_OF_BLOCK, IVlen, outFile) != (size_t)IVlen)
            {
                //printf("could not write IV\n");
                correct = false;
                goto end;
            }
        }
    }


    //printf("start translating files\n");
    // loop for reading the input file, en/decrypting and wirting to output file
	while(true)
	{
		// read next block
		if((read = fread(OT, SIZE_OF_BLOCK, READ_SIZE, inFile)) < (int)READ_SIZE)
		{
			// check if EOF or error occured
			if(feof(inFile))
			{
				break;
			}
			else if(ferror(inFile))
			{
				correct = false;
				goto end;
			}
		}

		// cipher the block
		if(!EVP_CipherUpdate(ctx, ST, &cipherTextLength, OT, read))
		{
			correct = false;
			goto end;
		}
		// write the block
		if((written = fwrite(ST, SIZE_OF_BLOCK, cipherTextLength, outFile)) != cipherTextLength)
		{
			correct = false;
			goto end;
		}
	}

    // cipher the last block
	if(read != 0)
	{
		if(!EVP_CipherUpdate(ctx, ST, &cipherTextLength, OT, read))
		{
			correct = false;
			goto end;
		}
		// write the block
		if((written = fwrite(ST, SIZE_OF_BLOCK, cipherTextLength, outFile)) != cipherTextLength)
		{
			correct = false;
			goto end;
		}
	}

	// finalize cryption
	if(!(EVP_CipherFinal_ex(ctx, ST, &cipherTextLength)))
	{
		correct = false;
		goto end;
	}

	// write the block
	if((written = fwrite(ST, SIZE_OF_BLOCK, cipherTextLength, outFile)) != cipherTextLength)
	{
		correct = false;
		goto end;
	}

    //printf("regular ");
// cleanup
end:
    //printf("cleanup\n");
    EVP_CIPHER_CTX_free(ctx);
    EVP_PKEY_free(keys);
    if(ekMalloced) free(ek);
    if(keyOpened) std::fclose(keyFile);
    if(inOpened) std::fclose(inFile);
    if(outOpened) std::fclose(outFile);
    if(!correct && outOpened)    // delete the output file if an error occured
    {
        //printf("removing outFile\n");
        remove(outFileName);
    }
    //printf("return %d\n", correct);
    return correct;
}

bool seal( const char * inFile, const char * outFile, const char * publicKeyFile, const char * symmetricCipher)
{
    //waiting for code...
    //printf("---------SEAL------\n");
    return crypt_data(inFile, outFile, publicKeyFile, symmetricCipher, false);
    //return true;
}


bool open( const char * inFile, const char * outFile, const char * privateKeyFile)
{
    //waiting for code...
    //printf("---------OPEN-------\n");
    return crypt_data(inFile, outFile, privateKeyFile);
    //return true;
}



#ifndef __PROGTEST__

int main ( void )
{
    printf("START\n");
    
    printf("1]====================Test seal - correct ===========================\n");
    assert( seal("fileToEncrypt", "sealed.bin", "PublicKey.pem", "aes-128-cbc") );
    printf("2]====================Test open - correct ===========================\n");
    assert( open("sealed.bin", "openedFileToEncrypt", "PrivateKey.pem") );
    printf("3]====================Test open2 - correct ==========================\n");
    assert( open("sealed_sample.bin", "opened_sample.txt", "PrivateKey.pem") );
    
    printf("+++++ GIVEN TESTS COPMLETED ++++\n");
    
    printf("4a]=======Test seal - nonexistent file to open==========\n");
    assert(! seal("NonexistentFile", "INVALIDsealed.bin", "PublicKey.pem", "aes-128-cbc"));
    printf("4b]=======Test open - nonexistent file to open==========\n");
    assert(! open("NonexistentFile", "INVALIDopened_sample.txt", "PrivateKey.pem") );
    printf("4c]=======Test seal - nonexistent file to open==========\n");
    assert( seal("fileToEncrypt", "NonexistentFile2", "PublicKey.pem", "aes-128-cbc"));
    printf("4d]=======Test open - nonexistent file to open==========\n");
    assert( open("sealed.bin", "NonexistentFile3", "PrivateKey.pem") );
    
    printf("5]=======Test seal - invalid cipher==========\n");
    assert(! seal("fileToEncrypt", "INVALIDsealed.bin", "PublicKey.pem", "asdg") );
    printf("6]=======Test seal - different cipher==========\n");
    assert( seal("fileToEncrypt", "sealed2.bin", "PublicKey.pem", "aes-128-ecb") );
    printf("6]=======Test open - different cipher==========\n");
    assert( open("sealed2.bin", "openedFileToEncrypt2", "PrivateKey.pem") );
    
    printf("7a]=======Test seal - nonexistent key to open==========\n");
    assert(! seal("fileToEncrypt", "INVALIDsealed.bin", "IVALIDPublicKey.pem", "aes-128-cbc"));
    printf("7b]=======Test open - nonexistent key to open==========\n");
    assert(! open("sealed.bin", "INVALIDopened_sample.txt", "INVALIDPrivateKey.pem") );
    

    printf("========================================================\n");
    printf("SUCCESS\n");
    return 0;
}

#endif /* __PROGTEST__ */

