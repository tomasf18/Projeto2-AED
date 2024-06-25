# Trabalho Prático - TAD GRAPH

## Departamento de Electrónica, Telecomunicações e Informática da Universidade de Aveiro
Curso: Licenciatura em Engenharia Informática   
Cadeira: Algoritmos e Estruturas de Dados  
Ano Letivo: 2023/2024 — 2º ano, 1º Semestre  
`Nota: 18.0`

## Objetivos

O tipo de dados GRAPH foi apresentado nas aulas, permitindo representar e operar sobre grafos e grafos orientados, com ou sem pesos associados às suas arestas. A estrutura de dados utilizada é constituída por uma lista de vértices e, para cada vértice, pela sua lista de adjacências, definidas usando o tipo de dados genérico SORTED-LIST.

O TAD GRAPH fornece apenas as operações básicas sobre grafos, com outros algoritmos implementados em módulos autónomos. Os principais objetivos deste trabalho são:

1. Concluir o desenvolvimento do TAD GRAPH, incluindo funções marcadas como incompletas.
   - Leitura da informação de um grafo a partir de um ficheiro de texto.
   - Criação de uma cópia de um grafo.
   - Remoção de uma aresta (arco orientado) de um grafo orientado.
   - Verificação dos invariantes.

2. Desenvolver algoritmos para determinar a ordenação topológica dos vértices de um grafo orientado, caso exista.

## Ordenação Topológica

Na aula teórico-prática foram apresentados três algoritmos para determinar uma ordenação topológica dos vértices de um grafo orientado. Estes algoritmos devem ser implementados e testados:

1. Algoritmo que usa uma cópia do grafo orientado e efetua sucessivos apagamentos dos arcos emergentes de vértices que não tenham arcos incidentes.
2. Algoritmo que não necessita de uma cópia do grafo dado e usa um array auxiliar para sucessivamente procurar o próximo vértice a juntar à ordenação topológica.
3. Algoritmo que usa uma fila para manter o conjunto dos vértices que irão ser sucessivamente adicionados à ordenação topológica.

## Tarefas Básicas

- Completar as funções do ficheiro `Graph.c`.
- Assegurar que essas funções executam corretamente para os grafos (muito simples) do ficheiro `exemplo1.c`.
- Completar as funções e testar os algoritmos pedidos no módulo `GraphTopologicalSorting`.
- Assegurar que os algoritmos executam corretamente para os dois grafos orientados (muito simples) do `exemplo2.c`.
- Testar os algoritmos com grafos orientados mais complexos.

## Trabalho Adicional – Para Notas Superiores a 16

- Escrever um relatório sucinto (máx. 5 págs.), caracterizando a complexidade dos três algoritmos de ordenação topológica.
- O relatório deve incluir uma breve descrição da(s) métrica(s) adotada(s) para medir a complexidade e uma tabela (gráfico) com os resultados dos testes efetuados.
- A entrega de um relatório não significa por si só que a nota do final do trabalho será superior a 16.

### Atenção – Desenvolvimento do Código:

- Os vértices de um grafo estão sequencialmente numerados: 0, 1, 2, …
- Respeitar os protótipos das funções definidos nos vários ficheiros cabeçalho.
- Pode criar funções auxiliares (static) locais ao módulo `GraphTopologicalSorting`.
- O código desenvolvido deve ser claro e comentado de modo apropriado: os identificadores escolhidos para as variáveis e a estrutura do código, bem como os eventuais comentários, devem ser suficientes para a sua compreensão.
- Não é necessário entregar qualquer relatório relativo ao desenvolvimento do TAD.
- Deverá apenas ser entregue o link para o repositório do trabalho no GitHub.

## Critérios de Avaliação

### Desenvolvimento e Teste das Funções Pedidas:

- Qualidade e clareza do código e comentários (10 pontos).
- Teste do código e verificação de fugas de memória (6 pontos).

### Relatório:

- Aspetos Gerais/Apresentação/Conclusão.
- Complexidade dos algoritmos de ordenação topológica.
  - Dados experimentais.

### Nota:

- O trabalho deve ser realizado em grupos de 2 alunos, mantendo-se os grupos do trabalho anterior, sempre que possível.
- A entrega do trabalho (repositório + relatório opcional) será feita através da plataforma eLearning.
