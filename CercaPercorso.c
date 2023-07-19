#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



#define STAZIONEADD "aggiungi-stazione"                               // la funzionialità "aggiungi-stazione"
#define STAZIONERM "demolisci-stazione"                               // la funzionialità "demolisci-stazione"
#define AUTOADD "aggiungi-auto"                                       // la funzionialità "aggiungi-auto"
#define AUTORM "rottama-auto"                                         // la funzionialità "rottama-auto"
#define PERCORSO "pianifica-percorso"                                 // la funzionialità "pianifica-percorso"
#define MAXCHAR 1024                                                  // Siccome acquisisco una stringa in ingresso e non conosco la sua dimensione massima, prendo la dimensione massima nel caso peggiore
#define RED true                                                      // un nodo rosso di un albero rosso-nero corrisponde a true
#define BLACK false                                                   // un nodo nero di un albero rosso-nero corrisponde a false



/* ALBERO RB E NODI */
typedef struct tree_ {
  struct node_ *root, *NIL;                                           // radice dell'albero, foglia NIL dell'albero
} tree_t;

typedef struct node_ {
  int key;                                                            // valore chiave identificativo del nodo
  bool color;                                                         // rosso = true, nero = false
  struct node_ *left, *right, *parent;                                // figlio sinistro, figlio destro, padre
} node_t;



tree_t* inTree ();                                                    // inizializza albero
node_t* inNode ();                                                    // inizializza nodo
void insertTree (tree_t*, node_t*);                                   // inserisci un nodo nell'albero
void insertFixup (tree_t*, node_t*);                                  // sistema l'ordinamento dell'albero dopo aver inserito un nodo
void leftRotate (tree_t*, node_t*);                                   // ruota nodi verso sinistra
void rightRotate (tree_t*, node_t*);                                  // ruota nodi verso destra
node_t* searchNode (tree_t*, node_t*, int);                           // cerca un nodo nell'albero
node_t* searchAndDelete (tree_t*, node_t*, int);                      // cerca ed elimina il nodo
void deleteFixup (tree_t*, node_t*);                                  // sistema l'ordinamento dell'albero dopo aver rimosso un nodo
node_t* treeSuccessor(tree_t*, node_t*);                              // successore di un nodo
node_t* treeMinimum(tree_t*, node_t*);                                // partendo da un nodo, trova il minimo andando a sinistra nell'albero
void destroyTree (tree_t*, node_t*);                                  // elimina l'albero
void autoadd();
void autorm();
void percorso();

int main (int argc, char *argv[]) {
/* OPERAZIONI PRELIMINARI */
  int i, num;
  char c, *str_tmp, *token;
  tree_t *tree;
  node_t *node_tmp;

  tree = inTree();                                                    // INIZIALIZZO L'ALBERO
  str_tmp = malloc(sizeof(char) * MAXCHAR);                           // INIZIALIZZO LA STRINGA

/* INIZIO */
  while (true) {                                                      // CICLO DEL PROGRAMMA
    i = 0;                                                            // <- QUI PARTE L'ACQUISIZIONE DELLA STRINGA, CONTROLLANDO CHE IL FILE NON SIA FINITO
    c = getchar_unlocked();
    while (c != '\n' && c != EOF) {
      *(str_tmp+i) = c;
      i++;
      c = getchar_unlocked();
    }
    if (c == EOF)
      break;
    *(str_tmp+i) = '\0';                                              // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
    token = strtok(str_tmp, " ");                                     // SEPARO LA STRINGA DI INPUT PER PRENDERE LA PRIMA PAROLA CONTENENTE IL COMANDO

    if (!strcmp(token, STAZIONEADD)) {                                // <- QUI AVVIENE LA SCELTA E L'ESECUZIONE DEL COMANDO
      token = strtok(NULL, " ");                                      // SEPARO LA STRINGA DI INPUT PER PRENDERE LA DISTANZA DELLA STAZIONE
      num = atoi(token);                                              // CONVERTO LA DISTANZA DA STRINGA A INTERO
      if (searchNode(tree, tree->root, num) == tree->NIL) {           // SE NON ESISTE LA STAZIONE ALLORA LA INSERISCO
        node_tmp = inNode();                                          // INIZIALIZZO IL NODO DA AGGIUNGERE
        node_tmp->key = num;                                          // ASSEGNO AL NODO LA CHIAVE (DISTANZA DELLA STAZIONE)
        insertTree(tree, node_tmp);                                   // INSERISCO IL NODO NELL'ALBERO
        puts("aggiunta");                                             // STAMPO IN OUTPUT CHE LA STAZIONE È STATA AGGIUNTA
        token = strtok(NULL, " ");                                    // SEPARO LA STRINGA DI INPUT PER PRENDERE IL NUMERO DI AUTO NELLA STAZIONE
        num = atoi(token);                                            // CONVERTO IL NUMERO DA STRINGA A INTERO
        for (i = num; i > 0; i--) {
          token = strtok(NULL, " ");                                  // SEPARO LA STRINGA DI INPUT PER PRENDERE L'AUTONOMIA DELL'AUTOMOBILE
          num = atoi(token);                                          // CONVERTO IL NUMERO DA STRINGA A INTERO
          // ...
        }
      } else {
        puts("non aggiunta");
      }

    } else if (!strcmp(token, STAZIONERM)) {
      token = strtok(NULL, " ");                                      // SCORRO L'INPUT PRECEDENTEMENTE SEPARATO
      num = atoi(token);                                              // CONVERTO IL NUMERO DA STRINGA A INTERO
      if (searchAndDelete(tree, tree->root, num) != tree->NIL) {
        puts("demolita");
      } else {
        puts("non demolita");
      }
    } else if (!strcmp(token, AUTOADD)) {
      token = strtok(NULL, " ");                                      // SCORRO L'INPUT PRECEDENTEMENTE SEPARATO
      num = atoi(token);
      autoadd();

    } else if (!strcmp(token, AUTORM)) {
      token = strtok(NULL, " ");                                      // SCORRO L'INPUT PRECEDENTEMENTE SEPARATO
      num = atoi(token);
      autorm();

    } else if (!strcmp(token, PERCORSO)) {
      token = strtok(NULL, " ");                                      // SCORRO L'INPUT PRECEDENTEMENTE SEPARATO
      num = atoi(token);
      percorso();
    }
  }
  free(str_tmp);
  destroyTree(tree, tree->root);
  return 0;
}



/**
 * @brief inizializza l'albero
 * 
 * @return tree_t* l'albero inizializzato
 */
tree_t* inTree () {
  tree_t *result;
  result = NULL;
  result = malloc (sizeof(tree_t));
  result->root = NULL;
  result->NIL = NULL;
  result->NIL = malloc (sizeof(node_t));
  result->root = result->NIL;
  return result;
}

/**
 * @brief inizializza il nodo
 * 
 * @return node_t* il nodo inizializzato
 */
node_t* inNode () {
  node_t *result;
  result = NULL;
  result = malloc (sizeof(node_t));
  result->color = BLACK;
  result->left = NULL;
  result->right = NULL;
  result->parent = NULL;
  return result;
}

/**
 * @brief inserisce un nodo nell'albero e chiama insertFixup per ripristinare le proprietà dell'albero rosso-nero
 * 
 * @param tree l'albero in cui inserire il nodo
 * @param z il nodo da inserire nell'albero
 */
void insertTree (tree_t *tree, node_t *z) {
  node_t *x, *y;
  
  y = tree->NIL;                                                      // y padre del nodo considerato
  x = tree->root;                                                     // nodo considerato
  while (x != tree->NIL) {                                            // scorro nell'albero mantenendo l'ordine lessicografico
    y = x;
    if (z->key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  z->parent = y;
  if (y == tree->NIL)
    tree->root = z;                                                   // l'albero tree e' vuoto
  else {                                  
    if (z->key < y->key) {                                            // il nodo è figlio sinistro o destro?
      y->left = z;
    } else {
      y->right = z;
    }
  }
  z->left = tree->NIL;
  z->right = tree->NIL;
  z->color = RED;
  insertFixup (tree, z);
}

/**
 * @brief una volta inserito il nodo, bisogna ripristinare le proprietà dell'albero rosso-nero
 * 
 * @param tree l'albero
 * @param z il nodo appena inserito nell'albero
 */
void insertFixup (tree_t *tree, node_t *z) {
  node_t *x, *y;

  if (z == tree->root)
    tree->root->color = BLACK;
  else {
    x = z->parent;                                                    // x è il padre di z
    if (x->color == RED) {
      if (x == x->parent->left) {                                     // se x è figlio sinistro
        y = x->parent->right;                                         // y è fratello di x
        if (y->color == RED) {
          x->color = BLACK;                                           // caso 1
          y->color = BLACK;                                           // caso 1
          x->parent->color = RED;                                     // caso 1
          insertFixup (tree, x->parent);                              // caso 1
        } else {
          if (z == x->right) {
            z = x;                                                    // caso 2
            leftRotate (tree, z);                                     // caso 2
            x = z->parent;                                            // caso 2
          }
          x->color = BLACK;                                           // caso 3
          x->parent->color = RED;                                     // caso 3
          rightRotate(tree, x->parent);                               // caso 3
        }
      } else {                                                        // se x è il figlio destro
        y = x->parent->left;                                          // y è fratello di x
        if (y->color == RED) {
          x->color = BLACK;                                           // caso 1
          y->color = BLACK;                                           // caso 1
          x->parent->color = RED;                                     // caso 1
          insertFixup (tree, x->parent);                              // caso 1
        } else {
          if (z == x->left) {
            z = x;                                                    // caso 2
            rightRotate (tree, z);                                    // caso 2
            x = z->parent;                                            // caso 2
          }
          x->color = BLACK;                                           // caso 3
          x->parent->color = RED;                                     // caso 3
          leftRotate(tree, x->parent);                                // caso 3
        }
      }
    }
  }
}

/**
 * @brief rotazione a sinistra dell'albero intorno al nodo x
 * 
 * @param tree l'albero
 * @param x il nodo attorno cui fare la rotazione
 */
void leftRotate (tree_t *tree, node_t *x) {
  node_t *y;

  y = x->right;
  x->right = y->left;                                                 // il sottoalbero sinistro di y diventa quello destro di x
  if (y->left != tree->NIL)
    y->left->parent = x;
  y->parent = x->parent;                                              // attacca il padre di x a y
  if (x->parent == tree->NIL)
    tree->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  y->left = x;                                                        // mette x a sinistra di y
  x->parent = y;
}

/**
 * @brief rotazione a destra dell'albero intorno al nodo y
 * 
 * @param tree l'albero
 * @param y il nodo attorno cui fare la rotazione
 */
void rightRotate (tree_t *tree, node_t *y) {
  node_t *x;

  x = y->left;
  y->left = x->right;                                                 // il sottoalbero destro di x diventa quello sinistro di y
  if (x->right != tree->NIL)
    x->right->parent = y;
  x->parent = y->parent;                                              // attacca il padre di y a x
  if (y->parent == tree->NIL)
    tree->root = x;
  else if (y == y->parent->right)
    y->parent->right = x;
  else
    y->parent->left = x;
  x->right = y;                                                       // mette y a destra di x
  y->parent = x;
}

/**
 * @brief cerca se esiste un nodo nell'albero che corrisponde alla chiave newKey
 * 
 * @param tree l'albero
 * @param x la radice dell'albero
 * @param newKey la chiave da ricercare nell'albero
 * @return node_t* il nodo dell'albero che corrisponde alla chiave, se non c'è viene restituita la foglia tree->NIL
 */
node_t* searchNode (tree_t *tree, node_t *x, int newKey) {
  if (x == tree->NIL)                                                 // Se sono a una foglia ritorno false
    return x;
  else if (x->key == newKey)                                          // caso base, se arrivo al valore cercato ritorno true
    return x;
  else if (newKey < x->key) {
    return searchNode(tree, x->left, newKey);
  } else {
    return searchNode(tree, x->right, newKey);
  }
}

/**
 * @brief prima tramite la funzione searchNode si cerca, se esiste, il nodo da eliminare con la relativa chiave,
 * poi se esiste la funzione lo elimina e chiama deleteFixup per ripristinare le proprietà dell'albero rosso-nero,
 * se non esiste non viene eliminato nessun nodo e viene ritornata la foglia tree->NIL
 * 
 * @param tree l'albero
 * @param z la radice dell'albero
 * @param newKey la chiave da ricercare nell'albero
 * @return node_t* il nodo eliminato, se non era presente viene ritornata la foglia tree->NIL
 */
node_t* searchAndDelete (tree_t *tree, node_t *z, int newKey) {
  node_t *x, *y;

  y = tree->NIL;
  z = searchNode(tree, z, newKey);
  if (z != tree->NIL) {
    if (z->left == tree->NIL || z->right == tree->NIL) {
      y = z;
    } else {
      y = treeSuccessor(tree, z); 
    }
    if (y->left != tree->NIL) {
      x = y->left;
    } else {
      x = y->right;
    }
    x->parent = y->parent;                                            // x potrebbe essere tree->NIL
    if (y->parent == tree->NIL) {
      tree->root = x;
    } else if (y == y->parent->left) {
      y->parent->left = x;
    } else {
      y->parent->right = x;
    }
    if (y != z) {
      z->key = y->key;
    }
    if (y->color == BLACK) {
      deleteFixup(tree, x);
    }
  }
  return y;
}

/**
 * @brief una volta eliminato il nodo, bisogna ripristinare le proprietà dell'albero rosso-nero
 * 
 * @param tree l'albero
 * @param x il nodo eliminato
 */
void deleteFixup (tree_t *tree, node_t *x) {
  node_t *w;
  if (x->color == RED || x->parent == tree->NIL) {
    x->color = BLACK;                                                 // caso 0
  } else if (x == x->parent->left) {                                  // x e' figlio sinistro
    w = x->parent->right;                                             // w e' fratello di x
    if (w->color == RED) {
      w->color = BLACK;                                               // caso 1
      x->parent->color = RED;                                         // caso 1
      leftRotate(tree, x->parent);                                    // caso 1
      w = x->parent->right;                                           // caso 1
    }
    if (w->left->color == BLACK && w->right->color == BLACK) {
      w->color = RED;                                                 // caso 2
      deleteFixup(tree, x->parent);                                   // caso 2
    } else {
      if (w->right->color == BLACK) {
        w->left->color = BLACK;                                       // caso 3
        w->color = RED;                                               // caso 3
        rightRotate(tree, w);                                         // caso 3
        w = x->parent->right;                                         // caso 3
      }
      w->color = x->parent->color;                                    // caso 4
      x->parent->color = BLACK;                                       // caso 4
      w->right->color = BLACK;                                        // caso 4
      leftRotate(tree, x->parent);                                    // caso 4
    }
  } else {
    w = x->parent->left;                                              // w e' fratello di x
    if (w->color == RED) {
      w->color = BLACK;                                               // caso 1
      x->parent->color = RED;                                         // caso 1
      rightRotate(tree, x->parent);                                   // caso 1
      w = x->parent->left;                                            // caso 1
    }
    if (w->right->color == BLACK && w->left->color == BLACK) {
      w->color = RED;                                                 // caso 2
      deleteFixup(tree, x->parent);                                   // caso 2
    } else {
      if (w->left->color == BLACK) {
        w->right->color = BLACK;                                      // caso 3
        w->color = RED;                                               // caso 3
        leftRotate(tree, w);                                          // caso 3
        w = x->parent->left;                                          // caso 3
      }
      w->color = x->parent->color;                                    // caso 4
      x->parent->color = BLACK;                                       // caso 4
      w->left->color = BLACK;                                         // caso 4
      rightRotate(tree, x->parent);                                   // caso 4
    }
  }
}

/**
 * @brief trova il nodo successore del nodo x
 * 
 * @param tree l'albero
 * @param x il nodo di partenza di cui si vuole trovare il successore
 * @return node_t* il nodo successore del nodo x
 */
node_t* treeSuccessor(tree_t *tree, node_t *x) {
  node_t *y;
  if (x->right != tree->NIL) {
    return treeMinimum(tree, x->right);
  }
  y = x->parent;
  while (y != tree->NIL && x == y->right) {
    x = y;
    y = y->parent;
  }
  return y;
}

/**
 * @brief partendo da un nodo x, scende nell'albero a sinistra cercando il minimo
 * 
 * @param tree l'albero
 * @param x il nodo di partenza
 * @return node_t* il nodo più in basso a sinistra (il minimo)
 */
node_t* treeMinimum(tree_t *tree, node_t *x) {
  while (x->left != tree->NIL) {
    x = x->left;
  }
  return x;
}

/**
 * @brief libera lo spazio allocato nell'albero
 * 
 * @param tree l'albero
 * @param x la radice dell'albero
 */
void destroyTree (tree_t *tree, node_t *x) {
  if (x != tree->NIL) {
    destroyTree (tree, x->left);
    destroyTree (tree, x->right);
    free (x);
  }
}

void autoadd() {

}

void autorm() {

}

void percorso() {

}