/*******************************************************************************
 * Asfaleia - Askisi 2 - Encryption / Decryption Ciphers                       *
 * C Source File - cyphers.c                                                   *
 * AuthorsL                                                                    *
 *      Karoutsou Maria                                                        *
 *      Latsas Anastasios                                                      *
 *******************************************************************************
 *																			   *
 * This program is free software; you can redistribute it and/or modify        *
 * it under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.										   *
 * 																			   *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License           *
 * along with this program; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.              *
 *                                                                             *
 *******************************************************************************/

//------------------------------------------------------------------------------
// Include Files
//------------------------------------------------------------------------------
#include "cyphers.h"

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------

	// global alphabet array, 26 [0-25] letters : A - Z
	const char alpha[ ] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M',
							'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'
				     	  };
	// global array with homophonic letters - 6x6
	// 1st column is character Ascii value, 100 is invalid value
	const unsigned char homLetter[6][6] = {
											{ 69, 17,  19,  23,  47,  64 }, // E
					    					{ 65,  8,  20,  25,  49, 100 }, // A
					    					{ 82,  1,  29,  65, 100, 100 }, // R
					    					{ 84, 16,  31,  85, 100, 100 }, // T
					    					{ 73, 32,  70,  73, 100, 100 }, // I
					    					{ 79,  0,   5,   7,  55, 100 }  // O
					  					  };

//------------------------------------------------------------------------------
// General Math Related Cipher Functions
//------------------------------------------------------------------------------
int gcd(int a, int b)
// finds and returns the Greatest Common Divisor
{
	int i;

	while (b != 0)
	{
		i = a % b;
		a = b;
		b = i;
	}
	return(a);
}

// finds the inverse of a number
int Inverse(int a)
{
	int x = 1;

	while ((a * x - 1) % 26 != 0) { x++; }
	
	return (x);
}

//------------------------------------------------------------------------------
// Caesar Cipher Functions
//------------------------------------------------------------------------------
int CaesarGetKey()
{
	int key = 0;

	scanf("%d", &key);
		
	return (key % 26);
}

int CaesarEncrypt(char text[], int letters, int key)
{
	int i, ascii, letterIndex, encLetterIndex;

	for (i = 0; i < letters; i++)
	{
		// find tha Ascii number of the letter
		ascii = (int)text[i];
		// if valid char (A-Z) then encrypt
		if ((ascii >= 'A') && (ascii <= 'Z'))
		{
			// find the index pointing to this letter in our ascii table
			// Ascii 'A' = 65 decimal
			letterIndex = ascii - 'A';

			// find the index pointing to the encrypted letter in our ascii table
			encLetterIndex = letterIndex + key;

			// if encLetterIndex exceeds table limits, adjust it
			if (encLetterIndex > 25)
				encLetterIndex = encLetterIndex - 26;

			// Encrypt
			text[i] = alpha[encLetterIndex];
		}
	}
	return 0;
}

int CaesarDecrypt(char text[], int letters, int key)
{
	int i, ascii, letterIndex, decLetterIndex;

	for (i = 0; i < letters; i++)
	{
		// find tha Ascii number of the letter
		ascii = (int)text[i];
		// if valid char (A-Z), decrypt
		if ((ascii >= 'A') && (ascii <= 'Z'))
		{
			// find the index pointing to this letter in our ascii table
			// Ascii 'A' -> 65 decimal
			letterIndex = ascii - 'A';

			// find the index pointing to the original (decrypted) letter in our ascii table
			decLetterIndex = letterIndex - key;

			if (decLetterIndex < 0)
				decLetterIndex = decLetterIndex + 26;

			// Decrypt
			text[i] = alpha[decLetterIndex];
		}
	}

	return 0;
}

//------------------------------------------------------------------------------
// Affine Cipher Functions
//------------------------------------------------------------------------------
int AffineGetKey(int* key1, int* key2)
{
	int a, b;
	a = b = 0;

	// get 1st key
	printf("Key 1 > ");
	scanf("%d", &a);
	
	// key must be relatively prime to 26
	while (gcd(a, 26) != 1)
	{
		printf("NOT relatively prime!Try again..\n> ");
		scanf("%d", &a);
	}
	//get 2nd key
	printf("Key 2 > ");
	scanf("%d", &b);
	
	*key1 = a;
	// if key > 26 adjust it!
	*key2 = b % 26;
	
	return 0;
}

int AffineEncrypt(char text[], int letters, int key1, int key2)
{
	int  i, ascii, letterIndex, encLetterIndex;

	for (i = 0; i < letters; i++)
	{
		// find tha Ascii number of the letter
		ascii = text[i];
		// if valid char (A-Z) then encrypt
		if ((ascii >= 'A') && (ascii <= 'Z'))
		{
			// find the index pointing to this letter in our ascii table
			// Ascii 'A' = 65 decimal
			letterIndex = ascii - 'A';

			// find the index pointing to the encrypted letter in our ascii table
			encLetterIndex = (key1 * letterIndex + key2) % 26;
			text[i] = alpha[encLetterIndex];
		}
	}
	return 0;
}

int AffineDecrypt(char text[], int letters, int key1, int key2)
{
	int invKey1, decKey2;
	
	// find decryption keys..(key1^-1, 26 - ((key1^-1 * key2) Mod 26))
	invKey1 = Inverse(key1);
	decKey2 = 26 - ((invKey1 * key2) % 26);

	AffineEncrypt(text, letters, invKey1, decKey2);
	
	return 0;
}

//------------------------------------------------------------------------------
// Monoalphabetic Substitution Cipher Functions
//------------------------------------------------------------------------------
char* MonoalphabeticGetKey(int* keysize)
{
	// create a string with max length 26 chars
	char* key = malloc(26 * sizeof(char));
	
	char ch;
	int  position = -1;

	// store input char in ch and if not new line enter loop
	while ((ch = getchar()) != '\n')
	{
		if ( (ch >= 'A' && ch <= 'Z') ||
			 (ch >= 'a' && ch <= 'z')
		   )
		{
			position++;
			key[position] = ch;
			// if last char just stored, exit loop
			if (position == 25)
				break;
		}
	}
	// adjust letter count
	*keysize = position + 1;
	
	int   i, j, duplicate;
	char* tmpKey = malloc(26 * sizeof(char));

	position  = -1;
	duplicate = 0;

	// counts letters in key string
	for (i = 0; i < *keysize; i++)
	{
		// checks previously assigned letters in tmp key
		for (j = position; j >= 0; j--)
			if (key[i] == tmpKey[j])
			{
				duplicate = 1;
				break;
			}
		// if key is not in key, store
		if (!duplicate)
		{
			position++;
			tmpKey[position] = key[i];
		}
		duplicate = 0;
	}
	*keysize = position + 1;

	// copy new string to old
	for (i = 0; i < *keysize; i++)
		key[i] = tmpKey[i];

	free(tmpKey);

	return (key);
}

char* MonoalphabeticCreateEncTable(char* key, int keysize)
{
	// 2D encryption table columns = keysize
	// calculate rows
	int rows;
	if (26 % keysize != 0)
		rows = (26 / keysize) + 1;
	else
		rows = 26 / keysize;

	char** twoDEncTable = malloc(rows * sizeof(char*));

	if (!twoDEncTable)
		exit(1);

	// r - 2D table rows counter
	int r;
	for (r = 0; r < rows; r++)
	{
		twoDEncTable[r] = malloc(keysize * sizeof(char));
		if (!twoDEncTable)
			exit(1);
	}

	// c - columns counter
	int c;
	// pass key in the 1st row of 2D encryption table
	for (c = 0; c < keysize; c++)
		twoDEncTable[0][c] = key[c];

	int a, i, duplicate;
	a = i = duplicate = 0;

	// r counts rows and c counts columns
	for (r = 1; r < rows; r++)
		for ( c = 0; c < keysize; c++)
		{
			// check if letter is in the key
			for (i = 0; i < keysize; i++)
				// if is in key then it is also in the array so is a duplicate
				if (alpha[a] == key[i])
				{
					duplicate = 1;
					break;
				}
			// a counts letters in alpha table
			if (a < 26)
			{
				// if not duplicate then store else roll-back
				if (!duplicate)
				{
					twoDEncTable[r][c] = alpha[a];
					a++;
				}
				else
				{
					a++;
					c--;
				}
			}
			else
				// empty space
				twoDEncTable[r][c] = '*';
			duplicate = 0;
		}

#if    DEBUG
	// show encryption table in 2D mode
	for (r = 0; r < rows; r++)
	{
		for ( c = 0; c < keysize; c++)
			printf("%c\t", twoDEncTable[r][c]);
		printf("\n");
	}

#endif

	char* enc_table = malloc(26 * sizeof(char));

	// transfer 2D table in 1D row by row
	i = 0;
	for (c = 0; c < keysize; c++)
		for ( r = 0; r < rows; r++)
			if ((i < 26) && (twoDEncTable[r][c] != '*'))
			{
				enc_table[i] = twoDEncTable[r][c];
				i++;
			}

	// free dyn array
	for (i = 0; i < rows; i++)
		free(twoDEncTable[i]);
	free(twoDEncTable);

	return (enc_table);
}

int MonoalphabeticEncrypt(char text[], int letters, char* enc_table)
{
	int i, ascii, letterIndex;
	for (i = 0; i < letters; i++)
	{
		ascii = (int)text[i];
		// if valid char (A-Z), encrypt
		if ((ascii >= 'A') && (ascii <= 'Z'))
		{
			// find the index pointing to this letter in our ascii table
			// Ascii 'A' -> 65 decimal
			letterIndex = ascii - 'A';
			text[i] = enc_table[letterIndex];
		}
	}

	return 0;
}

int MonoalphabeticDecrypt(char text[], int letters, char* enc_table)
{
	int i, j, letterIndex = 0;
	for (i = 0; i < letters; i++)
	{
		// if valid char (A-Z), encrypt
		if ((text[i] >= 'A') && (text[i] <= 'Z'))
		{
			for (j = 0; j < 26; j++)
				if (text[i] == enc_table[j])
					letterIndex = j;
			text[i] = alpha[letterIndex];
		}
	}

	return 0;
}

void MonoalphabeticCleanUp(char* ptr)
{
	free(ptr);
}

//------------------------------------------------------------------------------
// Transposition Cipher Functions
//------------------------------------------------------------------------------
int TranspositionGetColumns(int letters, int* key_columns)
{
	int tmp = 0;
	// get number of rows
	do
	{
		scanf("%d", &tmp);
	} while (tmp < 1 || tmp >= letters);
	// assign value to reference variable
	*key_columns = tmp;
	return 0;
}

int* TranspositionGetSequence(int key_columns)
{
	// dynamicly create array which will hold the row sequence
	// array type : int with size the # of user defined rows
	int* key_seq = malloc((key_columns) * sizeof(int));
	
	// if not enough memory exit with error code
	if (! key_seq)
		exit(1);

	int i, j;
	// get row sequence
	for (i = 0; i < key_columns; ++i)
	{
		scanf("%d", &key_seq[i]);
		// duplicate guard loop
		// prompts for value re-enter on duplicate entries
		for (j = i-1; j >= 0; --j)
			if (key_seq[i] == key_seq[j])
			{
				printf("Double entry, please re-enter!\n");
				--i; break;
			}
	}
	// check for bad input
	for (i = 0; i < key_columns; ++i)
		if (key_seq[i] < 1 || key_seq[i] > key_columns)
		{
			printf("Row sequence out of bounds...quiting\n");
			exit(1);
		}

	// return the address of allocated array
	return (key_seq);
}

char** TranspositionCreateTable(int letters, int key_columns, int* rows)
{
	// calculate rows for the encryption table based on # of
	// text letters and user defined columns number
	if ((letters % key_columns) != 0)
		*rows = (letters / key_columns) + 1;
	else
		*rows = letters / key_columns;

	// allocate array of char arrays
	char** enc_table = malloc((*rows) * sizeof(char*));

	// if not enough memory exit with error code
	if (! enc_table)
		exit(1);

	int i;
	// build 2D array
	for (i = 0; i < *rows; i++)
	{
		enc_table[i] = malloc(key_columns * sizeof(char));
		// if not enough memory exit with error code
		if (! enc_table[i])
			exit(1);
	}
	// return 2D array address
	return (enc_table);
}

int TranspositionFillTable(const char text[], int letters, char** enc_table, int key_columns, int rows)
{
	int i, j, txtletter;

	int letter_counter = 0;

	for (i = 0; i < rows; i++)
		for (j = 0; j < key_columns; j++)
			if (letter_counter < letters)
			{
				enc_table[i][j] = text[letter_counter];
				letter_counter++;
			}
			else
				enc_table[i][j] = ' ';

	return 0;
}

int TranspositionEncrypt(char text[], char** enc_table, int key_columns, int rows, int* key_seq)
{
	int d, i, j, indx = -1;

	// for #1 counts columns
	for (d = 0; d < key_columns; d++)
		// for #2 counts rows in specified column
		for (i = 0; i < rows; i++)
		{
			// if not space put in text[]
			// column is calculated by using the sequence key - 1 to adjust in C-like arrays
			j = key_seq[d] - 1;
			if (enc_table[i][j] != ' ')
			{
				indx++;
				text[indx] = enc_table[i][j];
			}
		}
	return 0;
}

int TranspositionDecrypt(char text[], int letters, int columns, int rows, int* key_seq, char** enc_table)
{
	// calculate the # of spaces
	int nSpaces = (columns * rows) - letters;
	
	int d, i, j, indx = -1;

	// this for counts columns
	for (d = 0; d < columns; d++)
		// and this one rows inside a column
		for (i = 0; i < rows; i++)
		{
			// if in this column originaly was a space then
			if ((key_seq[d] - 1) >= (columns - nSpaces))
				// if is the last row, exit rows loop and do not get from encrypted msg
				// leave blank space in 2D array
				if (i == rows - 1)
					break;
			j = key_seq[d] - 1;
			indx++;
			enc_table[i][j] = text[indx];
		}
	
	int letter_counter = 0;
	// transfer decrypted message from 2D array to msg string
	for (i = 0; i < rows; i++)
		for (j = 0; j < columns; j++)
			if (letter_counter < letters)
			{
				text[letter_counter] = enc_table[i][j];
				letter_counter++;
			}

		return 0;
}

void TranspositionKeyCleanUp(int* seq_key)
{
	free(seq_key);
}

void TranspositionTableCleanUp(char** enc_table, int rows)
{
	int i;
	for (i = 0; i < rows; i++)
		free(enc_table[i]);
	free(enc_table);
}

//------------------------------------------------------------------------------
// Homophonic Substitution Cipher Functions
//------------------------------------------------------------------------------
void HomophonicDisp()
// Displays the Homophonic Letters Table
{
	int i, j;
	for (i = 0; i < 6; i++)
	{
		for ( j = 0; j < 6; j++)
			if (j == 0)
				printf("%c\t", homLetter[i][j]);
			else if (homLetter[i][j] == 100)
				printf(" \t");
			else
				printf("%d\t", homLetter[i][j]);		
		printf("\n");
	}
}

int  HomophonicEncrypt(char text[], int letters)
{
	// homophonic letters counters
	int cE, cA, cR, cT, cI, cO;
	cE = cA = cR = cT = cI = cO = 0;

	int i;
	for (i = 0; i < letters; i++)
		switch(text[i])
		{
			case 'E':
			{
				cE++;
				if (homLetter[0][cE] == 100 || cE >= 6) { cE = 1; }
				text[i] = homLetter[0][cE];
				break;
			}
			case 'A':
			{
				cA++;
				if (homLetter[1][cA] == 100) { cA = 1; }
				text[i] = homLetter[1][cA];
				break;
			}
			case 'R':
			{
				cR++;
				if (homLetter[2][cR] == 100) { cR = 1; }
				text[i] = homLetter[2][cR];
				break;
			}
			case 'T':
			{
				cT++;
				if (homLetter[3][cT] == 100) { cT = 1; }
				text[i] = homLetter[3][cT];
				break;
			}
			case 'I':
			{
				cI++;
				if (homLetter[4][cI] == 100) { cI = 1; }
				text[i] = homLetter[4][cI];
				break;
			}
			case 'O':
			{
				cO++;
				if (homLetter[5][cO] == 100) { cO = 1; }
				text[i] = homLetter[5][cO];
				break;
			}
			default :
				text[i] = (text[i] - 'A') * 3;
		}
	return 0;
}

int  HomophonicDecrypt(char text[], int letters)
{
	int i, found, r, c;

	// for each letter
	for (i = 0; i < letters; i++)
	{
		r = 0; c = 1; found = 0;
		// while rows < 6 and not found
		while (r < 6 && !found)
		{
			// if found, letter is first entry in homophonic table
			if (text[i] == homLetter[r][c])
			{
				text[i] = homLetter[r][0];
				found = 1;
			}
			else
			{
				// increase columns and if last column increase rows
				c++;
				if (c >= 6) { c = 1; r++; }
			}
		}
		// if not found then letter is not homophonic, division gives letter
		if (!found)
			text[i] = alpha[text[i] / 3];
	}
	return 0;
}

//------------------------------------------------------------------------------
// Vigenere Cipher Functions
//------------------------------------------------------------------------------
char* VigenereGetKey(int* keysize, int maxKeySize)
{
	// create a key string
	char* key = malloc(maxKeySize * sizeof(char));
	
	char ch;
	int  position = -1;

	// store input char in ch and if not new line enter loop
	while ((ch = getchar()) != '\n')
	{
		if ( (ch >= 'A' && ch <= 'Z') ||
			 (ch >= 'a' && ch <= 'z')
		   )
		{
			position++;
			key[position] = ch;
			// if last char just stored, exit loop
			if (position == maxKeySize - 1)
				break;
		}
	}
	// adjust letter count
	*keysize = position + 1;

	return (key);
}

int VigenereEncrypt(char text[], int* letters, char* key, int keysize)
{
	int i, keyPos, origLetter, encLetter, encIndex;

	char* newText = malloc(20 * sizeof(char));

	keyPos = 0;

	// for each letter find the encryption letter using addition
	for (i = 0; i < *letters; i++)
	{
		origLetter = text[i] 	 - 'A'; // original letter
		encLetter  = key[keyPos] - 'A'; // letter in key string

		// make adjustments
		encIndex = origLetter + encLetter;
		if (encIndex > 25)
			encIndex = encIndex - 26;

		// store encrypted letter
		newText[i] = alpha[encIndex];

		// index inside key string
		keyPos++;
		if (keyPos == keysize)
			keyPos = 0;
	}

	// transfer encrypted text in original's table
	int newLetters = -1;
	keyPos = 0;
	for (i = 0; i < *letters; i++)
	{
		newLetters++;
		text[newLetters] = newText[i];
		keyPos++;
		// if keysize lingth reached add a blank space
		if (keyPos == keysize)
		{
			newLetters++;
			text[newLetters] = ' ';
			keyPos = 0;
		}
	}
	// adjust new letters including spaces
	*letters = newLetters + 1;
	//free tmp string
	free(newText);

	return 0;
}

int VigenereDecrypt(char text[], int* letters, char* key, int keysize)
{
	int i, keyPos, txtLetter, decLetter, decIndex, skips;

	keyPos = skips = 0;

	for (i = 0; i < *letters; i++)
	{
		// if not space, decrypt
		if (text[i] != ' ')
		{
			txtLetter = text[i]     - 'A'; // enc letter in text
			decLetter = key[keyPos] - 'A'; // letter inside key string, used for decryption

			// calculate decryption key using letter substitution
			decIndex = txtLetter - decLetter;
			// make adjustments
			if (decIndex < 0)
				decIndex = decIndex + 26;

			// decrypt --if any blank spaces detected then index is "letter-in-string-with-spaces" - "spaces"
			// with that mechanism we skip the blank spaces without having to use extra strings
			// and then tranfering it back to original
			text[i - skips] = alpha[decIndex];

			// index inside key string
			keyPos++;
			if (keyPos == keysize)
				keyPos = 0;
		}
		else
		// counts blank spaces generated at encryption time based on keysize
			skips++;
	}
	// adjust letters
	*letters = *letters - skips;

	return 0;
}

void  VigenereCleanUp (char* key)
{
	free(key);
}

//------------------------------------------------------------------------------
// Beauford Cipher Functions
//------------------------------------------------------------------------------
char* BeaufordGetKey(int* keysize, int maxKeySize)
{
	// create a key string
	char* key = malloc(maxKeySize * sizeof(char));
	
	char ch;
	int  position = -1;

	// store input char in ch and if not new line enter loop
	while ((ch = getchar()) != '\n')
	{
		if ( (ch >= 'A' && ch <= 'Z') ||
			 (ch >= 'a' && ch <= 'z')
		   )
		{
			position++;
			key[position] = ch;
			// if last char just stored, exit loop
			if (position == maxKeySize - 1)
				break;
		}
	}
	// adjust letter count
	*keysize = position + 1;

	return (key);
}

int BeaufordEncrypt(char text[], int* letters, char* key, int keysize)
{
	int i, keyPos, origLetter, encLetter, encIndex;

	char* newText = malloc(20 * sizeof(char));

	keyPos = 0;

	for (i = 0; i < *letters; i++)
	{
		origLetter = text[i] 	 - 'A';
		encLetter  = key[keyPos] - 'A';

		encIndex = origLetter - encLetter;
		if (encIndex < 0)
			encIndex = encIndex + 26;

		newText[i] = alpha[encIndex];

		keyPos++;
		if (keyPos == keysize)
			keyPos = 0;
	}

	int newLetters = -1;
	keyPos = 0;
	for (i = 0; i < *letters; i++)
	{
		newLetters++;
		text[newLetters] = newText[i];
		keyPos++;
		if (keyPos == keysize)
		{
			newLetters++;
			text[newLetters] = ' ';
			keyPos = 0;
		}
	}
	*letters = newLetters + 1;
	free(newText);

	return 0;
}

int BeaufordDecrypt(char text[], int* letters, char* key, int keysize)
{
	int i, keyPos, txtLetter, decLetter, decIndex, skips;

	keyPos = skips = 0;

	for (i = 0; i < *letters; i++)
	{
		if (text[i] != ' ')
		{
			txtLetter = text[i]     - 'A';
			decLetter = key[keyPos] - 'A';

			decIndex = txtLetter + decLetter;
			if (decIndex > 25)
				decIndex = decIndex - 26;

			text[i - skips] = alpha[decIndex];

			keyPos++;
			if (keyPos == keysize)
				keyPos = 0;
		}
		else
			skips++;
	}
	*letters = *letters - skips;

	return 0;
}

void  BeaufordCleanUp (char* key)
{
	free(key);
}
