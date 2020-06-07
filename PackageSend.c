#include <stdio.h>
  #include <stdlib.h>
#include <windows.h>
  #include <winsock.h>
#include <stdio.h>
  #include<sys/stat.h>
#include <limits.h>
  #include "utils/binaryFunctions.h"
#include <time.h>
  #include <stdbool.h>
#include <string.h>

#define LOCAL_SERVER_PORT 1500 //defina aqui qual porta usar na comunicação do socket.
#define MAX_MSG 100 //tamanho maximo de uma mensagem.
#define BUFFER_SIZE 480 

int main(int argc, char *argv[]){
  int i, j, serieNumber;
  char binarySerieNumber[33];
  char ackReceive[33];
  char auxReceive[32];
  char auxSerieNumber[32];
  memset(ackReceive, 0x0, 33);
  memset(auxReceive, 0x0, 32);
  memset(auxSerieNumber, 0x0, 32);
  bool confirmacao;
  bool vemACK;
  //inicializa a biblioteca de sockets no windows.
  int result; //variavel para validação de erros
  WSADATA wsaData;
  result = WSAStartup(MAKEWORD(2,1),&wsaData); 
  if(result != 0){ //verifica possíveis erros
    printf("WSAStartup failed: %d\n", result);
    return(1);
  }

  //Cria o socket do servidor  
  SOCKET sock = INVALID_SOCKET;
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock == INVALID_SOCKET) {
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
  if(result == SOCKET_ERROR) {
      printf("Nao foi possivel associar o endereço/porta ao socket\n");
      closesocket(sock); //encerra o socket
      return(1);
  }

  printf("preperating to receive menssages...\n\n");
  struct sockaddr_in clientAddress; 
  int cliLength = sizeof(clientAddress); 
  char buffer[MAX_MSG]; 
  while(1){

    memset(buffer,0x0,MAX_MSG);

    result = recvfrom(sock, buffer, MAX_MSG, 0, (struct sockaddr *) &clientAddress, &cliLength);
    if(result == SOCKET_ERROR){
      printf("Nao foi possivel receber os dados \n");
    }
    else{ 
      printf("De %s:UDP %u: %s\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port),buffer);
      printf("\n sending file...\n");

      FILE * reader;
      reader = fopen(buffer, "rb");
      if(reader != NULL){
        struct stat status;
        stat(buffer, &status);
        long int tam = status.st_size;
        unsigned char pacote[BUFFER_SIZE];
        char palavra1[17];
        char palavra2[17];
        char wordAux[17];
        char carryAdd;
        
        serieNumber = 1;

        //Sending file in packages
        while(tam > 0){
          memset(palavra1,0x0,17);
          fread(&pacote, sizeof(unsigned char), BUFFER_SIZE-17, reader);

          //To generating a checksum
          for(i = 0; i < BUFFER_SIZE-49; i++){
            if(i == 0){
              for(j = 0; j < CHAR_BIT; j++){
                palavra1[j] = (((pacote[i]>>j)&1)  + '0');
              }
            }
            else if(i == 1){
              for(j = 0; j < CHAR_BIT; j++){
                palavra1[j+8] = (((pacote[i]>>j)&1)  + '0');
              }
            }
            else if(i % 2 == 1 && i != 1){
              for(j = 0; j < CHAR_BIT; j++){
                palavra2[j+8] = (((pacote[i]>>j)&1)  + '0');
              }
              memset(wordAux,0x0,17);
              carryAdd = add(palavra1, palavra2,wordAux);
              if(carryAdd=='1') {
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
          comp(palavra1);
          printf("P1: %s\n", palavra1);
          j = 0;

          // Writing checksum in the package
          for(i = BUFFER_SIZE-49; i < BUFFER_SIZE-33; i++){
            pacote[i] = palavra1[j];
            j++;
          }
          if((tam - BUFFER_SIZE+49) <= 0){
            pacote[BUFFER_SIZE-33] = tam;
          }
          else{
            pacote[BUFFER_SIZE-33] = 1;
          }

          memset(binarySerieNumber,'0',32);

          result = decToBinary(serieNumber,binarySerieNumber);

          int aux = 0;
          for(i=BUFFER_SIZE-32; i <BUFFER_SIZE; i++){
             if (i<BUFFER_SIZE-(result+1)) {
               pacote[i]='0';
             }
             else {
               pacote[i] = binarySerieNumber[aux];
               aux++;
             }
          }
          confirmacao = false;
          while (confirmacao == false)
          {
            //Envia mensagem
            result = sendto(sock, (const char *)pacote, BUFFER_SIZE, 0,
            (LPSOCKADDR) &clientAddress, sizeof(struct sockaddr));
            if(result == SOCKET_ERROR) {
              printf("Nao pode enviar dados de resposta\n");
            }
            // printf("ENVIEI...\n");
              
            clock_t espera = clock() + 100; // timer de aproximadamente 0,5 segundo
            vemACK = true;
            while (clock() < espera)
            {
              if(vemACK){
                result = recvfrom(sock, ackReceive, 33, 0, (struct sockaddr *) &clientAddress, &cliLength);
                if(result == SOCKET_ERROR) {
                  printf("Nao pode receber ack\n");
                }
                else
                  vemACK = false;
                  // strncpy(auxReceive, &ackReceive[1], (size_t) 32);
                  // strncpy(auxSerieNumber, &pacote[BUFFER_SIZE-32], (size_t) 32);
              }
              j = 0;
              for(i = 1; i < 33; i++){
                auxSerieNumber[j] = ackReceive[i];
                j++;
              }
              j = 0;
              for(i = BUFFER_SIZE-32; i < BUFFER_SIZE; i++){
                auxReceive[j] = pacote[i];
                j++;
              }

              printf()
              if((ackReceive[0] == '1')&&(!strcmp(auxSerieNumber,auxReceive)))
              {
                printf("receive ack\n");
                confirmacao = true;
                break;
              }
              // printf("receive ack XX\n");
            }
          }
          tam = tam - (BUFFER_SIZE-17);
          serieNumber++;
        }
        fclose(reader);
      }
    }
  }
  //encerra o programa
  closesocket(sock);
  WSACleanup();
  return EXIT_SUCCESS;
}
