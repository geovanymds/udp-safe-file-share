//BIBLIOTECAS======================
#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <stdlib.h> //atoi()
#include <string.h> //String Functions
#include "utils/binaryFunctions.h" //Binary Work
#include <limits.h> //CHAR_BIT
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
  Compilar - gcc PackageSend.c -o clienteudp
  Executar - ./clienteudp 127.0.0.1 NomeDoArquivo
  (Arquivo incluso para testes é Butterfly.jpg)
*/

#define LOCAL_SERVER_PORT 1500 //defina aqui qual porta usar na comunicação do socket.
#define MAX_MSG 100
#define IP_MAX 16
#define PORT_MAX 6
#define BUFFER_SIZE 500  //tamanho de cada pacote do arquivo

int main(int argc, char *argv[])
{ 
  int i, j, k, x = 1, aux2;               //auxiliares
  int result;                       //variavel para validação de erros.
  char buffer[BUFFER_SIZE];         //buffer para troca de mensagens.
  char request[MAX_MSG];
  char serverIP[IP_MAX];
  char serverPortName[PORT_MAX];
  int serverPort;
  char binarySerieNumber[] = "0000000000000000";
  char ack[33];                     //pacote de confirmação
  memset(buffer, 0x0, BUFFER_SIZE); //garente que o buffer eseja vazio
  memset(request, 0x0, MAX_MSG);  //garente que o buffer eseja vazio
  memset(serverIP, 0x0, IP_MAX); //garente que o buffer eseja vazio
  memset(serverPortName, 0x0, PORT_MAX); //garente que o buffer eseja vazio


  //inicializa a biblioteca de sockets no windows.
  WSADATA wsaData;
  result = WSAStartup(MAKEWORD(2, 1), &wsaData);
  if (result != 0)
  { //verifica possíveis erros
    printf("WSAStartup failed: %d\n", result);
    return (1);
  }

  //Verifica se existem argumentos sucientes para execução do programa
  if (argc < 3)
  {
    printf("Nao ha argumentos suficientes para execução!\n");
    printf("Por favor siga o exemplo: (IP HOST) <nome do arquivo solicitando>");
    printf("127.0.0.1 Butterfly.jpg");
    return (1);
  }

  //Encontrar o host
  LPHOSTENT TrackerEntry; //Estrutura para guardar informaçõe de um host(IP, nome, ...)
  TrackerEntry = gethostbyname(argv[1]); //funciona como um DNS qque retorna as informações de um host usando o seu nome(IP)
  if (TrackerEntry == NULL)
  { //verifica possíveis erros
    printf("Tracker desconhecido %s\n", argv[1]);
    return (1);
  }

  //Define umma estrutura para guardar o endereço do host e vinculalo a porta escolhida.
  struct sockaddr_in TrackerAddress;
  TrackerAddress.sin_family = TrackerEntry->h_addrtype;
  TrackerAddress.sin_addr = *((LPIN_ADDR)*TrackerEntry->h_addr_list); //converte o endereço para o formato correto
  TrackerAddress.sin_port = htons(LOCAL_SERVER_PORT); //converte o numero da porta para o endereço corret

  //Cria o socket do cliente
  SOCKET sock = INVALID_SOCKET;
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == INVALID_SOCKET)
  { //verifica possíveis erros
    printf("Erro ao criar o socket\n");
    return (1);
  }

  //Vincula informações do IP e Porta do cliente.
  struct sockaddr_in clientAddress;
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  clientAddress.sin_port = htons(0);

  //Associa o endereço cliente vinculado ao sockets do cliente, verificando erros
  result = bind(sock, (struct sockaddr *)&clientAddress, sizeof(clientAddress));
  if (result == SOCKET_ERROR)
  { //verifica possíveis erros
    printf("Nao foi possivel associar o endereço/porta ao socket\n");
    closesocket(sock); 
    return (1);
  }

  //Envio da requisição pelo pacote
  int TrackerLength = sizeof(TrackerAddress);
  request[0] = '0';
  strcat(request, argv[2]);
  //envia a requisição pelo socket(sock) para o endereço na estrutura hostAddres
  //printf("REQUEST: %s\n", request);
  result = sendto(sock, request, MAX_MSG, 0,
                  (LPSOCKADDR)&TrackerAddress, sizeof(struct sockaddr));
  if (result == SOCKET_ERROR)
  { //Verifica possiveis erros
    printf("Nao pode enviar dados\n");
    closesocket(sock);
    return 1;
  }

  memset(request, 0x0, MAX_MSG);
  result = recvfrom(sock, request, BUFFER_SIZE, 0, (struct sockaddr *)&TrackerAddress, &TrackerLength);
  if (result == SOCKET_ERROR)
  { //verifica possíveis erros
    printf("Nao foi possivel receber os dados \n");
    closesocket(sock);
    return 1;
  }

  k = 0;
  while(request[k] != 'x'){
    serverIP[k] = request[k];
    k++;
  }
  k++;
  j = 0;
  for(k = k; k < strlen(request); k++){
    serverPortName[j] = request[k];
    j++;
  }
  serverPort = atoi(serverPortName);
  printf("IP: %s\n PORTA: %d\n", serverIP, serverPort);
  //Encontrar o host
  LPHOSTENT hostEntry; //Estrutura para guardar informaçõe de um host(IP, nome, ...)
  hostEntry = gethostbyname(serverIP); //funciona como um DNS qque retorna as informações de um host usando o seu nome(IP)
  if (hostEntry == NULL)
  { //verifica possíveis erros
    printf("Host desconhecido %s\n", serverIP);
    return (1);
  }

  //Define umma estrutura para guardar o endereço do host e vinculalo a porta escolhida.
  struct sockaddr_in hostAddress;
  hostAddress.sin_family = hostEntry->h_addrtype;
  hostAddress.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list); //converte o endereço para o formato correto
  hostAddress.sin_port = htons(serverPort); //converte o numero da porta para o endereço corret


  memset(request, 0x0, MAX_MSG);
  request[0] = '1';
  strcat(request, argv[2]);
  result = sendto(sock, request, MAX_MSG, 0,
                  (LPSOCKADDR)&hostAddress, sizeof(struct sockaddr));
  if (result == SOCKET_ERROR)
  { //Verifica possiveis erros
    printf("Nao pode enviar dados\n");
    closesocket(sock);
    return 1;
  }

  int hostLength = sizeof(hostAddress);
  //Prepatando para receber o pacote
  //Inicialização das variaveis auxiliares
  int receiving = 1;        //Para identificação e tratamento do último pacote do arquivo
  char pacote[BUFFER_SIZE]; //Estrutura que guarda pacotes
  char palavra1[] = "0000000000000000";        //auxiliar para soma do checksum
  char palavra2[] = "0000000000000000";        //auxiliar para soma do checksum
  char wordAux[] = "0000000000000000";         //auxiliar para soma do checksum
  char carryAdd;            //auxiliar para soma do checksum

  //Recebe os pacotes do arquivo
  FILE *writer; //Incializa um arquivo para escrever os pacotes enviados
  writer = fopen(argv[2], "wb");
  while (receiving == 1) //enquanto não for o ultimo pacote
  {
    //recebe um pacote
    result = recvfrom(sock, pacote, BUFFER_SIZE, 0, (struct sockaddr *)&hostAddress, &hostLength);
    if (result == SOCKET_ERROR)
    { //verifica possíveis erros
      printf("Nao foi possivel receber os dados \n");
    }
    else
    { //caso nao ocorra erro, Verifique o checksum
      for (i = 0; i < BUFFER_SIZE - 81; i++)
      { //Para a Primeira palavra ---
        if (i == 0)
        {
          for (j = 0; j < CHAR_BIT; j++) //Le Bit a Bit de de um byte
          {
            palavra1[j] = (((pacote[i] >> j) & 1) + '0');
          }
        }
        else if (i == 1)
        {
          for (j = 0; j < CHAR_BIT; j++)
          {
            palavra1[j + 8] = (((pacote[i] >> j) & 1) + '0');
          }
        }
        //Para as demais palavras ---
        else if (i % 2 == 1 && i != 1)
        {
          for (j = 0; j < CHAR_BIT; j++)
          {
            palavra2[j + 8] = (((pacote[i] >> j) & 1) + '0');
          }
          //Soma das palavras de 16bits
          memset(wordAux, 0x0, 17);
          carryAdd = add(palavra1, palavra2, wordAux);
          if (carryAdd == '1'){ //Tratamento de somas que excedem 16 bits
            carryAdd = add(wordAux, "0000000000000001", palavra1);
          }
          else{
            memset(palavra1, 0x0, 17);
            strcpy(palavra1, wordAux);
          }
        }
        else
        {
          memset(palavra2, 0x0, 17);
          for (j = 0; j < CHAR_BIT; j++)
          {
            palavra2[j] = (((pacote[i] >> j) & 1) + '0');
          }
        }
      }
      //soma o resultado com o checksum
      j = BUFFER_SIZE - 81;
      memset(palavra2, 0x0, 17);
      for (i = 0; i < 16; i++)
      {
        palavra2[i] = pacote[j];
        j++;
      }
      memset(wordAux, 0x0, 17);
      carryAdd = add(palavra1, palavra2, wordAux);
      aux2 = 0;
      for(k = BUFFER_SIZE-64; k < BUFFER_SIZE-32; k++){
            binarySerieNumber[aux2] = pacote[k];
            aux2++;
      }
      i = btoi(binarySerieNumber);
      //Se o resultado for "1111111111111111" Checksum verificado com sucesso
      if (strcmp(wordAux, "1111111111111111") == 0 && i == x)
      {
        ack[0] = '1'; //ACK
        j = BUFFER_SIZE - 64;
        for (i = 1; i < 33; i++) //número de sequencia
        {
          ack[i] = pacote[j];
          j++;
        }

        //Na necessidades de testes, descomente esse código para
        //verificar os ACKS's sendo enviado
        /*printf("ACK: %s\n", ack);*/
        
        //Com a confirmação escreve no arquivo
        if (pacote[BUFFER_SIZE - 65] == 0)
        { //tratamento do ultimo pacote
          printf("\n\nOK1\n");
          int tamanho;
          aux2 = 0;
          memset(binarySerieNumber, '0', 32);
          for(k = BUFFER_SIZE-32; k < BUFFER_SIZE; k++){
            binarySerieNumber[aux2] = pacote[k];
            aux2++;
          }
          printf("OK2\n");
          tamanho = btoi(binarySerieNumber);
          printf("OK3\n");
          printf("tamanho = %d\n", tamanho);
          printf("binario = %s\n", binarySerieNumber);
          fwrite(&pacote[k], sizeof(unsigned char), tamanho, writer);
        }
        else{
          fwrite(&pacote, sizeof(unsigned char), BUFFER_SIZE - 81, writer);
        };
        receiving = pacote[BUFFER_SIZE - 65];

        //Envia o ACK
        result = sendto(sock, ack, 33, 0,
        (LPSOCKADDR) &hostAddress, sizeof(struct sockaddr));
        // printf("OK2\n");
        if(result == SOCKET_ERROR) {
          printf("falha ao enviar ACK\n");
          closesocket(sock); 
          return 1;
        }

        //Na necessidades de testes, descomente esse código para
        //verificar a validação
        /*
          printf("Checksum Result %s: Package %d - [VALIDADO]\n", wordAux, x);
        */
        x++;
      }
      else if(i == x)
      { //Caso haja alguma falha na verficação de veracidade do pacote
        memset(ack, 0x0, 33);
        ack[0] = '0'; //NAK
        j = BUFFER_SIZE - 64;
        for (i = 1; i < 33; i++) //Nº serie
        {
          ack[i] = pacote[j];
        }

        //Envia NAK
        result = sendto(sock, ack, 33, 0,
        (LPSOCKADDR) &hostAddress, sizeof(struct sockaddr));
        if(result == SOCKET_ERROR) {
          printf("Nao pode enviar dados %d \n",i-1);
          closesocket(sock); //encerra o socket
          return 1;
        }

        //Na necessidades de testes, descomente esse código para
        //verificar a invalidação
        /*
          printf("Checksum Result %s: Package %d - [INVALIDO]\n", wordAux, x);
          x++;
        */
      }
    }
  }
  fclose(writer); //fecha o arquivo escritor
  printf("Arquivo Recebido Com Sucesso!!\n");

  //encerra o socket e finaliza o programa
  closesocket(sock);
  WSACleanup();
  return EXIT_SUCCESS;
}
