#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <SysCallDispatcher.h>


int myScanf(char* format,...){
	va_list ap;
	va_start( ap, format );

	int cont=0;
	int i=0,j=0;
	char * buffer = {0};
	SysCallDispatcher(3,0,(uint64_t)buffer,0,0,0, 0);
	int* num;
 	char* str;

	while(format[i]){
		if(format[i]!='%'){
      if((format[i]) != (buffer[j])){
          return cont;
      }
      else {
           i++;
           j++;
      }
		}
		else {
			i++;
			switch(format[i]){
				case '%':
                    if(buffer[j] != '%')
                    	return cont;
                    j++;
                    break;
				case 'd':
					num = va_arg(ap,int*);
					*num = 0;
					while(buffer[j]<='9' && buffer[j]>='0') {
						*num = (*num) *10 + buffer[j] - '0';
						j++;
					}
				    cont++;
					break;
				case 'c':
                    str = va_arg(ap, char*);
                    *str = buffer[j];
                    j++;
                    cont++;
                    break;
				case 's':
                    str = va_arg(ap,char*);
                    while(buffer[j]){
                        *str = buffer[j];
                        str++;
                        j++;
                    }
                    str = 0;
                    cont++;
			}
			i++;
		}
	}

	return cont;
}
