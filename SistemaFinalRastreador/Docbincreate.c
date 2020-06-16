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
  INDICE i1 = {
    .titulo = "Butterfly.jpg",
    .ip = "127.0.0.1",
    .porta = "5000"
  };

  INDICE i2 = {
    .titulo = "Floricultura.mp4",
    .ip = "127.0.0.1",
    .porta = "5000"
  };

  INDICE i3 = {
    .titulo = "Redes.jpg",
    .ip = "127.0.0.1",
    .porta = "8000"
  };

  INDICE i4 = {
    .titulo = "Ipv4eIpv6.mp4",
    .ip = "127.0.0.1",
    .porta = "8000"
  };

  INDICE i5 = {
    .titulo = "culinaria.jpg",
    .ip = "127.0.0.1",
    .porta = "5000"
  };

  INDICE i6 = {
    .titulo = "ArrozMasterchef.mp4",
    .ip = "127.0.0.1",
    .porta = "8080"
  };

  INDICE i7 = {
    .titulo = "Leao.webp",
    .ip = "127.0.0.1",
    .porta = "8000"
  };

  INDICE i8 = {
    .titulo = "Colorida.jpg",
    .ip = "127.0.0.1",
    .porta = "5000"
  };

  INDICE i9 = {
    .titulo = "tiktok.png",
    .ip = "127.0.0.1",
    .porta = "8080"
  };

  FILE *writer = fopen("Data.bin", "wb");
  fwrite(&i1, sizeof(INDICE), 1, writer);
  fwrite(&i2, sizeof(INDICE), 1, writer);
  fwrite(&i3, sizeof(INDICE), 1, writer);
  fwrite(&i4, sizeof(INDICE), 1, writer);
  fwrite(&i5, sizeof(INDICE), 1, writer);
  fwrite(&i6, sizeof(INDICE), 1, writer);
  fwrite(&i7, sizeof(INDICE), 1, writer);
  fwrite(&i8, sizeof(INDICE), 1, writer);
  fwrite(&i9, sizeof(INDICE), 1, writer);
  fclose(writer);
  
  return EXIT_SUCCESS;
}