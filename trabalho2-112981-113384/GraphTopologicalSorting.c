//
// Algoritmos e Estruturas de Dados --- 2023/2024
//
// Topological Sorting
//

#include "GraphTopologicalSorting.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "IntegersQueue.h"
#include "instrumentation.h"

struct _GraphTopoSort {
  int* marked;                     // Aux array
  unsigned int* numIncomingEdges;  // Aux array   -> Para contar as arestas apontadas a um vértice
  unsigned int* vertexSequence;    // The result  -> Ordem topológica dos vértices 
  int validResult;                 // 0 or 1      -> Dá para ordenar?
  unsigned int numVertices;        // From the graph
  Graph* graph;
};

/* Definir as macros a serem usadas para a análise da complexidade */
#define VERTEX_ITER InstrCount[0] // Número de iterações do ciclo que percorre os vértices
#define EDGE_ITER InstrCount[1]   // Número de iterações do ciclo que percorre as arestas
#define EDGE_REM InstrCount[2]    // Número de arestas removidas

// AUXILIARY FUNCTION
// Allocate memory for the struct
// And for its array fields
// Initialize all struct fields
//
static GraphTopoSort* _create(Graph* g) {
  assert(g != NULL);

  /* Alocar memória para a estrutura GraphTopoSort */
  GraphTopoSort* p = (GraphTopoSort*)malloc(sizeof(struct _GraphTopoSort));

  /* Verificar a correta alocação de memória */
  if (p == NULL) {
    /* Caso não tenha sido possível alocar memória, retornar NULL */
    return NULL;
  }

  /* Definir o número de vértices para a estrutura */
  p->numVertices = GraphGetNumVertices(g);

  /* Alocar memória para os arrays */
  p->marked = (int*) malloc(p->numVertices * sizeof(int));
  p->numIncomingEdges = (unsigned int*) malloc(p->numVertices * sizeof(unsigned int));
  p->vertexSequence = (unsigned int*) malloc(p->numVertices * sizeof(unsigned int));
  
  /* Verificar a correta alocação de memória */
  if (p->marked == NULL || p->numIncomingEdges == NULL || p->vertexSequence == NULL) {
    /* Caso não tenha sido possível alocar memória, libertar a memória já alocada e retornar NULL */
    free(p->marked);
    free(p->numIncomingEdges);
    free(p->vertexSequence);
    free(p);
    return NULL;
  }

  /* Inicializar as variáveis de GraphTopoSort */
  p->graph = g;
  p->validResult = 0;

  /* Inicializar os arrays */
  /* Nota: esta parte do código pode ser substituída por memset */
  for (unsigned int i = 0; i < p->numVertices; i++) {
    p->marked[i] = 0;
    p->vertexSequence[i] = 0;
    p->numIncomingEdges[i] = GraphGetVertexInDegree(g, i);
  }

  return p;
}

//
// Computing the topological sorting, if any, using the 1st algorithm:
// 1 - Create a copy of the graph
// 2 - Successively identify vertices without incoming edges and remove their
//     outgoing edges
// Check if a valid sorting was computed and set the isValid field
// For instance, by checking if the number of elements in the vertexSequence is
// the number of graph vertices
//
GraphTopoSort* GraphTopoSortComputeV1(Graph* g) {
  assert(g != NULL && GraphIsDigraph(g) == 1);

  // Create and initialize the struct

  GraphTopoSort* topoSort = _create(g);

  /* Criar uma cópia do grafo original */
  Graph* gCopy = GraphCopy(g);
  assert(GraphCheckInvariants(gCopy) == 1); // Confirmar se o grafo é válido

  /* Verificar a correta criação da cópia do grafo */
  if (gCopy == NULL) {
    /* Caso não tenha sido possível criar a cópia do grafo, libertar a memória alocada e retornar NULL */
    GraphTopoSortDestroy(&topoSort);
    return NULL;
  }

  /* Declarar a variável que irá contar o número de vértices adicionados (serve também de índice) */
  unsigned int addedVertices = 0;

  /* Repetir até preencher vertexSequence */
  while (addedVertices < GraphGetNumVertices(gCopy)) {

    /* Variável que verifica se foi encontrado um vértice sem arestas incidentes (no caso de não ser encontrado o grafo possui ciclos) */
    int foundVertex = 0;

    /* Percorrer todos os vértices do grafo */
    for (unsigned int i = 0; i < GraphGetNumVertices(gCopy); i++) {

      /* Incrementar o contador VERTEX_ITER */
      VERTEX_ITER++;

      /* Verificar se o vértice ainda não foi adicionado e se não tem arestas incidentes*/
      if (topoSort->marked[i] == 0 && GraphGetVertexInDegree(gCopy, i) == 0) {

        /* Caso a condição acima se verifique, adicionar o vértice ao array vertexSequence */
        topoSort->vertexSequence[addedVertices] = i;

        /* Incrementar o número de vértices adicionados */
        addedVertices++;

        /* Foi encontrado um vértice sem arestas incidentes */
        foundVertex = 1;

        /* Assinalar o vértice como "marked" */
        topoSort->marked[i] = 1;

        /* Obter o array com os vértices adjacentes a este */
        unsigned int* adjacents = GraphGetAdjacentsTo(gCopy, i);

        /* 
          Percorrer todos os vértices adjacentes
          Nota: o primeiro elemento do array 'adjacents' é o número de vértices adjacentes 
          Por isso começa-se a iteração com j = 1 
        */
        for (unsigned int j = 1; j <= adjacents[0]; j++) {

          /* Incrementar o contador EDGE_ITER */
          EDGE_ITER++;

          /* Remover aresta */
          GraphRemoveEdge(gCopy, i, adjacents[j]);
          assert(GraphCheckInvariants(gCopy) == 1); // Confirmar se o grafo continua válido

          /* Incrementar o contador EDGE_REM */
          EDGE_REM++;

        }

        /* Libertar a memória alocada para o array de vértices adjacentes */
        free(adjacents);

        break;
      }
    }

    /* Verificar se não foi encontrado um vértice sem arestas incidentes */
    if (foundVertex == 0) {
      
      /* Se não foi, então conclui-se que o grafo possui ciclos, e sai-se do loop 'while' */
      break;

    }

  }

  /* Verificar se o número de vértices adicionados é igual ao número de vértices do grafo */
  if (addedVertices == GraphGetNumVertices(gCopy)) {
    /* O resultado é válido */
    topoSort->validResult = 1;
  }

  /* Libertar a memória alocada para a cópia do grafo */
  GraphDestroy(&gCopy);

  /* Devovler a estrutura com os dados da ordem topológica */
  return topoSort;
}

//
// Computing the topological sorting, if any, using the 2nd algorithm
// Check if a valid sorting was computed and set the isValid field
// For instance, by checking if the number of elements in the vertexSequence is
// the number of graph vertices
//
GraphTopoSort* GraphTopoSortComputeV2(Graph* g) {
  assert(g != NULL && GraphIsDigraph(g) == 1);

  // Create and initialize the struct

  GraphTopoSort* topoSort = _create(g);

  /* Declarar a variável que irá contar o número de vértices adicionados */
  unsigned int addedVertices = 0;

  /* Repetir até preencher vertexSequence */
  while (addedVertices < GraphGetNumVertices(g)) {

    /* Variável que verifica se foi encontrado um vértice sem arestas incidentes (no caso de não ser encontrado o grafo possui ciclos) */
    int foundVertex = 0;

    /* Percorrer todos os vértices do grafo */
    for (unsigned int i = 0; i < GraphGetNumVertices(g); i++) {

      /* Incrementar o contador VERTEX_ITER */
      VERTEX_ITER++;

      /* Verificar se o vértice ainda não foi adicionado e se não tem arestas incidentes*/
      if (topoSort->marked[i] == 0 && topoSort->numIncomingEdges[i] == 0) {

        /* Adicionar o vértice ao array vertexSequence */
        topoSort->vertexSequence[addedVertices] = i;

        /* Incrementar o número de vértices adicionados */
        addedVertices++;

        /* Foi encontrado um vértice sem arestas incidentes */
        foundVertex = 1;

        /* Assinalar o vértice como "marked" */
        topoSort->marked[i] = 1;

        /* Array com os vértices adjacentes */
        unsigned int* adjacents = GraphGetAdjacentsTo(g, i);

        /* 
          Percorrer todos os vértices adjacentes
          Nota: o primeiro elemento do array adjacents é o número de vértices adjacentes 
          Por isso começa-se a iteração com j = 1
        */
        for (unsigned int j = 1; j <= adjacents[0]; j++) {

          /* Incrementar o contador EDGE_ITER */
          EDGE_ITER++;

          /* Decrementar o número de arestas incidentes */
          topoSort->numIncomingEdges[adjacents[j]]--;

          /* Incrementar o contador EDGE_REM */
          EDGE_REM++;

        }

        /* Libertar a memória alocada para o array de vértices adjacentes */
        free(adjacents);

        break;
      }
    }

    /* Verificar se não foi encontrado um vértice sem arestas incidentes */
    if (foundVertex == 0) {
      
      /* Se não foi, então conclui-se que o grafo possui ciclos, e sai-se do loop 'while' */
      break;

    }

  }

  /* Verificar se o número de vértices adicionados é igual ao número de vértices do grafo */
  if (addedVertices == GraphGetNumVertices(g)) {
    /* O resultado é válido */
    topoSort->validResult = 1;
  }

  return topoSort;
}

//
// Computing the topological sorting, if any, using the 3rd algorithm
// Check if a valid sorting was computed and set the isValid field
// For instance, by checking if the number of elements in the vertexSequence is
// the number of graph vertices
//
GraphTopoSort* GraphTopoSortComputeV3(Graph* g) {
  assert(g != NULL && GraphIsDigraph(g) == 1);

  // Create and initialize the struct

  GraphTopoSort* topoSort = _create(g);

  /* Criar uma fila */
  Queue* q = QueueCreate(GraphGetNumVertices(g));

  /* Adicionar os vértices com inDegree == 0 na fila */
  for (unsigned int i = 0; i < GraphGetNumVertices(g); i++) {

    /* Incrementar o contador VERTEX_ITER */
    VERTEX_ITER++;

    if (topoSort->numIncomingEdges[i] == 0) {
      QueueEnqueue(q, i);
    }
  }

  /* Declarar a variável que irá contar o número de vértices adicionados */
  unsigned int addedVertices = 0;

  /* Repetir até a fila estar vazia */
  while (!QueueIsEmpty(q)) {

    /* Retirar o vértice da fila */
    unsigned int v = QueueDequeue(q);

    /* Adicionar o vértice ao array vertexSequence */
    topoSort->vertexSequence[addedVertices] = v;

    /* Incrementar o número de vértices adicionados */
    addedVertices++;

    /* Array com os vértices adjacentes */
    unsigned int* adjacents = GraphGetAdjacentsTo(g, v);

    /*
    Percorrer todos os vértices adjacentes
    Nota: o primeiro elemento do array adjacents é o número de vértices adjacentes
    Por isso começa-se a iteração com j = 1
    */
    for (unsigned int j = 1; j <= adjacents[0]; j++) {

      /* Incrementar o contador EDGE_ITER */
      EDGE_ITER++;

      /* Decrementar o número de arestas incidentes */
      topoSort->numIncomingEdges[adjacents[j]]--;

      /* Incrementar o contador EDGE_REM */
      EDGE_REM++;

      /* Verificar se o vértice fica com inDegree == 0 após decrementar o número de arestas incidentes */
      if (topoSort->numIncomingEdges[adjacents[j]] == 0) {
        /* Adicionar o vértice à fila */
        QueueEnqueue(q, adjacents[j]);
      }

    }

    /* Libertar a memória alocada para o array de vértices adjacentes */
    free(adjacents);

  }

  /* Verificar se o número de vértices adicionados é igual ao número de vértices do grafo */
  if (addedVertices == GraphGetNumVertices(g)) {
    /* O resultado é válido */
    topoSort->validResult = 1;
  }

  /* Destruir a fila */
  QueueDestroy(&q);

  return topoSort;
}


void GraphTopoSortDestroy(GraphTopoSort** p) {
  assert(*p != NULL);

  GraphTopoSort* aux = *p;

  free(aux->marked);
  free(aux->numIncomingEdges);
  free(aux->vertexSequence);

  free(*p);
  *p = NULL;
}

//
// A valid sorting was computed?
//
int GraphTopoSortIsValid(const GraphTopoSort* p) { return p->validResult; }

//
// Getting an array containing the topological sequence of vertex indices
// Or NULL, if no sequence could be computed
// MEMORY IS ALLOCATED FOR THE RESULTING ARRAY
//
unsigned int* GraphTopoSortGetSequence(const GraphTopoSort* p) {
  assert(p != NULL);
  
  /* Verificar se o resultado é válido */
  if (GraphTopoSortIsValid(p) == 0) {
    return NULL;
  }

  /* Caso seja válido, retornar o array do resultado */
  return p->vertexSequence;
}

// DISPLAYING on the console

//
// The toplogical sequence of vertex indices, if it could be computed
//
void GraphTopoSortDisplaySequence(const GraphTopoSort* p) {
  assert(p != NULL);

  if (p->validResult == 0) {
    printf(" *** The topological sorting could not be computed!! *** \n");
    return;
  }

  printf("Topological Sorting - Vertex indices:\n");
  for (unsigned int i = 0; i < GraphGetNumVertices(p->graph); i++) {
    printf("%d ", p->vertexSequence[i]);
  }
  printf("\n");
}

//
// The toplogical sequence of vertex indices, if it could be computed
// Followed by the digraph displayed using the adjecency lists
// Adjacency lists are presented in topologic sorted order
//
void GraphTopoSortDisplay(const GraphTopoSort* p) {
  assert(p != NULL);

  // The topological order
  GraphTopoSortDisplaySequence(p);

  if (p->validResult == 0) {
    return;
  }

  // The Digraph
  for (unsigned int i = 0; i < GraphGetNumVertices(p->graph); i++) {
    GraphListAdjacents(p->graph, p->vertexSequence[i]);
  }
  printf("\n");
}

