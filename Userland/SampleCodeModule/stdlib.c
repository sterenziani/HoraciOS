#include <SysCallDispatcher.h>
#include <stdlib.h>
#define BLACK_BACKGROUND 0x000000
#define NORMAL 0
#define ESP 1
#define BUFFER_SIZE 60

void setCursor(int x, int y)
{
	SysCallDispatcher(6, (uint64_t)x, (uint64_t)y, 0, 0, 0, 0);
}

void beep()
{
	SysCallDispatcher(5, 0, 0, 0, 0, 0, 0);
}

int screen_width()
{
	int ret_value;
	SysCallDispatcher(7,0,(uint64_t)&ret_value,0, 0, 0, 0);
	return ret_value;
}

int screen_height()
{
	int ret_value;
	SysCallDispatcher(7, 1, (uint64_t)&ret_value, 0, 0, 0, 0);
	return ret_value;
}

int char_width()
{
	int ret_value;
	SysCallDispatcher(7,2,(uint64_t)&ret_value,0, 0, 0, 0);
	return ret_value;
}

int char_height()
{
	int ret_value;
	SysCallDispatcher(7, 3, (uint64_t)&ret_value, 0, 0, 0, 0);
	return ret_value;
}

void clearScreen()
{
	for(int x=0;x<screen_width();x++) {
		for(int y=0;y<screen_height();y++) {
			SysCallDispatcher(1,x,y,BLACK_BACKGROUND, 0, 0, 0);
		}
	}
}

char myGetChar()
{
  char c=0;
    SysCallDispatcher(3,(uint64_t)&c,0,0,0,0, 0);
  return c;
}

void myPutchar(int character)
{
  if(character=='\n')
    SysCallDispatcher(4,2,0,0,0,0, 0);
  else
    SysCallDispatcher(4,0,(uint64_t)&character,1,0,0, 0);
  return;
}

void myPutcharBig(int character,int color) {
	if(character=='\n')
    	SysCallDispatcher(8,0,0,0,0,0, 0);
  	else
   		SysCallDispatcher(8,color,(uint64_t)&character,1,0,0, 0);
  	return;
}

void myPrintf(char * cadena,...)
{
	char resp[BUFFER_SIZE];
	va_list ap;
	va_start (ap,cadena);
	int dim=strlen(cadena);
	int caso=NORMAL;
  	int index=0;
	for(int i=0;i<dim;i++){
		if(caso==NORMAL){
			if(cadena[i]!= '%')
				myPutchar(cadena[i]);
			else
				caso=ESP;
		}
    else{
			switch(cadena[i]){
				case 'd':
					toString(va_arg (ap, int),resp);
					while(resp[index]!=0)
						myPutchar(resp[index++]);
					caso=NORMAL;
					break;
				case 'c':
					myPutchar((char)(va_arg(ap,int)));
					caso=NORMAL;
					break;
				case 's':
					myPrintf(va_arg(ap, char*));
					caso=NORMAL;
					break;
				default:
					myPutchar('%');
					myPutchar(cadena[i]);
					caso=NORMAL;
					break;
			}
		}

	}
	va_end(ap);
}

void myPrintfBig(int color,char * cadena,...) {
	char resp[BUFFER_SIZE];
	va_list ap;
	va_start (ap,cadena);
	int dim=strlen(cadena);
	int caso=NORMAL;
  	int index=0;
	for(int i=0;i<dim;i++){
		if(caso==NORMAL){
			if(cadena[i]!= '%')
				myPutcharBig(cadena[i],color);
			else
				caso=ESP;
		}
    else{
			switch(cadena[i]){
				case 'd':
					toString(va_arg (ap, int),resp);
					while(resp[index]!=0)
						myPutcharBig(resp[index++],color);
					caso=NORMAL;
					break;
				case 'c':
					myPutcharBig((char)(va_arg(ap,int)),color);
					caso=NORMAL;
					break;
				case 's':
					myPrintfBig(color,va_arg(ap, char*));
					caso=NORMAL;
					break;
				default:
					myPutcharBig('%',color);
					myPutcharBig(cadena[i],color);
					caso=NORMAL;
					break;
			}
		}

	}
	va_end(ap);
}

void myPrintHex(uint64_t hex_number)
{
  SysCallDispatcher(11, hex_number, 0, 0, 0, 0, 0);
}

int strlen(char* str){
	int i=0;
	while(str[i]!= '\0'){
		i++;
	}
	return i;
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

int isNumber(char* s)
{
	int i=0;
	while(s[i] != 0)
	{
		if(s[i] < '0' || s[i] > '9')
			return 0;
		i++;
	}
	return 1;
}

int myAtoi(char *p)
{
  int k = 0;
  while (*p)
	{
      k = (k << 3) + (k << 1) + (*p) - '0';
      p++;
 }
 return k;
}
