/***********************************************************************/
/*                                                                     */
/* GSM-MG100受信用プログラム                                           */
/* シリアルポート受信サンプルプログラム                                */
/* Y.Ebihara (SiliconLinux)さんのプログラムを改造して                  */
/* GSM-MG100からのデータを受信する簡易プログラム                       */
/*                                                                     */
/* このプログラムはシリアルポートをopenして、データを16進数表示する    */
/* サンプルプログラムです。                                            */
/*   Ubuntuで動作検証をしています。                                    */
/*                                                                     */
/* 2016.9 Kouhei Ito @ KTC                                             */
/*                                                                     */
/***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#define DEV_NAME    "/dev/ttyUSB0"        // デバイスファイル名　ここは環境に合わせて変える必要がある
#define BAUD_RATE    B921600              // RS232C通信ボーレート
#define BUFF_SIZE    4096                 // 適当

// シリアルポートの初期化
void serial_init(int fd)
{
  struct termios tio;
  memset(&tio,0,sizeof(tio));
  tio.c_cflag = CS8 | CLOCAL | CREAD;
  tio.c_cc[VTIME] = 100;
  // ボーレートの設定
  cfsetispeed(&tio,BAUD_RATE);
  cfsetospeed(&tio,BAUD_RATE);
  // デバイスに設定を行う
  tcsetattr(fd,TCSANOW,&tio);
}


/* --------------------------------------------------------------------- */
/* メイン                                                                */
/* --------------------------------------------------------------------- */

int main(int argc,char *argv[]){
  int fd;

    int accelX,accelY,accelZ,aX,aY,aZ;
    int *addrX,*addrY,*addrZ;
    unsigned char *dummyX,*dummyY,*dummyZ;  

    accelX=0.0;
    accelY=0.0;
    accelZ=0.0;

  // デバイスファイル（シリアルポート）オープン
  fd = open(DEV_NAME,O_RDWR);
  if(fd<0){
    // デバイスの open() に失敗したら
    perror(argv[1]);
    exit(1);
  }

  // シリアルポートの初期化
  serial_init(fd);

  // メインの無限ループ
//  while(1){
    int i,k=0,l=0,m=0,n=0;
    int len;                            //  受信データ数（バイト）
    unsigned char buffer[BUFF_SIZE];    // データ受信バッファ
    int rcounter=0;

    aX=0.0;
    aY=0.0;
    aZ=0.0;

    addrX=&aX;
    addrY=&aY;
    addrZ=&aZ;

    dummyX= (unsigned char *)addrX;
    dummyY= (unsigned char *)addrY;
    dummyZ= (unsigned char *)addrZ;
    
    // ここで受信待ち
    len=read(fd,buffer,BUFF_SIZE);
    if(len==0){
      // read()が0を返したら、end of file
      // 通常は正常終了するのだが今回は無限ループ
//      continue;
    }
    if(len<0){
      printf("%s: ERROR\n",argv[0]);
      // read()が負を返したら何らかのI/Oエラー
      perror("");
      exit(2);
    }
    // read()が正を返したら受信データ数


    printf("-----------------------------得られたデータ------------------------\n");

    // 受信したデータを 16進数形式で表示    
    for(i=0; i<len; i++){
      unsigned char data=buffer[i];
      printf("%02X ",data);
    
      //受信データは意味の塊ごとに先頭に16進数で　16 16 06 02　と言うデータがつくことになっているので
      //以下でそのデータを受信するごとに改行して表示するようになっている．
      switch(rcounter){
      case 0:
        if (data==0x16)rcounter++;
        else rcounter=0;
        break;
      case 1:
        if (data==0x16)rcounter++;
        else rcounter=0;
        break;
      case 2:
        if (data==0x06)rcounter++;
        else rcounter=0;
        break;
      case 3:
        if (data==0x02){
          printf("\n");
	  printf("i=%d\n",i);

	  if(buffer[i+7]==0x43){
		k=i;
	  }
        }
        rcounter=0;
        break;
      }
    }     
  printf("\n");
  printf("------------------------------センサーデータ-------------------------------------\n");
  

  l=k+1;
  m=l+87;
 
  while(l<m){		//センサーデータのみ表示	　　　　
	if(l==0 || l==1)break;
	unsigned char hoge=buffer[l];
	printf("%02X ",hoge);
	l++;
  }

  printf("\n");

  if(buffer[k+7]==0x43){
	for(i=0;i<=3;i++){
		printf("point=%02X\n",buffer[k+i+20]);
		*dummyX=buffer[k+i+20];
		*dummyY=buffer[k+i+24];
		*dummyZ=buffer[k+i+28];

		*dummyX++;
		*dummyY++;
		*dummyZ++;
	
	}

	aX=aX*0.000001;
	aY=aY*0.000001;
	aZ=aZ*0.000001;

	printf("加速度x1=%d 加速度y1=%d 加速度z1=%d\n",aX,aY,aZ);

  	accelX=accelX+aX;
	accelY=accelY+aY;
	accelZ=accelZ+aZ;

	printf("ax=%d ay=%d az=%d\n",accelX,accelY,accelZ);

	FILE *file;
	file = fopen("test2.txt","w");
	fprintf(file,"%d %d %d",accelX,accelY,accelZ);
	fclose(file);
  }
  
  
  for(n=0;n<=len;n++){ //受信データ初期化
	buffer[n]=0;
  }

// }
}

