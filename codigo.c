
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define linhas_test 2248                //numero de linhas dos arquivos test
#define linhas_train 6746               //numero de linhas dos arquivos train
#define colunas 1601                    //numero de atributos maximo


int maxglobal;                          //numero de atributos escolhido pelo usuario
int numthreads;                         //numero de threads escolhido pelo usuario

float matriztestglobal[linhas_test][colunas];           //matriz global dos valores do arquivo test
float matriztrainglobal[linhas_train][colunas];         //matriz global dos valores do arquivo train

float vetor_menoresdistancias[linhas_test];             //vetor que armazena as menores distancias calculadas 

char vetorclasses_test[2248][50];                       //vetor que armazena todas as classes do arquivo test
char vetorclasses_train[6746][50];                      //vetor que armazena todas as classes do arquivo train


float euclidiana(float vetortr[], float vetor[]){       //funcao que calcula a distancia euclidiana
	float resultado;
	float soma, sub;
        
	for(int i=0; i<maxglobal; i++){
            sub = vetortr[i] - vetor[i];                //atributotrain - atributotest
	    soma = pow(sub, 2) + soma;                  
	}

	resultado = (float)sqrt(soma);                  //raiz quadrada do somatorio
	return resultado;
}


void *distanciathread(void *arg){                                               //funcao chamada pela pthread_create

    float distancia, menor;
    int i, j, k;
    int thread_id = (int) arg;
    printf("\nID da thread %i\n", thread_id);                                   //printando o numero da thread que esta sendo executada
    
    for(i=0; i<linhas_test; i++){                                               //for para cada linha do test
                                                                                
        menor=99999.0;
                
        for(j=0; j<linhas_train; j+=numthreads){                                //cada thread calcula a distancia da linha test para
                                                                                //para uma quantidade de linhas de train
            distancia = euclidiana(matriztrainglobal[j], matriztestglobal[i]);  
            if(distancia<menor){                                                //se a distancia obtida é menor que o menor atual
                menor = distancia;                                              //menor recebe a distancia
                
            }

        }
        
        vetor_menoresdistancias[i] = menor;                                     //insere a menor distancia desta linha do test no vetor
        
    }
    
}


int main() {
    
    int op;
    int max;
    char *test, *train;
    
    FILE *arq, *arqtrain;
    int i, j;
    char *atributo, classe[50], linha[1000000], linhatrain[1000000], classetrain[50], *atributotrain;
    
    printf("\n\t:: PRIMEIRO TRABALHO DE PROGRAMACAO CONCORRENTE ::\n");
    printf("\n\t\tQual bases deseja utilizar?\n");
    printf("\t\t1 - 59 atributos\n");
    printf("\t\t2 - 161 atributos\n");
    printf("\t\t3 - 256 atributos\n");
    printf("\t\t4 - 1380 atributos\n");
    printf("\t\t5 - 1601 atributos\n");
    printf("\t\tDigite a opção desejada: ");
    scanf("%d", &op);
    
    switch(op){
        case 1: 
            test="test_59.data";
            train="train_59.data";
            max = 59;
            break;
        case 2:
            test="test_154.data";
            train="train_154.data";
            max = 161;              //corrigido para 161
            break;
        case 3:
            test="test_256.data";
            train="train_256.data";
            max = 256;
            break;
        case 4:
            test="test_1379.data";
            train="train_1379.data";
            max = 1380;             //corrigido para 1380
            break;
        case 5:
            test="test_1601.data";
            train="train_1601.data";
            max = 1601;
            break;
        default:
            printf("\nVoce digitou uma opcao invalida\n");
     
    }
    
    maxglobal = max;   //atribuindo o valor do max escolhido para o max global

    int contclasse=0, contclassetr=0;       //indices para adicionar as classes encontradas no vetor de classes global
    float matrizteste[linhas_test][max];    //matriz local do arquivo test
    float matriztrain[linhas_train][max];   //matriz local do arquivo train
    int l=0, m=0;                           //l = indice para linhas de test, m = indice para linhas de train
    
    arq = fopen(test, "r");

    //::: TESTE :::
    while(fgets(linha, sizeof(linha), arq)!=NULL){        //pega o conteudo de cada linha do arquivo TEST

        i = 0;                                            //indice da coluna onde é inserido o atributo lido

        atributo = strtok(linha,",");                     //leitura até achar a virgula
        matrizteste[l][i]=atof(atributo);                 //matriz do test recebe o atributo lido no indice atual          

        while(atributo!=NULL){                            //while que vai lendo toda a linha e adicionado os atributos na matriztest			
            atributo = strtok(NULL,",");
            if(atributo!=NULL){
                matrizteste[l][i+1]=atof(atributo);
                i++;
                if(i==max){                               //se o indice é o ultimo, copia o atributo para uma variavel classe
                    strcpy(classe, atributo);
                    for(int g=0; g<50; g++){
                        vetorclasses_test[contclasse][g] = classe[g];   //adiciona a classe encontrada no vetor de classes
                    }
                }
            }
        }
        
        l++; 
        contclasse++;
    
    }
    
    fclose(arq);
    
    arqtrain = fopen(train, "r");
    //::: TRAIN :::
    while(fgets(linhatrain, sizeof(linhatrain), arqtrain)!=NULL){   //mesmo processo do while anterior     
        
        j = 0;
        atributotrain = strtok(linhatrain,",");   		
        matriztrain[m][j]=atof(atributotrain);

        while(atributotrain!=NULL){				
            atributotrain = strtok(NULL,",");
            if(atributotrain!=NULL){
                matriztrain[m][j+1]=atof(atributotrain);
                j++;
                if(j==max){
                    strcpy(classetrain, atributotrain);
                    for(int g=0; g<50; g++){
                        vetorclasses_train[contclassetr][g] = classetrain[g];
                    }
                }
            }
        }

        m++;
        contclassetr++;

    }
   
    fclose(arqtrain);
    

    //laços for utilizados para atribuir os valores das matrizes locais nas matrizes globais
    for(int x=0; x<linhas_test; x++){
        for(int y=0; y<max; y++){
            matriztestglobal[x][y] = matrizteste[x][y];
        }
    }

    for(int x=0; x<linhas_train; x++){
        for(int y=0; y<max; y++){
            matriztrainglobal[x][y] = matriztrain[x][y];
        }
    }
    
    
    int opt;
    printf("\n\t- Escolha o número de Threads à serem usadas na execução");
    printf("\n\t- Digite o número de Threads desejado: ");
    scanf("%d", &opt);
    numthreads = opt;

    pthread_t threads[numthreads];          //declarando um vetor de threads, com numthreads sendo atribuido pelo usuario
     
    printf("\nExecutando Threads...");
    int h;
    for(h = 0; h < numthreads; h++){
	if (pthread_create(&threads[h], NULL, distanciathread, (void *)h)) {    //criando as threads
            printf("Erro na criacao das threads");
	}
    }
    for(h = 0; h < numthreads; h++) {
	pthread_join(threads[h], NULL);                                         //fazendo que uma thread espere pela outra 
    }                                                                           //utilizando a funcao pthread_join
       

    return 0;
    
}
