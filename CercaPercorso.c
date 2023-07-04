#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



#define AGGIUNGI_STAZIONE "aggiungi_stazione"
#define ECCESSO 18



/* LISTA */
typedef struct list_ {
  char *res;
  struct list_ *next;  
  char key[];
} list_t;


/* ALBERO RB E NODI */
typedef struct tree_ {
  struct node_ *root, *NIL, *first, *tag1, *tag2;
} tree_t;

typedef struct node_ {
  bool color, delete;                                                 // rosso = true, nero = false
  struct node_ *left, *right, *parent, *next;
  char key[];
} node_t;



void insertList (int, char*);



int main () {
  return 0;
}



void insertList (int int, char* string) {
}