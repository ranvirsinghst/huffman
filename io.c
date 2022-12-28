#include "io.h"
#include "defines.h"
#include "code.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;
static uint8_t bit_buf[BLOCK];
static uint8_t code_buf[BLOCK];
static uint32_t buf_offset = 0;

int read_bytes(int fd, uint8_t *buff, int nbytes)
{
    int bytes_read_in = read(fd, buff, nbytes);
    int total_bytes_read = 0;

    while (bytes_read_in > 0)
    {
        bytes_read += bytes_read_in;
        total_bytes_read += bytes_read_in;
        if (total_bytes_read >= nbytes)
        {
            break;
        }
        bytes_read_in = read(fd, buff + total_bytes_read, nbytes - total_bytes_read);
    }
    return total_bytes_read;
}

int write_bytes(int fd, uint8_t *buff, int nbytes)
{
    int written = write(fd, buff, nbytes);
    int total_bytes_written = 0;

    while (written > 0)
    {
        // printf("bytes written in io: %d, total bytes written (all time in io): %d\n", written, bytes_written);
        bytes_written += written;
        total_bytes_written += written;
        if (total_bytes_written >= nbytes)
        {
            break;
        }
        written = write(fd, buff + total_bytes_written, nbytes - total_bytes_written);
    }
    return total_bytes_written;
}

bool read_bit(int infile, uint8_t *bit)
{
    // read individual bits from the infile and store them in the bit pointer
    static uint32_t bit_offset = 0;
    static uint32_t bytes = 0;
    /*
    if read bit has been called enough times that the buffer has
    been fully read through, reset the bit offset to trigger the
    following conditional
    */
    if (bit_offset == (bytes * 8))
    {
        bit_offset = 0;
        for (int i = 0; i < BLOCK; i++)
        {
            bit_buf[i] = 0;
        }
    }

    if (bit_offset == 0)
    {
        bytes = read_bytes(infile, bit_buf, BLOCK);
    }
    // if there are no more bytes to read, return false
    if (bytes == 0)
        return false;
    // set bit to the bit referred to by bit_offset
    // reusing my implementation of code_get_bit:
    uint8_t num = bit_buf[bit_offset / 8];
    uint8_t mask = (1UL << (bit_offset % 8));
    *bit = (num & mask) == mask;
    bit_offset++;
    return true;
}

void write_code(int outfile, Code *c)
{
    // buffer all bits in c, then place the final buffer in outfile
    static int bit_offset = 0;
    uint32_t top = c->top;
    // loop through the code passed in, buffering in each individual bit.
    for (int i = 0; i < top; i++)
    {
        uint8_t mask = (code_get_bit(c, i) << (bit_offset));
        code_buf[buf_offset / 8] |= mask;
        // incrementing the bit offset each time a bit is accessed
        bit_offset = (bit_offset + 1) % 8;
        buf_offset++;
        if ((buf_offset / 8) == BLOCK)
        {
            write_bytes(outfile, code_buf, BLOCK);
            for (uint32_t i = 0; i < BLOCK; i++)
            {
                code_buf[i] = 0;
            }
            buf_offset = 0;
        }
    }
    /*
    figuring out order in which to place bits in buffer so that they can be read in code-by-code
    in the same sequence as the letters in the input string
    */
    // if the Code has been fully read through, reset the offset of the bits being buffered in from the Code
    // if the buffer is full, write out the contents and reset the buffer and index from which we buffer bits
    return;
}

void flush_codes(int outfile)
{
    // write out remaining bits buffered by write_code
    // use code_buf and buf_offset
    write_bytes(outfile, code_buf, (buf_offset / 8) + ((buf_offset % 8) != 0));
    return;
}
