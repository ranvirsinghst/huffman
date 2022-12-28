#include "code.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// tested and fully working!

Code code_init(void) {
    Code new;
    // top starting at -1 so that it refers to the index last pushed to the code
    new.top = 0;
    // zeroing out all values in bits array
    for (int i = 0; i < MAX_CODE_SIZE; i++) {
        new.bits[i] = 0;
    }
    return new;
}

uint32_t code_size(Code *c) {
// 1 must be added to top because it is the index of the last pushed bit
    return c->top;
}

bool code_empty(Code *c) {
    return c->top == 0 ? true : false;
}

bool code_full(Code *c) {
    // top should be MAX_CODE_SIZE * 8 - 1 when the code is full of information
    return c->top == (MAX_CODE_SIZE * 8) ? true : false;
}

bool code_set_bit(Code *c, uint32_t i) {
    if (i > MAX_CODE_SIZE * 8) {
        return false;
    }
    uint32_t bit_list_index = i / 8;
    uint32_t bit_int_index = i % 8;
    c->bits[bit_list_index] |= 1UL << bit_int_index;
    return true; 
}

bool code_clr_bit(Code *c, uint32_t i) {
    if (i > MAX_CODE_SIZE * 8) {
        return false;
    }
    uint32_t bit_list_index = i / 8;
    uint32_t bit_int_index = i % 8;
    c->bits[bit_list_index] &= ~(1UL << bit_int_index);
    return true;
}

bool code_get_bit(Code *c, uint32_t i) {
    if (i > MAX_CODE_SIZE * 8) {
        return false;
    }
    uint32_t num = c->bits[i / 8];
    uint64_t mask = (1UL << (i % 8));
    uint64_t bit = num & mask;
    return bit == mask ? true : false;
}


bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) return false;
    uint32_t top = c->top;
    uint8_t code_bit_index = top / 8;
    uint8_t code_bit_place = top % 8;
    uint8_t shifted_bit = bit << code_bit_place;
    c->bits[code_bit_index] |= shifted_bit;
    c->top += 1;
    return true;
}

bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) return false;
    uint32_t top = c->top - 1;
    *bit = code_get_bit(c, top);
    uint8_t code_bit_index = top / 8;
    uint8_t code_bit_place = top % 8;
    c->bits[code_bit_index] &= ~(1LU << code_bit_place);
    c->top -= 1;
    return true;
}

void code_print(Code *c) {
    for (uint32_t i = 0; i < c->top; i++) {
        printf("%d", code_get_bit(c, i));
    }
    printf("\n");
}

bool div3(uint32_t i) {
    if (i % 3 == 0) {
        return 1;
    } else return 0;
}

// int main(int argc, char const *argv[])
// {
//     Code test = code_init();
//     int wrong = 0;
//     int  max_code_size = MAX_CODE_SIZE;
//     for (uint32_t i = 0; i < MAX_CODE_SIZE * 8; i++) {
//         code_push_bit(&test, div3(i));
//         if (code_get_bit(&test, test.top - 1) != div3(i)) {
//             printf("wrong at %d, %d\n", test.top, code_get_bit(&test, test.top - 1));
//             if (wrong != 1) {
//                 wrong = 1;
//             }
//         }
//     }
//     if (wrong == 0) {
//         printf("success!\n");
//     } else printf("failed\n");
//     code_print(&test);
//     return 0;
// }
