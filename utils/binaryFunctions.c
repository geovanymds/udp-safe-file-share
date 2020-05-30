#include <stdio.h>
#include <string.h>
#include "binaryFunctions.h"

//Soma
char add(char a[], char b[], char sum[])
{

    int index;
    char carry='0';
    printf("\na=%s", a);
    printf("\nb=%s", b);

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

    printf("\nSum=%s", sum);
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

// Ajusta o mais um do algarismo mais significativo 

void checkSum(char w1[],char w2[],char w3[]) {
    char wordAux[] = "0000000000000000";
    char carryAdd;

    carryAdd = add(w1,w2,wordAux);
    if(carryAdd=='1') {
        carryAdd = add(wordAux,"0000000000000001",w3);
    }
    comp(w3);
    printf("\nChecksum=%s", w3);
}

int main () {
    char word[]="0000000000000000";
    checkSum("1000100100100101","1000010111110001",word);
}
