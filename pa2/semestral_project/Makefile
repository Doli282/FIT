# compiler variables
CXX=g++
CXXFLAGS=-Wall -pedantic -Wextra -Wsuggest-override -g -std=c++17
# linker variables
LD=g++
LDFLAGS=-Wall -pedantic

# variables
PROGRAM=dolanluk
LIBS=-lncurses -lstdc++fs

# directory with source code #VPATH = src
SRCDIR=src
# all .cpp files
SRC=$(shell find ${SRCDIR}/ -type f -name '*.cpp')
# object files for every .cpp file
OBJ=$(patsubst ${SRCDIR}/%.cpp, ${SRCDIR}/%.o, ${SRC})
# patsubst: substitution template, output string, input string


# do not touch these files (if there is any by any chance)
.PHONY: all run compile doc clean


# create every output -> compile everything, executable file, documentation
all: compile doc

# run binary executable file
run: compile
	./${PROGRAM}

# compile verything, create exe file
compile: ${PROGRAM}

# generate documentation
doc: ./${SRCDIR}/*.h ./${SRCDIR}/*.cpp
	mkdir -p ./doc
	doxygen

# celanup - remove all objectfiles and core files, remove documentation, remove exe file, remove a.out (if left by mistake)
clean:
	rm -f ./${SRCDIR}/*.o *~ core
	rm -rf ./doc
	rm -f ./${PROGRAM}
	rm -f ./${SRCDIR}/a.out

# link object files together and include libraries -> create executable file -> final binary output
${PROGRAM}: ${OBJ}
	${LD} ${LDFLAGS} $^ -o $@ ${LIBS}


# compile all underlying files
${SRCDIR}/%.o: ${SRCDIR}/%.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

# object files and their dependencies 
# command used for generating dependencies: g++ -MM src/*.cpp >> Makefile

# CMap.o: CMap.cpp CMap.h CUnit.h CCoordinates.h CTowerWiki.h CTower.h CParser.h
# CParser.o: CParser.cpp CParser.h
# CParserMap.o: CParserMap.cpp
# CStartingMenu.o: CStartingMenu.cpp
# CTower.o: CTower.cpp CTower.h CUnit.h CCoordinates.h
# CTowerWiki.o: CTowerWiki.cpp CTowerWiki.h CTower.h CUnit.h CCoordinates.h CParser.h
# LoadFile.o: LoadFile.cpp CTower.h CUnit.h CCoordinates.h
# main.o: main.cpp CMap.h CUnit.h CCoordinates.h CTowerWiki.h CTower.h
# ShowMap.o: ShowMap.cpp CTowerWiki.h CTower.h CUnit.h CCoordinates.h
