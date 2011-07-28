/*******************************************************************************
 * Asfaleia - Askisi 2 - Encryption / Decryption Ciphers                       *
 * C Header File - cyphers.h                                                   *
 * Authors                                                                     *
 *   Karoutsou Maria                                                           *
 *   Latsas Anastasios                                                         *
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

#ifndef CYPHERS_H
#define CYPHERS_H

//------------------------------------------------------------------------------
// Include Files
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
#define DEBUG	0

//------------------------------------------------------------------------------
// General Math Related Cipher Function Prototypes
//------------------------------------------------------------------------------
int gcd		(int , int );
int Inverse (int);

//------------------------------------------------------------------------------
// Caesar Cipher Function Prototypes
//------------------------------------------------------------------------------
int CaesarGetKey  ();
int CaesarEncrypt (char text[], int letters, int key);
int CaesarDecrypt (char text[], int letters, int key);

//------------------------------------------------------------------------------
//  Affine Cipher Function Prototypes
//------------------------------------------------------------------------------
int AffineGetKey  (int* key1, int* key2);
int AffineEncrypt (char text[], int letters, int key1, int key2);
int AffineDecrypt (char text[], int letters, int key1, int key2);

//------------------------------------------------------------------------------
// Monoalphabetic Substitution Cipher Function Prototypes
//------------------------------------------------------------------------------
char* MonoalphabeticGetKey         (int* keysize);
char* MonoalphabeticCreateEncTable (char* key, int keysize);
int   MonoalphabeticEncrypt        (char text[], int letters, char* enc_table);
int   MonoalphabeticDecrypt        (char text[], int letters, char* enc_table);
void  MonoalphabeticCleanUp        (char* ptr);

//------------------------------------------------------------------------------
// Transposition Cipher Function Prototypes
//------------------------------------------------------------------------------
int    TranspositionGetColumns	 (int letters, int* key_columns);
int*   TranspositionGetSequence	 (int key_columns);
char** TranspositionCreateTable	 (int letters, int key_columns, int* rows);
int    TranspositionFillTable	 (const char text[], int letters, char** enc_table, int key_columns, int rows);
int    TranspositionEncrypt		 (char text[], char** enc_table, int key_columns, int rows, int* key_seq);
int    TranspositionDecrypt		 (char text[], int letters, int columns, int rows, int* key_seq, char** enc_table);
void   TranspositionKeyCleanUp	 (int* seq_key);
void   TranspositionTableCleanUp (char** enc_table, int rows);

//------------------------------------------------------------------------------
// Homophonic Substitution Cipher Function Prototypes
//------------------------------------------------------------------------------
void HomophonicDisp();
int  HomophonicEncrypt(char text[], int letters);
int  HomophonicDecrypt(char text[], int letters);

//------------------------------------------------------------------------------
// Vigenere Cipher Function Prototypes
//------------------------------------------------------------------------------
char* VigenereGetKey  (int* keysize, int maxKeySize);
int   VigenereEncrypt (char text[], int* letters, char* key, int keysize);
int   VigenereDecrypt (char text[], int* letters, char* key, int keysize);
void  VigenereCleanUp (char* key);

//------------------------------------------------------------------------------
// Beauford Cipher Function Prototypes
//------------------------------------------------------------------------------
char* BeaufordGetKey  (int* keysize, int maxKeySize);
int   BeaufordEncrypt (char text[], int* letters, char* key, int keysize);
int   BeaufordDecrypt (char text[], int* letters, char* key, int keysize);
void  BeaufordCleanUp (char* key);

#endif
