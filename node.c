#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// TESTED, WORKING

Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *new = calloc(1, sizeof(Node));
    new->symbol = symbol;
    new->frequency = frequency;
    new->left = NULL;
    new->right = NULL;
    return new;
}

void node_delete(Node **n) {
    free(*n);
    *n = NULL;
}

/*  create a parent node with [lchild, rchild] = [left, right]
    default symbol '$', frequency is the sum of children */
Node *node_join(Node *left, Node *right) {
    Node *parent = calloc(1, sizeof(Node));
    parent->symbol = '$';
    parent->frequency = left->frequency + right->frequency;
    parent->left = left;
    parent->right = right;
    return parent;
}

void node_print(Node *n) {
    printf("symbol: %d, frequency: %llu\n", n->symbol, n->frequency);
}

// compare node frequencies, return true if equal, else false
bool node_cmp(Node *n, Node *m) {
    if (n->frequency > m->frequency) {
        return true;
    } else {
        return false;
    }
}

void node_print_sym(Node *n) {
    printf("symbol: %c", n->symbol);
}
