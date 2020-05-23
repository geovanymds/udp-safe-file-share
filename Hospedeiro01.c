#include <windows.h>
  #include <winsock.h>
#include <stdio.h>
  #include <string.h>

#define LOCAL_SERVER_PORT 1500 //defina aqui qual porta usar na comunicação do socket.
#define BUFFER_SIZE 500 //tamanho maximo de uma mensagem.

typedef struct pacote{
  //ARQUIVO
  FILE *arq;

  //METADATA
  char nome[50];
  
  //SEGURANÇA
    

}PACKEGE;

int main(int argc, char *argv[]){
  int result; //variavel para validação de erros.
  char buffer[BUFFER_SIZE]; //buffer para troca de mensagens.
  memset(buffer,0x0,BUFFER_SIZE); //garente que o buffer eseja vazio

  //inicializa a biblioteca de sockets no windows.
  WSADATA wsaData;
  result = WSAStartup(MAKEWORD(2,1),&wsaData); 
  if(result != 0){ //verifica possíveis erros
    printf("WSAStartup failed: %d\n", result);
    return(1);
  }

  //Verifica se existem argumentos sucientes para execução do programa
  if(argc < 3){
    printf("Nao ha argumentos suficientes para execução!\n");
    printf("Por favor siga o exemplo: (IP HOST) <mensagem 1> ... mensagem n>");
    printf("121.0.0.1 Hello World");
    return(1);
  }

  //Encontrar o host
  LPHOSTENT hostEntry; //Estrutura para guardar informaçõe de um host(IP, nome, ...)
  hostEntry = gethostbyname(argv[1]); //funciona como um DNS qque retorna as informações de um host usando o seu nome(IP)
  if (hostEntry == NULL){ //verifica possíveis erros
      printf("Host desconhecido %s\n", argv[1]);
      return(1);
  } 

  //Define umma estrutura para guardar o endereço do host e vinculalo a porta escolhida.
  struct sockaddr_in hostAddress;
  hostAddress.sin_family = hostEntry->h_addrtype ;
  hostAddress.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list); //converte o endereço para o formato correto
  hostAddress.sin_port = htons(LOCAL_SERVER_PORT); //converte o numero da porta para o endereço corret

  //Cria o socket do cliente
  SOCKET sock = INVALID_SOCKET;
  sock = socket(AF_INET,SOCK_DGRAM,0);
  if(sock == INVALID_SOCKET) {
    printf("Erro ao criar o socket\n");
    return(1);
  }

  //Vincula informações do IP e Porta do cliente.
  struct sockaddr_in clientAddress;
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  clientAddress.sin_port = htons(0);

  //Associa o endereço cliente vinculado ao sockets do cliente, verificando erros
  result = bind(sock, (struct sockaddr *) &clientAddress, sizeof(clientAddress));
  if(result == SOCKET_ERROR) {
      printf("Nao foi possivel associar o endereço/porta ao socket\n");
      closesocket(sock); //encerra o socket
      return(1);
  }

  //Envio de dados
  int hostLength = sizeof(hostAddress);
  for(int i = 2; i < argc; i++) {
        //envia a menssagem i pelo socket(sock) para o endereço na estrutura hostAddres
        result = sendto(sock, argv[i], strlen(argv[i])+1, 0,
        (LPSOCKADDR) &hostAddress, sizeof(struct sockaddr));

        //Verifica possiveis erros
        if(result == SOCKET_ERROR) {
          printf("Nao pode enviar dados %d \n",i-1);
          closesocket(sock); //encerra o socket
          return 1;
        }

        result = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &hostAddress, &hostLength);
        if(result == SOCKET_ERROR){
          printf("Nao foi possivel receber os dados \n");
        }
        else{ //caso nao ocorra erro, imprima a mensagem e o remetente
          printf("De %s:UDP %u: %s\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port),buffer);
        }
  }
  //encerra o socket e finaliza o programa
  closesocket(sock);
  WSACleanup();
  return EXIT_SUCCESS;
}

/*
RESUMO DO PROGRAMA:
O programa assima tem por caracteristica, criar um socket e utiliza-lo para enviar menssagens
para um endereço de servidor, econtrado por um nome fornecido no prompt.
ETAPAS:
1)INICIALIZA BIBLIOTECAS.
2)DESCOBRE E VINCULA AS INFORMACOES DO ENDERECO HOST A UMA PORTA (GetHostByName() & socketaddr_in).
3)CRIA UM SOCKET E O ASSOCIA AO ENDEREÇO DO CLIENTE [A MÁQUINA COM ESSE PROGRAMA] (socket() & bind()).
4)ENVIA AS MENSAGENS USANDO O SOCKET PARA O ENDERECO DO SERVIDOR ANTES ENCONTRADO.
*/