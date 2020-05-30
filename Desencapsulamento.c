#include <stdio.h>
  #include <stdlib.h>
#include <sys/stat.h>
  #include <string.h>
//PARA EXECUTAR USE:
//gcc -Wall  Desencapsulamento.c -o execD
//./execD

typedef struct cabecalho{
  //CABEÇALHO, altere conforme necessário
  long int size;
  long int nameSize;
}CABECALHO;

//Função apenas para não ter duplicadedade de nomes
char* copy(char *titulo){
  char *copy;
  copy = "COPY";
  strcat(copy, titulo);
  return copy;
}

int main(int argc, char *argv[]){
  FILE *reader, *writer;
  CABECALHO receiver;

  //Le o segmento recebido
  reader = fopen("capsula.bin", "rb");
  if(reader ==  NULL){ //verifica erros
    perror("Falha ao abrir a capsula\n");
    return(1);  
  }
  fread(&receiver, sizeof(CABECALHO), 1, reader); //Le a estrutura do cabeçalho
  char *titulo = (char *) malloc(receiver.nameSize * sizeof(char)); //aloca espaço para receber o título
  fread(titulo, sizeof(char), receiver.nameSize, reader); //recebe o titulo
  unsigned char *arq = (unsigned char *)  malloc(receiver.size * sizeof(unsigned char)); //aloca espaço para receber o arquivo
  fread(arq, sizeof(unsigned char), receiver.size, reader); //Recebe o arquivo
  fclose(reader); //fecha o arquivo

  //Realiza o desencapsulamento
  writer = fopen("COPIADOARQUIVO.jpg", "wb"); //mude a extenssão para a mesma do arquivo
  if(writer == NULL){ //verifica erros
    printf("to aqui no erro");                 
    perror("Falha no desencapsulamento.\n");
    return(2);
  }
  fwrite(arq, sizeof(unsigned char), receiver.size, writer);
  fclose(writer); //fecha o arquivo
  
  printf("aquivo %s, recebido com sucesso!", titulo);
  return EXIT_SUCCESS;
}