# TP1 - Fundamentos de Sistemas Paralelos e Distribuídos
#### Pedro Tavares de Carvalho

Nesse trabalho paralelizamos uma implementação de uma simulação de criação do conjunto de Mandelbrot. Para isso utilizamos uma arquitetura
de *master/worker*, considerando um `thread` mestre que trabalha com entrada e saída de dados e cria tarefas para as `threads` trabalhadoras.

Para passar essas tarefas, foi feita uma fila implementada como uma lista encadeada, com limite por instanciação. A escolha de implementação
foi feita pois, para a coleta das estatísticas de execução de cada uma das tarefas, foi utilizada a mesma estrutura de dados, visto que o conteúdo
de cada nó da lista voi implementado como um `void*`, o que torna o mesmo flexível.

As estatísticas então foram processadas em *batch* ao final do processamento de todo o trabalho, na *thread* principal, depois de todos os
trabalhadores e da `thread` de entrada e saída fazerem `join` com a `main`.

## Benchmarks
### 1 threads
| Command | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| `N_THREADS=1 make run ARGS=bench/test.case` | 16.473 ± 0.022 | 16.440 | 16.502 | 1.00 |
### 8 thread
| Command | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| `N_THREADS=8 make run ARGS=bench/test.case` | 3.329 ± 0.041 | 3.277 | 3.385 | 1.00 |
