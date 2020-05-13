// (Comments are put before the packet code so that structure is visible when
// hovering over it in other files)

#ifndef PACKET_CODES_H
#define PACKET_CODES_H
/*This is the packet for new data from a Q
(that is raw char count in a part of a file)
------ 0 -----
1 byte (packet code)
uint -> lunghezza pathname
pathname /tmp/ciao.txt
1 byte (1:da cartella, 0:file singolo)
uint caratteri totali

0 uint
..
255 uint
*/
#define NEW_FILE_CODE 0
/*
*/
#define NEW_FILE_CODE_P1 1
/*
*/
#define NEW_FILE_CODE_P2 2
/*Packet for when we need to delete a file
------ 3 -------
1 byte (packet_code)
uint -> lunghezza pathname
pathname
*/
#define DELETE_FILE_CODE 3

#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif
#endif
