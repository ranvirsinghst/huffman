#include "huffman.h"
#include "io.h"
#include "node.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include "pq.h"

// HUFFMAN
// ALL FUNCTIONS UNTESTED

bool is_leaf(Node *n) {
    if (n->left == NULL && n->right == NULL) {
        return true;
    } else return false;
}

Node *build_tree(uint64_t hist[static ALPHABET]) {
    // creating a min heap for Nodes
    PriorityQueue *pq = pq_create(ALPHABET);
    // root will be returned
    Node *root;
    // temporary nodes for usage in building the tree Node-by-Node
    Node *temp_left;
    Node *temp_right;
    Node *temp_parent;
    // iterate through each letter in the histogram, enqueueing every Node that exists
    for (uint64_t i = 0; i < ALPHABET; i++) {
        if (hist[i] > 0) {
            enqueue(pq, node_create(i, hist[i]));
        }
    }
    // iterate through all Nodes in pq except for the root, joining the Nodes into temp_parent and enqueueing it
    while (pq_size(pq) > 1) {
        dequeue(pq, &temp_left);
        dequeue(pq, &temp_right);
        temp_parent = node_join(temp_left, temp_right);
        enqueue(pq, temp_parent);
    }
    dequeue(pq, &root);
    return root;
}

void build_codes_helper(Node *root, Code table[static ALPHABET], Code *c) {
    uint8_t *holder = (uint8_t *)calloc(1, sizeof(uint8_t));
    if (root != NULL) {
        if (is_leaf(root)) {
            table[root->symbol] = *c;
        } else {
            // traverse left subtree
            code_push_bit(c, 0);
            build_codes_helper(root->left, table, c);
            code_pop_bit(c, holder);
            // traverse right subtree
            code_push_bit(c, 1);
            build_codes_helper(root->right, table, c);
            code_pop_bit(c, holder);
        }
    }
}

void build_codes(Node *root, Code table[static ALPHABET]) {
    Code c = code_init();
    build_codes_helper(root, table, &c);
}

void dump_tree(int outfile, Node *root) {
    uint8_t leaf = 'L';
    uint8_t internal = 'I';
    if (root != NULL) {
        dump_tree(outfile, root->left);
        dump_tree(outfile, root->right);
        uint8_t *sym = &root->symbol;
        if (is_leaf(root)) {
            write_bytes(outfile, &leaf, 1);
            write_bytes(outfile, sym, 1);
        } else {
            write_bytes(outfile, &internal, 1);
        }
    }
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Stack *s = stack_create(nbytes);
    Node *temp_right;
    Node *temp_left;
    for(uint16_t i = 0; i < nbytes; i++) {
        if (tree[i] == 'L') {
            Node *n = node_create(tree[i + 1], 0);
            if (!stack_push(s, n)) {
                printf("error pushing to stack");
            }
            i++;
        } else if (tree[i] == 'I') {
            stack_pop(s, &temp_right);
            stack_pop(s, &temp_left);
            stack_push(s, node_join(temp_left, temp_right));
        }
    }
    // return last node remaining, the root node of the tree
    Node *root;
    stack_pop(s, &root);
    return root;
}

void delete_tree(Node **root) {
    if (*root != NULL) {
        if (is_leaf(*root)) {
            node_delete(root);
        } else {
            delete_tree(&((*root)->left));
            delete_tree(&((*root)->right));
        }
    }
    *root = NULL;
    return;
}
