/*******************************************************************************
 * Asfaleia - Askisi 2 - Encryption / Decryption Ciphers                       *
 * C Source File - main.c                                                      *
 * AuthorsL                                                                    *
 *   Karoutsou Maria                                                           *
 *   Latsas Anastasios                                                         *
 *******************************************************************************/

//------------------------------------------------------------------------------
// Include Files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "cyphers.h"

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
#define MAX_INPUT_LEN		20
#define VIGENERE_MAX_KEY	15
#define BEAUFORD_MAX_KEY	15

//------------------------------------------------------------------------------
// Main program function prototypes
//------------------------------------------------------------------------------
int MenuSelection		();
int GetInputText		(char text[], int size, int* letters);
int DisplayText			(const char text[], int letters);
int TextToUpper			(char text[], int letters);
int StripSpaces			(char text[], int* letters);
int PurgeText			(char text[], int* letters);
int Display2dCharArray	(char** array, int row, int column);

//------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------
int main( int argc, char* argv[])
{

	char text[MAX_INPUT_LEN];
	int  letters = 0, i = 0;

	switch (MenuSelection())
	{
		case 1:    // Caesar
			{
				int c_key = 0;

				// get message
				printf("Waiting input text (%d chars max) > ", MAX_INPUT_LEN);
				GetInputText(text, MAX_INPUT_LEN, &letters);

				// format it, algorithms work only with upper letters
				printf("-> Re-Formatting text. Please wait...\n");
				TextToUpper(text, letters);
				printf("New text : ");
				DisplayText(text, letters);

				// get key
				printf("Waiting encryption key > ");
				c_key = CaesarGetKey();
				printf("-> Using Caesar Cipher with key : '%d'\n", c_key);

				// encrypt
				printf("-> Encrypting text. Please Wait...\n");

				CaesarEncrypt(text, letters, c_key);

				printf("Encrypted text : ");
				DisplayText(text, letters);

				// decrypt, we need enc msg and key
				printf("-> Decrypting text. Please Wait...\n");

				CaesarDecrypt(text, letters, c_key);

				printf("Original text : ");
				DisplayText(text, letters);

				printf("Quiting...bye...\n");
				break;
			}

		case 2: // Affine
			{
				int afkey_1, afkey_2;

				printf("Waiting input text (%d chars max) > ", MAX_INPUT_LEN);
				GetInputText(text, MAX_INPUT_LEN, &letters);
				// format it, algorithms work only with upper letters
				printf("-> Re-Formatting text. Please wait...\n");
				TextToUpper(text, letters);

				printf("New text : ");
				DisplayText(text, letters);

				// get key
				printf("Waiting encryption key...\nKey1 must be relatively prime to 26 and key2 [0-25]\n");
				AffineGetKey(&afkey_1, &afkey_2);

				printf("-> Using Affine Cipher with key1 : '%d' and key2 : '%d'\n", afkey_1, afkey_2);
				printf("-> Encrypting text. Please Wait...\n");

				AffineEncrypt(text, letters, afkey_1, afkey_2);

				printf("Encrypted text : ");
				DisplayText(text, letters);

				printf("-> Decrypting text. Please Wait...\n");
				AffineDecrypt(text, letters, afkey_1, afkey_2);

				printf("Original text : ");
				DisplayText(text, letters);

				printf("Quiting...bye...\n");
				break;
			}

		case 3: // Monoalphabetic Substitution
			{
				char* mono_key 		= NULL;
				char* mono_encTable = NULL;
				int	  mono_keysize  = 0;

				// get message
				printf("Waiting input text (%d chars max) > ", MAX_INPUT_LEN);
				GetInputText(text, MAX_INPUT_LEN, &letters);

				// format it, algorithms work only with upper letters
				printf("-> Re-Formatting text. Please wait...\n");
				TextToUpper(text, letters);
				printf("New text : ");
				DisplayText(text, letters);
				// get key
				printf("Waiting encryption key (26 chars max) > ");
				mono_key = MonoalphabeticGetKey(&mono_keysize);
				
				StripSpaces(mono_key, &mono_keysize);
				TextToUpper(mono_key, mono_keysize);

				printf("-> Using Monoalphabetic Substitution Cipher with key : ");
				for (i = 0; i < mono_keysize; i++) { printf("%c", mono_key[i]); }
				printf("\n");

				printf("-> Calculating encryption table. Please wait...\n");
				mono_encTable = MonoalphabeticCreateEncTable(mono_key, mono_keysize);

				printf("-> Encryption table created successfully : \n");
				for (i = 0; i < 26; i++) { printf("%c ", mono_encTable[i]); }
				printf("\n");

				// encrypt
				printf("-> Encrypting text. Please Wait...\n");

				MonoalphabeticEncrypt(text, letters, mono_encTable);

				printf("Encrypted text : ");
				DisplayText(text, letters);

				// decrypt
				printf("-> Decrypting text. Please Wait...\n");

				MonoalphabeticDecrypt(text, letters, mono_encTable);

				printf("Original text : ");
				DisplayText(text, letters);
				// do some cleanup here
				printf("-> Cleaning the mesh...\n");
				MonoalphabeticCleanUp(mono_encTable);
				MonoalphabeticCleanUp(mono_key);

				printf("Quiting...bye...\n");
				break;
			}

		case 4: // Transposition
			{
				int    key_columns  =  0;
				int*   key_seq      =  NULL;
				char** enc_table    =  NULL;
				int    encTableRows =  0;
				
				// get message
				printf("Waiting input text (%d chars max) > ", MAX_INPUT_LEN);
				GetInputText(text, MAX_INPUT_LEN, &letters);

				// reformat, delete space chars and turn letters to upper
				printf("-> Re-Formatting text. Please wait...\n");

				StripSpaces(text, &letters);
				TextToUpper(text, letters);

				printf("New text : ");
				DisplayText(text, letters);

				// get key (d , π)
				printf("* For encryption keys enter the number of colums you want to use at your\n");
				printf("encryption table [2-%d) and then the sequence the columns will be used *\n", letters);
				printf("Columns > ");

				TranspositionGetColumns(letters, &key_columns);

				printf("Sequence [1-%d] > ", key_columns);

				key_seq = TranspositionGetSequence(key_columns);

				printf("-> Using Transposition Cipher with key (d, π) : ");
				printf("d = %d π = ( ");
				for (i = 0; i < key_columns; ++i)
					printf("%d ", key_seq[i]);
				printf(")\n");

				// create encryption table based on message
				printf("-> Calculating encryption table. Please wait...\n");
				enc_table = TranspositionCreateTable(letters, key_columns, &encTableRows);

				// fill table with message for encryption and display it
				TranspositionFillTable(text, letters, enc_table, key_columns, encTableRows);

				printf("-> Encryption table created successfully : \n");
				Display2dCharArray(enc_table, encTableRows, key_columns);

				// encrypt based on encryption table and sequence key (π)
				printf("-> Encrypting text. Please Wait...\n");

				TranspositionEncrypt(text, enc_table, key_columns, encTableRows, key_seq);

				printf("Encrypted text : ");
				DisplayText(text, letters);

				// do some cleanup here
				TranspositionTableCleanUp(enc_table, encTableRows);

				// encryption table needed to reverce procedure
				// we re-create it to show we don't need the previous one for decryption
				enc_table = TranspositionCreateTable(letters, key_columns, &encTableRows);

				// decrypt, we need only the msg & the key. also we use a 2D array to place decrypted message
				// before we pass it to the initial table
				printf("-> Decrypting text. Please Wait...\n");

				TranspositionDecrypt(text, letters, key_columns, encTableRows, key_seq, enc_table);

				printf("Original text : ");
				DisplayText(text, letters);

				// do some final cleanup here
				printf("-> Cleaning the mesh...\n");
				TranspositionKeyCleanUp(key_seq);
				TranspositionTableCleanUp(enc_table, encTableRows);
				printf("Quiting...bye...\n");
				break;
			}

		case 5:
			{
				// get message
				printf("Waiting input text (%d chars max) > ", MAX_INPUT_LEN);
				GetInputText(text, MAX_INPUT_LEN, &letters);

				printf("-> Re-Formatting text. Please wait...\n");
				TextToUpper(text, letters);
				PurgeText(text, &letters);

				printf("New text : ");
				DisplayText(text, letters);

				printf("-> Loading Homophonic Table. Please wait...\n");
				printf("Homophonic Table : \n");
				HomophonicDisp();

				printf("-> Encrypting text. Please Wait...\n");
				HomophonicEncrypt(text, letters);

				printf("Encrypted text : ");
				for ( i =0; i< letters; i++) printf("%d", text[i]);
				printf("\n");

				printf("-> Decrypting text. Please Wait...\n");
				HomophonicDecrypt(text, letters);

				printf("Original text : ");
				DisplayText(text, letters);

				printf("Quiting...bye...\n");
				break;
			}

		case 6: // Vigenere
			{
				char* v_key     = NULL;
				int   v_keysize = 0;

				// get message
				printf("Waiting input text (%d chars max) > ", MAX_INPUT_LEN);
				GetInputText(text, MAX_INPUT_LEN, &letters);

				// reformat, delete space chars and turn letters to upper
				printf("-> Re-Formatting text. Please wait...\n");
				TextToUpper(text, letters);
				PurgeText(text, &letters);

				printf("New text : ");
				DisplayText(text, letters);

				printf("Waiting encryption key (%d chars max) > ", VIGENERE_MAX_KEY);
				v_key = VigenereGetKey(&v_keysize, VIGENERE_MAX_KEY);
				TextToUpper(v_key, v_keysize);

				printf("-> Using Vigenere Cipher with key : ");
				for (i = 0; i < v_keysize; i++) { printf("%c", v_key[i]); }
				printf("\n");

				printf("-> Encrypting text. Please Wait...\n");

				VigenereEncrypt(text, &letters, v_key, v_keysize);

				printf("Encrypted text : ");
				DisplayText(text, letters);

				printf("-> Decrypting text. Please Wait...\n");

				VigenereDecrypt(text, &letters, v_key, v_keysize);

				printf("Original text : ");
				DisplayText(text, letters);

				// do some cleanup here
				printf("-> Cleaning the mesh...\n");
				VigenereCleanUp (v_key);
				printf("Quiting...bye...\n");
				break;
			}

		case 7: // Beauford
			{
				char* b_key     = NULL;
				int   b_keysize = 0;

				// get message
				printf("Waiting input text (%d chars max) > ", MAX_INPUT_LEN);
				GetInputText(text, MAX_INPUT_LEN, &letters);

				// reformat, delete space chars and turn letters to upper
				printf("-> Re-Formatting text. Please wait...\n");
				TextToUpper(text, letters);
				PurgeText(text, &letters);

				printf("New text : ");
				DisplayText(text, letters);

				printf("Waiting encryption key (%d chars max) > ", BEAUFORD_MAX_KEY);
				b_key = BeaufordGetKey(&b_keysize, BEAUFORD_MAX_KEY);
				TextToUpper(b_key, b_keysize);

				printf("-> Using Beauford Cipher with key : ");
				for (i = 0; i < b_keysize; i++) { printf("%c", b_key[i]); }
				printf("\n");

				printf("-> Encrypting text. Please Wait...\n");

				BeaufordEncrypt(text, &letters, b_key, b_keysize);

				printf("Encrypted text : ");
				DisplayText(text, letters);

				printf("-> Decrypting text. Please Wait...\n");

				BeaufordDecrypt(text, &letters, b_key, b_keysize);

				printf("Original text : ");
				DisplayText(text, letters);

				// do some cleanup here
				printf("-> Cleaning the mesh...\n");
				BeaufordCleanUp(b_key);
				printf("Quiting...bye...\n");
				break;
			}

		case 8: // Quit
			{
				return 0;
			}
	}
	return 0;

}

//------------------------------------------------------------------------------
// Function Source
//------------------------------------------------------------------------------
int MenuSelection()
// Displays main menu, returns user's selection
{                                                     
	char ch;

	system("clear");
	printf("\t\t+-------- Select Encryption Method --------+\n");
	printf("\t\t|                                          |\n");
	printf("\t\t|     1 --> Caesar                         |\n");
	printf("\t\t|     2 --> Affine                         |\n");
	printf("\t\t|     3 --> Monoalphabetic Substitution    |\n");
	printf("\t\t|     4 --> Transposition                  |\n");
	printf("\t\t|     5 --> Homophonic Substitution        |\n");
	printf("\t\t|     6 --> Vigenere                       |\n");
	printf("\t\t|     7 --> Beauford                       |\n");
	printf("\t\t|     8 --> Quit Program                   |\n");
	printf("\t\t|                                          |\n");
	printf("\t\t+------------------------------------------/\n");
	
	do
	{
		printf("\nSelect > ");
		ch = getchar();
	} while (ch < '1' || ch > '8');
	getchar();
    
	return (ch - '0');

}

//------------------------------------------------------------------------------

int GetInputText(char text[], int size, int* letters)
{
	char ch;
	int  position = -1;

	// store input char in ch and if not new line enter loop
	while ((ch = getchar()) != '\n')
	{
		position++;
		text[position] = ch;
		// if last char just stored, exit loop
		if (position == size - 1)
			break;
	}
	// adjust letter count
	*letters = position + 1;
	return 0;
}

//------------------------------------------------------------------------------

int DisplayText(const char text[], int letters)
{
	int i;

	for (i = 0; i < letters; i++)
		printf("%c", text[i]);
	printf("\n");

	return 0;
}

//------------------------------------------------------------------------------

int TextToUpper(char text[], int letters)
{
	int i =0;

	for (i = 0; i < letters; i++)
		if (isalpha(text[i]))
			text[i] = toupper(text[i]);

	return 0;
}

//------------------------------------------------------------------------------

int StripSpaces(char text[], int* letters)
{
	// create an identical to text[] array
	char* newText = malloc((*letters) * sizeof(char));
	int  newLetters = -1;
	
	int i;
	for (i = 0; i < *letters; i++)
		if (text[i] != ' ')
		{
			newLetters++;
			newText[newLetters] = text[i];
		}

	*letters = newLetters + 1;
	
	for (i = 0; i < *letters; i++)
		text[i] = newText[i];

	free(newText);
	return 0;
}

//------------------------------------------------------------------------------

int PurgeText(char text[], int* letters)
{
	// create an identical to text[] array
	char* newText = malloc((*letters) * sizeof(char));
	int  newLetters = -1;
	
	int i;
	for (i = 0; i < *letters; i++)
		if (text[i] >= 'A' && text[i] <= 'Z')
		{
			newLetters++;
			newText[newLetters] = text[i];
		}

	*letters = newLetters + 1;
	
	for (i = 0; i < *letters; i++)
		text[i] = newText[i];

	free(newText);
	return 0;
}

//------------------------------------------------------------------------------

int Display2dCharArray(char** array, int row, int column)
{
	int i, j;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < column; j++)
			printf("%c\t", array[i][j]);
		printf("\n");
	}
	return 0;
}
