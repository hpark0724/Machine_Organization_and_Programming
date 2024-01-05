///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Spring 2022, Deb Deppeler
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021-23 Deb Deppeler
// Posting or sharing this file is prohibited, including any changes/additions.
//
// We have provided comments and structure for this program to help you get
// started.  Later programs will not provide the same level of commenting,
// rather you will be expected to add same level of comments to your work.
// 09/20/2021 Revised to free memory allocated in get_board_size function.
// 01/24/2022 Revised to use pointers for CLAs
//
////////////////////////////////////////////////////////////////////////////////
// Main File:        my_magic_square
// This File:        my_magic_square
// Other Files:      None
// Semester:         CS 354 Fall 2023
// Instructor:       Deb Deppeler
//
// Author:           Hye Won Park
// Email:            hpark383
// CS Login:         hyep
// GG#:              1
//                   (See https://canvas.wisc.edu/groups for your GG number)
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   Track all work you do in your work log-p2a.pdf
//                   and fully credit all sources of help, including family,
//                   friends, tutors, Peer Mentors, TAs, and Instructor.
//
// Online sources:   Avoid relying on eeb searches to solve your problems,
//                   but if you do search, be sure to include Web URLs and
//                   description of any information you find in your work log.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
    int size;            // dimension of the square
    int **magic_square;  // pointer to heap allocated magic square
} MagicSquare;

/* TODO:
 * Promps the user for the magic square's size, read it,
 * check if it's an odd number >= 3 (if not, display the required
 * error message and exit)
 *
 * return the valid number
 */
int getSize() {
    int n = 0;
    printf("Enter magic square's size (odd integer >=3)");
    scanf("%d", &n);

    if (n % 2 == 1 && n >= 3) {
        return n;
    } else if (n % 2 == 1 && n < 3) {
        printf("Magic square size must be >= 3.");
        exit(1);
    } else if (n % 2 != 1) {
        printf("Magic square size must be odd.");
        exit(1);
    } else {
        printf("Magic square size must be odd and >=3.");
        exit(1);
    }
}

/* TODO:
 * Makes a magic square of size n,
 * and stores it in a MagicSquare (on the heap)
.*
 * It may use the Siamese magic square algorithm
 * or alternate from assignment
 * or another valid algorithm that produces a magic square.
 *
 * n - the number of rows and columns
 *
 * returns a pointer to the completed MagicSquare struct.
 */
MagicSquare *generateMagicSquare(int n) {
    MagicSquare *ms = malloc(sizeof(MagicSquare));

    // dynamically allocate 2d array of magic_square
    ms->size = n;
    ms->magic_square = malloc(sizeof(int *) * n);

    for (int i = 0; i < n; i++) {
        *(ms->magic_square + i) = malloc(sizeof(int) * n);
    }

    // initialize all the elements in the 2D array to 0
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            *(*(ms->magic_square + i) + j) = 0;
        }
    }

    // the total number in array (nxn)
    int num = n * n;
    // first row of the array
    int row = 0;
    // middle column of the array
    int col = (n - 1) / 2;

    // assign 1 to the first row, middle column
    *(*(ms->magic_square + row) + col) = 1;

    for (int i = 2; i <= num; i++) {
        row--;
        col++;

        // case 1 : if row and column is out of bound
        //          or there is a conflict
        if (row < 0 && col >= n) {
            col--;
            row += 2;
            *(*(ms->magic_square + row) + col) = i;
        }

        // case 2 : if row is out of bound
        else if ((row < 0 && col < n)) {
            row = n - 1;
            *(*(ms->magic_square + row) + col) = i;
        }

        // case 3 : if column is out of bound
        else if (row >= 0 && col >= n) {
            col = 0;
            *(*(ms->magic_square + row) + col) = i;
        }

        // case 4 : if there is no conflict and
        //         the position are in the bound
        else {
            if (*(*(ms->magic_square + row) + col) != 0) {
                col--;
                row += 2;
            }
            *(*(ms->magic_square + row) + col) = i;
        }
    }

    return ms;
}

/* TODO:
 * Opens a new file (or overwrites the existing file)
 * and writes the magic square values to the file
 * in the specified format.
 *
 * magic_square - the magic square to write to a file
 * filename - the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename) {
    FILE *inputFile = fopen(filename, "w");
    // int n = magic_square -> size;
    fprintf(inputFile, "%d\n", magic_square->size);

    for (int i = 0; i < magic_square->size; i++) {
        for (int j = 0; j < magic_square->size; j++) {
            if (j != magic_square->size - 1) {
                fprintf(inputFile, "%d, ", *(*(magic_square->magic_square + i) + j));
            } else {
                fprintf(inputFile, "%d", *(*(magic_square->magic_square + i) + j));
            }
        }
        fprintf(inputFile, "\n");
    }
    fclose(inputFile);
}

/* TODO:
 * Generates a magic square of the user specified size and
 * outputs the square to the output filename.
 *
 * Add description of required CLAs here
 */
int main(int argc, char **argv) {
    // TODO: Check input arguments to get output filename
    char *file = NULL;
    if (argc != 2) {
        printf("Usage: ./my_magic_square <output_filename>\n");
        exit(1);
    } else {
        file = *(argv + 1);
    }
    // TODO: Get magic square's size from user
    int msSize = getSize();

    // TODO: Generate the magic square by correctly interpreting
    //       the algorithm(s) in the write-up or by writing or your own.
    //       You must confirm that your program produces a
    //       Magic Sqare as described in the linked Wikipedia page.
    MagicSquare *ms = generateMagicSquare(msSize);
    fileOutputMagicSquare(ms, file);

    for (int i = 0; i < msSize; i++) {
        free(*(ms->magic_square + i));
        *(ms->magic_square + i) = NULL;
    }
    free(ms->magic_square);
    ms->magic_square = NULL;
    free(ms);
    ms = NULL;

    // TODO: Output the magic square
    return 0;
}

//    F23
