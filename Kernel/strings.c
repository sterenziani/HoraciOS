#include <strings.h>

int strlen(char* str){
	int i=0;
	while(str[i] != 0){
		i++;
	}
	return i;
}

int strncpy(char* dest, char *src, int n){
	int i;
	for (i = 0; i < n-1 && src[i] != 0; ++i)	{
		dest[i] = src[i];
	}
	dest[i] = 0;
	return 1;
}

void toString(int num,char* resp){
	int digit=0;
	char aux;
	if(num!=0){
    // Arma el numero en el vector
		while(num!=0){
			resp[digit]=(num%10)+'0';
			num=num/10;
			digit++;
		}
    //invierte el orden de los numeros
		for(int i=0;i<digit/2;i++){
			aux=resp[i];
			resp[i]=resp[digit-i-1];
			resp[digit-i-1]=aux;
		}
    //agrega el 0 al final
		resp[digit]=0;
	}
  else{
    //si el numero es cero carga un cero en el vector
		resp[0]='0';
	}
	return;
}

int strcmp(char string1[], char string2[])
{
    for (int i = 0; ; i++)
    {
        if (string1[i] != string2[i])
        {
            return string1[i] < string2[i] ? -1 : 1;
        }

        if (string1[i] == '\0')
        {
            return 0;
        }
    }
}
