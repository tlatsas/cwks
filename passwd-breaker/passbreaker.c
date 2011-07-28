/***************************************************************************
 * PASSWORD BREAKER PROGRAM    
 ***************************************************************************
 * C Source File - PassBreaker.c
 * (uses shared lib "Crypt()")
 * Compile	:
 *   cc -O2 -fomit-frame-pointer -lcrypt -O PassBreaker.c -o PassBreaker
 *
 * Authors:
 *  Karoutsou Maria 
 *  Latsas Anastasios
 *
 * Usage		:	PassBreaker [arguments]
 *
 * Arguments	:	<shadow file>
 *                  <shadow file> <dictionary file>
 *
 * Description	:
 *	When invoked with only one argument (shadow file)
 *	it tries to brute-force the shadow file passwords
 *
 *	When invoked with two arguments (shadow file + dictionary file)
 *	uses the dictionary to crack the shadow file passwords
 *
 * Examples		:
 *  (Brute force method) $ nice -n 15 ./PassBreaker shadow
 *  (Dictionary method)  $ nice -n 5  ./PassBreaker shadow D8.dic
 *
 * Output       : Results file contains cracked passwords
 *
 */

/***************************************************************************
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 ***************************************************************************
 */

/* LIBS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>
#include <unistd.h>

/* MACROS */
#define		SHA				1		/* SHADOW is 1st argument		*/
#define		DIC				2		/* DICTIONARY is 2nd argument	*/
#define		SZ				'\0'	/* String Zero terminator		*/
#define		MAX_BRUTE_LEN	2		/* max length of guessed passfrase generated */
#define		ASCII_MIN		32		/* the starting search char for brute-force default 32 = SPACE */
#define		ASCII_MAX		126		/* the final search char for brute-force default 126 = '~' */

/* Function Prototypes */
char*	GetUser(const char*);
char*	GetSalt(const char*);
char*	GetPass(const char*);
int		CmpCryptWord(const char* salt, const char* dicWord, const char* pass);
void	DispUsage();
void	AppendHeader(FILE*, const char*);
void	AppendFooter(FILE*, unsigned int, unsigned int);
void	OutputEntry(FILE*, const char*, const char*);
void	StartClock();
void	StopClock();
void	RecordTime(FILE*);

/* Global-scope variables - CPU clock specific */
static clock_t      st_time;	/* start time */
static clock_t      en_time;	/* end time */
static clock_t		df_time;	/* difference */

static struct tms   st_cpu;		/* start cpu times */
static struct tms   en_cpu;		/* stop cpu times */
static struct tms   df_cpu;		/* difference */

/*
 * Main Program
 */
int main(int argc, char* argv[])
{

	char	line[100];	/* stores one line from the shadow file */
	char	dicWord[10];/* stores a possible password read from dictionary */
	char	guess[10];	/* stores a passfrase generated from brute-force */
	char*	username;	/* stores the username, extracted from shadow file */
	char*	salt;		/* stores the salt, extracted from shadow file */
	char*	pass;		/* stores the hashed password, extracted from shadow file*/
	char	ascii;		/* stores cycling ascii chars used for passfrase generation */

	int		len;		/* stores string lengths for general purpose string operations */
	int 	length;		/* stores current length for generated passwords */
	int		pos;		/* used for position checks inside guessed passfrase */

	unsigned int match;			/* used as flag, if we have a match exit loop etc */
	unsigned int matchCounter;  /* counts matches, (passwords cracked) */
	unsigned int update_next;	/* flag used for guessed password ascii cycling */
	unsigned int isfull;		/* flag showing if generated passfrase has reached maximum value */
	unsigned int shadowCounter; /* counts shadow file entries */
	
	double 		 guessCounter;  /* counts generated passfrase */

	/* file pointers for dictionary, shadow & results files */
	FILE* fpDictionary;
	FILE* fpShadow;
	FILE* fpResults;

	/* init some variables here */
	len = matchCounter = shadowCounter = pos = update_next = isfull = guessCounter = 0;

	switch(argc)
	{
		case 2 :/*
				 * 2 arguments, 1=program name 2=shadow file
				 * init Brute Force method on shadow file passwords
				 * 
				 * Brute-Force Method searches a passfrase starting with
				 * length 1 and increases the length until MAX_BRUTE_LEN reached
				 * It uses the [ASCII_MIN, ASCII_MAX] char range [32, 126] is default
				 * e.g. With length 4 we have 95^4 + 95^3 + 95^2 + 95 = 82317120 passfrases
				 */

				/* open shadow file read-only */
				if ((fpShadow = fopen(argv[SHA], "r")) == NULL)
				{
					printf("Cannot open %s for reading. Quiting...\n", argv[SHA]);
					exit(1);
				}
				printf("-> Shadow file opened : %s [ro]\n", argv[SHA]);

				/* append results file (Create if it doesn't exist) */
				if ((fpResults = fopen("Results", "a")) == NULL)
				{
					printf("Cannot open Results file for Writing. Quiting...\n");
					exit(1);
				}
				printf("-> Results file created / appended\n");
				AppendHeader(fpResults, "Brute Force Attack");

				/* get cpu clock instance */
				StartClock();

				/* for each line entry in shadow file... */
				while (fgets(line, 100, fpShadow) != NULL)
				{
					/* count shadow entry */
					shadowCounter++;

					/* remove NL from shadow file line */
					len = strlen(line);
					line[len - 1] = SZ;

					length	= 1;		/* first check for 1char passfrase */
					guess[length]= SZ;	/* terminate string at position 1 and leave position 0 */
					match 	= 0;		/* init match variable here in case a password previously found */

					/* extract information from shadow file line */
					username	= GetUser(line);
					salt		= GetSalt(line);
					pass		= GetPass(line);

					printf("-> Guessing password for username : %s\n", username);

					/* Brute Force main loop - search with char length 1 - MAX_BRUTE_LEN */
					while (length <= MAX_BRUTE_LEN && !match)
					{
						/* init search letter */
						ascii = ASCII_MIN;
						/* cycle ascii letters */
						while (ascii <= ASCII_MAX && !match)
						{
							guess[0] = ascii;

							/* check generated password with password */
							if (CmpCryptWord(salt, guess, pass))
							{
								/* create a record in Results file */
								OutputEntry(fpResults, username, guess);
								matchCounter++;
								match = 1;
								break;
							}
							ascii++;
						}

						if (!match)
						{
							isfull = 1;
							/* check to see if generated password reached final ascii */
							for (pos = 0; pos < length; pos++)
								if (guess[pos] != ASCII_MAX)
									isfull = 0;

							/* if yes then increase password length */
							if (isfull)
							{
								length++;
								/* init newly added position with ASCII_MIN - 1 */
								/* loop below will take care and increase char to starting (ASCII_MIN) */
								guess[length - 1] = ASCII_MIN - 1;
								/* terminate string */
								guess[length] = SZ;
							}

							/* update flag */
							update_next = 0;
							/* process each char in guessed string */
							for (pos = 0; pos < length ; pos++)
							{
								/* if it is not max char but previous char was then... */
								if (guess[pos] != ASCII_MAX && update_next)
								{
									/* increase by 1 */
									guess[pos]++;
									/* don't update next yet because we are not in max ascii */
									/* max ascii this is taken care by the next if */
									update_next = 0;
									/* exit loop */
									break;
								}

								/* if max char reached, start over and update next char */
								if (guess[pos] == ASCII_MAX)
								{
									guess[pos] = ASCII_MIN;
									update_next = 1;
								}

							} /* end in-string manipulation for */
						} /* end no-match if */
					} /* end brute loop */
				} /* end shadow loop */
				/* get another cpu clock instance */
				StopClock();
				/* append footer @ results file */
				AppendFooter(fpResults, matchCounter, shadowCounter);
				/* append elapsed time @ results file and display it @ stdout */
				RecordTime(fpResults);
				fclose(fpResults);
				fclose(fpShadow);
				break;

		case 3 :/*
				 * 3 arguments, 1=program name 2=shadow file
				 * 				3=dictionary file
				 * init dictionary attack method on shadow file passwords
				 */

				/* open dictionary read-only */	
				if ((fpDictionary = fopen(argv[DIC], "r")) == NULL)
				{
					printf("Cannot open %s for reading. Quiting...\n", argv[DIC]);
					exit(1);
				}
				printf("-> Dictionary opened : %s [ro]\n", argv[DIC]);

				/* open shadow file read-only */
				if ((fpShadow = fopen(argv[SHA], "r")) == NULL)
				{
					printf("Cannot open %s for reading. Quiting...\n", argv[SHA]);
					exit(1);
				}
				printf("-> Shadow file opened : %s [ro]\n", argv[SHA]);

				/* append results file (Create if it doesn't exist) */
				if ((fpResults = fopen("Results", "a")) == NULL)
				{
					printf("Cannot open Results file for Writing. Quiting...\n");
					exit(1);
				}
				printf("-> Results file created / appended\n");

				/* record method used @ Results file */
				AppendHeader(fpResults, "Dictionary Attack");
				/* get cpu clock instance */
				StartClock();
				/* for each line entry in shadow file... */
				while (fgets(line, 100, fpShadow) != NULL)
				{
					/* init match variable here in case a password previously found */
					match = 0;

					shadowCounter++;

					/* remove NL */
					len = strlen(line);
					line[len - 1] = SZ;

					/* extract information from shadow file line */
					username	= GetUser(line);
					salt		= GetSalt(line);
					pass		= GetPass(line);

					printf("-> Guessing password for username : %s\n", username);

					/* ...check every word in dictionary for a match */
					while ((fgets(dicWord, 10, fpDictionary) != NULL) && (!match))
					{
						/* remove NL */
						len = strlen(dicWord);
						dicWord[len - 1] = SZ;

						if (CmpCryptWord(salt, dicWord, pass))
						{
							OutputEntry(fpResults, username, dicWord);
							matchCounter++;
							match = 1;
						}
					}
					rewind(fpDictionary);
				}
				StopClock();

				AppendFooter(fpResults, matchCounter, shadowCounter);
				RecordTime(fpResults);
				fclose(fpResults);
				fclose(fpDictionary);
				fclose(fpShadow);
				break;
		default :
				DispUsage();
				return 0;
	}

	printf("\nTOTAL PASSWORDS FOUND %d/%d\n", matchCounter, shadowCounter);
	printf("See Results file for details.\n");

	return 0;
}

/*
 * Function Code
 */

/* extract username from shadow file */
char* GetUser(const char* line)
{
	static char username[25];

	int	len = strlen(line);
	int	i;

	for (i = 0; i < len; i++)
	{
		/* take chars until ':' */
		if ( line[i] == ':' )
			break;
		username[i] = line[i];
	}

	username[i] = SZ;
	return (username);
}	

/* extract salt from shadow file */
char* GetSalt(const char* line)
{
	static char salt[15];

	int len = strlen(line);
	int i, j, dolar;
	i = j = dolar = 0;

	/* process line char-by-char */
	while ( i < len )
	{
		/* dolar counter */
		if (line[i] == '$')
			dolar++;
		/* if at least 1 '$' then it is salt, take it */
		if (dolar > 0)
		{
			salt[j] = line[i];
			j++;
			/* if 3rd dolar just taken then exit, salt is over */
			if (dolar  == 3)
				break;
		}
		i++;
	}
	salt[j] = SZ;
	return (salt);
}

/* extract hashed passfrase from shadow file */
char* GetPass(const char* line)
{
	static char pass[25];

	int len = strlen(line);
	int i, j, dolar, flag;
	i = j = dolar = flag = 0;
	
	/* process line char-by-char */
	while (i < len)
	{
		/* count dolars..at 3rn one we begin extracting */
		if (line[i] == '$')
			dolar++;
		/* if first ':' passed and we are at 2nd (shown by flag) then passfrase ends */
		if (line[i] == ':' && flag == 1)
			break;
		/* @ 3rd dolar activate flag and reset dolar counter */
		/* we don't get inside this if again */
		if (dolar == 3)
		{
			i++;
			flag = 1;
			dolar = 0;
		}
		/* extract passfrase char-by-char */
		if (flag)
		{
			pass[j] = line[i];
			j++;
		}
		i++;
	}

	/* terminato string */
	pass[j] = SZ;
	return (pass);
}

/* Calls Crypt */
int CmpCryptWord(const char* salt, const char* genpass, const char* pass)
{
	char* hash;
	/* store crypt result in hash */
	/* crypt input : generated passfrace, salt taken from shadow file */
	hash = (char*) crypt(genpass, salt);

	/* if NULL pointer exit with error */
	if (!hash)
	{
		perror("Crypt");
		exit(1);
	}

	char newpass[25];

	int len = strlen(salt);
	/* remove tha salt entry and copy string for comparison below */
	strcpy(newpass, hash+len);

	/* if we have a match return true else exit with false */
	if (strcmp(pass, newpass) == 0)
		return (1);
	
	return (0);
}

/* display program usage */
void DispUsage()
{
	printf(	"\n"
			" Usage       :   PassBreaker [arguments]\n"
			"\n"
			" Arguments   :   <shadow file>\n"
			"                 <shadow file> <dictionary file>\n"
			"\n"
			" Description :\n"
			"	When invoked with only one argument (shadow file)\n"
			"	it tries to brute-force the shadow file passwords\n"
			"\n"
			"	When invoked with two arguments (shadow file + dictionary file)\n"
			"	uses the dictionary to crack the shadow file passwords\n"
			"\n"
			" Examples    :\n"
			"  (Brute force method) $ nice -n 15 ./PassBreaker shadow\n"
			"  (Dictionary method)  $ nice -n 5  ./PassBreaker shadow D8.dic\n"
			"\n"
			" Output      : \"Results\" file contains cracked passwords\n"
			"\n\n"
		);
}

/* Append header @ results file */
void AppendHeader(FILE* fp, const char* method)
{
    struct tm*  timendate;
    time_t      t;

	/* take local time */
    t = time(NULL);
    timendate = localtime(&t);

    fputs("-----------------------------------------------------\n", fp);
    fputs("PassBreaker scan initiated : ", fp);
    fputs(asctime(timendate), fp);
	fputs("-----------------------------------------------------\n", fp);
	fputs("Method   : ", fp);
	fputs(method, fp);
 	fputs("\n\n", fp);
}

/* Append footer with sum results @ results file */
void AppendFooter(FILE* fp, unsigned int matchCounter, unsigned int shadowCounter)
{
	fputs("\n", fp);
	fputs("Total passwords found : ", fp);
	fprintf (fp, "%d", matchCounter);
	fputs(" / ", fp);
	fprintf (fp, "%d", shadowCounter);
}

/* generate an entry in Results file with username / password found */
void OutputEntry(FILE* fp, const char* user, const char* pass)
{
	fputs("Username : ", fp);
	fputs(user, fp);
	fputs("\t\tPassword : ", fp);
	fputs(pass, fp);
	fputs("\n", fp);
}

/* get cpu time instance */
void StartClock()
{
	/* store CPU clocks in st_times and detailed (system-user) in struct st_cpu */
    st_time = times(&st_cpu);
}

/* get cpu time instance and calculate difference from start */
void StopClock()
{
	/* get clocks per sec */
    en_time = times(&en_cpu);
	/* calculate real elapsed time */
	df_time = (intmax_t)(en_time - st_time) / sysconf(_SC_CLK_TCK);
	/* calculate user elapsed time */
	df_cpu.tms_utime = (intmax_t)(en_cpu.tms_utime - st_cpu.tms_utime) / sysconf(_SC_CLK_TCK);
	/* calculate system elapsed time */
	df_cpu.tms_stime = (intmax_t)(en_cpu.tms_stime - st_cpu.tms_stime) / sysconf(_SC_CLK_TCK);
}

/* Display elapsed time in stdout and create an entry in Results file */
void RecordTime(FILE* fp)
{
	printf("\nReal Time :\t%ds\nUser Time :\t%ds\nSystem Time :\t%ds\n", df_time,
																		 df_cpu.tms_utime,
																		 df_cpu.tms_stime);

	fprintf(fp, "\nReal Time :\t%ds\nUser Time :\t%ds\nSystem Time :\t%ds\n\n", df_time,
																			  df_cpu.tms_utime,
																			  df_cpu.tms_stime);
}
