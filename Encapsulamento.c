#include <stdio.h>
  #include <stdlib.h>
#include <sys/stat.h>
  #include <string.h>
//PARA EXECUTAR USE:
//gcc -Wall  Encapsulamento.c -o execE
//./execE nomedoarquivo.extenção

typedef struct cabecalho{
  //CABEÇALHO, altere conforme necessário
  long int size;
  long int nameSize;
}CABECALHO;

int main(int argc, char *argv[]){
  FILE *reader, *writer;
  struct stat status;

  if(argc < 2){ //verifica se passaram um nome de um arquivo
    printf("Faltam Argumentos!\n");
    printf("Ex: <nome do arquivo>");
    return(1);
  }
  
  //Abre o arquivo
  stat(argv[1], &status);
  reader = fopen(argv[1], "rb");
  if(reader == NULL){ //verifi erros na abertura
    perror("Erro ao abrir o arquivo reader.\n");
    return(2);
  }

  //Leia o aquivo
  unsigned char *arq = (unsigned char *) malloc(status.st_size * sizeof(unsigned char));
  fread(arq, sizeof(unsigned char), status.st_size, reader);
  fclose(reader); //fecha o arquivo

  //Adiciona os cabeçalhos
  CABECALHO sender = {
    .size = status.st_size,
    .nameSize = strlen(argv[1])
  };

  //Encapsula o segmento
  writer = fopen("capsula.bin", "wb");
  if(writer == NULL){ //verifica erros
    perror("falha na criacao do segmento.\n");
    return(3);
  }
  fwrite(&sender, sizeof(CABECALHO), 1, writer);
  fwrite(argv[1], sizeof(char), strlen(argv[1]), writer);
  fwrite(arq, sizeof(unsigned char), status.st_size, writer);
  fclose(writer); //fecha o arquivo 

  return EXIT_SUCCESS;
}