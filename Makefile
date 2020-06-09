all: //TODO metti i comandi da eseguire quando chiama make

OPTS = -std=gnu90
COMPILE = gcc $(OPTS)
SOURCEFOLDER = ./src
DESTFOLDER = ./bin
COMMONFOLDER = $(SOURCEFOLDER)/common
COMMONFOLDERDS = $(COMMONFOLDER)/datastructures
REPORTFOLDER = $(SOURCEFOLDER)/report
REPORTFOLDERDS = $(REPORTFOLDER)/data_structures

#Common folder
namesList:
	$(COMPILE) $(COMMONFOLDERDS)/namesList.c -c -o $(COMMONFOLDERDS)/namesList.o
utils:
	$(COMPILE) $(COMMONFOLDER)/utils.c -c -o $(COMMONFOLDER)/utils.o
parser:
	$(COMPILE) $(COMMONFOLDER)/parser.c -c -o $(COMMONFOLDER)/parser.o
packets: 
	$(COMPILE) $(COMMONFOLDER)/packets.c -c -o $(COMMONFOLDER)/packets.o
fileList:
	$(COMPILE) $(COMMONFOLDER)/fileList.c -c -o $(COMMONFOLDER)/fileList.o
miniQList:
	$(COMPILE) $(COMMONFOLDER)/miniQlist.c -c -o $(COMMONFOLDER)/miniQlist.o

#Report folder
analyzerList:
	$(COMPILE) $(REPORTFOLDERDS)/analyzer_list.c -c -o $(REPORTFOLDERDS)/analyzer_list.o
analyzerDataStruct: 
	$(COMPILE) $(REPORTFOLDERDS)/analyzer_data_structure.c -c -o $(REPORTFOLDERDS)/analyzer_data_structure.o
fwsList:
	$(COMPILE) $(REPORTFOLDERDS)/file_with_stats_list.c -c -o $(REPORTFOLDERDS)/file_with_stats_list.o
fwsDataStruct:
	$(COMPILE) $(REPORTFOLDERDS)/file_with_stats_data_structure.c -c -o $(REPORTFOLDERDS)/file_with_stats_data_structure.o
packetHanlder:
	$(COMPILE) $(REPORTFOLDER)/packet_handler.c -c -o $(REPORTFOLDER)/packet_handler.o
reportPrintFunctions:
	$(COMPILE) $(REPORTFOLDER)/report_print_functions.c -c -o $(REPORTFOLDER)/report_print_functions.o

#Report
report: utils analyzerList packetHanlder reportPrintFunctions analyzerDataStruct fwsList namesList fwsDataStruct packets
	$(COMPILE) $(REPORTFOLDER)/report.c \
	$(COMMONFOLDER)/utils.o \
	$(REPORTFOLDERDS)/analyzer_list.o \
	$(REPORTFOLDERDS)/analyzer_data_structure.o \
	$(REPORTFOLDERDS)/file_with_stats_list.o \
	$(REPORTFOLDER)/packet_handler.o \
	$(REPORTFOLDER)/report_print_functions.o \
	$(COMMONFOLDERDS)/namesList.o \
	$(REPORTFOLDERDS)/file_with_stats_data_structure.o \
	$(COMMONFOLDER)/packets.o \
	-o $(REPORTFOLDER)/report.o

.PHONY: build
build: $(mainFile)
#	Create the build directory if needed
	@if [ ! -d $(DESTFOLDER) ]; then \
	mkdir $(DESTFOLDER); \
	fi
#	Compile main
	@$(COMPILE) $(SOURCEFOLDER)/main.c -o $(DESTFOLDER)/main.o

.PHONY: help
help:
	#TODO

.PHONY: clean
clean:
	@rm -fr $(DESTFOLDER)


#From now on these are not required ones, just for conveniency
.PHONY: rebuild
rebuild: clean build

.PHONY: run
run: build
	@$(DESTFOLDER)/main.o
