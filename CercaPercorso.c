/* CercaPercorso riceve in ingresso una stringa contenente l'istruzione da eseguire. Le istruzioni possono essere:
  - "aggiungi-stazione X Y Z_1 Z_2 ..." aggiunge una stazione alla distanza X, che contiene Y automobili, ciascuna di autonomia Z_1, Z_2, ...;
  - "demolisci-stazione X" demolisce la stazione alla distanza X (e rottama tutte le sue auto);
  - "aggiungi-auto X Y" alla stazione di distanza X aggiunge un'auto di autonomia Y;
  - "rottama-auto X Y" alla stazione di distanza X rottama l'auto di autonomia Y;
  - "pianifica-percorso X Y" partendo dalla stazione di distanza X trova il percorso che raggiunge la stazione di distanza Y passando per il minor numero di stazioni possibile.

  Più precisamente, le stazioni vengono salvate in una struttura dati a grafo, ogni vertice del grafo conterrà le informazioni riguardanti la singola stazione tra cui le sue auto,
  immagazzinate in un'altra struttuta dati ad albero rosso-nero.

  Il grafo contiene il numero totale dei suoi vertici numVertices e un vettore di vertici **adjLists (la cui dimensione è gestita dinamicamente tramite realloc).
  I vertici sono identificati da un numero intero che si riferisce alla relativa distanza della stazione.
  I collegamenti tra i vertici sono gestiti dalle funzioni addCarEdge e addStationEdge, settando minNext e maxNext (bound),
  addAutoEdge controlla se la stazione può raggiungere altre stazioni dopo che ho aggiunto/rimosso un'auto,
  addStationEdge controlla se la stazione può essere raggiunta dalle altre quando aggiungo una stazione,
  con l'accortezza di mantenere gli indici corretti (minNext e maxNext) quando si aggiungono/tolgono i vertici al grafo (stazioni).
  I collegamenti direzionali che vanno da un vertice verso gli altri vertici sono presenti implicitamente nel campo maxNext (analogo per minNext),
  dove sarà presente l'indice in **adjLists del vertice più distante che un vertice può raggiungere (se A raggiunge C e B sta tra A e C allora A raggiunge anche B).
  Inoltre, ogni vertice avrà un array di int *prev che contiene tutti gli indici in **adjLists dei vertici da cui un vertice può essere raggiunto.
  Per la ricerca del cammino minimo è stato implementata la visita in ampiezza mediante l'algoritmo Breadth-First Search.
  In ausilio all'algoritmo è stata implementata una nuova struttura dati: la coda.
  Una volta trovata la distanza minima dal nodo di partenza, parto dal vertice di arrivo e vado a ritroso cercando quelli che stanno a distanza -1 e così via,
  fino a raggiungere quello a distanza 0 che è il nodo di partenza. Per fare ciò si sfrutta *prev.

  L'albero rosso-nero ha una radice "root" e una foglia "NIL" alla quale tutte le foglie sono collegate.
  Ogni nodo dell'albero ha un genitore "parent", un figlio sinistro "left" e un figlio destro "right",
  oltre che al suo valore identificativo (in questo caso l'autonomia dell'auto) e a un identificatore del colore (rosso o nero).
  Dato un nodo, se scendo a sinistra trovo il minore, se scendo a destra invece il maggiore; inoltre con le regole
  dei colori rosso/nero, si riesce a "bilanciare" l'albero da un punto di vista spaziale.
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
#define MAXCHAR 1024                                                  // siccome acquisisco una stringa in ingresso e non conosco la sua dimensione massima, prendo la dimensione massima nel caso peggiore
#define RED true                                                      // un nodo rosso di un albero rosso-nero corrisponde a true
#define BLACK false                                                   // un nodo nero di un albero rosso-nero corrisponde a false



/* STRUCTS */

/* CODA */
typedef struct queue_ {
  struct vertex_ **data;                                              // dati contenuti nella coda (vertici)
  int head;                                                           // puntatore alla testa della coda (indice del primo elemento)
  int tail;                                                           // puntatore alla coda della coda (indice del prossimo elemento da inserire)
  int length;                                                         // numero di elementi della coda (in altre implementazioni della coda, è il numero massimo di elementi che la coda può avere)
} queue_t;



/* GRAFO CON LISTE DI ADIACENZA */
typedef enum {WHITE, GREY, DARK} Color;

typedef struct vertex_ {
  int key;                                                            // valore chiave identificativo del vertice (distanza della stazione)
  struct tree_ *cars;                                                 // albero contenente le auto nel vertice
  Color color;                                                        // il colore del vertice utilizzato per l'algoritmo di ricerca
  int distance;                                                       // la distanza del vertice dal nodo di partenza nell'algoritmo di ricerca
  int maxNext;                                                        // indice del vertice di massima distanza verso dx della stazione che posso raggiungere
  int minNext;                                                        // indice del vertice di massima distanza verso sx della stazione che posso raggiungere
  int *prev;                                                          // array contenente gli indici relativi ad adjLists dei vertici da cui il vertice può essere raggiunto
  int dimPrev;                                                        // numero di vertici da cui il vertice è raggiungibile, aka numero di elementi di *prev
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



/* FUNZIONI */
graph_t* inGraph();                                                   // inizializza il grafo
vertex_t* inVertex();                                                 // inizializza il vertice
void printGraph(graph_t*);                                            // stampa a video il grafo
int searchVertexOrdered(graph_t*, int);                               // cerca se nel grafo è presente un determinato vertice
int insertGraphOrdered(graph_t*, vertex_t*);                          // inserisce un vertice nel grafo
vertex_t* searchAndDeleteVertexOrdered(graph_t*, int);                // cerca ed elimina il vertice
void addCarEdge(graph_t*, node_t*, int);                              // aggiunge le frecce per connettere i vertici quando aggiungo un'auto (compila minNext e maxNext del vertice dove ho aggiunto/rimosso l'auto)
void addStationEdge(graph_t*, int);                                   // aggiunge le frecce per connettere i vertici quando aggiungo una stazione (compila minNext e maxNext degli altri vertici)
void BFS(graph_t*, int);                                              // algoritmo di ricerca BFS
void destroyGraph(graph_t*);                                          // elimina il grafo
void destroyVertex(vertex_t*);                                        // elimina il vertice
void inQueue();                                                       // inizializza la coda q già allocata e presente in una variabile globale
void printQueue();                                                    // stampa a video la coda q presente in una variabile globale
void enqueue(vertex_t*);                                              // aggiunge un vertice alla coda q (variabile globale)
vertex_t* dequeue();                                                  // toglie un vertice dalla coda q (variabile globale)
tree_t* inTree ();                                                    // inizializza albero
node_t* inNode ();                                                    // inizializza nodo
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
void destroyTree (tree_t*, node_t*);                                  // elimina l'albero



/* VARIABILI GLOBALI */
int start, end;
queue_t *q;



/* MAIN START */
int main (int argc, char *argv[]) {
  int i, num, v_i, v_i2, dist;
  char c, *str_tmp, *token;
  graph_t *graph;
  vertex_t *vertex_tmp;
  tree_t *tree_tmp;
  node_t *node_tmp;

/* OPERAZIONI PRELIMINARI */
  graph = inGraph();                                                  // INIZIALIZZO IL GRAFO
  str_tmp = malloc(sizeof(char) * MAXCHAR);                           // INIZIALIZZO LA STRINGA
  q = malloc(sizeof(queue_t));                                        // INIZIALIZZO LA CODA

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
      if (searchVertexOrdered(graph, num) == -1) {                    // SE NON ESISTE LA STAZIONE ALLORA LA INSERISCO
        vertex_tmp = inVertex();                                      // INIZIALIZZO IL VERTICE DA AGGIUNGERE
        vertex_tmp->key = num;                                        // ASSEGNO AL VERTICE LA CHIAVE (DISTANZA DELLA STAZIONE)
        graph->adjLists = realloc(graph->adjLists, (graph->numVertices+1) * sizeof(vertex_t*)); // ALLOCO DINAMICAMENTE LA MEMORIA AL VETTORE CONTENENTE I VERTICI
        v_i = insertGraphOrdered(graph, vertex_tmp);                  // INSERISCO IL VERTICE NEL GRAFO
        token = strtok(NULL, " ");                                    // SEPARO LA STRINGA DI INPUT PER PRENDERE IL NUMERO DI AUTO NELLA STAZIONE
        num = atoi(token);                                            // CONVERTO IL NUMERO DA STRINGA A INTERO
        tree_tmp = inTree();
        vertex_tmp->cars = tree_tmp;
        for (i = num; i > 0; i--) {
          token = strtok(NULL, " ");                                  // SEPARO LA STRINGA DI INPUT PER PRENDERE L'AUTONOMIA DELL'AUTOMOBILE
          v_i2 = atoi(token);                                         // CONVERTO IL NUMERO DA STRINGA A INTERO
          node_tmp = inNode();
          node_tmp->key = v_i2;
          insertTree(tree_tmp, node_tmp);                             // INSERISCO L'AUTO NELL'ALBERO
        }
        if (num != 0) {                                               // AGGIUNGO SE SERVE IL COLLEGAMENTO DAL VERTICE AGLI ALTRI VERTICI
          node_tmp = treeMaximum(graph->adjLists[v_i]->cars, graph->adjLists[v_i]->cars->root);
          addCarEdge(graph, node_tmp, v_i);
        }
        addStationEdge(graph, v_i);                                   // CONTROLLO SE LA STAZIONE PUò ESSERE RAGGIUNTA DALLE ALTRE
        puts("aggiunta");                                             // STAMPO IN OUTPUT CHE LA STAZIONE È STATA AGGIUNTA
      } else {
        puts("non aggiunta");
      }

    } else if (!strcmp(token, STAZIONERM)) {
      token = strtok(NULL, " ");                                      // SCORRO L'INPUT PRECEDENTEMENTE SEPARATO
      num = atoi(token);                                              // CONVERTO IL NUMERO DA STRINGA A INTERO
      vertex_tmp = searchAndDeleteVertexOrdered(graph, num);
      if (vertex_tmp != NULL) {
        destroyVertex(vertex_tmp);
        graph->adjLists = realloc(graph->adjLists, (graph->numVertices) * sizeof(vertex_t*)); // DEALLOCO DINAMICAMENTE LA MEMORIA AL VETTORE CONTENENTE I VERTICI
        puts("demolita");
      } else {
        puts("non demolita");
      }

    } else if (!strcmp(token, AUTOADD)) {
      token = strtok(NULL, " ");                                      // SEPARO LA STRINGA DI INPUT PER PRENDERE LA DISTANZA DELLA STAZIONE
      num = atoi(token);
      v_i = searchVertexOrdered(graph, num);
      if (v_i != -1) {
        token = strtok(NULL, " ");                                    // SEPARO LA STRINGA DI INPUT PER PRENDERE L'AUTONOMIA DELL'AUTO
        num = atoi(token);
        node_tmp = inNode();
        node_tmp->key = num;
        vertex_tmp = graph->adjLists[v_i];
        insertTree(vertex_tmp->cars, node_tmp);
        addCarEdge(graph, node_tmp, v_i);
        puts("aggiunta");
      } else {
        puts("non aggiunta");
      }

    } else if (!strcmp(token, AUTORM)) {
      token = strtok(NULL, " ");                                      // SEPARO LA STRINGA DI INPUT PER PRENDERE LA DISTANZA DELLA STAZIONE
      num = atoi(token);
      v_i = searchVertexOrdered(graph, num);
      if (v_i != -1) {
        token = strtok(NULL, " ");                                    // SEPARO LA STRINGA DI INPUT PER PRENDERE L'AUTONOMIA DELL'AUTO
        num = atoi(token);
        vertex_tmp = graph->adjLists[v_i];
        node_tmp = searchAndDeleteNode(vertex_tmp->cars, vertex_tmp->cars->root, num);
        if (node_tmp != vertex_tmp->cars->NIL) {
          num = node_tmp->key;                                        // L'AUTONOMIA DELL'AUTO RIMOSSA
          free(node_tmp);
          if (graph->adjLists[v_i]->cars->root != graph->adjLists[v_i]->cars->NIL) {  // SE ESISTE ALMENO UN'ALTRA AUTO
            node_tmp = treeMaximum(graph->adjLists[v_i]->cars, graph->adjLists[v_i]->cars->root); // TROVO LA MACCHINA CON LA MAGGIOR AUTONOMIA
            if (num > node_tmp->key) {                                // SE L'AUTO CHE HO RIMOSSO AVEVA UN'AUTONOMIA MAGGIORE RISPETTO ALLE ALTRE AUTO ALLORA FORZO LA RICOMPILAZIONE DEI COLLEGAMENTI TRA IL VERTICE E GLI ALTRI
              addCarEdge(graph, node_tmp, v_i);
            }
          } else {                                                    // SE NON CI SONO PIÙ AUTO NELLA STAZIONE
            graph->adjLists[v_i]->maxNext = -1;                       // NON POSSO PIÙ RAGGIUNGERE NESSUNA STAZIONE
            graph->adjLists[v_i]->minNext = -1;
          }
          puts("rottamata");
        } else {
          puts("non rottamata");
        }
      } else {
        puts("non rottamata");
      }

    } else if (!strcmp(token, PERCORSO)) {
      token = strtok(NULL, " ");                                      // SEPARO LA STRINGA DI INPUT PER PRENDERE LA STAZIONE DI PARTENZA
      start = atoi(token);
      v_i = searchVertexOrdered(graph, start);
      if (v_i != -1) {
        token = strtok(NULL, " ");                                    // SEPARO LA STRINGA DI INPUT PER PRENDERE LA STAZIONE DI ARRIVO
        end = atoi(token);
        v_i2 = searchVertexOrdered(graph, end);
        if (v_i2 != -1) {
          BFS(graph, v_i);
          dist = graph->adjLists[v_i2]->distance;
          for (i = 0, vertex_tmp = graph->adjLists[v_i]; dist == -1; i++) {
            printf("%d ",vertex_tmp->key);
          }
        } else {
          puts("nessun percorso");
        }
      } else {
        puts("nessun percorso");
      }    
    }
  }
  free(q);
  free(str_tmp);
  destroyGraph(graph);
  return 0;
}



/* DEFINIZIONE DELLE FUNZIONI */

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
 * @return vertex_t* il vertice inizializzato
 */
vertex_t* inVertex() {
  vertex_t *result;

  result = NULL;
  result = malloc (sizeof(vertex_t));
  result->maxNext = -1;
  result->minNext = -1;
  return result;
}

/**
 * @brief stampa a video il grafo
 * 
 * @param graph il grafo
 */
void printGraph(graph_t *graph) {
  int i, j;

  for (i = 0; i < graph->numVertices; i++) {                          // scorro i vertici del grafo
    printf("Vertex %d\n: ", graph->adjLists[i]->key);                 // stampo il vertice nel grafo
    for (j = graph->adjLists[i]->minNext; j != -1 && j <= graph->adjLists[i]->maxNext; j++) { // scorro i vertici che un vertice può raggiungere
      if (i != j) {
        printf("%d -> ", graph->adjLists[j]->key);                    // stampo il corrispondente valore
      }
    }
    puts("");
    //printf("MIN: %d - MAX: %d\n", graph->adjLists[i]->minNext, graph->adjLists[i]->maxNext);
  }
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

  if (graph->adjLists == NULL) {
    return -1;
  }
  i = 0;
  currentVertex = graph->adjLists[i];
  while (i < graph->numVertices && currentVertex->key <= searchKey) { // scorro il grafo tenendo conto che è ordinato
    if (currentVertex->key == searchKey) {
      return i;
    }
    i++;
    currentVertex = graph->adjLists[i];
  }
  if (i == graph->numVertices) {                                      // se il vertice non è stato trovato    
    return -1;
  }
  if (currentVertex->key != searchKey) {                              // se il vertice non c'è e la ricerca è stata interrotta prematuramente per l'ottimizzazione sull'ordinamento
    return -1;
  }
  return -1;
}

/**
 * @brief inserisce un vertice nel grafo mantenendo l'ordine
 * 
 * @param graph il grafo
 * @param vertex il vertice da aggiungere
 * @return int la posizione del vertice aggiunto della adjLists (il suo indice)
 */
int insertGraphOrdered(graph_t *graph, vertex_t *vertex) {
  int i, j;

  i = 0;
  if (graph->adjLists[i] == NULL) {                                   // se il grafo non ha vertici viene inserito in testa
    graph->adjLists[i] = vertex;
    graph->numVertices++;
    return i;
  }
  while (i < graph->numVertices && vertex->key > graph->adjLists[i]->key) { // scorro il grafo
    i++;
  }
  if (i == graph->numVertices) {                                      // se arrivo alla fine del grafo faccio un inserimento in coda
    graph->adjLists[i] = vertex;
    graph->numVertices++;
    return i;
  }
  for (j = graph->numVertices; j >= 0; j--) {                         // se sono qua devo inserire il vertice in mezzo alla lista o in testa
    if (j > i) {
      graph->adjLists[j] = graph->adjLists[j-1];                      // sposto tutti i vertici a destra di uno fino alla posizione desiderata
      graph->adjLists[j]->maxNext++;                                  // sistemo il limite superiore
      if (graph->adjLists[j]->minNext >= i) {
        graph->adjLists[j]->minNext++;                                // sistemo il limite inferiore
      }
    } else if (j == i) {
      graph->adjLists[i] = vertex;                                    // inserisco il vertice
      graph->numVertices++;
    } else {
      if (graph->adjLists[j]->maxNext >= i) {
        graph->adjLists[j]->maxNext++;
      }
      if (graph->adjLists[j]->minNext >= i) {
        graph->adjLists[j]->minNext++;                                // sistemo il limite inferiore
      }
    }
  }
  return i;
}

/**
 * @brief cerca ed elimina il vertice dal grafo mantenendo l'ordine
 * 
 * @param graph il grafo
 * @param searchKey la chiave nel vertice da eliminare
 * @return vertex_t* il vertice eliminato, se non è stato trovato ritorna NULL
 */
vertex_t* searchAndDeleteVertexOrdered(graph_t *graph, int searchKey) {
  int i, j;
  vertex_t *vertex;

  i = 0;
  while (i < graph->numVertices && graph->adjLists[i]->key <= searchKey) {// cerco il vertice da eliminare
    if (graph->adjLists[i]->key == searchKey) {
      break;
    }
    i++;
  }
  if (i == graph->numVertices) {                                      // se il vertice non è stato trovato
    return NULL;
  }
  if (graph->adjLists[i]->key != searchKey) {                         // se il vertice non c'è e la ricerca è stata interrotta prematuramente per l'ottimizzazione sull'ordinamento
    return NULL;
  }      
  vertex = graph->adjLists[i];                                        // se ho trovato il vertice da eliminare
  if (i == graph->numVertices-1) {                                    // se il vertice da eliminare è la coda del grafo
    graph->adjLists[i] = NULL;
    for (j = 0; j < graph->numVertices-1; j++) {                      // sistemo maxNext
      if (graph->adjLists[j]->maxNext == i) {
        graph->adjLists[j]->maxNext--; 
      }                               
    }
  } else {                                                            // se il vertice da eliminare è in mezzo o se è la testa
    for (j = 0; j < graph->numVertices-1; j++) {                      // parto da 0 per fixare maxNext
      if (j < i) {
        if (graph->adjLists[j]->maxNext >= i) {
          graph->adjLists[j]->maxNext--;
        }
      } else {                                                        // arrivato a i, sposto anche i vertici a sinistra di uno
        graph->adjLists[j] = graph->adjLists[j+1];                    // sposto tutti i vertici a sinistra di uno
        if (graph->adjLists[j]->maxNext >= i) {
          graph->adjLists[j]->maxNext--;
        }
        if (graph->adjLists[j]->minNext > i) {                        // sistemo minNext
          graph->adjLists[j]->minNext--;                                
        }
      }
    }
  }
  graph->numVertices--;
  return vertex;
}

/**
 * @brief aggiunge uno o più collegamenti tra due vertici quando aggiungo un'auto, settando prima maxNext, poi minNext, infine compilando *prev
 * 
 * @param graph il grafo
 * @param car l'auto inserita
 * @param v_i l'indice del vertice dove l'auto è situata
 */
void addCarEdge(graph_t *graph, node_t *car, int v_i) {
  int i, curr_distance;
  bool found;

  if (treeMaximum(graph->adjLists[v_i]->cars, graph->adjLists[v_i]->cars->root) == car) { // se l'auto che aggiungo ha un'autonomia maggiore rispetto a quelle che ho già allora posso aggiungere gli archi
    curr_distance = graph->adjLists[v_i]->key;
    for (i = graph->numVertices-1, found = false; i >= 0 && !found; i--) {  // scorro i vertici nel grafo
      if (car->key >= abs(graph->adjLists[i]->key - curr_distance)) { // controllo se non sto considerando il vertice stesso e se posso raggiungere la stazione i da v_i
        graph->adjLists[v_i]->maxNext = i;                            // mi salvo l'indice relativo all'adjLists del vertice massimo a dx che posso raggiungere 
        found = true;
      }
    }
    for (i = 0, found = false; i <= graph->adjLists[v_i]->maxNext && !found; i++) { // scorro i vertici nel grafo
      if (car->key >= abs(graph->adjLists[i]->key - curr_distance)) { // controllo se non sto considerando il vertice stesso e se posso raggiungere la stazione i da v_i
        graph->adjLists[v_i]->minNext = i;                            // mi salvo l'indice relativo all'adjLists del vertice massimo a sx che posso raggiungere 
        found = true;
      }
    }
    for (i = graph->adjLists[v_i]->minNext; i != -1 && i <= graph->adjLists[v_i]->maxNext; i++) {  // qua compilo invece dimPrev rimanendo nel bound di vertici raggiungibili che ho trovato
      if (i != v_i) {
        graph->adjLists[i]->dimPrev++;
        graph->adjLists[i]->prev = realloc(graph->adjLists[i]->prev, sizeof(int) * graph->adjLists[i]->dimPrev);  // alloco dinamicamente lo spazio al campo *prev del vertice che può essere raggiunto dal vertice che sto elaborando
        graph->adjLists[i]->prev[graph->adjLists[i]->dimPrev - 1] = v_i;  // mi salvo l'indice relativo all'adjLists del vertice che è raggiunto
      }
    }
  }
}

/**
 * @brief verifica se la stazione aggiunta può essere raggiunta dalle altre stazioni
 * 
 * @param graph il grafo
 * @param v_i l'indice del vertice (stazione) aggiunto
 */
void addStationEdge(graph_t *graph, int v_i) {
  int i;                                                              // l'indice corrispondente alla stazione
  node_t *node;                                                       // l'auto di autonomia massima presente nella stazione

  for (i = 0; i < graph->numVertices; i++) {                          // scorro tra i vertici del grafo
    if (i != v_i && graph->adjLists[i]->cars->root != graph->adjLists[i]->cars->NIL) {  // se il vertice che elaboro è diverso da quello che ho appena aggiunto e se contiene almeno un'auto
      node = treeMaximum(graph->adjLists[i]->cars, graph->adjLists[i]->cars->root); // prendo l'auto con la massima autonomia
      if (node->key >= abs(graph->adjLists[i]->key - graph->adjLists[v_i]->key)) {  // controllo se posso raggiungere la stazione i da v_i
        if (graph->adjLists[i]->minNext == -1 || v_i < graph->adjLists[i]->minNext) { // se non ho ancora settato minNext o se la stazione è precedente a minNext allora aggiorno minNext
          graph->adjLists[i]->minNext = v_i;
        }
        if (graph->adjLists[i]->maxNext == -1 || v_i > graph->adjLists[i]->maxNext) {  // se non ho ancora settato maxNext o se la stazione è successiva a maxNext allora aggiorno maxNext
          graph->adjLists[i]->maxNext = v_i;
        }
      }
    }
  }
}

/**
 * @brief esegue l'algoritmo di ricerca del cammino minimo Breadth-First Search 
 * 
 * @param graph il grafo
 * @param v_i l'indice del nodo di partenza
 */
void BFS(graph_t *graph, int v_i) {
  int i;
  bool flag;                                                          // se esamino la distanza del vertice di destinazione non devo controllare gli altri vertici
  vertex_t *vertex, *v;

  for (i = 0, vertex = graph->adjLists[v_i]; i < graph->numVertices; i++) {
    if (graph->adjLists[i] != vertex) {
      graph->adjLists[i]->color = WHITE;
      graph->adjLists[i]->distance = -1;
    }
  }
  vertex->color = GREY;
  vertex->distance = 0;
  inQueue();
  enqueue(vertex);
  flag = false;
  while (q->length > 0 && !flag) {
    vertex = dequeue();
    for (i = vertex->minNext; i <= vertex->maxNext; i++) {
      if (i != v_i) {
        v = graph->adjLists[i];
        if (v->color == WHITE) {
          v->color = GREY;
          v->distance = (vertex->distance) + 1;
          if (v->key == end) {
            flag = true;
            break;
          }
          enqueue(v);
        }
      }
    }
    vertex->color = DARK;
  }
}

/**
 * @brief libera la memoria del grafo chiamando la funzione destroyVertex per ogni vertice del grafo
 * 
 * @param graph il grafo da deallocare
 */
void destroyGraph(graph_t *graph) {
  int i;

  for (i = 0; i < graph->numVertices; i++) {
    destroyVertex(graph->adjLists[i]);
  }
  if (graph->numVertices > 0) {
    free(graph->adjLists);
  }
  free(graph);
}

/**
 * @brief libera la memoria di un vertice, quindi l'albero contenente le macchine, la lista di adiacenza del vertice e infine il vertice stesso
 * 
 * @param vertex il vertice da deallocare
 */
void destroyVertex(vertex_t *vertex) {
  destroyTree(vertex->cars, vertex->cars->root);
  if (vertex->dimPrev > 0) {
    free(vertex->prev);
  }
  free(vertex);
}

/**
 * @brief inizializza la coda, tenendo conto che sia già stata effettuata una malloc() in una variabile globale q
 * 
 * @return queue_t* la coda inizializzata
 */
void inQueue() {
  q->head = 0;
  q->tail = 0;
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
  q->data = realloc(q->data, sizeof(vertex_t*) * q->length);          // alloco dinamicamente lo spazio nella coda per l'aggiunta di un vertice
  q->data[q->tail] = x;                                               // inserisco il vertice
  q->tail++;                                                          // aumento la posizione della coda
}

/**
 * @brief scarica un vertice presente nella testa della coda q (variabile globale)
 * 
 * @return vertex_t* il vertice scaricato
 */
vertex_t* dequeue() {
  int i;
  vertex_t *x;

  if (q->head == q->tail) {
    puts("error underflow");
    return NULL;
  }
  x = q->data[q->head];                                               // prendo il vertice in testa alla coda
  q->length--;                                                        // diminuisco la lunghezza della coda
  for (i = 0; i < q->length; i++) {                                   // sposto i vertici nella coda a sinistra di uno
    q->data[i] = q->data[i+1];
  }
  q->tail--;
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
 * @brief libera lo spazio allocato nell'albero
 * 
 * @param tree l'albero
 * @param x la radice dell'albero
 */
void destroyTree (tree_t *tree, node_t *x) {
  if (x != tree->NIL) {
    destroyTree (tree, x->left);
    destroyTree (tree, x->right);
    if (x != tree->root) {
      x->color = BLACK;
      free (x);
    } else {
      free(tree->root);
      free(tree->NIL);
      free(tree);
    }
  }
}

/* TODO
  Implementa *prev.
  Implementa l'algoritmo pianifica-percorso facendo che BFS() tiene traccia dei vertici visitati in una lista apposita (da implementare),
  poi partendo dal nodo di arrivo vai a vedere in quella lista quale ha distanza -1 a ritroso tramite *prev
  Forse si può evitare la addStationEdge() e aggiungere i link nella insertGraphOrdered().
  Ricontrolla destroyTree(). Dovrebbe essere giusta.
  La searchAndDeleteNode() è giusta? E la deleteFixup()? Dovrebbero essere giuste.
  Implementare uno scorrimento iterativo in un albero con *next è più efficiente per eliminarlo nella destroyTree()?
*/
