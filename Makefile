OPTS = -std=gnu90
COMPILE = gcc $(OPTS)
SOURCEFOLDER = ./src
DESTFOLDER = ./build
mainFile = $(SOURCEFOLDER)/main.c

build: $(mainFile)
#	Create the build directory if needed
	@if [ ! -d $(DESTFOLDER) ]; then \
	mkdir $(DESTFOLDER); \
	fi
#	Compile main
	@$(COMPILE) $(mainFile) -o $(DESTFOLDER)/main.o

.PHONY: help
help:
	#TODO

.PHONY: clean
clean:
	@rm -fr ./build


#From now on these are not required ones, just for conveniency
rebuild: clean build

run: build
	@$(DESTFOLDER)/main.o