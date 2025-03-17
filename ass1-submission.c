/* Program to generate hierarchical reports from TSV structured data

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2024, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.
   All included code is (c) Copyright University of Melbourne, 2024

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial skeleton
   file, the  program contained in this submission is completely my own
   individual work, except where explicitly noted by further comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students, or by
   non-students as a result of request, solicitation, or payment, may not be
   submitted for assessment in this subject.  I understand that submitting for
   assessment work developed by or in collaboration with other students or
   non-students constitutes Academic Misconduct, and may be penalized by mark
   deductions, or by other penalties determined via the University of
   Melbourne Academic Honesty Policy, as described at
   https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will I
   do so until after the marks are released. I understand that providing my
   work to other students, regardless of my intention or any undertakings made
   to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring service,
   or drawing the attention of others to such services and code that may have
   been made available via such a service, may be regarded as Student General
   Misconduct (interfering with the teaching activities of the University
   and/or inciting others to commit Academic Misconduct).  I understand that
   an allegation of Student General Misconduct may arise regardless of whether
   or not I personally make use of such solutions or sought benefit from such
   actions.

   Signed by: Eric Zhang
   Dated:     13/09/2024

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define CHAR_CR '\r'	     /* CR character, in PC-format text files */
#define CHAR_NL '\n'         /* newline character, ends each input line */
#define CHAR_TB '\t'         /* tab character, ends each TSV field */

#define STATUS_EOL 1         /* return value for end of line field */
#define STATUS_EOF 2         /* return value for end of file field */
#define STATUS_NML 3         /* return value if neither of previous two */

#define MAXFIELDLEN 50       /* maximum field length */
#define MAXINPUTLEN 1000     /* maximum input lines length */   
#define MAXCOLUMNLEN 30      /* maximum column length */

#define COLUMNHEADER 0          /* row index for column header */
#define ROWNUMLEN 3
#define SPACING "    "       /* spacing to format table in stage 3 */
#define SPACINGLEN 4         /* string length of `SPACING` */
#define COUNTLEN 6           /* string length of " Count" to help format 
                                table lines in stage 3 */

/* one tsv field, stored within a fixed-length character string */
typedef char field_t[MAXFIELDLEN+1];


/**************************************************************/

int  mygetchar(void);
int  getfield(field_t dest);
void print_row(field_t tsv_table[][MAXCOLUMNLEN], int row, 
                    int totalcol);
void do_stage1(field_t tsv_table[][MAXCOLUMNLEN], int* row, int* col,
               int* totalcol);
void do_stage2(field_t tsv_table[][MAXCOLUMNLEN], int sort_keys[], 
               int *len_sort_keys, int argc, char *argv[], int row, 
               int totalcol);
void do_stage3(field_t tsv_table[][MAXCOLUMNLEN], int sort_keys[],
               int len_sort_keys, int row, int totalcol);
void read_keys(int sort_keys[], int *len_sort_keys, int argc, char *argv[]);
int compare_rows(field_t row1[], field_t row2[], int sort_keys[],
                 int len_sort_keys, int totalcol);
void sort_table(field_t tsv_table[][MAXCOLUMNLEN], int row, 
                int totalcol, int sort_keys[], int *len_sort_keys);
void row_swap(field_t tsv_table[][MAXCOLUMNLEN], int j, int totalcol);
void column_swap(field_t *str1, field_t *str2);
void print_table(field_t tsv_table[][MAXCOLUMNLEN], int row, int totalcol);
int longest_last_entry(field_t tsv_table[][MAXCOLUMNLEN], int row, int totalcol, 
                   int sort_keys[], int len_sort_keys);
void print_lines(int len_sort_keys, int maxlen);
void print_column_combination(field_t tsv_table[][MAXCOLUMNLEN],
                              int sort_keys[], int len_sort_keys,
                              int i, int mismatch, int maxlen);
void print_hiearchical_data(field_t tsv_table[][MAXCOLUMNLEN], int row, 
                            int totalcol, int sort_keys[], int len_sort_keys, 
                            int maxlen);
void print_spacing(int maxval);
/**************************************************************/

/* main program provides traffic control
*/
int
main(int argc, char *argv[]) {
   /* initialise variables for row count, column count, 
   and total columns*/
   int row, col, totalcol;
   /* 2D array to store entries in tsv file */
   field_t tsv_table[MAXINPUTLEN][MAXCOLUMNLEN];

   do_stage1(tsv_table, &row, &col, &totalcol);
   
   /* array to store sort keys in command line */
   int sort_keys[totalcol];
   /* records length of `sort_keys`*/
   int len_sort_keys=0;

   do_stage2(tsv_table, sort_keys, &len_sort_keys, argc, argv, row, totalcol);
   do_stage3(tsv_table, sort_keys, len_sort_keys, row, totalcol);

   printf("ta daa!\n");
   
	return 0;
}

/**************************************************************/

/* read characters and build a string, stopping when a tab or newline
   as encountered, with the return value indicating what that
   terminating character was
*/
int
getfield(field_t dest) {

	int ch, nchs=0;
	int status=STATUS_NML;

	dest[0] = '\0';
	while ((ch=mygetchar())
		&& (ch != CHAR_TB) && (ch != CHAR_NL) && (ch != EOF)) {

		if (nchs < MAXFIELDLEN) {
			/* ok to save this character */
			dest[nchs++] = ch;
			dest[nchs] = '\0';
		} else {
			/* silently discard extra characters if present */
		}
	}

	/* return status is defined by what character stopped the loop */
	if (ch == EOF) {
		status = STATUS_EOF;
	} else if (ch == CHAR_NL) {
		status = STATUS_EOL;
	} else if (ch == CHAR_TB) {
		status = STATUS_NML;
	}
	return status;
}

/**************************************************************/

/* read a single character, bypassing any CR characters encountered,
   so as to work correctly with either PC-type or Unix-type input
 */
int
mygetchar(void) {
	int c;
	while ((c=getchar()) == CHAR_CR) {
		/* empty loop body */
	}
	return c;
}

/**************************************************************/

/* prints column data from a specified row */
void 
print_row(field_t tsv_table[][MAXCOLUMNLEN], int row, int totalcol) {
   printf("row %d is:\n", row);
   int col_index;
   for (col_index = 0; col_index < totalcol; col_index++) {
      /* Prints column data */
      printf("%4d: %-11s%s\n", \
      col_index + 1, tsv_table[0][col_index], tsv_table[row][col_index]);
   }
}


/**************************************************************/

/* 
   stage 1 output:
   - number of rows and columns in tsv data
   - column data of last row
*/
void
do_stage1(field_t tsv_table[][MAXCOLUMNLEN], int *row, int *col, 
             int *totalcol) {

   /* Reads tsv file and inputs entries into corresponding rows and columns
   of a 2d array of strings `tsv_table`*/
   int result;
   while ((result = (getfield(tsv_table[*row][*col]))) != STATUS_EOF) {
      if (result == STATUS_NML) {
         *col += 1;
      }
      else if (result == STATUS_EOL) {
         *col += 1;
         /* Keeps track of total number of columns in a row */
         if (*col > *totalcol) {
            *totalcol = *col;
         }
         *row += 1;
         *col = 0;
      }
   }
   /* Ignores last newline character */
   *row -= 1;

   printf("Stage 1\n");
   printf("input tsv data has %d rows and %d columns\n", *row, *totalcol);
   print_row(tsv_table, *row, *totalcol);
}

/**************************************************************/

/* reads sorting keys from command-line and puts values into an 
   integer array
*/

void
read_keys(int sort_keys[], int *len_sort_keys, int argc, char *argv[]) {

   /* if no sort keys in command line, then terminate program */
   if (argc == 1) {
      exit(EXIT_SUCCESS);
   }
   int i;
   for (i=1; i < argc; i++) {
      /* -1 to convert one-index to zero-index */
      sort_keys[i-1] = (atoi((argv[i]))-1);
      *len_sort_keys += 1;
   }

}
/**************************************************************/

/* 
   stage 2 output:
   - order of sort keys used to sort rows
   - column data for first, middle, and last row in sorted table
*/

void 
do_stage2(field_t tsv_table[][MAXCOLUMNLEN], int sort_keys[], 
               int *len_sort_keys, int argc, char *argv[], int row, 
               int totalcol) {

   read_keys(sort_keys, len_sort_keys, argc, argv);
   sort_table(tsv_table, row, totalcol, sort_keys, len_sort_keys);
   printf("\nStage 2\n");

   /* Prints order of sort keys */
   printf("sorting by \"%s\",\n", tsv_table[COLUMNHEADER][sort_keys[0]]);
   int i;
   for (i=1; i<(*len_sort_keys-1); i++) {
      printf("   then by \"%s\",\n", tsv_table[COLUMNHEADER][sort_keys[i]]);
   }
   /* Prints final sort key without comma */
   printf("   then by \"%s\"\n", tsv_table[COLUMNHEADER][sort_keys[i]]);

   /* Print column data for first, middle, and last rows */
   int row_num[ROWNUMLEN] = {1, ((row+1)/2), row};
   int j;
   for (j=0; j<ROWNUMLEN; j++) {
      print_row(tsv_table, row_num[j], totalcol);
   }
}

/**************************************************************/

/* uses insertion sort to lexicographically sort rows in `tsv_table`
   by order of sort keys from command line
*/

void 
sort_table(field_t tsv_table[][MAXCOLUMNLEN], int row, 
                    int totalcol, int sort_keys[], int *len_sort_keys) {

   int i, j;
   for (i=2; i<=row; i++) {
      for (j=i-1; i>0 && j>=1 && compare_rows(tsv_table[j+1], tsv_table[j], 
                                 sort_keys, *len_sort_keys, totalcol); j--) {
                                    
         row_swap(tsv_table, j, totalcol);
         }
      }
   }

/**************************************************************/

/* compares two rows by sort keys, with each consecutive sort key
   acting as a tiebreaker if previous sort keys are the same
*/

int 
compare_rows(field_t row1[], field_t row2[], int sort_keys[], 
             int len_sort_keys, int totalcol) {

   int result, i;
   for (i=0; i<(len_sort_keys); i++) {
      if ((result = strcmp(row1[sort_keys[i]], row2[sort_keys[i]])) < 0) {
         return 1;
      }
      else if (result > 0) {
         return 0;
      }
   }
   return 0; 
}

/**************************************************************/

/* Swaps row data in `tsv_table`*/

void row_swap(field_t tsv_table[][MAXCOLUMNLEN], int j, int totalcol) {

   int k;

   for (k=0; k<totalcol; k++) {
      column_swap(&tsv_table[j][k], &tsv_table[j+1][k]);
   }
}
   
/**************************************************************/

void 
column_swap(field_t *str1, field_t *str2) {

   field_t temp;

   strcpy(temp, *str1);
   strcpy(*str1, *str2);
   strcpy(*str2, temp);
}

/**************************************************************/

/* debugging function used to check rows and columns are in the 
   correct position
*/

void
print_table(field_t tsv_table[][MAXCOLUMNLEN], int row, int totalcol) {

   int i, j;
   for (i=0; i<=row; i++) {
      for (j=0; j<totalcol; j++) {
         printf("%-15s", tsv_table[i][j]);
      }
      printf("\n");
   }
   printf("\n");
}

/**************************************************************/

/* compares adjacent rows based on sorting keys and prints hierarchical
summary for each row 
*/

void 
print_hiearchical_data(field_t tsv_table[][MAXCOLUMNLEN], int row, 
                            int totalcol, int sort_keys[], int len_sort_keys, 
                            int maxlen) {
   /* mismatch records index in `sort_keys` where a mismatch between two rows
    has occured */
   int i, j, counter=1, mismatch=0;
   
   for (i=1; i<(row); i++) {
      for (j=0; j<len_sort_keys; j++) {
         if (strcmp(tsv_table[i][sort_keys[j]], 
                    tsv_table[i+1][sort_keys[j]])) {

            print_column_combination(tsv_table, sort_keys, len_sort_keys, i, 
                                     mismatch, maxlen);
            /* Prints count of rows with matching column combinations */
            printf("%5d\n", counter);
            mismatch = j;
            counter = 1;

            break;
         }
      }
      if (j == len_sort_keys) {
         counter++;
         
      }
    }
    /* Print last row */
    print_column_combination(tsv_table, sort_keys, len_sort_keys, i, mismatch,
                             maxlen);

    printf("%5d\n", counter);
}

/**************************************************************/
/* prints the hiearchical column data for each row, including 
   header row
*/

void 
print_column_combination(field_t tsv_table[][MAXCOLUMNLEN],
                              int sort_keys[], int len_sort_keys, int i,
                              int mismatch, int maxlen) {

    /* depending on which sort key mismatch has occurred, format
       and print hiearchical data for row */
    int k;
    for (k=0;(k + mismatch) < (len_sort_keys - 1); k++) {
      print_spacing(k + mismatch);
      printf("%s\n", tsv_table[i][sort_keys[k + mismatch]]);
    }
      /* Print last column entry */
      print_spacing(k + mismatch);
      printf("%-*s ", maxlen, tsv_table[i][sort_keys[k + mismatch]]);
}

/**************************************************************/
/* finds longest string length of last entry in each row to help
   format the table in stage 3
*/

int
longest_last_entry(field_t tsv_table[][MAXCOLUMNLEN], int row, int totalcol, 
                   int sort_keys[], int len_sort_keys) {

   int maxlen=0, i, currentlen;
   for (i=0; i<row; i++) {
      if ((currentlen = strlen(tsv_table[i][sort_keys[len_sort_keys-1]])) > 
           maxlen) {
            
         maxlen = currentlen;
         }
      }
   return maxlen;
}

/**************************************************************/

/* helper function to format hiearchical data for table in stage 3 */

void
print_spacing(int maxval) {
   int i;
   for (i=0; i<maxval; i++) {
      printf("%s", SPACING);
   }
}

/**************************************************************/

void
print_lines(int len_sort_keys, int maxlen) {
   int i;
   for (i=0; i < ((len_sort_keys-1) * SPACINGLEN + maxlen + COUNTLEN); i++) {
      printf("-");
   }
   printf("\n");
}

/**************************************************************/
/*
   stage 3 output:
   - table containing hierarchical summary of rows by sort keys, and
   including count of duplicate rows that have the same column value for
   each sort key
*/

void
do_stage3(field_t tsv_table[][MAXCOLUMNLEN], int sort_keys[], 
          int len_sort_keys, int row, int totalcol) {

   int maxlen = longest_last_entry(tsv_table, row, totalcol, sort_keys,
                                   len_sort_keys);

   printf("\nStage 3\n");
   print_lines(len_sort_keys, maxlen);
   /* Print column header */
   print_column_combination(tsv_table, sort_keys, len_sort_keys, 
                            COLUMNHEADER, COLUMNHEADER, maxlen);

   printf("Count\n");
   print_lines(len_sort_keys, maxlen);
   print_hiearchical_data(tsv_table, row, totalcol, sort_keys, len_sort_keys,
                          maxlen);

   print_lines(len_sort_keys, maxlen);
}

/**************************************************************/

/* algorithms are fun */
