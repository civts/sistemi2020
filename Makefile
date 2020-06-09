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
$(TMP_COM_DS)/namesList.o: $(COMMONFOLDERDS)/namesList.c
	@$(COMPILE) $(COMMONFOLDERDS)/namesList.c -c -o $(TMP_COM_DS)/namesList.o
$(TMP_COM_DS)/fileList.o: $(COMMONFOLDERDS)/fileList.c
	@$(COMPILE) $(COMMONFOLDERDS)/fileList.c -c -o $(TMP_COM_DS)/fileList.o
$(TMP_COM)/miniQlist.o:  $(COMMONFOLDERDS)/miniQlist.c
	@$(COMPILE) $(COMMONFOLDERDS)/miniQlist.c -c -o $(TMP_COM)/miniQlist.o
$(TMP_COM)/utils.o:  $(COMMONFOLDER)/utils.c
	@$(COMPILE) $(COMMONFOLDER)/utils.c -c -o $(TMP_COM)/utils.o
$(TMP_COM)/parser.o: $(COMMONFOLDER)/parser.c
	@$(COMPILE) $(COMMONFOLDER)/parser.c -c -o $(TMP_COM)/parser.o
$(TMP_COM)/packets.o: $(COMMONFOLDER)/packets.c
	@$(COMPILE) $(COMMONFOLDER)/packets.c -c -o $(TMP_COM)/packets.o
$(TMP_COM)/mymath.o: $(COMMONFOLDER)/mymath.c
	@$(COMPILE) $(COMMONFOLDER)/mymath.c -c -o $(TMP_COM)/mymath.o


#Analyzer folder
$(TMP_AN)/q.o: $(ANALYZERFOLDER)/q.c
	@$(COMPILE) $(ANALYZERFOLDER)/q.c -c -o $(TMP_AN)/q.o
$(TMP_AN)/p.o: $(ANALYZERFOLDER)/p.c
	@$(COMPILE) $(ANALYZERFOLDER)/p.c -c -o $(TMP_AN)/p.o
$(TMP_AN)/crawler.o: $(ANALYZERFOLDER)/crawler.c
	@$(COMPILE) $(ANALYZERFOLDER)/crawler.c -c -o $(TMP_AN)/crawler.o
$(TMP_AN)/miniQ.o: $(ANALYZERFOLDER)/miniQ.c
	@$(COMPILE) $(ANALYZERFOLDER)/miniQ.c -c -o $(TMP_AN)/miniQ.o
$(TMP_AN)/controller.o: $(ANALYZERFOLDER)/controller.c
	@$(COMPILE) $(ANALYZERFOLDER)/controller.c -c -o $(TMP_AN)/controller.o


#Report folder
$(TMP_REP_DS)/analyzer_list.o: $(REPORTFOLDERDS)/analyzer_list.c
	@$(COMPILE) $(REPORTFOLDERDS)/analyzer_list.c -c -o $(TMP_REP_DS)/analyzer_list.o
$(TMP_REP_DS)/analyzer_data_structure.o: $(REPORTFOLDERDS)/analyzer_data_structure.c
	@$(COMPILE) $(REPORTFOLDERDS)/analyzer_data_structure.c -c -o $(TMP_REP_DS)/analyzer_data_structure.o
$(TMP_REP_DS)/file_with_stats_list.o: $(REPORTFOLDERDS)/file_with_stats_list.c
	@$(COMPILE) $(REPORTFOLDERDS)/file_with_stats_list.c -c -o $(TMP_REP_DS)/file_with_stats_list.o
$(TMP_REP_DS)/file_with_stats_data_structure.o: $(REPORTFOLDERDS)/file_with_stats_data_structure.c
	@$(COMPILE) $(REPORTFOLDERDS)/file_with_stats_data_structure.c -c -o $(TMP_REP_DS)/file_with_stats_data_structure.o
$(TMP_REP)/packet_handler.o: $(REPORTFOLDER)/packet_handler.c
	@$(COMPILE) $(REPORTFOLDER)/packet_handler.c -c -o $(TMP_REP)/packet_handler.o
$(TMP_REP)/report_print_functions.o: $(REPORTFOLDER)/report_print_functions.c
	@$(COMPILE) $(REPORTFOLDER)/report_print_functions.c -c -o $(TMP_REP)/report_print_functions.o


#Analyzer
analyzer: createDest createTempCommonDs createTempAnalyzer $(TMP_COM)/parser.o $(TMP_COM)/mymath.o $(TMP_COM)/packets.o $(TMP_COM_DS)/fileList.o $(TMP_COM)/miniQlist.o $(TMP_COM_DS)/namesList.o $(TMP_COM)/utils.o $(TMP_AN)/p.o $(TMP_AN)/q.o $(TMP_AN)/crawler.o $(TMP_AN)/miniQ.o $(TMP_AN)/controller.o
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
report: createDest createTempCommonDs createTempRepDs createDest $(TMP_COM)/utils.o $(TMP_REP_DS)/analyzer_list.o $(TMP_REP)/packet_handler.o $(TMP_REP)/report_print_functions.o $(TMP_REP_DS)/analyzer_data_structure.o $(TMP_REP_DS)/file_with_stats_list.o $(TMP_COM_DS)/namesList.o $(TMP_REP_DS)/file_with_stats_data_structure.o $(TMP_COM)/packets.o
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

build: $(mainFile) createDest report analyzer
#	Compile main
	@$(COMPILE) $(SOURCEFOLDER)/main.c $(DESTFOLDER)/analyzer $(DESTFOLDER)/report -o $(DESTFOLDER)/main

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
 
