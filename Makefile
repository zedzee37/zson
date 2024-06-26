D_CC = clang
CC = gcc
SRC = src/*.c
BIN = json
D_BIN = debug_json
INPUT = test.json
INCLUDE = include

release:
	${CC} ${SRC} -I${INCLUDE} -o ${BIN} -O3 

debug:
	${D_CC} ${SRC} -I${INCLUDE} -o ${D_BIN} -DDEBUGF=1 -g

run:
	make release
	./${BIN} ${INPUT}

run_debug:
	make debug
	./${D_BIN} ${INPUT}

