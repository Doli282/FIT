CXX=g++
CXXFLAGS=--std=c++20 -Ofast

LIBS = -maes

SCRIPT=compare.sh
SOURCE_FILES= aestest5_Tbox.cpp  aestest5_speed.cpp  aestest6_int.cpp  aestest6_int_on_the_fly.cpp
EXECUTABLES=$(patsubst %.cpp, %.out, ${SOURCE_FILES})

all: compile

run: compile
	echo "Compiled with: ${CXX} ${CXXFLAGS} ${LIBS}"
	./${SCRIPT} ${EXECUTABLES}


compile: ${EXECUTABLES}

%.out: %.cpp
	${CXX} ${CXXFLAGS} ${LIBS} $< -o $@

clean:
	rm -f *.out

.PHONY: all compile run clean
.SILENT: all compile run clean