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
#include <sys/time.h>


#define DEV_NAME    "/dev/ttyUSB0"        // デバイスファイル名　ここは環境に合わせて変える必要がある
#define BAUD_RATE    B921600              // RS232C通信ボーレート
#define BUFF_SIZE    4096                 // 適当

    int accelX,accelY,accelZ;
    int aX,aY,aZ;
    int vX1=0,vY1=0,vZ1=0,vX2,vY2,vZ2,vX3,vY3,vZ3;
    float pX1,pY1,pZ1,pX2,pY2,pZ2,pX3,pY3,pZ3;
    
    int *addrX,*addrY,*addrZ;
    unsigned char *dummyX,*dummyY,*dummyZ;  
    float dt,kt,ut,at;
    unsigned long previoustime, currenttime;

    struct timeval tv;

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

		printf("a\n");

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
  while(1){
    int i,t,k=0,l=0,m=0,n=0,o=0,p=0;
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

    gettimeofday(&tv,NULL);  //	時間を測定
    previoustime = currenttime;
    currenttime = 1000000 * tv.tv_sec + tv.tv_usec;
    dt = (currenttime - previoustime) / 1000000.0;
//  printf("dt=%06f\n",dt);	
    kt=kt+dt;
    
//  printf("kt1=%06f\n",kt);

    for(n=0;n<=len;n++){ //受信データ初期化
	buffer[n]=0;
    }

    // ここで受信待ち
    len=read(fd,buffer,BUFF_SIZE);
    if(len==0){
      // read()が0を返したら、end of file
      // 通常は正常終了するのだが今回は無限ループ
      continue;
    }
    if(len<0){
      printf("%s: ERROR\n",argv[0]);
      // read()が負を返したら何らかのI/Oエラー
      perror("");
      exit(2);
    }
    // read()が正を返したら受信データ数


//    printf("-----------------------------得られたデータ------------------------\n");

    // 受信したデータを 16進数形式で表示    
    for(i=0; i<len; i++){
      unsigned char data=buffer[i];
//      printf("%02X ",data);
    
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
//	  printf("i=%d\n",i);
//	  printf("\n");

	  if(buffer[i+7]==0x40 && buffer[i+6]==0x00){
		k=i;
	  }

        }
        rcounter=0;
        break;
      }
    }     
//  printf("\n");
//  printf("------------------------------航法データ-------------------------------------\n");
  
  o=k+1;
  l=k+1;
  m=l+87;
 
/*  while(l<m){		//航法データのみ表示	　　　　
	if(l==0 || l==1)break;
	unsigned char hoge=buffer[l];
	printf("%02X ",hoge);
	l++;
  }*/

//  printf("\n");

  if(o==0 || o==1)continue;
  if(buffer[k+7]==0x40 && buffer[i+6]==0x00){ 

	for(i=0;i<=3;i++){  		//加速度を抽出

		*dummyX=buffer[k+i+24];
		*dummyY=buffer[k+i+28];
		*dummyZ=buffer[k+i+32];

		*dummyX++;
		*dummyY++;
		*dummyZ++;
	
	}

	aX=aX; 		        	  //分解能0.001
	aY=aY;
	aZ=aZ/9.80665;

	vX2=(accelX+aX)*0.01/2;  	  //加速度→速度
	vY2=(accelY+aY)*0.01/2;
	vZ2=(accelZ+aZ)*0.01/2;

	pX1=(vX1+vX2)*0.01/2;             //速度→位置
	pY1=(vY1+vY2)*0.01/2;
	pZ1=(vZ1+vZ2)*0.01/2;

	accelX=aX;  
	accelY=aY;
	accelZ=aZ; 

	vX1=vX2;
	vY1=vY2;
	vZ1=vZ2;

	pX2=pX2+pX1;
	pY2=pY2+pY1;
	pZ2=pZ2+pZ1;

//	if(kt >= 0.10){  //0.1秒経った時

//	printf("加速度x=%d 加速度y=%d 加速度z=%d\n",aX,aY,aZ);
//	printf("ax=%d ay=%d az=%d\n",accelX,accelY,accelZ);
//	printf("速度x=%d 速度y=%d 速度z=%d\n",vX2,vY2,vZ2);

	pX3=pX3+pX2;
	pY3=pY3+pY2;
	pZ3=pZ3+pZ2;

//	printf("px=%d py=%d pz=%d\n",pX,pY,pZ);
//	printf("変位x=%d 変位y=%d 変位z=%d\n",pX3,pY3,pZ3);
//	printf("\n");

//	printf("kt2=%06f\n",kt);

	ut=ut+kt;
//	printf("ut=%f\n",ut);

	kt=0.0;

	accelX=0.0;
      	accelY=0.0;
       	accelZ=0.0;

	vX1=0.0;
	vY1=0.0;
	vZ1=0.0;

	pX2=0.0;
	pY2=0.0;
	pZ2=0.0;

	if(ut>3.0){

		at=at+ut;

		pX3=pX3/300;	//ノイズが大きいので0.01秒あたりの移動量を求める
		pY3=pY3/300;
		pZ3=pZ3/300;

		printf("変位x=%f 変位y=%f 変位z=%f\n",pX3,pY3,pZ3);

		pX3=0.0;
		pY3=0.0;
		pZ3=0.0;

		ut=0.0;
	}
 // }

   }

  else ;


 }
}

