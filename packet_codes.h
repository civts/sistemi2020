// (Comments are put before the packet code so that structure is visible when
// hovering over it in other files)

#ifndef PACKET_CODES_H
#define PACKET_CODES_H
//----------------------------------CODES FOR-----------------------------------
//--------------------------------PACKETS FROM Q--------------------------------
/*Packet for new data from a Q/MiniQ
(that is raw char count in a part of a file)
------ header -----
1 byte (packet code)
INT_SIZE dimensione in byte sezione dati
------body------
INT_SIZE pid analyzer
INT_SIZE file id: riferimento al nome del file per l'altra pipe
INT_SIZE m: numero parti in cui è stato diviso il file
INT_SIZE i: numero della parte di file che ho analizzato in questo pacchetto 0
<= i < m
INT_SIZE D: dimensione file
INT_SIZE numCaratteriLettiInQuestaPorzione (?confermate se vi serve?)
INT_SIZE[256] dati
*/
#define Q_NEW_DATA_CODE 6

/*Packet for signalling an error in processing a given file
------ header -----
1 byte packet code (1)
INT_SIZE dimensione sezione dati
------body------
INT_SIZE pid analyzer
INT_SIZE file id su cui ho avuto il problema
*/
#define Q_FILE_ERROR_CODE 11

//----------------------------------CODES FOR-----------------------------------
//--------------------------------PACKETS FROM A--------------------------------
/*Packet for adding new file (w/ complete path)
------ header -----
1 byte packet code (0)
INT_SIZE dimensione in byte sezione dati
------body------
INT_SIZE pid dell'A
INT_SIZE id file
string path
*/
#define  A_ANALYZER_START 5
#define A_NEW_FILE_COMPLETE 8
// As A_NEW_FILE_COMPLETE but contains only first half of the path
#define A_NEW_FILE_INCOMPLETE_PART1 9
// As A_NEW_FILE_COMPLETE but contains only second half of the path
#define A_NEW_FILE_INCOMPLETE_PART2 10
/*Packet for when we need to delete a file
pipeA nel caso di eliminazione file:
------ header -----
1 byte packet code (3)
INT_SIZE dimensione in byte sezione dati
------body------
INT_SIZE pid dell'A
INT_SIZE id file
*/
#define A_DELETE_FILE_CODE 7
/*Packet to DELETE (w/ complete path)
------ header -----
1 byte packet code (6)
INT_SIZE dimensione in byte sezione dati
------body------
INT_SIZE pid dell'A
string path
*/
#define A_DELETE_FOLDER 12
// As A_NEW_FILE_COMPLETE but contains only first half of the path
#define A_DELETE_FOLDER_INCOMPLETE_PART1 13
// As A_NEW_FILE_COMPLETE but contains only second half of the path
#define A_DELETE_FOLDER_INCOMPLETE_PART2 14
#define A_ERROR_LOG 18
// #ifndef bool
// typedef unsigned char bool;
// #define false 0
// #define true 1
// #endif
 #endif
