#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

// all functions tested and working

struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

Stack *stack_create(uint32_t capacity) {
    Stack *new = (Stack *)calloc(1, sizeof(Stack));
    new->capacity = capacity;
    new->top = 0;
    new->items = (Node **)calloc(capacity, sizeof(Node *));
    return new;
}

void stack_delete(Stack **s) {
    free((*s)->items);
    free(*s);
    *s = NULL;
}

bool stack_empty(Stack *s) {
    /* 
    if the top of the stack is at its default value
    or has been reduced back down to its default 
    */
    if (s->top == 0) {
        return true;
    } else {
        return false;
    }
}

bool stack_full(Stack *s) {
    // if the top of the stack is the capacity of the stack
    if (s->top == s->capacity) {
        return true;
    } else {
        return false;
    }
}

uint32_t stack_size(Stack *s) {
    return s->top;
}

bool stack_push(Stack *s, Node *n) {
    if (stack_full(s)) {
        return false;
    } else {
        s->items[s->top] = n;
        s->top += 1;
        return true;
    }

}

bool stack_pop(Stack *s, Node **n) {
    if (stack_empty(s)) {
        return false;
    } else {
        *n = s->items[s->top - 1];
        s->top -= 1;
        return true;
    }
    
}

void stack_print(Stack *s) {
    for (uint32_t i = 0; s->items[i] != NULL; i++) {
        node_print(s->items[i]);
    }
}
