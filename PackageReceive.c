#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include "utils/binaryFunctions.h"
#include <limits.h>

#define LOCAL_SERVER_PORT 1500 //defina aqui qual porta usar na comunicação do socket.
#define BUFFER_SIZE 480        //tamanho maximo de uma mensagem.

int main(int argc, char *argv[])
{
  int j, x = 1;                     //auxiliares
  int result;                       //variavel para validação de erros.
  char buffer[BUFFER_SIZE];         //buffer para troca de mensagens.
  char ack[33];                     //pacote de confirmação
  memset(buffer, 0x0, BUFFER_SIZE); //garente que o buffer eseja vazio

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
    printf("121.0.0.1 Hello World");
    return (1);
  }

  //Encontrar o host
  LPHOSTENT hostEntry;                //Estrutura para guardar informaçõe de um host(IP, nome, ...)
  hostEntry = gethostbyname(argv[1]); //funciona como um DNS qque retorna as informações de um host usando o seu nome(IP)
  if (hostEntry == NULL)
  { //verifica possíveis erros
    printf("Host desconhecido %s\n", argv[1]);
    return (1);
  }

  //Define umma estrutura para guardar o endereço do host e vinculalo a porta escolhida.
  struct sockaddr_in hostAddress;
  hostAddress.sin_family = hostEntry->h_addrtype;
  hostAddress.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list); //converte o endereço para o formato correto
  hostAddress.sin_port = htons(LOCAL_SERVER_PORT);              //converte o numero da porta para o endereço corret

  //Cria o socket do cliente
  SOCKET sock = INVALID_SOCKET;
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == INVALID_SOCKET)
  {
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
  {
    printf("Nao foi possivel associar o endereço/porta ao socket\n");
    closesocket(sock); //encerra o socket
    return (1);
  }

  //Envio de dados
  int hostLength = sizeof(hostAddress);
  for (int i = 2; i < argc; i++)
  {
    //envia a menssagem i pelo socket(sock) para o endereço na estrutura hostAddres
    result = sendto(sock, argv[i], strlen(argv[i]) + 1, 0,
                    (LPSOCKADDR)&hostAddress, sizeof(struct sockaddr));

    //Verifica possiveis erros
    if (result == SOCKET_ERROR)
    {
      printf("Nao pode enviar dados %d \n", i - 1);
      closesocket(sock); //encerra o socket
      return 1;
    }

    int receiving = 1;
    char pacote[BUFFER_SIZE];
    char palavra1[17];
    char palavra2[17];
    char wordAux[17];
    char carryAdd;
    FILE *writer;
    writer = fopen("COPIA.jpg", "wb");
    while (receiving == 1)
    {
      result = recvfrom(sock, pacote, BUFFER_SIZE, 0, (struct sockaddr *)&hostAddress, &hostLength);
      if (result == SOCKET_ERROR)
      {
        printf("Nao foi possivel receber os dados \n");
      }
      else
      { //caso nao ocorra erro, imprima a mensagem e o remetente
        for (i = 0; i < BUFFER_SIZE - 49; i++)
        {
          if (i == 0)
          {
            for (j = 0; j < CHAR_BIT; j++)
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
          else if (i % 2 == 1 && i != 1)
          {
            for (j = 0; j < CHAR_BIT; j++)
            {
              palavra2[j + 8] = (((pacote[i] >> j) & 1) + '0');
            }
            memset(wordAux, 0x0, 17);
            carryAdd = add(palavra1, palavra2, wordAux);
            if (carryAdd == '1')
            {
              carryAdd = add(wordAux, "0000000000000001", palavra1);
            }
            else
            {
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
        j = BUFFER_SIZE - 49;
        memset(palavra2, 0x0, 17);
        for (i = 0; i < 16; i++)
        {
          palavra2[i] = pacote[j];
          j++;
        }
        memset(wordAux, 0x0, 17);
        carryAdd = add(palavra1, palavra2, wordAux);
        if (strcmp(wordAux, "1111111111111111") == 0)
        {
          ack[0] = '1';
          j = BUFFER_SIZE - 32;
          for (i = 1; i < 33; i++)
          {
            ack[i] = pacote[j];
          }
          if (pacote[BUFFER_SIZE - 33] != 1)
          {
            fwrite(&pacote, sizeof(unsigned char), (long int)pacote[BUFFER_SIZE - 33], writer);
          }
          else
          {
            fwrite(&pacote, sizeof(unsigned char), BUFFER_SIZE - 49, writer);
          }
          receiving = pacote[BUFFER_SIZE - 33];
          printf("%s: VALIDADO %d\n", wordAux, x);
          x++;
        }
        else
        {
          ack[0] = '0';
          j = BUFFER_SIZE - 32;
          for (i = 1; i < 33; i++)
          {
            ack[i] = pacote[j];
          }
          printf("%s: INVALIDADO %d\n", wordAux, x);
          x++;
        }
      }
    }
    fclose(writer);
    printf("FINE!!!!");
  }
  //encerra o socket e finaliza o programa
  closesocket(sock);
  WSACleanup();
  return EXIT_SUCCESS;
}
