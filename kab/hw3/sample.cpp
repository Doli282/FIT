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

    bool correct = true;    // flag of correctness
    
    FILE * inFile = nullptr;
    FILE * outFile = nullptr;
    int read = 0, written = 0, cipherTextLength = 0;
    unsigned char OT[BUFFER_SIZE] = ""; // input buffer
    unsigned char ST[BUFFER_SIZE + BLOCK_SIZE] = ""; // output buffer

    // flags of opened files
    bool inOpened = false;
    bool outOpened = false;
    bool keyOpened = false;
    bool ekMalloced = false;

    FILE * keyFile = nullptr;
    EVP_PKEY * keys = nullptr;
    EVP_CIPHER_CTX * ctx = nullptr;
    const EVP_CIPHER * cipherType = nullptr;
    int cipherNID = 0;

    int IVlen = 0;
    unsigned char IV[EVP_MAX_IV_LENGTH] = "";
    int ekl = 0; // length of encrypted symmetric key
    unsigned char * ek = nullptr; // buffer for symmetric key -> I will use only one

    // check input arguments
    if(!inFileName || !outFileName || !keyFileName || (!functionOpen && !symmetricCipher))
    {
        return false;
    }


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
    
    // open files
    if(!(keyFile = fopen(keyFileName, "rb")))
	{
		correct = false;
        goto end;
	}
    keyOpened = true;
	if(!(inFile = fopen(inFileName, "rb")))
	{
		correct = false;
        goto end;
	}
    inOpened= true;
	if(!(outFile = fopen(outFileName, "wb")))
	{
		correct = false;
        goto end;
	}
    outOpened = true;

    if(functionOpen)
    { // for decryption
        // prepare private key
        if(!(PEM_read_PrivateKey(keyFile, &keys, NULL, NULL)))
        {
            correct = false;
            goto end;
        }

        // get cipher and length of the encrypted symmetric key from the input file
        if((fread(&cipherNID, 4, 1, inFile) != 1) || feof(inFile) || ferror(inFile) ||
            (fread(&ekl, 4, 1, inFile) != 1) || feof(inFile) || ferror(inFile))
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
        // check if the key is long just right and allocate buffer for it
        if(ekl <= 0 || ekl != EVP_PKEY_size(keys))
        {
            correct = false;
            goto end;
        }
        ek = (unsigned char *) malloc(ekl*sizeof(*ek));
        if(!ek)
        {
            correct = false;
            goto end;
        }
        ekMalloced = true;

        // read the key
        if(fread(ek, 1, ekl, inFile) != (size_t)ekl || feof(inFile) || ferror(inFile))
        {
            correct = false;
            goto end;
        }

        // get IV, if there is any
        IVlen = EVP_CIPHER_iv_length(cipherType);
        if(IVlen > 0)
        {
            if(fread(IV, 1, IVlen, inFile) != (size_t)IVlen || feof(inFile) || ferror(inFile))
            {
                correct = false;
                goto end;
            }
        }

        // start decryption
        if(!(EVP_OpenInit(ctx, cipherType, ek, ekl, IV, keys)))
        {
            correct = false;
            goto end;
        }

    }
    else
    { // for encryption
        // get info about the symmetric cipher
        if(!(cipherType = EVP_get_cipherbyname(symmetricCipher)))
        {
            correct = false;
            goto end;
        }
        cipherNID = EVP_CIPHER_nid(cipherType);
        IVlen = EVP_CIPHER_iv_length(cipherType);
        
        // prepare public key
        if(!(PEM_read_PUBKEY(keyFile, &keys, NULL, NULL)))
        {
            correct = false;
            goto end;
        }
        // get the size of the encrypted symmetric key and allocate buffer for it
        if(!(ekl = EVP_PKEY_size(keys)))
        {
            correct = false;
            goto end;
        }
        ek = (unsigned char *) malloc(ekl*sizeof(*ek));
        if(!ek)
        {
            correct = false;
            goto end;
        }
        ekMalloced = true;

        // start cryption
        if(!(EVP_SealInit(ctx, cipherType, &ek, &ekl, IV, &keys, 1)))
        {
            correct = false;
            goto end;
        }

        // write header in the outFile
        if((fwrite(&cipherNID, 4, 1, outFile) != 1) || ferror(outFile) ||
            (fwrite(&ekl, 4, 1, outFile) != 1) || ferror(outFile) ||
            (fwrite(ek, SIZE_OF_BLOCK, ekl, outFile) != (size_t)ekl) || ferror(outFile))
        {
            correct = false;
            goto end;
        }
        //wirte IVlen if it is used
        if(IVlen > 0)
        {
            if(fwrite(IV, SIZE_OF_BLOCK, IVlen, outFile) != (size_t)IVlen || ferror(outFile))
            {
                correct = false;
                goto end;
            }
        }
    }


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
		if((written = fwrite(ST, SIZE_OF_BLOCK, cipherTextLength, outFile)) != cipherTextLength || ferror(outFile))
		{
			correct = false;
			goto end;
		}
	}

    // cipher and write the last block
	if(read != 0)
	{
		if(!EVP_CipherUpdate(ctx, ST, &cipherTextLength, OT, read))
		{
			correct = false;
			goto end;
		}
		if((written = fwrite(ST, SIZE_OF_BLOCK, cipherTextLength, outFile)) != cipherTextLength || ferror(outFile))
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

	// write the remainder
	if((written = fwrite(ST, SIZE_OF_BLOCK, cipherTextLength, outFile)) != cipherTextLength || ferror(outFile))
	{
		correct = false;
		goto end;
	}

// cleanup
end:
    EVP_CIPHER_CTX_free(ctx);
    EVP_PKEY_free(keys);
    if(ekMalloced) free(ek);
    if(keyOpened) std::fclose(keyFile);
    if(inOpened) std::fclose(inFile);
    if(outOpened) std::fclose(outFile);
    if(!correct && outOpened)    // delete the output file if an error occured
    {
        remove(outFileName);
    }
    return correct;
}

bool seal( const char * inFile, const char * outFile, const char * publicKeyFile, const char * symmetricCipher)
{
    return crypt_data(inFile, outFile, publicKeyFile, symmetricCipher, false);
}


bool open( const char * inFile, const char * outFile, const char * privateKeyFile)
{
    return crypt_data(inFile, outFile, privateKeyFile);
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

