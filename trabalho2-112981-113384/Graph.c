//
// Algoritmos e Estruturas de Dados --- 2023/2024
//
// Joaquim Madeira, Joao Manuel Rodrigues - June 2021, Nov 2023
//
// Graph - Using a list of adjacency lists representation
//

#include "Graph.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "SortedList.h"
#include "instrumentation.h"

/* Dados de um vértice */
struct _Vertex {
  unsigned int id;          /* ID do vértice */
  unsigned int inDegree;    /* Número de arestas incidentes */
  unsigned int outDegree;   /* Número de arestas que saem dele */
  List* edgesList;          /* Lista de arestas */
};

/* Dados de uma aresta */
struct _Edge {
  unsigned int adjVertex;   /* Vértice adjacente */
  double weight;            /* Custo */
};

/* Cabeçalho de um grafo -> Dados que o compẽm */
struct _GraphHeader {
  int isDigraph;            /* É grafo orientado? 0 ou 1 */
  int isComplete;           /* É completo (tem o número máximo de arestas sem lacetes nem paralelas)? 0 ou 1 */
  int isWeighted;           /* Tem custos nas suas arestas? 0 ou 1 */
  unsigned int numVertices; /* Número de vértices */
  unsigned int numEdges;    /* Número de arestas */
  List* verticesList;       /* Lista dos vértices */
};

/* Definir as macros a serem usadas para a análise da complexidade */
#define VERTEX_ITER InstrCount[0]
#define EDGE_ITER InstrCount[1]
#define EDGE_REM InstrCount[2]

// The comparator for the VERTICES LIST
/* Comparador de vértices com base nos seus IDs (retorna 1 se ID de v1 > ID de v2, -1 caso seja menor e 0 se forem iguais) */
int graphVerticesComparator(const void* p1, const void* p2) {
  unsigned int v1 = ((struct _Vertex*)p1)->id;
  unsigned int v2 = ((struct _Vertex*)p2)->id;
  int d = v1 - v2;
  return (d > 0) - (d < 0);
}

// The comparator for the EDGES LISTS
/* Comparador de arestas com base nos IDs dos respetivos vértices adjacentes (retorna 1 se ID de v1 > ID de v2, -1 caso seja menor e 0 se forem iguais) */
int graphEdgesComparator(const void* p1, const void* p2) {
  unsigned int v1 = ((struct _Edge*)p1)->adjVertex;
  unsigned int v2 = ((struct _Edge*)p2)->adjVertex;
  int d = v1 - v2;
  return (d > 0) - (d < 0);
}


/* Criar um grafo */
Graph* GraphCreate(unsigned int numVertices, int isDigraph, int isWeighted) {
  Graph* g = (Graph*)malloc(sizeof(struct _GraphHeader));
  if (g == NULL) abort();

  /* Inicializar as suas características */
  g->isDigraph = isDigraph;
  g->isComplete = 0;
  g->isWeighted = isWeighted;

  g->numVertices = numVertices;
  g->numEdges = 0;

  /* Criar uma lista com os seus vértices, de acordo com os seus IDs */
  g->verticesList = ListCreate(graphVerticesComparator);

  /* E, para cada vértice... */
  for (unsigned int i = 0; i < numVertices; i++) {
    struct _Vertex* v = (struct _Vertex*)malloc(sizeof(struct _Vertex));
    if (v == NULL) abort();

    v->id = i;        /* ... atribuir-lhe um ID, ... */
    v->inDegree = 0;  /* ... inicializar o seu número de arestas incidentes, ... */
    v->outDegree = 0; /* ... inicializar o número de arestas que saem dele, ... */

    v->edgesList = ListCreate(graphEdgesComparator);  /* ... criar uma lista ordenada, que conterá os vértices adjacente a ele ... */

    ListInsert(g->verticesList, v);                   /* ... e adicionar o vértice à lista de vértices do grafo */
  }

  assert(g->numVertices == ListGetSize(g->verticesList));

  return g;
}


/* Criar um grafo completo */
Graph* GraphCreateComplete(unsigned int numVertices, int isDigraph) {

  /* Começa-se por criar um grafo normal */
  Graph* g = GraphCreate(numVertices, isDigraph, 0);

  g->isComplete = 1;                /* Classifica-se como sendo um grafo completo */

  List* vertices = g->verticesList; /* Obtém-se a lista dos seus vértices */
  ListMoveToHead(vertices);         /* Aponta-se para o primeiro vértice na lista */
  unsigned int i = 0;
  
  /* E, para cada vértice... */
  for (; i < g->numVertices; ListMoveToNext(vertices), i++) {

    /* ... obtém-se o mesmo, ...*/
    struct _Vertex* v = ListGetCurrentItem(vertices);

    /* ... obtém-se a lista das suas arestas/vértices adjacentes, ... */
    List* edges = v->edgesList;

    /*... percorrem-se de novo todos os vértices da lista do grafo, ... */
    for (unsigned int j = 0; j < g->numVertices; j++) {
      /* ... caso seja igual ao vértice da iteração atual, então salta esta iteração (não existem lacetes), ... */ 
      if (i == j) {
        continue;
      }

      /* ... aloca-se memória para uma nova aresta que ligará o vértice da iteração do primeiro ciclo ao da iteração do segundo ciclo, ... */
      struct _Edge* new = (struct _Edge*)malloc(sizeof(struct _Edge));
      if (new == NULL) abort();

      /* ... define-se como vértice adjacente (ligado à aresta criada em cima) o da iteração atual do segundo ciclo, ... */
      new->adjVertex = j;
      new->weight = 1;    // não é weighted

      /* ... adiciona-se o novo vértice adjacente (é adjacente através da aresta new) à lista das arestas do vértice da iteração do primeiro ciclo, ...*/
      ListInsert(edges, new);
    }

    /* ... se for digrafo, ... */
    if (g->isDigraph) {
      v->inDegree = g->numVertices - 1;   /* ... o seu grau de entrada é agora o maior possível num digrafo completo, ... */
      v->outDegree = g->numVertices - 1;  /* ... o seu grau de saída é agora o maior possível num digrafo completo, ... */
    } else {
      /* ... e, finalmente, caso não seja digrafo, ... */
      v->outDegree = g->numVertices - 1;  /* ... então o seu grau é agora o maior possível num grafo completo, ... */
    }
  }

  /* Voltando ao tratamento do grafo completo, ... */ 
  if (g->isDigraph) {
    g->numEdges = numVertices * (numVertices - 1);      /* ... o seu número de arestas é máximo, caso seja tanto um digrafo ... */
  } else {
    g->numEdges = numVertices * (numVertices - 1) / 2;  /* ... como um grafo não orientado */
  }

  /* Devolver o grafo criado */
  return g;
}


/* Destruir um grafo */
void GraphDestroy(Graph** p) {
  assert(*p != NULL);
  Graph* g = *p;

  List* vertices = g->verticesList;
  if (ListIsEmpty(vertices) == 0) {
    ListMoveToHead(vertices);
    unsigned int i = 0;
    for (; i < g->numVertices; ListMoveToNext(vertices), i++) {
      struct _Vertex* v = ListGetCurrentItem(vertices);

      List* edges = v->edgesList;
      if (ListIsEmpty(edges) == 0) {
        unsigned int i = 0;
        ListMoveToHead(edges);
        for (; i < ListGetSize(edges); ListMoveToNext(edges), i++) {
          struct _Edge* e = ListGetCurrentItem(edges);
          free(e);
        }
      }
      ListDestroy(&(v->edgesList));
      free(v);
    }
  }

  ListDestroy(&(g->verticesList));
  free(g);

  *p = NULL;
}


/* Copiar um grafo (função a ser usada numa das implementações de ordenação topológica) */
Graph* GraphCopy(const Graph* g) {
  assert(g != NULL);

  /* Criar um novo grafo com as mesmas propriedades do grafo original */
  Graph* copy = GraphCreate(g->numVertices, g->isDigraph, g->isWeighted);
  assert(copy != NULL);

  /* Lista dos vértices do grafo original */
  List* originalVertices = g->verticesList;

  /* Lista dos vértices do grafo cópia */
  List* copyVertices = copy->verticesList;

  /* Apontar para o início das duas listas */
  ListMoveToHead(originalVertices);
  ListMoveToHead(copyVertices);

  /* Para cada vértice do grafo original, copiá-lo para o grafo cópia */
  for (unsigned int i = 0; i < g->numVertices; ListMoveToNext(originalVertices), ListMoveToNext(copyVertices), i++) {

    /* Incrementar o número de iterações */
    VERTEX_ITER++;

    /* Vértice do grafo g, original */
    struct _Vertex* vOriginal = ListGetCurrentItem(originalVertices);

    /* Vértice do grafo cópia */
    struct _Vertex* vCopy = ListGetCurrentItem(copyVertices);

    /* Ao vértice do grafo cópia, atribuir as características do vérice do grafo original */
    vCopy->id = vOriginal->id;                // esta linha pode ser omitida
    vCopy->inDegree = vOriginal->inDegree;
    vCopy->outDegree = vOriginal->outDegree;

    /* Lista das arestas relativas ao vértice do grafo original (com os respetivos vértices adjacentes) */
    List* vOriginalEdges = vOriginal->edgesList;

    /* Colocar o ponteiro no início da lista referida acima */
    ListMoveToHead(vOriginalEdges);

    /* Para cada aresta do grafo original */
    for (unsigned int j = 0; j < ListGetSize(vOriginalEdges); ListMoveToNext(vOriginalEdges), j++) {

      /* Incrementar o contador EDGE_ITER */
      EDGE_ITER++;

      /* Obter a aresta do grafo g (original) desta iteração */
      struct _Edge* edgeOriginal = ListGetCurrentItem(vOriginalEdges);

      /* Alocar memória para a aresta do grafo cópia desta iteração */
      struct _Edge* edgeCopy = (struct _Edge*) malloc(sizeof(struct _Edge));
      if (edgeCopy == NULL) abort();

      /* Copiar os dados da aresta original para a aresta cópia */
      edgeCopy->adjVertex = edgeOriginal->adjVertex;
      edgeCopy->weight = edgeOriginal->weight;

      /* E adicionar a aresta na lista de arestas do vértice do grafo cópia */
      ListInsert(vCopy->edgesList, edgeCopy);
    }
  }

  /* Copiar o número de arestas do grafo original para o grafo cópia */
  copy->numEdges = g->numEdges;

  /* Devolver a cópia do grafo */
  return copy;
}


/* Ler a informação de um grafo a partir de um ficheiro de texto */
Graph* GraphFromFile(FILE* f) {
  assert(f != NULL);

  /* Ler a primeira linha, que informa se é ou não grafo orientado */
  int isDigraph;
  if (fscanf(f, "%d", &isDigraph) != 1) {
    fprintf(stderr, "Error reading if is digraph\n");
    return NULL;
  }

  /* Ler a segunda linha, que informa se o grafo tem ou não custos associados às arestas */
  int isWeighted;
  if (fscanf(f, "%d", &isWeighted) != 1) {
    fprintf(stderr, "Error reading if is weighted graph\n");
    return NULL;
  }

  /* Ler a terceira linha, que informa sobre o número de vértices do grafo */
  unsigned int numVertices;
  if (fscanf(f, "%u", &numVertices) != 1) {
    fprintf(stderr, "Error reading number of vertices\n");
    return NULL;
  }

  /* Ler a quarta linha, que informa sobre o número de arestas do grafo */
  unsigned int numEdges;
  if (fscanf(f, "%u", &numEdges) != 1) {
    fprintf(stderr, "Error reading number of edges\n");
    return NULL;
  }

  /* Criar um novo grafo */
  Graph* g = GraphCreate(numVertices, isDigraph, isWeighted);
  if (g == NULL) return NULL;

  /* 
    Criar um ciclo 'for' para ler o ficheiro até ao fim. O ficheiro apresenta os dados da seguinte forma:
          -> primeira coluna: vértice inicial;
          -> segunda coluna: vértice final;
          -> terceira coluna: apenas existe se o grafo for weighted, e indica o custo da aresta entre os dois vértices acima referidos. 
  */
  for (unsigned int i = 0; i < numEdges; i++) {

    /* Vértice inicial e vértice final */
    unsigned int v, w;

    /* Variável que conterá o custo da aresta, se for weighted */
    double weight;

    /* Ler os dois vértices */
    if (fscanf(f, "%u %u", &v, &w) != 2) {
      fprintf(stderr, "Error reading edge on line %d\n", 4+i+1);
      GraphDestroy(&g);
      return NULL;
    }

    /* Se os vértices forem iguais, como não existem lacetes, passa-se para a próxima iteração */
    if (v == w) {
      continue;
    }

    /* Ler o custo da aresta, se for weighted */
    if (isWeighted) {
      if (fscanf(f, "%lf", &weight) != 1) {
        fprintf(stderr, "Error reading edge weight on line %d\n", 4+i+1);
        GraphDestroy(&g);
        return NULL;
      }

      /* Adicionar a aresta com custo ao grafo */
      GraphAddWeightedEdge(g, v, w, weight);

    } else {

      /* Adicionar a aresta sem custo ao grafo */
      GraphAddEdge(g, v, w);

    }
  }

  return g;
}

// Graph
/* Saber se é ou não grafo orientado */
int GraphIsDigraph(const Graph* g) { return g->isDigraph; }
/* Saber se é ou não grafo completo */
int GraphIsComplete(const Graph* g) { return g->isComplete; }
/* Saber se é ou não grafo weighted (com custos nas arestas) */
int GraphIsWeighted(const Graph* g) { return g->isWeighted; }
/* Saber o número de vértices do grafo */
unsigned int GraphGetNumVertices(const Graph* g) { return g->numVertices; }
/* Saber o número de arestas no grafo */
unsigned int GraphGetNumEdges(const Graph* g) { return g->numEdges; }


//
// For a graph
//
/* Saber o grau médio dos vértices de um grafo (== soma dos graus todos dos vértices a dividir pelo número de vértices)*/
double GraphGetAverageDegree(const Graph* g) {
  assert(g->isDigraph == 0);
  /* "2.0" para não ser divisão inteira */
  return 2.0 * (double)g->numEdges / (double)g->numVertices;
}


/* Obter o maior grau entre os vértices do grafo (tal e qual como descobrir o máximo de uma lista de inteiros) */
static unsigned int _GetMaxDegree(const Graph* g) {
  List* vertices = g->verticesList;
  if (ListIsEmpty(vertices)) return 0;

  unsigned int maxDegree = 0;
  ListMoveToHead(vertices);
  unsigned int i = 0;
  for (; i < g->numVertices; ListMoveToNext(vertices), i++) {
    struct _Vertex* v = ListGetCurrentItem(vertices);
    if (v->outDegree > maxDegree) {
      maxDegree = v->outDegree;
    }
  }
  return maxDegree;
}


//
// For a graph
//
/* Obter o maior grau entre os vértices de um grafo usando a função acima desenvolvida */
unsigned int GraphGetMaxDegree(const Graph* g) {
  assert(g->isDigraph == 0);
  return _GetMaxDegree(g);
}


//
// For a digraph
//
/* Obter o maior grau de saída entre os vértices de um digrafo usando a função acima desenvolvida */
unsigned int GraphGetMaxOutDegree(const Graph* g) {
  assert(g->isDigraph == 1);
  return _GetMaxDegree(g);
}

// Vertices

//
// returns an array of size (outDegree + 1)
// element 0, stores the number of adjacent vertices
// and is followed by indices of the adjacent vertices
//
/* Obter os vértices adjacentes ao vértice 'v' */
unsigned int* GraphGetAdjacentsTo(const Graph* g, unsigned int v) {
  assert(v < g->numVertices);

  // Node in the list of vertices
  /* Obter a lista de vértices do grafo */
  List* vertices = g->verticesList;

  /* Nessa lista, apontar para a posição onde está o vértice 'v', inserido como argumento */
  ListMove(vertices, v);     

  /* Estando já a apontar para a posição desejada, então obtém-se o item (ponteiro para void) que lá está ('v'), realizando um cast */       
  struct _Vertex* vPointer = ListGetCurrentItem(vertices);  

  /* O número de vértices adjacentes corresponde ao grau de saída do vértice 'v' */
  unsigned int numAdjVertices = vPointer->outDegree;

  /* 
    Aloca espaço para uma lista de double que conterá os vértices adjacentes 
    Tem mais um elemento porque o primeiro elemento conterá apenas o número de véritices adjacentes
  */  
  unsigned int* adjacent = (unsigned int*) calloc(1 + numAdjVertices, sizeof(unsigned int));

  if (numAdjVertices > 0) {

    /* O primeiro elemento corresponde, então, ao número de vértices adjacentes */ 
    adjacent[0] = numAdjVertices;

    /* Obter a lista das arestas adjacentes (aquelas a que o vértice está ligado) */
    List* adjList = vPointer->edgesList;

    /* Apontar para o início dessa lista */
    ListMoveToHead(adjList);

    /* Agora, para cada aresta do array acima (adjacente ao vértice), ... */
    for (unsigned int i = 0; i < numAdjVertices; ListMoveToNext(adjList), i++) {
      /* ... obter o ponteiro para ela ... */
      struct _Edge* ePointer = ListGetCurrentItem(adjList);
      /* ... e adicionar o vértice que está na outra ponta ao array dos vértices adjacentes */
      adjacent[i + 1] = ePointer->adjVertex;
    }
  }

  return adjacent;
}


//
// returns an array of size (outDegree + 1)
// element 0, stores the number of adjacent vertices
// and is followed by the distances to the adjacent vertices
//
/* 
  Obter a distância/o custo da aresta para cada um dos vértices adjacentes de 'v' 
  O processo é muito semelhante ao da função anterior
*/
double* GraphGetDistancesToAdjacents(const Graph* g, unsigned int v) {
  assert(v < g->numVertices);

  // Node in the list of vertices
  List* vertices = g->verticesList;
  ListMove(vertices, v);
  struct _Vertex* vPointer = ListGetCurrentItem(vertices);
  unsigned int numAdjVertices = vPointer->outDegree;

  double* distance = (double*)calloc(1 + numAdjVertices, sizeof(double));

  if (numAdjVertices > 0) {
    distance[0] = numAdjVertices;
    List* adjList = vPointer->edgesList;
    ListMoveToHead(adjList);
    for (unsigned int i = 0; i < numAdjVertices; ListMoveToNext(adjList), i++) {
      struct _Edge* ePointer = ListGetCurrentItem(adjList);
      distance[i + 1] = ePointer->weight;
    }
  }

  return distance;
}


//
// For a graph
//
/* Obter o grau associado a um vértice 'v' num grafo */
unsigned int GraphGetVertexDegree(Graph* g, unsigned int v) {
  assert(g->isDigraph == 0);
  assert(v < g->numVertices);

  ListMove(g->verticesList, v);
  struct _Vertex* p = ListGetCurrentItem(g->verticesList);

  return p->outDegree;
}


//
// For a digraph
//
/* Obter o grau de saída associado a um vértice 'v' num digrafo */
unsigned int GraphGetVertexOutDegree(Graph* g, unsigned int v) {
  assert(g->isDigraph == 1);
  assert(v < g->numVertices);

  ListMove(g->verticesList, v);
  struct _Vertex* p = ListGetCurrentItem(g->verticesList);

  return p->outDegree;
}


//
// For a digraph
//
/* Obter o grau de entrada associado a um vértice 'v' num digrafo */
unsigned int GraphGetVertexInDegree(Graph* g, unsigned int v) {
  assert(g->isDigraph == 1);
  assert(v < g->numVertices);

  ListMove(g->verticesList, v);
  struct _Vertex* p = ListGetCurrentItem(g->verticesList);

  return p->inDegree;
}


// Edges
/* Adicionar uma aresta com ou sem custo a um grafo */
static int _addEdge(Graph* g, unsigned int v, unsigned int w, double weight) {
  struct _Edge* edge = (struct _Edge*)malloc(sizeof(struct _Edge));
  edge->adjVertex = w;
  edge->weight = weight;

  ListMove(g->verticesList, v);
  struct _Vertex* vertex = ListGetCurrentItem(g->verticesList);
  int result = ListInsert(vertex->edgesList, edge);

  if (result == -1) {
    free(edge);
    return 0;
  } else {
    g->numEdges++;
    vertex->outDegree++;

    ListMove(g->verticesList, w);
    struct _Vertex* destVertex = ListGetCurrentItem(g->verticesList);
    destVertex->inDegree++;
  }

  if (g->isDigraph == 0) {
    // Bidirectional edge
    struct _Edge* edge = (struct _Edge*)malloc(sizeof(struct _Edge));
    edge->adjVertex = v;
    edge->weight = weight;

    ListMove(g->verticesList, w);
    struct _Vertex* vertex = ListGetCurrentItem(g->verticesList);
    result = ListInsert(vertex->edgesList, edge);

    if (result == -1) {
      free(edge);
      return 0;
    } else {
      // g->numEdges++; // Do not count the same edge twice on a undirected
      // graph !!
      vertex->outDegree++;
    }
  }

  return 1;
}


/* Adicionar uma aresta sem custo (default == 1) a um grafo usando a função desenvolvida acima */
int GraphAddEdge(Graph* g, unsigned int v, unsigned int w) {
  assert(g->isWeighted == 0);
  assert(v != w);
  assert(v < g->numVertices);
  assert(w < g->numVertices);

  return _addEdge(g, v, w, 1.0);
}


/* Adicionar uma aresta com custo a um grafo usando a função desenvolvida acima */
int GraphAddWeightedEdge(Graph* g, unsigned int v, unsigned int w,
                         double weight) {
  assert(g->isWeighted == 1);
  assert(v != w);
  assert(v < g->numVertices);
  assert(w < g->numVertices);

  return _addEdge(g, v, w, weight);
}


/* Remover uma aresta de um grafo usando os dois vértices extremos desta */
int GraphRemoveEdge(Graph* g, unsigned int v, unsigned int w) {
  assert(g != NULL);

  /* Apontar para a poição do vértice 'v' na lista */
  ListMove(g->verticesList, v);
  /* Obter o seu ponteiro */
  struct _Vertex* vertex = ListGetCurrentItem(g->verticesList);

  /* Apontar para oo início da lista de arestas ligadas ao vértice 'v' */
  ListMoveToHead(vertex->edgesList);

  /* Para cada aresta na lista acima */
  for (unsigned int i = 0; i < ListGetSize(vertex->edgesList); ListMoveToNext(vertex->edgesList), i++) {

    /* Incrementar o contador EDGE_ITER */
    EDGE_ITER++;

    /* Obter o seu ponteiro */
    struct _Edge* e = ListGetCurrentItem(vertex->edgesList);

    /* Verificar se o outro vértice extremo (adjacente a 'v') corresponde ao vértice 'w' da aresta desejada */
    if (e->adjVertex == w) {

      /* Remover a aresta que liga os dois (que corresponde à aresta desta iteração) */
      ListRemoveCurrent(vertex->edgesList);

      /* Libertar a memória associada à aresta */
      free(e);

      /* Sair do ciclo 'for' */
      break;
    }
  }

  /* Atualizar o número de arestas */
  g->numEdges--;

  /* Atualizar o grau do vértice */
  vertex->outDegree--;

  /* Fazer o mesmo para o vértice adjacente: */
  /* Apontar para a poição do vértice 'w' na lista */
  ListMove(g->verticesList, w);
  /* Obter o seu ponteiro */
  struct _Vertex* adjVertex = ListGetCurrentItem(g->verticesList);
  /* Atualizar o seu grau */
  adjVertex->inDegree--;

  /* Se for um grafo não direcionado, remover no sentido oposto, i.e., do vértice adjacente 'w' para 'v' */
  if (!g->isDigraph /*== 0*/) {

    /* Obter o vértice */
    ListMove(g->verticesList, w);
    struct _Vertex* vertex = ListGetCurrentItem(g->verticesList);

    /* Obter a aresta a remover */
    ListMoveToHead(vertex->edgesList);
    for (unsigned int i = 0; i < ListGetSize(vertex->edgesList); ListMoveToNext(vertex->edgesList), i++) {
      struct _Edge* e = ListGetCurrentItem(vertex->edgesList);
      if (e->adjVertex == v) {

        /* Remover a aresta */
        ListRemoveCurrent(vertex->edgesList);

        /* Libertar a memória associada à aresta */
        free(e);
        break;
      }
    }
    /* Atualizar o grau do vértice adjacente */
    vertex->outDegree--;

  }
  return 0;
}

// CHECKING

int GraphCheckInvariants(const Graph* g) {
  assert(g != NULL);
  
  /* Verificar o tamanho da lista dos vértices */
  if (g->numVertices != ListGetSize(g->verticesList)) {
    return 0;
  }

  /* Variáves que contêm os graus dos vértices */
  unsigned int sumOutDegree = 0;
  unsigned int sumInDegree = 0;

  List* vertices = g->verticesList;
  ListMoveToHead(vertices);

  for (unsigned int i = 0; i < g->numVertices; ListMoveToNext(vertices), i++) {

    struct _Vertex* v = ListGetCurrentItem(vertices);

    /* Atualizar as variáveis das somas */
    sumOutDegree += v->outDegree;
    sumInDegree += v->inDegree;

    /* Verificar IDs dos vértices */
    if (v->id > g->numVertices) {
      return 0;  // ID inválido
    }

  }
  
  if (GraphIsDigraph(g)) {

    /* Verificar se as somas são iguais e se igualam numEdges */
    if (sumInDegree != sumOutDegree || sumInDegree != g->numEdges) {
      return 0;
    }
    
  } else {

    /* Verificar se a soma dos graus é igual a 2 * numEdges */
    if (sumOutDegree != 2 * g->numEdges) {
      return 0;
    }
  }


  /* Verificar o número de arestas */
  if (GraphIsComplete(g)) {

    /* Verificar se o grafo é completo */
    if (g->isDigraph) {

      /* Se for digrafo, verificar se o número de arestas é igual ao número máximo de arestas */
      if (g->numEdges != g->numVertices * (g->numVertices - 1)) {
        return 0;
      }
    } else {

      /* Se for grafo, verificar se o número de arestas é igual ao número máximo de arestas */
      if (g->numEdges != g->numVertices * (g->numVertices - 1) / 2) {
        return 0;
      }
    }
  } else {

    /* Se não for completo, verificar se o número de arestas é menor que o número máximo de arestas */
    if (g->isDigraph) {

      if (g->numEdges >= g->numVertices * (g->numVertices - 1)) {
        return 0;
      }
    } else {

      if (g->numEdges >= g->numVertices * (g->numVertices - 1) / 2) {
        return 0;
      }
    }
  }

  /* Todas as condições foram verificadas */
  return 1;
}

// DISPLAYING on the console
/* Imprimir os dados relativos a um grafo 'g' */
void GraphDisplay(const Graph* g) {
  printf("---\n");
  if (g->isWeighted) {
    printf("Weighted ");
  }
  if (g->isComplete) {
    printf("COMPLETE ");
  }
  if (g->isDigraph) {
    printf("Digraph\n");
    printf("Max Out-Degree = %d\n", GraphGetMaxOutDegree(g));
  } else {
    printf("Graph\n");
    printf("Max Degree = %d\n", GraphGetMaxDegree(g));
  }
  printf("Vertices = %2d | Edges = %2d\n", g->numVertices, g->numEdges);

  List* vertices = g->verticesList;
  ListMoveToHead(vertices);
  unsigned int i = 0;
  for (; i < g->numVertices; ListMoveToNext(vertices), i++) {
    printf("%2d ->", i);
    struct _Vertex* v = ListGetCurrentItem(vertices);
    if (ListIsEmpty(v->edgesList)) {
      printf("\n");
    } else {
      List* edges = v->edgesList;
      unsigned int i = 0;
      ListMoveToHead(edges);
      for (; i < ListGetSize(edges); ListMoveToNext(edges), i++) {
        struct _Edge* e = ListGetCurrentItem(edges);
        if (g->isWeighted) {
          printf("   %2d(%4.2f)", e->adjVertex, e->weight);
        } else {
          printf("   %2d", e->adjVertex);
        }
      }
      printf("\n");
    }
  }
  printf("---\n");
}

void GraphListAdjacents(const Graph* g, unsigned int v) {
  printf("---\n");

  unsigned int* array = GraphGetAdjacentsTo(g, v);

  printf("Vertex %d has %d adjacent vertices -> ", v, array[0]);

  for (unsigned int i = 1; i <= array[0]; i++) {
    printf("%d ", array[i]);
  }

  printf("\n");

  free(array);

  printf("---\n");
}
