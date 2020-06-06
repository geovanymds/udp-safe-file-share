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

int decToBinary(int n, char binaryNum[]) 
{ 
    // counter for binary array 
    int i = 0; 
    while (n > 0) { 
  
        // storing remainder in binary array 
        binaryNum[i] = n % 2; 
        n = n / 2; 
        i++; 
    } 
    return(i);
}


