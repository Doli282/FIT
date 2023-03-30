// Created: Lukas Dolansky (dolanluk) 2023/03/30

#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstring>

#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

struct crypto_config
{
	const char * m_crypto_function;
	std::unique_ptr<uint8_t[]> m_key;
	std::unique_ptr<uint8_t[]> m_IV;
	size_t m_key_len;
	size_t m_IV_len;
};

#endif /* _PROGTEST_ */

// check if crypto_config is correct
bool checkConfiguration(const EVP_CIPHER * cipher, crypto_config & config)
{
	// check if cipher mode does use inicialization vector
	unsigned int mode = EVP_CIPHER_mode(cipher);
	if((EVP_CIPH_ECB_MODE == mode) || (mode == EVP_CIPH_CTR_MODE))
	{
		// check if IV is long enough
		size_t iv_len = EVP_CIPHER_iv_length(cipher);
		if(iv_len > config.m_IV_len)
		{
			// generate new IV
			if(RAND_bytes(config.m_IV.get(), iv_len) != 1)
			{
				return false;
			}
			config.m_IV_len = iv_len;
		}
	}

	//check if key is long enough
	size_t key_len = EVP_CIPHER_key_length(cipher);
	if((key_len > config.m_key_len))
	{
		// generate new IV
		if(RAND_bytes(config.m_key.get(), key_len) != 1)
		{
			return false;
		}
		config.m_key_len = key_len;
	}
	return true;
}

// 1 encryption, 0 decryption
bool crypt_data ( const std::string & in_filename, const std::string & out_filename, crypto_config & config, int encryptOption)
{
	const unsigned int HEADER_SIZE = 18;
	const unsigned int BUFFER_SIZE = 512; // AES - block = 16 bytes -> 128 bits -> max 256 bits
	const unsigned int READ_SIZE = 256;
	const size_t SIZE_OF_BLOCK = sizeof(unsigned char);

	bool correct = true;

	FILE * inFile;
	FILE * outFile;
	int read;
	int written;
	int cipherTextLength;

	unsigned char OT[BUFFER_SIZE];
	unsigned char ST[BUFFER_SIZE];

	EVP_CIPHER_CTX * ctx;
	const EVP_CIPHER * cipher;

	OpenSSL_add_all_algorithms();

	//check configuration
	if(!(cipher = EVP_get_cipherbyname(config.m_crypto_function)))
	{
		return false;
	}
	if(!(checkConfiguration(cipher, config)))
	{
		return false;
	}

	// create new context
	if(!(ctx = EVP_CIPHER_CTX_new()))
	{
		return false;
	}
	
	// open files
	if(!(inFile = fopen(in_filename.c_str(), "rb")))
	{
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	if(!(outFile = fopen(out_filename.c_str(), "wb")))
	{
		EVP_CIPHER_CTX_free(ctx);
		std::fclose(inFile);
		return false;
	}

	// read header and write header
	if((read = fread(OT, SIZE_OF_BLOCK, HEADER_SIZE, inFile)) != HEADER_SIZE)
	{
		correct = false;
		goto end;
	}
	if((written = fwrite(OT,SIZE_OF_BLOCK, HEADER_SIZE, outFile)) != HEADER_SIZE)
	{
		correct = false;
		goto end;
	}

	// inicialization of ecryption
	//EVP_EncryptInit_ex(ctx, type, NULL, key, iv);
	if(!(EVP_CipherInit_ex(ctx, cipher, NULL, config.m_key.get(), config.m_IV.get(), encryptOption)))
	{
		correct = false;
		goto end;
	}

	while(true)
	{
		// read next block
		if((read = fread(OT, SIZE_OF_BLOCK, READ_SIZE, inFile)) == 0)
		{
			break;
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

end:
	EVP_CIPHER_CTX_free(ctx);
	std::fclose(inFile);
	std::fclose(outFile);
	
	return correct;	
}

bool encrypt_data ( const std::string & in_filename, const std::string & out_filename, crypto_config & config )
{
	return crypt_data(in_filename, out_filename, config, 1);
}

bool decrypt_data ( const std::string & in_filename, const std::string & out_filename, crypto_config & config )
{
	return crypt_data(in_filename, out_filename, config, 0);
}


#ifndef __PROGTEST__

bool compare_files ( const char * name1, const char * name2 )
{
	const size_t BUFFER_SIZE = 512;
	bool correct = true;
	
	FILE * file1;
	FILE * file2;
	size_t read1;
	size_t read2;
	unsigned char buffer1[BUFFER_SIZE];
	unsigned char buffer2[BUFFER_SIZE];

	// open files
	if(!(file1 = fopen(name1, "rb")))
	{
		return false;
	}
	if(!(file2 = fopen(name2, "rb")))
	{
		std::fclose(file1);
		return false;
	}

	while(correct)
	{
		// read bytes from files
		read1 = fread(buffer1, sizeof(buffer1[0]), BUFFER_SIZE, file1);
		read2 = fread(buffer2, sizeof(buffer2[0]), BUFFER_SIZE, file2);
		// check if the number of read bytes is the same
		if(read1 != read2)
		{
			correct = false;
			break;
		}
		// check for every byte if they are the same
		for(size_t i = 0; i < read1; i++)
		{
			if(buffer1[i] != buffer2[i])
			{
				correct = false;
				break;
			}
		}
		// read less than buffer? -> reached the EOF
		if(read1 < BUFFER_SIZE)
		{
			break;
		}
	}

	// close files and return
	std::fclose(file1);
	std::fclose(file2);
	return correct;
}

int main ( void )
{
	crypto_config config {nullptr, nullptr, nullptr, 0, 0};

	// ECB mode
	config.m_crypto_function = "AES-128-ECB";
	config.m_key = std::make_unique<uint8_t[]>(16);
 	memset(config.m_key.get(), 0, 16);
	config.m_key_len = 16;

	assert( encrypt_data  ("homer-simpson.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "homer-simpson_enc_ecb.TGA") );

	assert( decrypt_data  ("homer-simpson_enc_ecb.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "homer-simpson.TGA") );

	assert( encrypt_data  ("UCM8.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "UCM8_enc_ecb.TGA") );

	assert( decrypt_data  ("UCM8_enc_ecb.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "UCM8.TGA") );

	assert( encrypt_data  ("image_1.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "ref_1_enc_ecb.TGA") );

	assert( encrypt_data  ("image_2.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "ref_2_enc_ecb.TGA") );

	assert( decrypt_data ("image_3_enc_ecb.TGA", "out_file.TGA", config)  &&
		    compare_files("out_file.TGA", "ref_3_dec_ecb.TGA") );

	assert( decrypt_data ("image_4_enc_ecb.TGA", "out_file.TGA", config)  &&
		    compare_files("out_file.TGA", "ref_4_dec_ecb.TGA") );

	// CBC mode
	config.m_crypto_function = "AES-128-CBC";
	config.m_IV = std::make_unique<uint8_t[]>(16);
	config.m_IV_len = 16;
	memset(config.m_IV.get(), 0, 16);

	assert( encrypt_data  ("UCM8.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "UCM8_enc_cbc.TGA") );

	assert( decrypt_data  ("UCM8_enc_cbc.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "UCM8.TGA") );

	assert( encrypt_data  ("homer-simpson.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "homer-simpson_enc_cbc.TGA") );

	assert( decrypt_data  ("homer-simpson_enc_cbc.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "homer-simpson.TGA") );

	assert( encrypt_data  ("image_1.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "ref_5_enc_cbc.TGA") );

	assert( encrypt_data  ("image_2.TGA", "out_file.TGA", config) &&
			compare_files ("out_file.TGA", "ref_6_enc_cbc.TGA") );

	assert( decrypt_data ("image_7_enc_cbc.TGA", "out_file.TGA", config)  &&
		    compare_files("out_file.TGA", "ref_7_dec_cbc.TGA") );

	assert( decrypt_data ("image_8_enc_cbc.TGA", "out_file.TGA", config)  &&
		    compare_files("out_file.TGA", "ref_8_dec_cbc.TGA") );
	printf("SUCCESS\n");
	return 0;
}

#endif /* _PROGTEST_ */
