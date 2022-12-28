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
#include <string.h>
#define options "hi:o:v"

void help()
{
    fprintf(stderr, "SYNOPSIS\n"
                    "  A Huffman encoder.\n"
                    "  Compresses a file using the Huffman coding algorithm.\n"
                    "\n"
                    "USAGE\n"
                    "  ./encode [-h] [-i infile] [-o outfile]\n"
                    "\n"
                    "OPTIONS\n"
                    "  -h             Program usage and help.\n"
                    "  -v             Print compression statistics.\n"
                    "  -i infile      Input file to compress.\n"
                    "  -o outfile     Output of compressed data.\n");
    return;
}

int get_num_unique_symbols(Node *root)
{
    // unique symbols are leaves in the tree
    if (root != NULL)
    {
        if (root->left == NULL && root->right == NULL)
        {
            return 1;
        }
        return get_num_unique_symbols(root->left) + get_num_unique_symbols(root->right);
    }
    return 0;
}

void construct_histogram(uint64_t *histogram, uint8_t *data, int nbytes)
{
    for (int i = 0; i < nbytes; i++)
    {
        histogram[data[i]]++;
    }

    if (histogram[0] == 0)
    {
        histogram[0] = 1;
    }
    if (histogram[1] == 1)
    {
        histogram[1] = 1;
    }
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
    // construct histogram
    uint64_t hist[ALPHABET] = {0};

    // read infile and get infile info
    int infile_fd = fileno(in_file);
    FILE *tmp = NULL;
    if (infile_fd == 0)
    {
        tmp = tmpfile();
    }

    uint8_t contents[BLOCK] = {0};
    int bytes_read_in = read_bytes(infile_fd, contents, BLOCK);
    while (bytes_read_in > 0)
    {
        if (bytes_read % BLOCK == 0)
        {
            construct_histogram(hist, contents, BLOCK);
            if (infile_fd == 0)
            {
                // stdin is input, so write input into tmpfile
                write_bytes(fileno(tmpfile), contents, BLOCK);
            }
            memset(contents, 0, BLOCK);
        }
        bytes_read_in = read_bytes(infile_fd, contents, BLOCK);
    }
    construct_histogram(hist, contents, bytes_read % BLOCK);

    // construct huffman tree
    Node *huffman_tree = build_tree(hist);

    // construct code table
    Code table[ALPHABET] = {0};
    build_codes(huffman_tree, table);

    // construct header
    struct stat fs;
    fstat(infile_fd, &fs);
    Header file_header;
    file_header.magic = MAGIC;
    file_header.permissions = fs.st_mode;
    file_header.tree_size = (3 * get_num_unique_symbols(huffman_tree)) - 1;
    file_header.file_size = fs.st_size;
    if (infile_fd == 0)
    {
        file_header.file_size = bytes_read;
    }

    // set outfile permissions to be same as infile permissions
    int outfile_fd = fileno(out_file);
    fchmod(outfile_fd, file_header.permissions);

    // write header to outfile
    write_bytes(outfile_fd, (uint8_t *)&file_header, sizeof(Header));

    // write tree dump
    dump_tree(outfile_fd, huffman_tree);

    // write codes
    rewind(in_file);
    int fd = infile_fd;
    if (infile_fd == 0)
    {
        // stdin is in the input, so use the tmpfile
        fd = fileno(tmp);
    }
    memset(contents, 0, BLOCK);
    bytes_read_in = read_bytes(fd, contents, BLOCK);
    int total_bytes_read = 0;
    while (bytes_read_in > 0)
    {
        total_bytes_read += bytes_read_in;
        if (total_bytes_read % BLOCK == 0)
        {
            for (int i = 0; i < BLOCK; i++)
            {
                write_code(outfile_fd, &table[(uint8_t)contents[i]]);
            }
            memset(contents, 0, BLOCK);
        }
        bytes_read_in = read_bytes(fd, contents, BLOCK);
    }
    for (int i = 0; i < total_bytes_read % BLOCK; i++)
    {
        write_code(outfile_fd, &table[(uint8_t)contents[i]]);
    }

    // flush remaining buffered
    flush_codes(outfile_fd);

    // printing compression stats
    if (compression_stats)
    {
        int uncompressed = file_header.file_size;
        int compressed = bytes_written;
        double saving = 100 * (1 - ((double)compressed / (double)uncompressed));
        fprintf(stderr, "Uncompressed file size: %d bytes\nCompressed file size: %d bytes\nSpace saving: %.2f%%\n", uncompressed, compressed, saving);
    }

    // close infile and outfile
    if (tmp != NULL)
    {
        fclose(tmp);
    }
    fclose(in_file);
    fclose(out_file);

    return 0;
}
