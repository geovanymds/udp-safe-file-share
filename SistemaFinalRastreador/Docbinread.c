#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct indice
{
  char titulo[50];
  char ip[16];
  char porta[6];
}INDICE;

int main(int argc, char *argv[]){
  INDICE i;
  FILE *reader = fopen("Data.bin", "rb");
  while(!feof(reader)){
    fread(&i, sizeof(INDICE), 1, reader);
    printf("Nome: %s\n", i.titulo);
    printf("Ip: %s\n", i.ip);
    printf("Porta: %s\n\n", i.porta);
  }
  return EXIT_SUCCESS;
}