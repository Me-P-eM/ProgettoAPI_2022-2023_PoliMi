/*
  CercaPercorso riceve in ingresso una stringa contenente l'istruzione da eseguire. Le istruzioni possono essere:
  - "aggiungi-stazione X Y Z_1 Z_2 ..." aggiunge una stazione alla distanza X, che contiene Y automobili, ciascuna di autonomia Z_1, Z_2, ...;
  - "demolisci-stazione X" demolisce la stazione alla distanza X (e rottama tutte le sue auto);
  - "aggiungi-auto X Y" alla stazione di distanza X aggiunge un'auto di autonomia Y;
  - "rottama-auto X Y" alla stazione di distanza X rottama l'auto di autonomia Y;
  - "pianifica-percorso X Y" partendo dalla stazione di distanza X trova il percorso che raggiunge la stazione di distanza Y passando per il minor numero di stazioni possibile.

  Più precisamente, le stazioni vengono salvate in una struttura dati a grafo, ogni vertice del grafo conterrà le informazioni riguardanti la singola stazione tra cui le sue auto,
  immagazzinate in un'altra struttuta dati a lista.
  Le stazioni vengono inoltre salvate in un'altra struttura dati, ovvero un'hash map, che ottimizza le tempistiche per la ricerca delle stazioni.

  Il grafo contiene il numero totale dei suoi vertici numVertices e un vettore ordinato di vertici **adjLists (la cui dimensione è gestita dinamicamente tramite realloc).
  I vertici sono identificati da un numero intero che si riferisce alla relativa distanza della stazione.
  Per la ricerca del cammino minimo è stato implementata la visita in ampiezza mediante l'algoritmo Breadth-First Search.
  In ausilio all'algoritmo è stata implementata una nuova struttura dati: la coda.
  Una volta trovata la distanza minima dal nodo di partenza, parto dal vertice di arrivo e vado a ritroso cercando quelli che stanno a distanza -1 e così via,
  fino a raggiungere quello a distanza 0 che è il nodo di partenza, con l'accortezza che, in caso di vertici con distanza uguale, venga preso quello più vicino all'inizio dell'autostrada.
  Per fare ciò si sfrutta una nuova struttura dati, la lista concatenata, in particolare tutti i vertici analizzati dall'algoritmo BFS vengono inseriti in una lista chiamata visited.
  Infine, le stazioni corrette sono inserite nella lista result che viene stampata a video.
*/



/* INCLUDES */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/* DEFINES */
#define STAZIONEADD "aggiungi-stazione"                               // la funzionialità "aggiungi-stazione"
#define STAZIONERM "demolisci-stazione"                               // la funzionialità "demolisci-stazione"
#define AUTOADD "aggiungi-auto"                                       // la funzionialità "aggiungi-auto"
#define AUTORM "rottama-auto"                                         // la funzionialità "rottama-auto"
#define PERCORSO "pianifica-percorso"                                 // la funzionialità "pianifica-percorso"
#define MAXCHAR 5000                                                  // siccome acquisisco una stringa in ingresso e non conosco la sua dimensione massima, prendo la dimensione massima nel caso peggiore
#define DIMHASHMAP 50000                                              // la dimensione dell'hashmap
#define SKIPSIZE 500                                                  // numero di vertici per lo skip
#define RED true                                                      // un nodo rosso di un albero rosso-nero corrisponde a true
#define BLACK false                                                   // un nodo nero di un albero rosso-nero corrisponde a false



/* STRUCTS */

/* LINKED LIST */
typedef struct list_ {
  struct vertex_ *data;                                               // dati contenuti nella lista (vertici)
  struct list_ *next;                                                 // puntatore all'elemento successivo della lista
} list_t;



/* LINKED LIST 2 */
typedef struct list2_ {
  int data;                                                           // autonomia dell'auto
  struct list2_ *next;                                                // puntatore all'elemento successivo della lista
} list2_t;



/* CODA */
typedef struct queue_ {
  struct vertex_ **data;                                              // dati contenuti nella coda (vertici)
  int length;                                                         // numero di elementi della coda (in altre implementazioni della coda, è il numero massimo di elementi che la coda può avere)
} queue_t;



/* GRAFO CON LISTE DI ADIACENZA */
typedef enum {WHITE, GREY, DARK} color;

typedef struct vertex_ {
  int key;                                                            // valore chiave identificativo del vertice (distanza della stazione)
  struct list2_ *cars;                                                // lista contenente le auto nel vertice
  int maxCar;                                                         // l'auto con la massima autonomia
  color color;                                                        // il colore del vertice utilizzato per l'algoritmo di ricerca
  int distance;                                                       // la distanza del vertice dal nodo di partenza nell'algoritmo di ricerca
  int i;                                                              // indice del vertice corrente nell'adjLists
} vertex_t;

typedef struct graph_ {
  int numVertices;                                                    // numero di vertici presenti nel grafo
  struct vertex_ **adjLists;                                          // puntatore al primo vertice
} graph_t;



/* ALBERO RB E NODI */
typedef struct tree_ {
  struct node_ *root, *NIL;                                           // radice dell'albero, foglia NIL dell'albero
} tree_t;

typedef struct node_ {
  int key;                                                            // valore chiave identificativo del nodo
  bool color;                                                         // rosso = true, nero = false
  struct node_ *left, *right, *parent;                                // figlio sinistro, figlio destro, padre
} node_t;



/* HASHMAP */
typedef struct hashMap_ {
  struct list_ **buckets;
} hashMap_t;



/* FUNZIONI */
list_t* insertList(list_t*, vertex_t*);                               // inserisce un elemento alla lista in testa
list2_t* insertList2(list2_t*, int);                                  // inserisce un elemento alla lista in testa
void insertListOrdered(list_t**, vertex_t*);                          // inserisce un elemento alla lista in modo ordinato
int searchAndDeleteList(list_t**, int);                               // cerca ed elimina un elemento nella lista
bool searchAndDeleteList2(list2_t**, vertex_t*, int);                 // cerca ed elimina un elemento nella lista
bool searchAndDeleteListOrdered(list_t**, int);                       // cerca ed elimina un elemento nella lista mantenendo l'ordine
void viewList(list_t*);                                               // stampa a video la lista
void viewList2(list2_t*);                                             // stampa a video la lista
list_t* clearList(list_t*);                                           // distrugge la lista e i suoi elementi
list2_t* clearList2(list2_t*);                                        // distrugge la lista e i suoi elementi
graph_t* inGraph();                                                   // inizializza il grafo
vertex_t* inVertex(int);                                              // inizializza il vertice
int searchVertexOrdered(graph_t*, int);                               // cerca se nel grafo è presente un determinato vertice
int insertGraphOrdered(graph_t*, vertex_t*, list_t*);                 // inserisce un vertice nel grafo
vertex_t* deleteVertexOrdered(graph_t*, int);                         // cerca ed elimina il vertice
list_t* BFS(graph_t*, int, int);                                      // algoritmo di ricerca BFS
void pianificaPercorso(list_t**, list_t**);                           // cerca il cammino minimo data una lista di stazioni con le relative distanze dal punto di partenza
void destroyGraph(graph_t*);                                          // elimina il grafo
void destroyVertex(vertex_t*);                                        // elimina il vertice
hashMap_t* inHashMap();                                               // inizializza l'hashmap
void viewHashMap(hashMap_t*);                                         // stampa a video l'hashMap
int hashFunction(int, int);                                           // calcola la funzione di hash
int searchHashMap(hashMap_t*, int);                                   // cerca un elemento nell'hashmap
void insertHashMap(hashMap_t*, vertex_t*);                            // inserisce un elemento nell'hashmap
int removeHashMap(hashMap_t*, int);                                   // elimina un elemento dall'hashmap
void destroyHashMap(hashMap_t*);                                      // elimina l'hashmap
void inQueue();                                                       // inizializza la coda q già allocata e presente in una variabile globale
void printQueue();                                                    // stampa a video la coda q presente in una variabile globale
void enqueue(vertex_t*);                                              // aggiunge un vertice alla coda q (variabile globale)
vertex_t* dequeue();                                                  // toglie un vertice dalla coda q (variabile globale)
tree_t* inTree ();                                                    // inizializza albero
node_t* inNode (int);                                                 // inizializza nodo
void insertTree (tree_t*, node_t*);                                   // inserisci un nodo nell'albero
void insertFixup (tree_t*, node_t*);                                  // sistema l'ordinamento dell'albero dopo aver inserito un nodo
void leftRotate (tree_t*, node_t*);                                   // ruota nodi verso sinistra
void rightRotate (tree_t*, node_t*);                                  // ruota nodi verso destra
node_t* searchNode (tree_t*, node_t*, int);                           // cerca un nodo nell'albero
node_t* searchAndDeleteNode (tree_t*, node_t*, int);                  // cerca ed elimina il nodo
void deleteFixup (tree_t*, node_t*);                                  // sistema l'ordinamento dell'albero dopo aver rimosso un nodo
node_t* treeSuccessor(tree_t*, node_t*);                              // successore di un nodo
node_t* treeMinimum(tree_t*, node_t*);                                // partendo da un nodo, trova il minimo andando a sinistra nell'albero
node_t* treeMaximum(tree_t*, node_t*);                                // partendo da un nodo, trova il massimo andando a destra nell'albero
void destroyTree (node_t*, node_t*);                                  // elimina i nodi dell'albero
void freeTree(tree_t*);                                               // elimina l'albero



/* VARIABILI GLOBALI */
int start, end;
queue_t *q;



/* MAIN START */
int main () {
  unsigned int i;
  int j, num, v_i, v_i2;
  char c, *str_tmp;
  graph_t *graph;
  hashMap_t *hashMap;
  vertex_t *vertex_tmp;
  list_t *list_tmp, *visited, *result, *skip;

/* OPERAZIONI PRELIMINARI */
  graph = inGraph();                                                  // INIZIALIZZO IL GRAFO
  hashMap = inHashMap();                                              // INIZIALIZZO L'HASHMAP
  str_tmp = NULL;
  str_tmp = malloc(sizeof(char) * MAXCHAR);                           // INIZIALIZZO LA STRINGA
  q = NULL;
  visited = NULL;                                                     // INIZIALIZZO LA LISTA DEGLI ELEMENTI CHE VISITO DURANTE IL BFS
  result = NULL;                                                      // INIZIALIZZO LA LISTA CONTENENTE LE STAZIONI A DISTANZA MINORE
  skip = NULL;                                                        // INIZIALIZZO LA LISTA CHE MI PERMETTE DI OTTIMIZZARE LO SCORRIMENTO DEL GRAFO

/* INIZIO */
  while (true) {                                                      // CICLO DEL PROGRAMMA
    i = 0;                                                            // <- QUI PARTE L'ACQUISIZIONE DELLA STRINGA, CONTROLLANDO CHE IL FILE NON SIA FINITO
    c = getchar_unlocked();
    while (c != ' ' && c != EOF) {
      *(str_tmp+i) = c;
      i++;
      c = getchar_unlocked();
    }
    if (c == EOF) {
      break;
    }
    *(str_tmp+i) = '\0';                                              // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE

    if (!strcmp(str_tmp, STAZIONEADD)) {                              // <- QUI AVVIENE LA SCELTA E L'ESECUZIONE DEL COMANDO
      i = 0;                                                          // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
      c = getchar_unlocked();
      while (c != ' ') {
        *(str_tmp+i) = c;
        i++;
        c = getchar_unlocked();
      }
      *(str_tmp+i) = '\0';                                            // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
      num = atoi(str_tmp);                                            // CONVERTO LA DISTANZA DELLA STAZIONE DA STRINGA A INTERO
      if (searchHashMap(hashMap, num) == -1) {                        // SE NON ESISTE LA STAZIONE LA INSERISCO
        vertex_tmp = inVertex(num);
        insertHashMap(hashMap, vertex_tmp);
        insertGraphOrdered(graph, vertex_tmp, skip);
        if (graph->numVertices % SKIPSIZE == 0 && graph->numVertices >= SKIPSIZE) { // OGNI SKIPSIZE STAZIONI CHE INSERISCO, UNA LA INSERISCO NELLA LISTA DI SKIP
          insertListOrdered(&skip, vertex_tmp);
        }
        i = 0;                                                        // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
        c = getchar_unlocked();
        while (c != ' ' && c != '\n') {
          *(str_tmp+i) = c;
          i++;
          c = getchar_unlocked();
        }
        *(str_tmp+i) = '\0';                                          // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
        num = atoi(str_tmp);                                          // CONVERTO IL NUMERO DI AUTO DA STRINGA A INTERO
        for (j = num; j > 0; j--) {
          i = 0;                                                      // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
          c = getchar_unlocked();
          while (c != ' ' && c != '\n') {
            *(str_tmp+i) = c;
            i++;
            c = getchar_unlocked();
          }
          *(str_tmp+i) = '\0';                                        // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
          v_i = atoi(str_tmp);                                        // CONVERTO L'AUTONOMIA DELL'AUTO DA STRINGA A INTERO
          vertex_tmp->cars = insertList2(vertex_tmp->cars, v_i);      // INSERISCO L'AUTO NELLA LISTA
          if (vertex_tmp->maxCar < v_i) {                             // SE AGGIUNGO UN'AUTO CON AUTONOMIA MAGGIORE RISPETTO A QUELLA MASSIMA CHE HO ALLORA AGGIORNO
            vertex_tmp->maxCar = v_i;
          }
        }
        puts("aggiunta");                                             // STAMPO IN OUTPUT CHE LA STAZIONE È STATA AGGIUNTA
      } else {
        while ((c = getchar_unlocked()) != '\n' && c != EOF);         // "PULISCO" IL BUFFER DI INPUT
        puts("non aggiunta");
      }

    } else if (!strcmp(str_tmp, STAZIONERM)) {
      i = 0;                                                          // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
      c = getchar_unlocked();
      while (c != '\n') {
        *(str_tmp+i) = c;
        i++;
        c = getchar_unlocked();
      }
      *(str_tmp+i) = '\0';                                            // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
      num = atoi(str_tmp);                                            // CONVERTO LA DISTANZA DELLA STAZIONE DA STRINGA A INTERO
      v_i = removeHashMap(hashMap, num);
      if (v_i != -1) {
        searchAndDeleteListOrdered(&skip, num);
        vertex_tmp = deleteVertexOrdered(graph, v_i);
        destroyVertex(vertex_tmp);
        puts("demolita");
      } else {
        puts("non demolita");
      }

    } else if (!strcmp(str_tmp, AUTOADD)) {
      i = 0;                                                          // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
      c = getchar_unlocked();
      while (c != ' ') {
        *(str_tmp+i) = c;
        i++;
        c = getchar_unlocked();
      }
      *(str_tmp+i) = '\0';                                            // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
      num = atoi(str_tmp);                                            // CONVERTO LA DISTANZA DELLA STAZIONE DA STRINGA A INTERO
      v_i = searchHashMap(hashMap, num);
      if (v_i != -1) {
        i = 0;                                                        // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
        c = getchar_unlocked();
        while (c != '\n') {
          *(str_tmp+i) = c;
          i++;
          c = getchar_unlocked();
        }
        *(str_tmp+i) = '\0';                                          // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
        num = atoi(str_tmp);                                          // CONVERTO L'AUTONOMIA DELL'AUTO DA STRINGA A INTERO
        vertex_tmp = graph->adjLists[v_i];
        vertex_tmp->cars = insertList2(vertex_tmp->cars, num);
        if (vertex_tmp->maxCar < num) {                               // SE AGGIUNGO UN'AUTO CON AUTONOMIA MAGGIORE RISPETTO A QUELLA MASSIMA CHE HO ALLORA AGGIORNO
          vertex_tmp->maxCar = num;
        }
        puts("aggiunta");
      } else {
        while ((c = getchar_unlocked()) != '\n' && c != EOF);         // "PULISCO" IL BUFFER DI INPUT
        puts("non aggiunta");
      }

    } else if (!strcmp(str_tmp, AUTORM)) {
      i = 0;                                                          // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
      c = getchar_unlocked();
      while (c != ' ') {
        *(str_tmp+i) = c;
        i++;
        c = getchar_unlocked();
      }
      *(str_tmp+i) = '\0';                                            // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
      num = atoi(str_tmp);                                            // CONVERTO LA DISTANZA DELLA STAZIONE DA STRINGA A NUMERO INTERO
      v_i = searchHashMap(hashMap, num);
      if (v_i != -1) {
        i = 0;                                                        // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
        c = getchar_unlocked();
        while (c != '\n') {
          *(str_tmp+i) = c;
          i++;
          c = getchar_unlocked();
        }
        *(str_tmp+i) = '\0';                                          // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
        num = atoi(str_tmp);                                          // CONVERTO L'AUTONOMIA DELL'AUTO DA STRINGA A NUMERO INTERO
        vertex_tmp = graph->adjLists[v_i];
        if (searchAndDeleteList2(&vertex_tmp->cars, vertex_tmp, num)) {
          puts("rottamata");
        } else {
          puts("non rottamata");
        }
      } else {
        while ((c = getchar_unlocked()) != '\n' && c != EOF);         // "PULISCO" IL BUFFER DI INPUT
        puts("non rottamata");
      }

    } else if (!strcmp(str_tmp, PERCORSO)) {
      i = 0;                                                          // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
      c = getchar_unlocked();
      while (c != ' ') {
        *(str_tmp+i) = c;
        i++;
        c = getchar_unlocked();
      }
      *(str_tmp+i) = '\0';                                            // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
      start = atoi(str_tmp);                                          // CONVERTO LA DISTANZA DELLA STAZIONE DI PARTENZA
      v_i = searchHashMap(hashMap, start);
      if (v_i != -1) {
        i = 0;                                                        // <- QUI CONTINUA L'ACQUISIZIONE DELLA STRINGA
        c = getchar_unlocked();
        while (c != '\n') {
          *(str_tmp+i) = c;
          i++;
          c = getchar_unlocked();
        }
        *(str_tmp+i) = '\0';                                          // <- QUI FINISCE L'ACQUISIZIONE DELLA STRINGA, INSERENDO IL CARATTERE TERMINATORE
        end = atoi(str_tmp);                                          // CONVERTO LA DISTANZA DELLA STAZIONE DI ARRIVO
        v_i2 = searchHashMap(hashMap, end);
        if (v_i2 != -1) {
          visited = BFS(graph, v_i, v_i2);                            // ELABORO LE DISTANZE DELLE ALTRE STAZIONI DALLA STAZIONE DI PARTENZA
          if (visited->data->key == end) {                            // SE ESISTE UN CAMMINO CHE COLLEGA LA PARTENZA CON L'ARRIVO
            pianificaPercorso(&visited, &result);                     // CERCO IL CAMMINO MINIMO
            list_tmp = result;                                        // STAMPO A VIDEO IL PERCORSO MIGLIORE E MAN MANO LIBERO LA MEMORIA
            result = result->next;
            printf("%d", list_tmp->data->key);
            free(list_tmp);
            list_tmp = NULL;
            while (result != NULL) {
              list_tmp = result;
              result = result->next;
              printf(" %d", list_tmp->data->key);
              free(list_tmp);
              list_tmp = NULL;
            }
            puts("");
          } else {
            visited = clearList(visited);
            puts("nessun percorso");
          }
        } else {
          puts("nessun percorso");
        }
      } else {
        while ((c = getchar_unlocked()) != '\n' && c != EOF);         // "PULISCO" IL BUFFER DI INPUT
        puts("nessun percorso");
      }    
    }
  }
  
/* IL PROGRAMMA TERMINA */
/*
  if (q != NULL) {                                                    // LIBERO LA CODA
    if (q->data != NULL) {
      free(q->data);
    }
    free(q);
    q = NULL;
  }
  free(str_tmp);                                                      // LIBERO LA STRINGA
  str_tmp = NULL;
  destroyGraph(graph);                                                // LIBERO IL GRAFO
  graph = NULL;
  destroyHashMap(hashMap);                                            // LIBERO L'HASHMAP
  hashMap = NULL;
  clearList(skip);                                                    // LIBERO LA LISTA DI SKIP
  skip = NULL;
*/
  return 0;
}



/* DEFINIZIONE DELLE FUNZIONI */

/**
 * @brief inserisce in testa alla lista un elemento
 * 
 * @param h la testa della lista
 * @param vertex il vertice da inserire nella lista
 * @return lista_t* la lista aggiornata con l'elemento appena aggiunto
 */
list_t* insertList(list_t *h, vertex_t *vertex) {
  list_t *tmp;

  tmp = malloc(sizeof(list_t));
  tmp->data = vertex;
  tmp->next = h;
  h = tmp;
  return h;
}

/**
 * @brief inserisce in testa alla lista un elemento
 * 
 * @param h la testa della lista
 * @param num l'autonomia dell'auto da inserire
 * @return lista2_t* la lista aggiornata con l'elemento appena aggiunto
 */
list2_t* insertList2(list2_t *h, int num) {
  list2_t *tmp;

  tmp = malloc(sizeof(list2_t));
  tmp->data = num;
  tmp->next = h;
  h = tmp;
  return h;
}

/**
 * @brief inserisce un elemento in una lista in modo ordinato
 * 
 * @param head la testa della lista
 * @param vetrex il vertice da inserire
*/
void insertListOrdered(list_t **head, vertex_t *vertex) {
  list_t *tmp, *current;

  tmp = malloc(sizeof(list_t));
  tmp->data = vertex;
  tmp->next = NULL;
  if (*head == NULL || vertex->key < (*head)->data->key) {                 // se la lista è vuota o se l'elemento è il più piccolo della lista inserisco in testa
    tmp->next = *head;
    *head = tmp;
    return;
  }  
  current = *head;                                                    // se sono qua devo inserire in mezzo o in coda
  while (current->next != NULL && current->next->data->key <= vertex->key) {
    current = current->next;
  }
  tmp->next = current->next;
  current->next = tmp;
}

/**
 * @brief cerca se è presente un elemento nella lista e lo elimina
 * 
 * @param head la testa della lista
 * @param searchKey la chiave da cercare
 * @return int l'indice del vertice eliminato, se non è stato trovato ritorna -1
 */
int searchAndDeleteList(list_t **head, int searchKey) {
  list_t *prev = NULL;
  list_t *current = *head;

  while (current != NULL) {                                           // scorro la lista
    if (current->data->key == searchKey) {                            // ho trovato il vertice
      if (prev == NULL) {
        *head = current->next;                                        // il vertice è in testa alla lista
      } else {
        prev->next = current->next;
      }
      return current->data->i;
    }
    prev = current;
    current = current->next;
  }
  return -1;
}

/**
 * @brief cerca se è presente l'autonomia dell'auto da eliminare
 * 
 * @param head la testa della lista
 * @param vertex la stazione che contiene l'auto da rottamare
 * @param num l'autonomia da eliminare
 * @return bool l'esito dell'eliminazione 
 */
bool searchAndDeleteList2(list2_t **head, vertex_t *vertex, int num) {
  list2_t *current;
  list2_t *prev;

  if (*head == NULL)
    return false;
  current = *head;
  prev = NULL;
  if (num > vertex->maxCar) {                                         // se l'auto da cercare ha un'autonomia più alta dell'auto con autonomia massima presente nella stazione allora non esiste
    return false;
  }
  if (num == vertex->maxCar) {                                        // se l'auto da eliminare è quella con l'autonomia massima, dovrò ricercare quale sarà l'auto con la nuova massima autonomia
    vertex->maxCar = -1;
    while (current != NULL && current->data != num) {                 // cerca l'auto da eliminare
      prev = current;
      current = current->next;
      if (prev->data > vertex->maxCar) {                              // man mano che scorro tengo aggiornato maxCar
        vertex->maxCar = prev->data;
      }
    }
    if (current != NULL) {                                            // se l'auto è stata trovata
      if (prev == NULL) {                                             // se l'elemento da eliminare è il primo
        *head = current->next;
      } else {
        prev->next = current->next;
      }
      free(current);                                                  // libero la memoria e continuo per aggiornare maxCar
      current = NULL;
      if (*head == NULL) {                                            // se non ho più macchine allora setto maxCar a -1
        vertex->maxCar = -1;
        return true;
      } else {                                                        // se ho altre macchine nella stazione allora finisco di scorrere la lista per aggiornare maxCar
        if (prev == NULL) {                                           // se l'elemento eliminato è il primo
          current = *head;
        } else {
          current = prev->next;
        }
        while (current != NULL) {                                     // aggiorno maxCar continuando il ciclo
          prev = current;
          current = current->next;
          if (prev->data > vertex->maxCar) {                          // se serve aggiorno maxCar
            vertex->maxCar = prev->data;
          }
        }
        if (prev->data > vertex->maxCar) {                            // se serve aggiorno maxCar
          vertex->maxCar = prev->data;
        }
        return true;
      }
    } else {
      return false;
    }
  }                                                                   // se l'auto da eliminare non è quella con la massima autonomia, non dovrò ricercare quale sarà l'auto con la nuova massima autonomia
  while (current != NULL && current->data != num) {                   // cerca l'auto da eliminare
    prev = current;
    current = current->next;
  }
  if (current != NULL) {                                              // se l'auto è stata trovata
    if (prev == NULL) {                                               // se l'elemento da eliminare è il primo
      *head = current->next;
    } else {
      prev->next = current->next;
    }
    free(current);
    current = NULL;
    return true;
  } else {
    return false;
  }
}

/**
 * @brief cerca se è presente un elemento nella lista e lo elimina, mantenendo l'ordine
 * 
 * @param head la testa della lista
 * @param searchKey l'elemento da eliminare
 * @return bool l'esito della rimozione
*/
bool searchAndDeleteListOrdered(list_t **head, int searchKey) {
  list_t *current = *head;
  list_t *prev = NULL;

  while (current != NULL && current->data->key != searchKey) {
    prev = current;
    current = current->next;
  }
  if (current == NULL) {
    return false;
  }
  if (prev == NULL) {                                                 // se l'elemento da eliminare è in testa
    *head = current->next;
  } else {
    prev->next = current->next;
  }
  free(current);
  current = NULL;
  return true;
}

/**
 * @brief stampa a video la lista
 * 
 * @param h la lista da stampare
 */
void viewList(list_t *h) {
  while (h != NULL) {
    printf("Vertice: %d, distanza: %d\n", h->data->key, h->data->distance);
    h = h->next;
  }
}

/**
 * @brief stampa a video la lista
 * 
 * @param h la lista da stampare
 */
void viewList2(list2_t *h) {
  while (h != NULL) {
    printf("Auto: %d\n", h->data);
    h = h->next;
  }
}

/**
 * @brief elimina la lista
 * 
 * @param list la lista da eliminare
 * @return list_t* la lista eliminata
 */
list_t* clearList(list_t *list) {
  list_t *tmp;

  while (list != NULL) {
    tmp = list;
    list = list->next;
    free(tmp);
    tmp = NULL;
  }
  return list;
}

/**
 * @brief elimina la lista
 * 
 * @param list la lista da eliminare
 * @return list_t* la lista eliminata
 */
list2_t* clearList2(list2_t *list) {
  list2_t *tmp;

  while (list != NULL) {
    tmp = list;
    list = list->next;
    free(tmp);
    tmp = NULL;
  }
  return list;
}

/**
 * @brief inizializza il grafo
 * 
 * @return graph_t* il grafo inizializzato
 */
graph_t* inGraph() {
  graph_t *result;

  result = NULL;
  result = malloc(sizeof(graph_t));
  result->numVertices = 0;
  result->adjLists = NULL;
  return result;
}

/**
 * @brief inizializza il vertice
 * 
 * @param num il valore chiave del vertice
 * @return vertex_t* il vertice inizializzato
 */
vertex_t* inVertex(int num) {
  vertex_t *result;

  result = NULL;
  result = malloc (sizeof(vertex_t));
  result->key = num;
  result->cars = NULL;
  result->maxCar = -1;
  result->color = WHITE;
  result->distance = -1;
  result->i = -1;
  return result;
}

/**
 * @brief cerca se un vertice è presente nel grafo
 * 
 * @param graph il grafo
 * @param searchKey la chiave da cercare nei vertici
 * @return int l'indice del vertice in adjLists, se non è presente ritorna -1
 */
int searchVertexOrdered(graph_t *graph, int searchKey) {
  int i;
  vertex_t *currentVertex;

  if (graph->numVertices == 0) {
    return -1;
  }
  i = 0;
  currentVertex = graph->adjLists[i];
  while (i < graph->numVertices) {                                    // scorro il grafo tenendo conto che è ordinato
    currentVertex = graph->adjLists[i];
    if (currentVertex->key == searchKey) {
      return i;
    } else if (currentVertex->key > searchKey) {                      // siccome i vertici sono ordinati, se ho superato il valore allora il vertice non c'è
      break;
    }
    i++;
  }
  return -1;
}

/**
 * @brief inserisce il vertice nel grafo mantenendo l'ordine
 * 
 * @param graph il grafo
 * @param vertex il vertice da aggiungere
 * @param skip la lista per skippare i vertici
 * @return int la posizione del vertice aggiunto della adjLists (il suo indice)
 */
int insertGraphOrdered(graph_t *graph, vertex_t *vertex, list_t *skip) {
  int i, j;

  i = 0;
  while (skip != NULL) {
    if (skip->data->key > vertex->key) {
      break;
    }
    i = skip->data->i;
    skip = skip->next;
  }
  while (i < graph->numVertices && vertex->key > graph->adjLists[i]->key) {   // scorro il grafo
    i++;
  }
  if (i == graph->numVertices) {                                      // se il grafo non ha vertici o se arrivo alla fine del grafo faccio un inserimento in coda
    graph->numVertices++;
    graph->adjLists = realloc(graph->adjLists, (graph->numVertices) * sizeof(vertex_t*)); // alloco dinamicamente la memoria del vettore dei vertici
    graph->adjLists[i] = vertex;
    vertex->i = i;
    return i;
  }
  graph->numVertices++;                                               // se sono qua devo inserire il vertice in mezzo alla lista o in testa
  graph->adjLists = realloc(graph->adjLists, (graph->numVertices) * sizeof(vertex_t*)); // alloco dinamicamente la memoria del vettore dei vertici
  for (j = graph->numVertices-1; j >= i; j--) {
    if (j > i) {
      graph->adjLists[j] = graph->adjLists[j-1];                      // sposto tutti i vertici a destra di uno fino alla posizione desiderata
      graph->adjLists[j]->i = j;
    } else {
      graph->adjLists[i] = vertex;                                    // inserisco il vertice
      vertex->i = i;
    }
  }
  return i;
}

/**
 * @brief elimina il vertice dal grafo mantenendo l'ordine
 * 
 * @param graph il grafo
 * @param i l'indice nell'adjLists del vertice da eliminare
 * @return vertex_t il vertice rimosso dal grafo
 */
vertex_t* deleteVertexOrdered(graph_t *graph, int i) {
  int j;
  vertex_t *vertex;

  vertex = graph->adjLists[i];                                        // il vertice da eliminare
  for (j = i; j < graph->numVertices - 1; j++) {
    graph->adjLists[j] = graph->adjLists[j+1];                        // sposto tutti i vertici a sinistra di uno
    graph->adjLists[j]->i = j;
  }
  graph->numVertices--;
  graph->adjLists = realloc(graph->adjLists, (graph->numVertices) * sizeof(vertex_t*)); // dealloco dinamicamente la memoria del vettore che contiene i vertici
  return vertex;
}

/**
 * @brief esegue l'algoritmo di ricerca del cammino minimo Breadth-First Search 
 * 
 * @param graph il grafo
 * @param v_i l'indice del vertice di partenza
 * @param v_i2 l'indice del vertice di arrivo
 * @return list_t* la lista dei vertici visitati durante l'algoritmo
 */
list_t* BFS(graph_t *graph, int v_i, int v_i2) {
  int i, curr_distance;
  bool flag;                                                          // se esamino la distanza del vertice di destinazione non devo controllare gli altri vertici
  vertex_t *vertex, *v;
  list_t *visited;

  visited = NULL;
  vertex = graph->adjLists[v_i];
  
  if (start < end) {                                                  // se nel cammino devo andare verso destra allora prendo i vertici che stanno a destra del vertice
    for (i = v_i; i <= v_i2; i++) {
      if (i != v_i) {
        graph->adjLists[i]->color = WHITE;
        graph->adjLists[i]->distance = -1;
      } else {
        graph->adjLists[i]->color = GREY;
        graph->adjLists[i]->distance = 0;
      }
    }
  } else {                                                            // se nel cammino devo andare verso sinistra allora prendo i vertici che stanno a sinistra del vertice
    for (i = v_i; i >= v_i2; i--) {
      if (i != v_i) {
        graph->adjLists[i]->color = WHITE;
        graph->adjLists[i]->distance = -1;
      } else {
        graph->adjLists[i]->color = GREY;
        graph->adjLists[i]->distance = 0;
      }
    }
  }

  inQueue();
  enqueue(vertex);
  visited = insertList(visited, vertex);
  flag = false;
  while (q->length > 0 && !flag) {
    vertex = dequeue();
    curr_distance = vertex->key;
    if (start < end) {                                                // se nel cammino devo andare verso destra allora prendo i vertici che stanno a destra del vertice
      for (i = vertex->i + 1; i < graph->numVertices; i++) {
        v = graph->adjLists[i];
        if (vertex->maxCar != -1 && vertex->maxCar >= abs(v->key - curr_distance)) {  // controllo se posso raggiungere la stazione
          if (v->color == WHITE) {
            v->color = GREY;
            v->distance = vertex->distance + 1;
            visited = insertList(visited, v);
            if (v->key == end) {
              flag = true;
              break;
            }
            enqueue(v);
          }
        } else {
          break;
        }
      }
      vertex->color = DARK;
    } else {                                                          // se nel cammino devo andare verso sinistra allora prendo i vertici che stanno a sinistra del vertice
      for (i = vertex->i - 1; i >= 0; i--) {
        v = graph->adjLists[i];
        if (vertex->maxCar != -1 && vertex->maxCar >= abs(v->key - curr_distance)) {  // controllo se posso raggiungere la stazione
          if (v->color == WHITE) {
            v->color = GREY;
            v->distance = vertex->distance + 1;
            visited = insertList(visited, v);
            if (v->key == end) {
              flag = true;
              break;
            }
            enqueue(v);
          }
        } else {
          break;
        }
      }
      vertex->color = DARK;
    }
  }
  return visited;
}

/**
 * @brief cerca il cammino minimo data una lista di stazioni con la relativa distanza,
 * con l'accortezza che, in caso di stazioni con distanza uguale, venga preso quello più vicino all'inizio dell'autostrada.
 * 
 * @param visited le stazioni visitate da BFS
 * @param result le stazioni corrette che rappresentano il percorso minimo
 */
void pianificaPercorso(list_t **visited, list_t **result) {
  int dist;
  bool flag;
  vertex_t *vertex_tmp;
  list_t *list_tmp;

  vertex_tmp = (*visited)->data;                                      // parto dalla stazione di arrivo e procedo a ritroso cercando il vertice a distanza -1
  (*result) = insertList((*result), vertex_tmp);
  dist = vertex_tmp->distance;
  flag = false;
  for (list_tmp = (*visited)->next; list_tmp != NULL; list_tmp = list_tmp->next) {  // controllo tutti i possibili percorsi a ritroso
    if (start < end) {                                                // se il percorso è verso destra
      if (list_tmp->data->distance < dist && list_tmp->data->key < (*result)->data->key) { // se la distanza scende di uno e il vertice si trova a sinistra dell'altro, ho trovato quello più veloce
        if (list_tmp->data->maxCar >= abs((*result)->data->key - list_tmp->data->key) && flag == false) { // controllo se il più veloce è raggiungibile
          vertex_tmp = list_tmp->data;
          flag = true;
        } else if (flag == true && list_tmp->data->key < vertex_tmp->key && list_tmp->data->maxCar >= abs((*result)->data->key - list_tmp->data->key)) {
          vertex_tmp = list_tmp->data;                                // se non è il primo che trovo ed è più vicino all'autostrada rispetto a quello precedentemente trovato ed è raggiungibile, allora devo aggiornare
        }
        if (list_tmp->data->key == start || (vertex_tmp->distance > list_tmp->next->data->distance && flag == true)) { // se sono arrivato alla fine entro, altrimenti se esistono vertici successivi con la stessa distanza prendo i successivi e salto questo passaggio
          dist--;
          (*result) = insertList((*result), vertex_tmp);              // lo inserisco nel risultato
          flag = false;
        }
      }
    } else {                                                          // se il percorso è verso sinistra
      if (list_tmp->data->distance < dist && list_tmp->data->key > (*result)->data->key) { // se la distanza scende di uno e il vertice si trova a destra dell'altro, ho trovato quello più veloce
        if (list_tmp->data->maxCar >= abs((*result)->data->key - list_tmp->data->key) && flag == false) { // controllo se il più veloce è raggiungibile e se è il primo che trovo
          vertex_tmp = list_tmp->data;
          flag = true;                                                // se aggiorno vertex_tmp potrò entrare nell'inserimento
        } else if (flag == true && list_tmp->data->key < vertex_tmp->key && list_tmp->data->maxCar >= abs((*result)->data->key - list_tmp->data->key)) { // se non è il primo che trovo ed è più vicino all'autostrada rispetto a quello precedentemente trovato ed è raggiungibile, allora devo aggiornare
          vertex_tmp = list_tmp->data;
        }
        if (list_tmp->data->key == start || (vertex_tmp->distance > list_tmp->next->data->distance && flag == true)) { // se sono arrivato alla fine entro, altrimenti se esistono vertici successivi con la stessa distanza prendo i successivi e salto questo passaggio
          dist--;
          (*result) = insertList((*result), vertex_tmp);              // lo inserisco nel risultato
          flag = false;
        }
      }
    }
    free((*visited));                                                 // libero la memoria dei visitati man mano che scorro
    (*visited) = NULL;
    (*visited) = list_tmp;
  }
  free((*visited));                                                   // libero l'ultimo elemento dalla lista dei visitati
  (*visited) = NULL;
}

/**
 * @brief libera la memoria del grafo chiamando la funzione destroyVertex per ogni vertice del grafo
 * 
 * @param graph il grafo da deallocare
 */
void destroyGraph(graph_t *graph) {
  int i;

  if (graph != NULL) {
    if (graph->adjLists != NULL) {
      for (i = 0; i < graph->numVertices; i++) {
        if (graph->adjLists[i] != NULL) {
          destroyVertex(graph->adjLists[i]);
        }
      }
      free(graph->adjLists);
    }
    free(graph);
  }
}

/**
 * @brief libera la memoria di un vertice, quindi l'albero contenente le macchine, la lista di adiacenza del vertice e infine il vertice stesso
 * 
 * @param vertex il vertice da deallocare
 */
void destroyVertex(vertex_t *vertex) {
  clearList2(vertex->cars);
  free(vertex);
  vertex = NULL;
}

/**
 * @brief inizializza l'hashmap
 * 
 * @return hashMap_t l'hashmap inizializzata
*/
hashMap_t* inHashMap() {
  int i;
  hashMap_t *result;

  result = NULL;
  result = malloc(sizeof(hashMap_t));
  result->buckets = NULL;
  result->buckets = malloc(sizeof(list_t) * DIMHASHMAP);
  for (i = 0; i < DIMHASHMAP; i++) {
    result->buckets[i] = NULL;
  }
  return result;
}

/**
 * @brief visualizza gli elementi presenti nell'hashmap
 * 
 * @param hashMap l'hashmap
*/
void viewHashMap(hashMap_t *hashMap) {
  int i;
  list_t *current;

  for (i = 0; i < DIMHASHMAP; i++) {
    current = hashMap->buckets[i];
    if (current != NULL) {
      printf("Stazione: %d, indice hashmap: %d, indice adjLists: %d -> ", current->data->key, i, current->data->i);
      current = current->next;
      while (current != NULL) {
        printf("Stazione: %d, indice hashmap: %d, indice adjLists: %d -> ", current->data->key, i, current->data->i);
        current = current->next;
      }
      puts("");
    }
  }
}

/**
 * @brief calcola la funzione di hash
 * 
 * @param key la chiave della stazione che sto aggiungendo/inserendo/...
 * @param size la dimensione dell'hashmap
 * @return int il risultato del calcolo della funzione di hash
*/
int hashFunction(int key, int size) {
  return key % size;
}

/**
 * @brief cerca un elemento nell'hashmap
 * 
 * @param hashMap l'hashmap
 * @param key la chiave da ricercare
 * @return int l'indice della stazione dell'adjLists, se non è stato trovato ritorna -1
*/
int searchHashMap(hashMap_t *hashMap, int key) {
  int i;
  list_t *current;

  i = hashFunction(key, DIMHASHMAP);
  current = hashMap->buckets[i];
  while (current != NULL) {
    if (current->data->key == key) {
      return current->data->i;
    }
    current = current->next;
  }
  return -1;
}

/**
 * @brief inserisce un elemento nell'hashmap
 * 
 * @param hashMap l'hashmap
 * @param vertex il vertice da aggiungere
*/
void insertHashMap(hashMap_t *hashMap, vertex_t *vertex) {
  int i;

  i = hashFunction(vertex->key, DIMHASHMAP);
  hashMap->buckets[i] = insertList(hashMap->buckets[i], vertex);
}

/**
 * @brief rimuove un elemento dell'hashmap
 * 
 * @param hashMap l'hashmap
 * @param key la chiave dell'elemento
 * @return int l'indice della stazione dell'adjLists eliminata, se non è stata eliminata ritorna -1
*/
int removeHashMap(hashMap_t *hashMap, int key) {
  int i;
  list_t *current;

  i = hashFunction(key, DIMHASHMAP);
  current = hashMap->buckets[i];
  i = searchAndDeleteList(&current, key);
  return i;
}

/**
 * @brief libera la memoria dell'hashmap
 * 
 * @param hashMap l'hashmap
*/
void destroyHashMap(hashMap_t *hashMap) {
  int i;
  list_t *tmp, *current;

  if (hashMap != NULL) {
    if (hashMap->buckets != NULL) {
      for (i = 0; i < DIMHASHMAP; i++) {
        current = hashMap->buckets[i];
        while (current != NULL) {
          tmp = current;
          current = current->next;
          free(tmp);
          tmp = NULL;
        }
      }
      free(hashMap->buckets);
    }
    free(hashMap);
  }
}

/**
 * @brief inizializza la coda, tenendo conto che sia già stata effettuata una malloc() in una variabile globale q
 * 
 */
void inQueue() {
  if (q == NULL) {                                                    // se la coda non è ancora stata inizializzata la inizializzo
    q = malloc(sizeof(queue_t));
    q->data = NULL;
    q->length = 0;
    return;
  }
  if (q->length > 0) {                                                // se la coda è già stata inizializzata, controllo se contiene dei dati, in caso affermativo li libero
    free(q->data);
  }
  q->data = NULL;
  q->length = 0;
}

/**
 * @brief stampa a video la coda q
 * 
 */
void printQueue() {
  int i;

  puts("QUEUE");
  for (i = 0; i < q->length; i++) {
    printf("Data %d: %d\n", i, q->data[i]->key);
  }
}

/**
 * @brief aggiunge un elemento alla coda q (variabile globale)
 * 
 * @param x il vertice da aggiungere alla coda
 */
void enqueue(vertex_t *x) {
  q->length++;                                                        // aumento la dimensione della coda
  if (q->length == 1) {
    q->data = malloc(sizeof(vertex_t));
  } else {
    q->data = realloc(q->data, sizeof(vertex_t*) * q->length);        // alloco dinamicamente lo spazio nella coda per l'aggiunta di un vertice
  }
  q->data[q->length-1] = x;                                           // inserisco il vertice
}

/**
 * @brief scarica un vertice presente nella testa della coda q (variabile globale)
 * 
 * @return vertex_t* il vertice scaricato
 */
vertex_t* dequeue() {
  int i;
  vertex_t *x;

  x = q->data[0];                                                     // prendo il vertice in testa alla coda
  q->length--;                                                        // diminuisco la lunghezza della coda
  for (i = 0; i < q->length; i++) {                                   // sposto i vertici nella coda a sinistra di uno
    q->data[i] = q->data[i+1];
  }
  q->data = realloc(q->data, sizeof(vertex_t*) * q->length);          // diminuisco la dimensione allocata per la coda
  
  return x;
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
  result->NIL = NULL;
  result->NIL = malloc (sizeof(node_t));
  result->NIL->key = 0;
  result->NIL->color = BLACK;
  result->NIL->left = NULL;
  result->NIL->right = NULL;
  result->NIL->parent = NULL;
  result->root = result->NIL;
  return result;
}

/**
 * @brief inizializza il nodo
 * 
 * @param num il valore chiave del nodo
 * @return node_t* il nodo inizializzato
 */
node_t* inNode (int num) {
  node_t *result;

  result = NULL;
  result = malloc (sizeof(node_t));
  result->key = num;
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
 * @brief cerca se esiste un nodo nell'albero che corrisponde alla chiave searchKey
 * 
 * @param tree l'albero
 * @param x la radice dell'albero
 * @param searchKey la chiave da ricercare nell'albero
 * @return node_t* il nodo dell'albero che corrisponde alla chiave, se non c'è viene restituita la foglia tree->NIL
 */
node_t* searchNode (tree_t *tree, node_t *x, int searchKey) {
  if (x == tree->NIL)                                                 // Se sono a una foglia ritorno false
    return x;
  else if (x->key == searchKey)                                       // caso base, se arrivo al valore cercato ritorno true
    return x;
  else if (searchKey < x->key) {
    return searchNode(tree, x->left, searchKey);
  } else {
    return searchNode(tree, x->right, searchKey);
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
node_t* searchAndDeleteNode (tree_t *tree, node_t *z, int newKey) {
  node_t *x, *y;

  x = tree->NIL;
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

  w = tree->NIL;
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

  y = tree->NIL;
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
 * @brief partendo da un nodo x, scende nell'albero a destra cercando il massimo
 * 
 * @param tree l'albero
 * @param x il nodo di partenza
 * @return node_t* il nodo più in basso a destra (il massimo)
 */
node_t* treeMaximum(tree_t *tree, node_t *x) {
  while (x->right != tree->NIL) {
    x = x->right;
  }
  return x;
}

/**
 * @brief elimina i nodi dell'albero
 * 
 * @param node la radice
 * @param NIL la foglia
 */
void destroyTree(node_t *node, node_t *NIL) {
  if (node != NIL) {
    destroyTree(node->left, NIL);
    destroyTree(node->right, NIL);
    free(node);
  }
}

/**
 * @brief elimina l'albero
 * 
 * @param tree l'albero
 */
void freeTree(tree_t *tree) {
  if (tree != NULL) {
    destroyTree(tree->root, tree->NIL);
    free(tree->NIL);
    free(tree);
  }
}
