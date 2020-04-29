OPTS = -std=gnu90
COMPILE = gcc $(OPTS)
SOURCEFOLDER = ./src
DESTFOLDER = ./bin
mainFile = $(SOURCEFOLDER)/main.c

.PHONY: build
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
	@rm -fr $(DESTFOLDER)


#From now on these are not required ones, just for conveniency
.PHONY: rebuild
rebuild: clean build

.PHONY: run
run: build
	@$(DESTFOLDER)/main.o