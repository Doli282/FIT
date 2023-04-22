#ifndef _MSC_VER
#define _snprintf snprintf
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void usage(void)
{
	printf("usage: bek1 name\n");
	printf("\n");
	printf("bek1 prints your name and does a few other things.\n");
	printf("Watch out for errors, there is a lot of them!\n");
	exit(EXIT_SUCCESS);
}

int CalcMagicNum(int nHi, int nLo)
{
	int nResult;

	if (nHi > 9)
		nResult = CalcMagicNum(nHi / 10, nHi % 10) + nLo;
	else
		nResult = nHi + nLo;

	if (nResult > 9)
		return CalcMagicNum(1, nResult % 10);

	return nResult;
}

int CalcMagicStr(const char* psz)
{
	int nResult = 0;
	char c = psz[0];

	if (c != 0)
	{
		c -= '0';
		nResult = c + CalcMagicStr(++psz);
		nResult = CalcMagicNum(nResult, 0);
	}

	return nResult;
}

void encrypt(char* psz)
{
	while (*psz)
	{
		char c = psz[0];
		if (c >= 'A' && c <= 'Z')
			c = 'Z' - c + 'A';
		else if (c >= 'a' && c <= 'z')
			c = 'z' - c + 'a';
		else if (c >= '0' && c <= '9')
			c = '9' - c + '0';
		*psz++ = c;
	}
}

int main(int argc, char* argv[])
{
	const char* pszName;
	char szName[32];
	char szDay[16];
	char szMonth[16];
	char szYear[16];
	char szComb[16];
	char szBuffer[80];
	char szMagic[16];
	unsigned short sMagic = 0;
	FILE* pf = NULL;

	//-------------------------------
	//  Check for the help argument
	//-------------------------------
	if (!strcmp(argv[1], "/?") || !strcmp(argv[1], "-?"))
		usage();

	//--------------------------------------------------
	//  Check whether we already have user information
	//--------------------------------------------------
	pf = fopen(argv[1], "rt");
	if (pf != NULL)
	{
		printf("Your name is: ");
		fgets(szName, sizeof(szName), pf);
		encrypt(szName);
		printf(szName);
		fgets(szDay, sizeof(szName), pf);
		encrypt(szDay);
		fgets(szMonth, sizeof(szName), pf);
		encrypt(szMonth);
		fgets(szYear, sizeof(szName), pf);
		encrypt(szYear);
		fgets(szMagic, sizeof(szName), pf);
		encrypt(szMagic);
		sscanf(szMagic, "%hd", &sMagic);
		printf("\nYou were born on %s-%s-%s\n", szYear, szMonth, szDay);
	}
	else
	{
		//-------------------------------
		//  Print the user's name first
		//-------------------------------
		pszName = argv[1];
		printf("Your name is: ");
		printf(pszName);
		printf("\n");

		//--------------------------------
		//  Get the user's birth details
		//--------------------------------
		printf("Enter your day of birth: ");
		scanf("%s", szDay);
		printf("Enter your month of birth: ");
		scanf("%s", szMonth);
		printf("Enter your year of birth: ");
		scanf("%s", szYear);

		//--------------------------------------------
		//  Combine all 3 strings into one, securely
		//--------------------------------------------
		_snprintf(szComb, sizeof(szComb), "%s%s%s", szDay, szMonth, szYear);

		//------------------------------------------
		//  Calculate and print the "magic" number
		//------------------------------------------
		sMagic = CalcMagicStr(szComb);

		//----------------------------------------
		//  Write all encrypted data into a file
		//----------------------------------------
		pf = fopen(argv[1], "wt");
		encrypt(argv[1]);
		encrypt(szDay);
		encrypt(szMonth);
		encrypt(szYear);
		sprintf(szMagic, "%d", sMagic);
		encrypt(szMagic);
		fprintf(pf, "%s\n%s\n%s\n%s\n%d", argv[1], szDay, szMonth, szYear, sMagic);
		fclose(pf);
	}
	
	printf("Your magic number is: %d\n", sMagic);

	return EXIT_SUCCESS;
}
