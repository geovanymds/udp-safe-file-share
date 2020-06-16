#include <windows.h>
  #include <winsock.h>
#include <stdio.h>
  #include <string.h>

#define LOCAL_SERVER_PORT 1500 //defina aqui qual porta usar na comunicação do socket.
#define MAX_MSG 100 //tamanho maximo de uma mensagem.

typedef struct indice
{
  char titulo[50];
  char ip[16];
  char porta[6];
}INDICE;

int main(int argc, char *argv[]){
  char fileName[MAX_MSG];
  char index[22];
  memset(fileName,0x0,MAX_MSG);
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
  //Recebe as mensagens.
  struct sockaddr_in clientAddress; //guarda o endereço do cliente se comunicando
  int cliLength = sizeof(clientAddress); //guarda o tamanho do struct para conversão correta
  char buffer[MAX_MSG]; //buffer para as mensagens;
  while(1){
    //inicializa o buffer vazio
    memset(buffer,0x0,MAX_MSG);

    //recebe a mensagem via socket(sock) de um cliente com endereço "cleintAddress" a registrar, verificando erros na funcao.
    do{
    result = recvfrom(sock, buffer, MAX_MSG, 0, (struct sockaddr *) &clientAddress, &cliLength);
    }while(buffer[0] != '0');
    if(result == SOCKET_ERROR){
      printf("Nao foi possivel receber os dados \n");
    }
    else{ //caso nao ocorra erro, imprima a mensagem e o remetente
      for(int k = 1; k < strlen(buffer); k++){
        fileName[k-1] = buffer[k];
      }

      INDICE auxiliar;
      int t = 0;
      FILE *reader = fopen("Data.bin", "rb");
      while(!feof(reader)){
        fread(&auxiliar, sizeof(INDICE), 1, reader);
        if(strcmp(auxiliar.titulo, fileName) == 0){
          strcpy(index, auxiliar.ip);
          strcat(index, "x");
          strcat(index, auxiliar.porta);
          t = 1;
          break;
        }
      }
      fclose(reader);
      printf("index = %s\n", index);

      if(t == 0){
        printf("Arquivo não encontrado!");
        return 1;
      }  

      printf("FILE NAME: %s\n\n", fileName);
      printf("De %s:UDP %u: File Requested - %s\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), fileName);
      printf("\n sending Sever IP...\n");

      result = sendto(sock, index, strlen(index)+1, 0,
      (LPSOCKADDR) &clientAddress, sizeof(struct sockaddr));
      //Verifica possiveis erros
      if(result == SOCKET_ERROR) {
        printf("Nao pode enviar dados de resposta\n");
        closesocket(sock); //encerra o socket
        return 1;
      }
    }
  }

  //encerra o programa
  closesocket(sock);
  WSACleanup();
  return EXIT_SUCCESS;
}

/*
RESUMO DO PROGRAMA:
O programa consiste em criar um socket associado ao endereço da maquina que comporta o programa que 
funcionará como servidor, recebendo mensagens de outro cliente.
ETAPAS:
1)INICIALIZA AS BIBLIOTECAS
2)CRIA UM SOCKET E O ASSOCIA AO ENDERECO DO PROGRAMA SERVIDOR VINCULADO(socket(), sockaddr_in & bind())
3)RECEBE AS MENSAGENS, EM LOOP, DE UM CLIENTE VIA BUFFER(recvfrom())
*/