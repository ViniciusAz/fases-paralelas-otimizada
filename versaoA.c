#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

main(int argc, char** argv)
  {
  /* Parametros */    
  int pedaco_troca = 30;       // 1/% do pedaco que deve ser trocado com o vizinho
  int tam_vet      = 600000;   // Tamanho total do Vetor a ser Ordenado
  int rand_range   = 1000000;  // intervalo de numeros que serao randomizados, ex: 100 = entre 0 e 99
  int debug        = 0;        // prints debug
  int debug_limite = 0;        // ativa limite iteracoes (para entender caso entre em loop - problema que estava tendo com rand)
  int limite = 20;             // numero de limite de iteracoes que o programa vai rodar, caso debug_limite = 1
  int so_faz_um_bs = 0;        // se = 1, entao so fará BS uma vez
  int ja_fez_bs = 0;           // nao alterar

  /* Variaveis */
  int my_rank;          // Identificador deste processo
  int np;               // Numero de processos disparados pelo usuario na linha de comando (np)
  int vetor[tam_vet];   // Vetor 
  int i,j,k,l;          // Variavel Auxiliar para For
  int tam_vet_pedaco;   // Tamanho do Vetor para Cada Pedaco   
  int ultimo;           // Variavel Auxiliar para Ultima Posicao do Vetor do Vizinho
  int ordenado;         // Flag que verifica se está ordenado!
  int result_troca;     // Resultado da soma do Vetor de Troca
  MPI_Status status;    // estrutura que guarda o estado de retorno   
  clock_t total;        // calculo de tempo de execucao
  clock_t t_comp, comp; // calculo de tempo de comparacao
  clock_t t_troc, troc; // calculo de tempo de trocas


  MPI_Init(&argc , &argv);                 // funcao que inicializa o MPI, todo o codigo paralelo estah abaixo
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // pega pega o numero do processo atual (rank)
  MPI_Comm_size(MPI_COMM_WORLD, &np);      // pega informacao do numero de processos (quantidade total)

    ordenado = 0;
    tam_vet_pedaco = tam_vet/np;                     // Tamanho do Pedaco que cada Processo vai ordenar
    int vet_aux[tam_vet_pedaco];                     // Vetor auxiliar que realiza a ordenacao
    int troca[np];                                   // Flag para saber se precisa trocar pedacos do vetor
    int aux[(tam_vet_pedaco*pedaco_troca/100)];      // Vetor Auxiliar para Envio das Trocas
    int aux1[(tam_vet_pedaco*pedaco_troca/100)];     // Vetor Auxiliar para Envio das Trocas
    int aux2[(tam_vet_pedaco*pedaco_troca/100 * 2)]; // Vetor para fazer o interleaving
    int i1, i2, i_aux, tam;                               // variaveis auxiliares para fazer o interleaving
    //printf("Tam Metade = %d", tam_vet_pedaco);

    srand(time(NULL));

  /*-*-*-*-*-*-*-*-*-*-* Inicio do Processo , o Rank 0 esta coordenando -*-*-*-*-*-*-*-*-*-* */ 
  if ( my_rank == 0 ){ // sou o primeiro?
    for(i = 0; i < tam_vet; i++){
        //vetor[i] = tam_vet - i;
        vetor[i] = rand() % rand_range;
    }
    // Manda para todo mundo o vetor
    k = tam_vet_pedaco;
    l = 0;
    for(i = 1; i < np; i++){
        l = 0;
        for(j = k; j < (k + tam_vet_pedaco); j++){
            vet_aux[l] = vetor[j];
            l++;
        }
        k = k + tam_vet_pedaco;
        MPI_Send(&vet_aux, tam_vet_pedaco, MPI_INT, i, 1, MPI_COMM_WORLD); 
    }
  }else
    MPI_Recv(&vet_aux, tam_vet_pedaco, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // recebo primeiro vetor

    // Arrumar o vetor do Rank 0
    if(my_rank == 0)
        for(i = 0; i < tam_vet_pedaco; i++){
            vet_aux[i] = vetor[i];
        }

  /* -*-*-*-*-*-*-*-*-*-* Fim do Processo inicial -*-*-*-*-*-*-*-*-*-* */  

    total = clock();
    t_troc = 0;
    t_comp = 0;

    while(ordenado == 0){
        if(so_faz_um_bs == 0 || (so_faz_um_bs == 1 && ja_fez_bs == 0)) {
            bs(tam_vet_pedaco,vet_aux);
            ja_fez_bs = 1;
        }


        comp = clock();
        troca[my_rank] = 0;                           // Informa que a troca nao e necessaria

        // print  VETOR ordenado do PROCESSO 
        if(debug == 1) { printf(" \n BS PID : %d = ", my_rank); for(i = 0; i < tam_vet_pedaco; i++) printf(" %d, ", vet_aux[i]); printf(" \n"); }


        if(my_rank != (np-1))                         // Se eu nao for o ultimo eu mando para meu vizinho da direita
            MPI_Send(&vet_aux[tam_vet_pedaco - 1], 1, MPI_INT, (my_rank+1), 1, MPI_COMM_WORLD); 
        if(my_rank != 0){
            MPI_Recv(&ultimo, 1, MPI_INT, (my_rank -1), MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(ultimo > vet_aux[0]){ // Precisa trocar
                troca[my_rank] = 1;
                if(debug == 1) { printf("\n Sou o PID %d e o ult é [%d] > {%d} primeiro e troca = %d  ",my_rank ,ultimo, vet_aux[0], troca[my_rank]); }
            }else{         
                if(debug == 1) { printf("\n Sou o PID %d e o ult é [%d] > {%d} primeiro e troca = %d  ",my_rank ,ultimo, vet_aux[0], troca[my_rank]); } // Nao precisa Trocar
                troca[my_rank] = 0;
            }
        }

        // Multicast
        for(i = 0; i < np; i++)
            MPI_Bcast(&troca[i], 1, MPI_INT, i, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        // Imprimo o Vetor de Troca 
        result_troca = 0;
        for(i = 0; i < np; i++){
            //printf("\n Sou o %d e o valor do TROCA = %d ",my_rank ,troca[i]);
            result_troca = result_troca + troca[i];
        }
        if(debug == 1) { if(my_rank == 0) printf("\n result troca = %d", result_troca); }
        if(result_troca > 0){
            ordenado = 0;
            //printf("\n VOU ORDENAR MAIS!%d", ordenado);

        }else{
            ordenado = 1;
            //printf("\n ACABEI %d", ordenado);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        t_comp += clock() - comp;
                
        /* Processo de Troca */
        troc = clock();
        
        if(troca[my_rank] == 1 && my_rank > 0){
            //printf(" \n DEBUG Proc1 Troca Pid: %d : ", my_rank);
            for(i = 0; i < (tam_vet_pedaco*pedaco_troca/100);i++){
                aux[i] = vet_aux[i];
            }
            MPI_Send(&aux, (tam_vet_pedaco*pedaco_troca/100), MPI_INT, (my_rank-1), 1, MPI_COMM_WORLD); 
            MPI_Recv(&aux1, (tam_vet_pedaco*pedaco_troca/100), MPI_INT, (my_rank-1), MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            for(i = 0; i < (tam_vet_pedaco*pedaco_troca/100);i++){
                vet_aux[i] = aux1[i];
            }
            /* DEBUG VETOR FINAL do PROCESSO */
            /*printf(" \n DEBUG TROCOU Pid: %d : ", my_rank);
            for(i = 0; i < tam_vet_pedaco; i++){
                printf(" %d, ", vet_aux[i]);
            }
            printf(" \n");*/
        }

        if(troca[my_rank + 1] == 1 && my_rank < (np-1)){
            //printf(" \n DEBUG Proc2 Troca Pid: %d : ", my_rank);
            MPI_Recv(&aux, (tam_vet_pedaco*pedaco_troca/100), MPI_INT, (my_rank+1), MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            k = 0;
            for(i = tam_vet_pedaco - (tam_vet_pedaco*pedaco_troca/100); i < tam_vet_pedaco;i++){
                aux1[k] = vet_aux[i];
                k++;
            }

    	    //interleaving 
            tam = (tam_vet_pedaco*pedaco_troca/100 * 2); //pedaco final + inicial que dois processos estao trocando
            i1 = 0;
            i2 = 0;

            for (i_aux = 0; i_aux < tam; i_aux++) {
                if (((aux[i1] <= aux1[i2]) && (i1 < (tam / 2))) || (i2 == (tam / 2)))
                    aux2[i_aux] = aux[i1++];
                else
                    aux2[i_aux] = aux1[i2++];
            }

            if(debug == 1) { printf(" \n Interleaving Pid: %d : ", my_rank); for(i = 0; i < tam; i++){ printf(" %d, ", aux2[i]); } printf("\n "); }

            for(i = 0; i < tam/2; i++) {
                aux[i] = aux2[i];
                aux1[i] = aux2[i+(tam/2)];
            }
            //fim interleaving

            k = 0;
            for(i = tam_vet_pedaco - (tam_vet_pedaco*pedaco_troca/100); i < tam_vet_pedaco;i++){
                vet_aux[i] = aux[k];
                k++;
            }
            MPI_Send(&aux1, (tam_vet_pedaco*pedaco_troca/100), MPI_INT, (my_rank+1), 1, MPI_COMM_WORLD); 
        }
        
        
        MPI_Barrier(MPI_COMM_WORLD);
        t_troc += clock() - troc;

        
        if(debug_limite == 1) { if(limite == 0) ordenado = 1; limite--; }
    }
     

    /* DEBUG VETOR FINAL do PROCESSO */
    if(debug == 1) { printf(" \n VETOR FINAL Pid: %d : ", my_rank); for(i = 0; i < tam_vet_pedaco; i++){ printf(" %d, ", vet_aux[i]); } printf("\n "); }

    total = clock() - total;
    double time_taken = ((double)total)/CLOCKS_PER_SEC; // in seconds 
    double time_troc = ((double)t_troc)/CLOCKS_PER_SEC; // in seconds 
    double time_comp = ((double)t_comp)/CLOCKS_PER_SEC; // in seconds 
    double troc_percent = (time_troc * 100) / time_taken;
    double comp_percent = (time_comp * 100) / time_taken;
    if(my_rank == 0) printf("Tempo total = %.2f segundos para ordenar %d mil elementos (-np %d) \n", time_taken, (tam_vet/1000), np);
    if(my_rank == 0) printf("Comparação = %.1f segundos (%.2f%s) - Troca = %.1f segundos (%.2f%s) \n", time_comp, comp_percent, "%", time_troc, troc_percent, "%");
     

  MPI_Finalize();
}