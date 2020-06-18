//BIBLIOTECAS======================
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include<sys/stat.h>
#include <limits.h>
#include "./../utils/binaryFunctions.h"
#include <time.h>
#include <stdbool.h>
#include <string.h>
//=================================

/*
  --- PROJETO TRABALHO 02 - UDP SEGURO ---
  Disciplina COM240 - Redes de Computadores
  DATA: 21/06/2020
  
  Grupo:
  Geovany Carlos Mendes - 2018013746
  Leonardo Vieira Ferreira - 2018009799
  Thiago Silva Pereira - 2018008209

  Instruções para compilação e execução:
  Compilar - gcc PackageSend.c -o servidorudp
  Executar - ./servidorudp
  (Precisa estar na mesma pasta que os arquivos a qual consegue enviar)
*/

#define LOCAL_SERVER_PORT 8080 //defina aqui qual porta usar na comunicação do socket.
#define MAX_MSG 100 //tamanho maximo de uma mensagem comum (para troca de informações como ip's).
#define BUFFER_SIZE 500 //tamanho de cada pacote do arquivo

int main(int argc, char *argv[]){

  //Inicialização das variaveis auxiliares
  int i, j, serieNumber, aux, result, t = 0;
  char binarySerieNumber[33], buffer[MAX_MSG];
  char ackReceive[] = "000000000000000000000000000000000";
  char auxReceive[] = "00000000000000000000000000000000";
  char auxSerieNumber[] = "00000000000000000000000000000000";
  char fileName[MAX_MSG];
  unsigned char myChar;
  memset(fileName,0x0,MAX_MSG);
  bool confirmacao;
  bool retry;
  bool vemACK; 

  //Inicializa a biblioteca de sockets no windows.
  WSADATA wsaData;
  result = WSAStartup(MAKEWORD(2,1),&wsaData); 
  if(result != 0){ //verifica possíveis erros
    printf("WSAStartup failed: %d\n", result);
    return(1);
  }

  //Cria o socket do servidor  
  SOCKET sock = INVALID_SOCKET;
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock == INVALID_SOCKET) { //verifica possíveis erros
    printf("Erro ao criar o socket\n");
    return(1);
  }

  //Vincula informações do IP e Porta do servidor.
  struct sockaddr_in ServerAddress;
  ServerAddress.sin_family = AF_INET;
  ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  ServerAddress.sin_port = htons(LOCAL_SERVER_PORT);

  //Associa o endereço cliente vinculado ao sockets do cliente, verificando erros
  result = bind(sock, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress));
  if(result == SOCKET_ERROR) { //verifica possíveis erros
      printf("Nao foi possivel associar o endereço/porta ao socket\n");
      closesocket(sock);
      return(1);
  }

  struct sockaddr_in clientAddress; //estrutura para guardar as informações do cliente requisitante
  int cliLength = sizeof(clientAddress); //tamanho do endereço do requisitante
  while(1){

    printf("\npreparing to receive requests...\n\n");
    memset(buffer,0x0,MAX_MSG);

    //Aguarda o recebimento de requisições
    do{
    result = recvfrom(sock, buffer, MAX_MSG, 0, (struct sockaddr *) &clientAddress, &cliLength);
    }while(buffer[0] != '1');
    if(result == SOCKET_ERROR){ //verifica possíveis erros
      printf("Someone is trying to connect: status [FAILED]\n");
    }
    else{ //Se a conexão ocorreu sem falhas
      for(int k = 1; k < strlen(buffer); k++){
        fileName[k-1] = buffer[k];
      }
      printf("De %s:UDP %u: File Requested - %s\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), fileName);
      printf("\n sending file...\n");

      FILE * reader; //ponteiro para o arquivo requisitado
      reader = fopen(fileName, "rb"); //Abre a leitura em binário
      if(reader != NULL){ //verifica possíveis erros
        
        //Inicializa as variaveis auxiliares
        struct stat status; //armazena as principais informações do arquivo sendo enviado
        stat(fileName, &status);
        long int tam = status.st_size;
        unsigned char pacote[BUFFER_SIZE];
        char palavra1[17], palavra2[17], wordAux[17];
        char carryAdd;   //
        serieNumber = 1; //inicializa o contador para o número de série

        //Envia o arquiv por pacotes.
        while(tam > 0){
          //Lê do arquivo um pacote(BUFFER_SIZE)
          fread(&pacote, sizeof(unsigned char), BUFFER_SIZE-81, reader); //(-81 é o tamanho do cabeçalho)

          //Gerador de Checksum de 16bits
          memset(palavra1,0x0,17);
          for(i = 0; i < BUFFER_SIZE-81; i++){
            //Para a Primeira palavra ---
            if(i == 0){
              for(j = 0; j < CHAR_BIT; j++){ //Le Bit a Bit de de um byte
                palavra1[j] = (((pacote[i]>>j)&1)  + '0');
              }
            }
            else if(i == 1){
              for(j = 0; j < CHAR_BIT; j++){
                palavra1[j+8] = (((pacote[i]>>j)&1)  + '0');
              }
            }
            //Para as demais palavras ---
            else if(i % 2 == 1 && i != 1){
              for(j = 0; j < CHAR_BIT; j++){
                palavra2[j+8] = (((pacote[i]>>j)&1)  + '0');
              }

              //Soma das palavras de 16bits
              memset(wordAux,0x0,17);
              carryAdd = add(palavra1, palavra2,wordAux);
              if(carryAdd=='1') { //Tratamento de somas que excedem 16 bits
                carryAdd = add(wordAux,"0000000000000001", palavra1);               
              }
              else{
                memset(palavra1,0x0,17);
                strcpy(palavra1, wordAux);
              }  
            }
            else{
              memset(palavra2,0x0,17);
              for(j = 0; j < CHAR_BIT; j++){
                palavra2[j] = (((pacote[i]>>j)&1)  + '0');
              }
            }
          }
          //Realiza o complemento de 1
          comp(palavra1);

          //Na necessidades de testes, descomente esse código para
          //verificar os Checksum's sendo enviado
          /*printf("CheckSum: %s\n", palavra1);*/
          
          //Escreve o checksum no cabeçalho do pacote
          j = 0;
          for(i = BUFFER_SIZE-81; i < BUFFER_SIZE-65; i++){
            pacote[i] = palavra1[j];
            j++;
          }
          if((tam - BUFFER_SIZE+81) <= 0){
            printf("tamanho = %d\n", tam);
            pacote[BUFFER_SIZE-65] = 0;
            memset(binarySerieNumber,'0',32);
            decToBinary(tam,binarySerieNumber);
            aux = 0;
            for(i=BUFFER_SIZE-32; i <BUFFER_SIZE; i++){
              pacote[i] = binarySerieNumber[aux];
              printf(" %c", pacote[i]); 
              aux++;
            }
          }
          else{
            pacote[BUFFER_SIZE-65] = 1;
          }

          //Na necessidades de testes, descomente  os próximos 3 "printf's"
          //para verificar o número de senquencia sendo enviado
          //Escreve o numero de sequência no pacote
          memset(binarySerieNumber,'0',32);
          decToBinary(serieNumber,binarySerieNumber); //coversor de decimal para binário
          /*printf("N Sequencia: ");*/
          aux = 0;
          for(i=BUFFER_SIZE-64; i <BUFFER_SIZE-32; i++){
            pacote[i] = binarySerieNumber[aux]; 
            /*printf(" %c", pacote[i]);*/
            aux++;
          }
          /*printf("\n");*/

          //Envio e Espera de ACK
          confirmacao = false;
          while (confirmacao == false)
          {
            //Envia mensagem
            result = sendto(sock, (const char *)pacote, BUFFER_SIZE, 0,
            (LPSOCKADDR) &clientAddress, sizeof(struct sockaddr));
            if(result == SOCKET_ERROR){ //verifica possíveis erros
              printf("Falha no envio do pacote %d\n", serieNumber);
              retry = true;
            }
            else{retry = false;}

            if(!retry){  //se não houve erro no envio inicializa temporizador.
              clock_t espera = clock() + 100; // timer de aproximadamente 0,5 segundo
              vemACK = true;
              while (clock() < espera)
              {
                //Espera receber um ack do pacote enviado
                if(vemACK){
                  result = recvfrom(sock, ackReceive, 33, 0, (struct sockaddr *) &clientAddress, &cliLength);
                  if(result == SOCKET_ERROR) {
                    printf("Falha ao receber o ack\n");
                  }
                  else
                    vemACK = false;
                }

                //Copia o numero de sequencia recebido e envviado para comparação
                j = 0;
                for(i = 1; i < 33; i++){
                  auxSerieNumber[j] = ackReceive[i];
                  j++;
                }
                j = 0;
                for(i = BUFFER_SIZE-64; i < BUFFER_SIZE-32; i++){
                  auxReceive[j] = pacote[i];
                  j++;
                }

                //Se foi recebido com um ack"(ackReceive[0] == '1')" e com o número de senquência correto 
                if((ackReceive[0] == '1')&&(!strcmp(auxSerieNumber,auxReceive)))
                {
                  confirmacao = true;
                  break; //finaliza o temporizador e envia o próximo pacote
                }
                else{ //caso ocorra um Nak ou erro no N de sequencia
                  break; //finaliza o temporizado e reenvia o mesmo pacote
                }
              }
            }
          }
          tam = tam - (BUFFER_SIZE-81);
          serieNumber++;
        }
        fclose(reader); //Após o envio fecha o arquivo sendo lido
      }
    }
  }
  //encerra o programa
  closesocket(sock);
  WSACleanup();
  return EXIT_SUCCESS;
}
