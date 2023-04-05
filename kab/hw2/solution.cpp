// Created: Lukas Dolansky (dolanluk)

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
// encrypt = 1 for encryption / 0 for decryption
bool checkConfiguration(const EVP_CIPHER * cipher, crypto_config & config, int encrypt)
{
	// check if cipher mode does use inicialization vector
	unsigned int mode = EVP_CIPHER_mode(cipher);
	if(!((EVP_CIPH_ECB_MODE == mode) || (mode == EVP_CIPH_CTR_MODE)))
	{
		// check if IV is long enough
		size_t iv_len = EVP_CIPHER_iv_length(cipher);
		if((config.m_IV == nullptr) || (iv_len > config.m_IV_len))
		{
			// decrypt function can not create new IV
			if(encrypt == 0)
			{
				return false;
			}
			// generate new IV for encryption
			config.m_IV = make_unique<uint8_t[]>(iv_len);
			if(RAND_bytes(config.m_IV.get(), iv_len) != 1)
			{
				return false;
			}
			config.m_IV_len = iv_len;
		}
	}

	//check if key is long enough
	size_t key_len = EVP_CIPHER_key_length(cipher);
	if((config.m_key == nullptr) || (key_len > config.m_key_len))
	{
		// decrypt function can not create new key
		if(encrypt == 0) 
		{
			return false;
		}
		// generate new key for encryption
		config.m_key = make_unique<uint8_t[]>(key_len);
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
	const unsigned int HEADER_SIZE = 18; // header of the .TGA file
	const unsigned int BLOCK_SIZE = 128; // AES - block = 16 bytes -> 128 bits
	const unsigned int BUFFER_SIZE = 1024;  // should be larger than BLOCK_SIZE
	const unsigned int READ_SIZE = BUFFER_SIZE;
	const size_t SIZE_OF_BLOCK = sizeof(unsigned char);

	bool correct = true; // flag if everything is going smoothly

	FILE * inFile;
	FILE * outFile;
	int read;
	int written;
	int cipherTextLength;

	unsigned char OT[BUFFER_SIZE];
	unsigned char ST[BUFFER_SIZE+BLOCK_SIZE];

	OpenSSL_add_all_algorithms();
	EVP_CIPHER_CTX * ctx;
	const EVP_CIPHER * cipher = EVP_get_cipherbyname(config.m_crypto_function);

	//check configuration
	if(!cipher)
	{
		return false;
	}
	if(!(checkConfiguration(cipher, config, encryptOption)))
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
	if(!(EVP_CipherInit_ex(ctx, cipher, NULL, config.m_key.get(), config.m_IV.get(), encryptOption)))
	{
		correct = false;
		goto end;
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
				//printf("feof\n");
				break;
			}
			else if(ferror(inFile))
			{
				//printf("reading error\n");
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

	// cleanup
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
			printf("read1(%lu) != read2(%lu)\n", read1, read2);
			break;
		}
		// check for every byte if they are the same
		for(size_t i = 0; i < read1; i++)
		{
			if(buffer1[i] != buffer2[i])
			{
				printf("buffer1[%lu] (%u) != (%u)buffer2[%lu]\n", i, buffer1[i], buffer2[i], i);
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

	printf("ECB invalid key length\n");
	config.m_crypto_function = "AES-128-ECB";
	config.m_key_len = 2;
	config.m_key = std::make_unique<uint8_t[]>(2);
	memset(config.m_key.get(), 0, 2);
	assert( encrypt_data  ("homer-simpson.TGA", "out_file.TGA", config) &&
			decrypt_data  ("out_file.TGA", "out_file2.TGA", config) &&
			compare_files ("out_file2.TGA", "homer-simpson.TGA") );

	printf("ECB larger key\n");
	config.m_crypto_function = "AES-128-ECB";
	config.m_key_len = 20;
	config.m_key = std::make_unique<uint8_t[]>(20);
	memset(config.m_key.get(), 0, 20);
	assert( encrypt_data  ("homer-simpson.TGA", "out_file.TGA", config) &&
			decrypt_data  ("out_file.TGA", "out_file2.TGA", config) &&
			compare_files ("out_file2.TGA", "homer-simpson.TGA") );
	
	printf("CBC without key and IV\n");
	config.m_crypto_function = "AES-128-CBC";
	config.m_key = nullptr;
	config.m_key_len = 0;
	config.m_IV = nullptr;
	config.m_IV_len = 0;
	assert( encrypt_data  ("UCM8.TGA", "out_file.TGA", config) &&
			decrypt_data  ("out_file.TGA", "out_file2.TGA", config) &&
			compare_files ("out_file2.TGA", "UCM8.TGA") );

	printf("CBC without key and IV but has length\n");
	config.m_crypto_function = "AES-128-CBC";
	config.m_key = nullptr;
	config.m_key_len = 20;
	config.m_IV = nullptr;
	config.m_IV_len = 20;
	assert( encrypt_data  ("UCM8.TGA", "out_file.TGA", config) &&
			decrypt_data  ("out_file.TGA", "out_file2.TGA", config) &&
			compare_files ("out_file2.TGA", "UCM8.TGA") );

	printf("CBC larger key and IV\n");
	config.m_crypto_function = "AES-128-CBC";
	config.m_key_len = 20;
	config.m_key = std::make_unique<uint8_t[]>(20);
	memset(config.m_key.get(), 0, 20);
	config.m_IV_len = 20;
	config.m_IV = std::make_unique<uint8_t[]>(20);
	memset(config.m_IV.get(), 0, 20);
	assert( encrypt_data  ("UCM8.TGA", "out_file.TGA", config) &&
			decrypt_data  ("out_file.TGA", "out_file2.TGA", config) &&
			compare_files ("out_file2.TGA", "UCM8.TGA") );

	printf("noexistent input file\n");
	assert(! (encrypt_data("invalid_File.TGA", "out_file2.TGA", config)));
	printf("Invalid header in input file\n");
	assert(! (encrypt_data("invalid_Header.TGA", "out_file2.TGA", config)));
	printf("Invalid Read\n");
	assert(! (encrypt_data("nonreadable.TGA", "out_file2.TGA", config)));
	printf("Invalid Write\n");
	assert(! (encrypt_data("out_file2.TGA", "nonwritable.TGA", config)));

	printf("Invalid Inputs - SUCCESS\n");
	
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
