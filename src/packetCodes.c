#define NEW_PACKET_CODE 0
#define DELETE_FILE_CODE 1

#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif

/*
------ 0 -----
1 byte (packet code)
uint -> lunghezza pathname
pathname /tmp/ciao.txt
byte (1:da cartella, 0:file singolo)
uint caratteri totali

0 uint
..
255 uint
*/

/*
------ 1 -------
1 byte (delete_file)
uint -> lunghezza pathname
pathname
*/

#define MAXFILE 20

[ "ciao.txt", {"mario.txt", total = 40, occorenze[256]}, ... ]

    [file, file, cartelle = {[fiel1, file2, cartella = {}]}]
