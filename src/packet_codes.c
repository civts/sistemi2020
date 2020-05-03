//(Comments are put before the packet code so that structure is visible when
//hovering over it in other files)

/*
------ 0 -----
1 byte (packet code)
INT_SIZE bytes packet length
uint -> lunghezza pathname
pathname /tmp/ciao.txt
1 byte (1:da cartella, 0:file singolo)
uint caratteri totali

0 uint
..
255 uint
*/
#define NEW_PACKET_CODE 0

/*
------ 1 -------
1 byte (packet_code)
INT_SIZE bytes packet length
uint -> lunghezza pathname
pathname
*/
#define DELETE_FILE_CODE 1

#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif

// #define MAXFILE 20

// [ "ciao.txt", {"mario.txt", total = 40, occorenze[256]}, ... ]

//     [file, file, cartelle = {[fiel1, file2, cartella = {}]}]
