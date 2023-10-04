# ProgettoAPI
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
