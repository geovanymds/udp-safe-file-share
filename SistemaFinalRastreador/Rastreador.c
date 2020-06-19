#include <windows.h>
  #include <winsock.h>
#include <stdio.h>
  #include <string.h>

/*
  --- PROJETO TRABALHO 02 - UDP SEGURO ---
  Disciplina COM240 - Redes de Computadores
  DATA: 21/06/2020
  
  Grupo:
  Geovany Carlos Mendes - 2018013746
  Leonardo Vieira Ferreira - 2018009799
  Thiago Silva Pereira - 2018008209

  Instruções para compilação e execução:
  Compilar - gcc Rastreador.c -o rastreadorudp
  Executar - ./rastreadorudp
  (Precisa estar na mesma pasta que os arquivos a qual consegue enviar)
*/

#define LOCAL_SERVER_PORT 1500 //defina aqui qual porta usar na comunicação do socket.
#define MAX_MSG 100 //tamanho maximo de uma mensagem.

typedef struct indice //estrutura para guardar os ips e portas
{
  char titulo[50];
  char ip[16];
  char porta[6];
}INDICE;

int main(int argc, char *argv[]){  
  char fileName[MAX_MSG];       //buffer para receber guardar o nome dos arquvios
  char index[22];               //buffer para leitura no banco de dados

  //inicializa a biblioteca de sockets no windows.
  int result; //variavel para validação de erros
  WSADATA wsaData;
  result = WSAStartup(MAKEWORD(2,1),&wsaData); 
  if(result != 0){ //verifica possíveis erros
    printf("WSAStartup failed: %d\n", result);
    return(1);
  }

  //Cria o socket do rastreador  
  SOCKET sock = INVALID_SOCKET;
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock == INVALID_SOCKET) {
    printf("Erro ao criar o socket\n");
    return(1);
  }

  //Vincula informações de IP e Porta do rastrador.
  struct sockaddr_in ServerAddress;
  ServerAddress.sin_family = AF_INET;
  ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  ServerAddress.sin_port = htons(LOCAL_SERVER_PORT);

  //Associa o endereço ao sockets
  result = bind(sock, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress));
  if(result == SOCKET_ERROR) 
  { //verifica possíveis erros
    printf("Nao foi possivel associar o endereço/porta ao socket\n");
    closesocket(sock); //encerra o socket
    return(1);
  }

  printf("preperating to receive menssages...\n\n");
  //Recebe mensagens de requisição.
  struct sockaddr_in clientAddress;       //guarda o endereço do cliente se comunicando
  int cliLength = sizeof(clientAddress);  //guarda o tamanho do struct para conversão correta
  char buffer[MAX_MSG];                   //buffer para as mensagens;
  while(1){
    //inicializa o buffer vazio
    memset(buffer,0x0,MAX_MSG);

    //recebe a mensagem via socket(sock) de um cliente com endereço "cleintAddress" a registrar
    do{
    result = recvfrom(sock, buffer, MAX_MSG, 0, (struct sockaddr *) &clientAddress, &cliLength);
    }while(buffer[0] != '0');
    if(result == SOCKET_ERROR)
    { //verifica possíveis erros
      printf("Nao foi possivel receber os dados \n");
    }
    else{ //caso nao ocorra erro, verifique o pedido...
      memset(fileName,0x0,MAX_MSG);
      for(int k = 1; k < strlen(buffer); k++){ //separa o nome do arquivo requisitado
        fileName[k-1] = buffer[k];
      }
      printf("FILE NAME: %s\n\n", fileName);

      //Vasculha sua base de dados para encontrar quem possui o arquivo
      INDICE auxiliar;
      int t = 0;
      FILE *reader = fopen("Data.bin", "rb");
      // fseek(reader, 0, SEEK_SET);
      memset(index,0x0,22);
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

      /* printf("index = %s\n", index); */

      if(t == 0){ //Caso o arquivo requisitado não exista
        printf("Arquivo nao encontrado!");
        return 1; //como o cliente não trata esse tipo de casos encerra o programa
      }  

      // printf("FILE NAME: %s\n\n", fileName);
      printf("De %s:UDP %u: File Requested - %s\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), fileName);
      printf("\n sending Sever IP...\n");

      //envia o IP e PORTA do servidor encontrado
      result = sendto(sock, index, strlen(index)+1, 0,
      (LPSOCKADDR) &clientAddress, sizeof(struct sockaddr));
      if(result == SOCKET_ERROR)
      { //verifica possíveis erros
        printf("Nao pode enviar dados de resposta\n");
        closesocket(sock); //encerra o socket
        return 1;
      }

      //Atualiza o arquivo com as informações do cliente
      FILE *updater = fopen("Data.bin", "ab");
      INDICE aux;
      strcpy(aux.titulo, fileName);
      strcpy(aux.ip, inet_ntoa(clientAddress.sin_addr));
      strcpy(aux.porta, itoa(clientAddress.sin_port, aux.porta, 10));
      fwrite(&aux, sizeof(INDICE), 1, updater);
      fclose(updater);
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
funcionará como rastreador, recebendo mensagens de outro cliente com requisições para encontrar
IP e PORTA de servidores que possuem um arquivo esperado.

ETAPAS:
1) Inicializar um socket servidor
2) Esperar requisições por arquivos
3) Verificar quem possui o arquivo
4) Envia IP e PORTA
5) Reescrever informações com o cliente
*/