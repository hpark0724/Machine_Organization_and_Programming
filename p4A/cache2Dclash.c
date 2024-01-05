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
// Main File:        cache2Dclash
// This File:        cache2Dclash
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

#include<stdio.h>

int arr2D[3000][500];

int main(){
    // loop 100 times
    for(int iteration = 0; iteration < 100; iteration++){
        // iterates over the rows, increment by 64 instead of 1
        for(int row = 0; row < 128; row+=64){
            // iterate over the columns of the array
            for(int col = 0; col < 8; col++){
                arr2D[row][col] = iteration + row + col;
            }
        }
    }
    return 0;

}