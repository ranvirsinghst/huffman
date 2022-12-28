# Huffman Coding:

### Building Executables
With the required files in your directory, execute the "make" command in your terminal.
Once the command is done running, the ./encode and ./decode executables will be available.

### Running Executables
./encode:

-h = prints program usage

-i [file] = specify [file] as input for encoder, default = stdin

-o [file] = specify [file] as output for encoder, default = stdout

-v = print compression stats to stderr

./decode: 

-h = prints program usage

-i [file] = specify [file] as input for decoder, default = stdin

-o [file] = specify [file] as output for decoder, default = stdout

-v = print decompression stats to stderr

### File Descriptions
- DESIGN.pdf: outlines design of my submission, and explains certain decisions
- README.md: contains instructions for building and running program
- Makefile: compiles code, removes derivative files, and formats code
- code.c: contains impelment
- code.h: header file for code.c
- defines.h: contains macro definitions utilized throughout other files
- header.h: contains the struct definition for a file header
- huffman.c: contains implementation of the Huffman Coding module interface
- huffman.h: header file for huffman.c
- io.c: contains implementation of the I/O module
- io.h: header file for io.c
- node.c: contains implementation of the Node ADT
- node.h: header file for node.c
- pq.c: contains implementation of the priority queue ADT
- pq.h: header file for pq.c
- stack.c: contains implementation of the stack ADT
- stack.h: header file for stack.c
