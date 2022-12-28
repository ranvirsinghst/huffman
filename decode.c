#include "code.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include "header.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#define options "hi:o:v"

void help() {
    fprintf(stderr, "SYNOPSIS\n"
           "  A Huffman decoder.\n"
           "  Decompresses a file using the Huffman coding algorithm.\n"
           "\n"
           "USAGE\n"
           "  ./decode [-h] [-i infile] [-o outfile]\n"
           "\n"
           "OPTIONS\n"
           "  -h             Program usage and help.\n"
           "  -v             Print compression statistics.\n"
           "  -i infile      Input file to decompress.\n"
           "  -o outfile     Output of decompressed data.\n");
}

bool leaf(Node *root) {
    if (root->left == NULL && root->right == NULL) {
        return 1;
    } else return 0;
}

int main(int argc, char const *argv[])
{
    int opt = 0;

    FILE *in_file = stdin;
    FILE *out_file = stdout;
    bool compression_stats = false;

    while ((opt = getopt(argc, argv, options)) != -1)
    {
        switch (opt)
        {
            case 'h':
                help();
                return 0;
            case 'i':
                in_file = fopen(optarg, "r");
                break;
            case 'o':
                out_file = fopen(optarg, "w");
                break;
            case 'v':
                compression_stats = true;
                break;
            default:
                help();
                return 0;
        }
    }

    // read infile header
    int infile_fd = fileno(in_file);
    Header file_header;
    read_bytes(infile_fd, (uint8_t *)&file_header, sizeof(Header));
    if (file_header.magic != MAGIC) {
	    fprintf(stderr, "Invalid magic number.\n");
	    return 1;
    }

    // set outfile permissions to perms specified in infile
    uint64_t outfile_fd = fileno(out_file);
    fchmod(outfile_fd, file_header.permissions);

    // read tree dump into array of tree_size bytes
    uint8_t tree[file_header.tree_size];
    int r = read_bytes(infile_fd, tree, file_header.tree_size);
    Node *root = rebuild_tree(file_header.tree_size, tree);

    // read infile bit-by-bit, traversing tree based on bits read until the number of symbols written matches the file size
    uint8_t bit_holder;
    Node *curr_node;
    for (int symbols_written = 0; symbols_written < file_header.file_size; symbols_written++) {
        // start at the root of the tree before traversing based on code
        curr_node = root;
        // if the current node is internal, read in a bit to know where to traverse
        while (!leaf(curr_node)) {
            read_bit(infile_fd, &bit_holder);
            if (bit_holder == 1) {
                curr_node = curr_node->right;
            } else {
                curr_node = curr_node->left;
            }
            // if the new current node is a leaf, exit the while loop to iterate back through the for loop
            if (leaf(curr_node)) {
                write_bytes(outfile_fd, &curr_node->symbol, sizeof(uint8_t));
                break;
            }
        }
    }

    // printing compression stats
    if (compression_stats) {
        int compressed = bytes_read;
        int decompressed = file_header.file_size;
        float saving = 100 * (1 - ((float) compressed / (float) decompressed));
        fprintf(stderr, "Compressed file size: %d bytes\nDecompressed file size: %d bytes\nSpace saving: %.2f%%\n", compressed, decompressed, saving);
    }

    // close in_file and out_file
    fclose(in_file);
    fclose(out_file);


    return 0;
}
