#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <math.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctype.h>
#include <string.h>



main(){
	int ido1=0 ;   
        int *addrido;
        unsigned char *dummyido;  

        addrido=&ido1;

        dummyido= (unsigned char *)addrido;

	printf("%d\n",ido1);
	*dummyido=0x20;
	*dummyido++;

	printf("%d\n",ido1);
	*dummyido=0x16;
	*dummyido++;

	printf("%d\n",ido1);
	*dummyido=0x16;
	*dummyido++;

	printf("%d\n",ido1);
	*dummyido=0x06;
	*dummyido++;

	printf("%d\n",ido1);
	*dummyido=0x02;
	*dummyido++;

	printf("%d\n",ido1);
	*dummyido=0x50;
	*dummyido++;

	printf("%d\n",ido1);
	*dummyido=0xF6;
	*dummyido++;

	printf("%d\n",ido1);
	*dummyido=0x01;
	*dummyido++;


	printf("%d\n",ido1);



}
