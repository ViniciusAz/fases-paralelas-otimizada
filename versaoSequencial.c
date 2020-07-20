#include <stdio.h>
#include <time.h>
#include <stdlib.h>


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


main(int argc, char** argv) {

    int tam_vet = 600000;      // Tamanho total do Vetor a ser Ordenado
    int vetor[tam_vet];        // Vetor 
    int rand_range = 1000000;  // intervalo de numeros que serao randomizados, ex: 100 = entre 0 e 99
    int i;
    int debug = 0;             // se = 1 vai printar o array antes e depois de ser ordenado
    
    clock_t t;

    srand(time(NULL));
    for(i = 0; i < tam_vet; i++)
        vetor[i] = rand() % rand_range;

    t = clock();

    if(debug == 1) { printf(" \n Original : "); for(i = 0; i < tam_vet; i++) printf(" %d, ", vetor[i]); printf(" \n"); }

    bs(tam_vet,vetor);

    if(debug == 1) { printf(" \n BubbleS  : "); for(i = 0; i < tam_vet; i++) printf(" %d, ", vetor[i]); printf(" \n"); }

    t = clock() - t; 
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
  
    printf("> %.2f segs p/ rodar o BubbleSort seq. com %d mil elementos \n", time_taken, (tam_vet/1000)); 

}


/*  Execuções Realizadas:
> 0.48 segs p/ rodar BubbleSort seq. com 10 mil elementos - no LAD
> 0.49 segs p/ rodar BubbleSort seq. com 10 mil elementos - no LAD
> 0.49 segs p/ rodar BubbleSort seq. com 10 mil elementos - no LAD

> 17.65 segs p/ rodar BubbleSort seq. com 60 mil elementos - no LAD
> 17.69 segs p/ rodar BubbleSort seq. com 60 mil elementos - no LAD
> 17.70 segs p/ rodar BubbleSort seq. com 60 mil elementos - no LAD

> 49.13 segs p/ rodar BubbleSort seq. com 100 mil elementos - no LAD
> 49.17 segs p/ rodar BubbleSort seq. com 100 mil elementos - no LAD
> 49.12 segs p/ rodar BubbleSort seq. com 100 mil elementos - no LAD

> 196.86 segs p/ rodar BubbleSort seq. com 200 mil elementos (3m16s) - no LAD
> 196.93 segs p/ rodar BubbleSort seq. com 200 mil elementos (3m16s) - no LAD

> 442.36 segs p/ rodar BubbleSort seq. com 300 mil elementos (7m22s) - no LAD

> 1769.89 segs p/ rodar BubbleSort seq. com 600 mil elementos (29m29s) - no LAD
> 1770.51 segs p/ rodar BubbleSort seq. com 600 mil elementos (29m30s) - no LAD
> 1797.30 segs p/ rodar BubbleSort seq. com 600 mil elementos (29m57s) - em programiz.com (compilador online)
> 1798.87 segs p/ rodar BubbleSort seq. com 600 mil elementos (29m58s) - em programiz.com (compilador online)

Média de execuções para 600 mil elementos: 
(1769.89 + 1772.51 + 1797.30 + 1798.87) / 4   =   1784.64 (29m44s)

*/