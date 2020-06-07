#include <stdio.h>
#include <string.h>
#include "binaryFunctions.h"

//Soma
char add(char a[], char b[], char sum[])
{

    int index;
    char carry='0';

    for (index = 15; index >= 0; index--)
    {
        if (a[index] == '0' && b[index] == '0' && carry == '0')
        {
            sum[index] = '0';
            carry = '0';
        }
        else if (a[index] == '0' && b[index] == '0' && carry == '1')
        {
            sum[index] = '1';
            carry = '0';
        }
        else if (a[index] == '0' && b[index] == '1' && carry == '0')
        {
            sum[index] = '1';
            carry = '0';
        }
        else if (a[index] == '0' && b[index] == '1' && carry == '1')
        {
            sum[index] = '0';
            carry = '1';
        }
        else if (a[index] == '1' && b[index] == '0' && carry == '0')
        {
            sum[index] = '1';
            carry = '0';
        }
        else if (a[index] == '1' && b[index] == '0' && carry == '1')
        {
            sum[index] = '0';
            carry = '1';
        }
        else if (a[index] == '1' && b[index] == '1' && carry == '0')
        {
            sum[index] = '0';
            carry = '1';
        }
        else if (a[index] == '1' && b[index] == '1' && carry == '1')
        {
            sum[index] = '1';
            carry = '1';
        }
        else
            break;
    }
    return (carry);
}

//Complemento
void comp(char sum[])
{
    int index;

    for (index = 0; index < 16; index++)
    {
        if (sum[index] == '0')
        {
            sum[index] = '1';
        }
        else
        {
            sum[index] = '0';
        }
    }

    return;
}

void decToBinary(int n, char binaryNum[]) 
{ 
    int r; // Resultado do deslocamento
    int i; // Contador

    // Utiliza um número de 32 bits como base para a conversão.
    for(i = 31; i >= 0; i--) {
    // Executa a operação shift right até a 
    // última posição da direita para cada bit.
    r = n >> i;
            
    // Por meio do "e" lógico ele compara se o valor 
    // na posição mais à direita é 1 ou 0 
    // e imprime na tela até reproduzir o número binário.
    if(r & 1) {
        binaryNum[31 - i] = '1';
    } else {
        binaryNum[31 - i] = '0';
    }
    }
}


