all: //TODO metti i comandi da eseguire quando chiama make

#Compiler options
OPTS = -std=gnu90
COMPILE = gcc $(OPTS)


#Folder paths
SOURCEFOLDER = ./src
DESTFOLDER = ./bin
TEMPFOLDER = ./.tmp
COMMONFOLDER = $(SOURCEFOLDER)/common
COMMONFOLDERDS = $(COMMONFOLDER)/datastructures
REPORTFOLDER = $(SOURCEFOLDER)/report
REPORTFOLDERDS = $(REPORTFOLDER)/data_structures
ANALYZERFOLDER = $(SOURCEFOLDER)/analyzer

TMP_COM = $(TEMPFOLDER)/common
TMP_COM_DS = $(TEMPFOLDER)/common/datastructures

TMP_REP = $(TEMPFOLDER)/report
TMP_REP_DS = $(TEMPFOLDER)/report/data_structures

TMP_AN = $(TEMPFOLDER)/analyzer


#Creates all the directories in which to put the build files if needed
.PHONY: createDest
createDest:
	@if [ ! -d $(DESTFOLDER) ]; then \
	mkdir $(DESTFOLDER); \
	fi
.PHONY: createTemp
createTemp:
	@if [ ! -d $(TEMPFOLDER) ]; then \
	mkdir $(TEMPFOLDER); \
	fi
.PHONY: createTempCommon
createTempCommon: createTemp
	@if [ ! -d $(TMP_COM) ]; then \
	mkdir $(TMP_COM); \
	fi
.PHONY: createTempCommonDs
createTempCommonDs: createTempCommon
	@if [ ! -d $(TMP_COM_DS) ]; then \
	mkdir $(TMP_COM_DS); \
	fi
.PHONY: createTempReport
createTempReport: createTemp
	@if [ ! -d $(TMP_REP) ]; then \
	mkdir $(TMP_REP); \
	fi
.PHONY: createTempRepDs
createTempRepDs: createTempReport
	@if [ ! -d $(TMP_REP_DS) ]; then \
	mkdir $(TMP_REP_DS); \
	fi
.PHONY: createTempAnalyzer
createTempAnalyzer: createTemp
	@if [ ! -d $(TMP_AN) ]; then \
	mkdir $(TMP_AN); \
	fi


#Common folder
namesList: createTempCommonDs
	@$(COMPILE) $(COMMONFOLDERDS)/namesList.c -c -o $(TMP_COM_DS)/namesList.o
fileList: createTempCommonDs
	@$(COMPILE) $(COMMONFOLDERDS)/fileList.c -c -o $(TMP_COM_DS)/fileList.o
miniQList: createTempCommon
	@$(COMPILE) $(COMMONFOLDERDS)/miniQlist.c -c -o $(TMP_COM)/miniQlist.o
utils: createTempCommon
	@$(COMPILE) $(COMMONFOLDER)/utils.c -c -o $(TMP_COM)/utils.o
parser: createTempCommon
	@$(COMPILE) $(COMMONFOLDER)/parser.c -c -o $(TMP_COM)/parser.o
packets:  createTempCommon
	@$(COMPILE) $(COMMONFOLDER)/packets.c -c -o $(TMP_COM)/packets.o
myMath: createTempCommon
	@$(COMPILE) $(COMMONFOLDER)/mymath.c -c -o $(TMP_COM)/mymath.o


#Analyzer folder
q: createTempAnalyzer
	@$(COMPILE) $(ANALYZERFOLDER)/q.c -c -o $(TMP_AN)/q.o
p: createTempAnalyzer
	@$(COMPILE) $(ANALYZERFOLDER)/p.c -c -o $(TMP_AN)/p.o
crawler: createTempAnalyzer
	@$(COMPILE) $(ANALYZERFOLDER)/crawler.c -c -o $(TMP_AN)/crawler.o
miniQ: createTempAnalyzer
	@$(COMPILE) $(ANALYZERFOLDER)/miniQ.c -c -o $(TMP_AN)/miniQ.o
controller: createTempAnalyzer
	@$(COMPILE) $(ANALYZERFOLDER)/controller.c -c -o $(TMP_AN)/controller.o


#Report folder
analyzerList: createTempRepDs
	@$(COMPILE) $(REPORTFOLDERDS)/analyzer_list.c -c -o $(TMP_REP_DS)/analyzer_list.o
analyzerDataStruct:  createTempRepDs
	@$(COMPILE) $(REPORTFOLDERDS)/analyzer_data_structure.c -c -o $(TMP_REP_DS)/analyzer_data_structure.o
fwsList: createTempRepDs
	@$(COMPILE) $(REPORTFOLDERDS)/file_with_stats_list.c -c -o $(TMP_REP_DS)/file_with_stats_list.o
fwsDataStruct: createTempRepDs
	@$(COMPILE) $(REPORTFOLDERDS)/file_with_stats_data_structure.c -c -o $(TMP_REP_DS)/file_with_stats_data_structure.o
packetHanlder: createTempReport
	@$(COMPILE) $(REPORTFOLDER)/packet_handler.c -c -o $(TMP_REP)/packet_handler.o
reportPrintFunctions: createTempReport
	@$(COMPILE) $(REPORTFOLDER)/report_print_functions.c -c -o $(TMP_REP)/report_print_functions.o


#Analyzer
analyzer: parser myMath packets fileList miniQList namesList utils p q crawler miniQ controller
	@$(COMPILE) $(ANALYZERFOLDER)/analyzer.c \
	$(TMP_COM)/parser.o \
	$(TMP_COM)/mymath.o \
	$(TMP_COM)/packets.o \
	$(TMP_COM_DS)/fileList.o \
	$(TMP_COM)/miniQlist.o \
	$(TMP_COM_DS)/namesList.o \
	$(TMP_COM)/utils.o \
	$(TMP_AN)/p.o \
	$(TMP_AN)/q.o \
	$(TMP_AN)/crawler.o \
	$(TMP_AN)/miniQ.o \
	$(TMP_AN)/controller.o \
	-lm -o $(DESTFOLDER)/analyzer


#Report
report: createDest utils analyzerList packetHanlder reportPrintFunctions analyzerDataStruct fwsList namesList fwsDataStruct packets
	@$(COMPILE) $(REPORTFOLDER)/report.c \
	$(TMP_COM)/utils.o \
	$(TMP_COM)/packets.o \
	$(TMP_COM_DS)/namesList.o \
	$(TMP_REP)/packet_handler.o \
	$(TMP_REP)/report_print_functions.o \
	$(TMP_REP_DS)/analyzer_list.o \
	$(TMP_REP_DS)/file_with_stats_list.o \
	$(TMP_REP_DS)/analyzer_data_structure.o \
	$(TMP_REP_DS)/file_with_stats_data_structure.o \
	-o $(DESTFOLDER)/report


build: $(mainFile)
#	Create the build directory if needed
	@if [ ! -d $(DESTFOLDER) ]; then \
	mkdir $(DESTFOLDER); \
	fi
#	Compile main
	@$(COMPILE) $(SOURCEFOLDER)/main.c -o $(DESTFOLDER)/main.o

.PHONY: help
help:
	less README

.PHONY: clean
clean:
	@rm -fr $(TEMPFOLDER)


#From now on these are not required ones, just for conveniency
.PHONY: rebuild
rebuild: clean build

.PHONY: run
run: build
	@$(DESTFOLDER)/main.o
