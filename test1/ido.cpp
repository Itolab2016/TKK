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


using namespace std;

#define DEV_NAME    "/dev/ttyUSB0"        // デバイスファイル名　ここは環境に合わせて変える必要がある
#define BAUD_RATE    B921600              // RS232C通信ボーレート
#define BUFF_SIZE    4096                 // 適当

    double accelX,accelY,accelZ;
    int q=0,r=0;
    int aX,aY,aZ,ido1,keido1,ido2[10000],keido2[10000];
    float ido3,keido3;
    double vX1=0,vY1=0,vZ1=0,vX2,vY2,vZ2;
    double pX1,pY1,pZ1,pX2=0.0,pY2=0.0,pZ2=0.0,pX3,pY3,pZ3,pX4,pY4,pZ4;
    double scale1,scale2,scale3;

    double hoge1[10000],hoge2[10000],hoge3[10000],time1[10000],sokudox[10000],sokudoy[10000],sokudoz[10000],times1=0.0;
    double idoux[10000],idouy[10000],idouz[10000];
    
    int *addrX,*addrY,*addrZ,*addrido,*addrkeido;
    unsigned char *dummyX,*dummyY,*dummyZ,*dummyido,*dummykeido;  
    float dt,kt,ut;
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
    int i,t,kouho=0,INS=0,l=0,m=0,n=0,o=0,p=0;
    int len;                            //  受信データ数（バイト）
    unsigned char buffer[BUFF_SIZE];    // データ受信バッファ
    int rcounter=0;

    aX=0.0;
    aY=0.0;
    aZ=0.0;

    addrX=&aX;
    addrY=&aY;
    addrZ=&aZ;

    addrido=&ido1;
    addrkeido=&keido1;

    dummyX= (unsigned char *)addrX;
    dummyY= (unsigned char *)addrY;
    dummyZ= (unsigned char *)addrZ;

    dummyido= (unsigned char *)addrido;
    dummykeido= (unsigned char *)addrkeido;

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

	  if(buffer[i+7]==0x40){
		kouho=i;
	  }
	  else if(buffer[i+7]==0x20){
		INS=i;
	  }

        }
        rcounter=0;
        break;
      }
    }     

  o=kouho;
  l=INS;
  m=l+63;


  /* while(l<m){		//航法データのみ表示	　　　　
	if(l==0 || l==1)break;
	unsigned char hoge=buffer[l];
	printf("%02X ",hoge);
	l++;
  }*/

 // printf("\n");

  if(buffer[INS+8+38]!=0x04 || INS==0 || buffer[INS+8+31]!=0x01 )continue; //GPSが有効,屋外モードになるまでループ


  if(buffer[kouho+7]==0x40 ){ 
	for(i=0;i<=3;i++){  		//加速度を抽出

		*dummyX=buffer[kouho+i+8+17];
		*dummyY=buffer[kouho+i+8+21];
		*dummyZ=buffer[kouho+i+8+25];

	//	printf("16進数=%d %d %d \n",kouho+i+8+17,kouho+i+8+21,kouho+i+8+25);
	//	printf("aX=%d aY=%d aZ=%d\n",aX,aY,aZ);

		*dummyX++;
		*dummyY++;
		*dummyZ++;

	//	printf("16進数=%d\n",kouho+i+8+17);
	
	}
//	printf("\n");

/*	if(16700000<=aX<=17000000 || 16700000<=aX<=17000000 ||16700000<=aX<=17000000 ){
			aX=0.0;
			aY=0.0;
			aZ=0.0;
	}
*/
	for(i=0;i<=7;i++){  		//緯度,経度を抽出

		*dummyido=buffer[kouho+i+8+41];
		printf("%02X ",buffer[kouho+i+8+41]);

		*dummyido++;
	
	}
	printf("\n");
	for(i=0;i<=7;i++){
		*dummykeido=buffer[kouho+i+8+49];
		printf("%02X ",buffer[kouho+i+8+49]);

		*dummykeido++;
	}

	printf("\n");

	ido3=ido1/(10^9*10);
	keido3=keido1/(10^9*10);		

	printf("ido=%d ",ido1);
	printf("keido=%d \n",keido1);

//	printf("加速度x=%d 加速度y=%d 加速度z=%d\n",aX,aY,aZ);
//	printf("\n");

	aX=aX; 		       	  //分解能0.001
	aY=aY;
	aZ=aZ-9.80665;


	hoge1[q]=aX;
	hoge2[q]=aY;
	hoge3[q]=aZ;

	times1=times1+0.01;
	time1[q]=times1;

	vX2=(accelX+aX)*0.01/2;  	  //加速度→速度
	vY2=(accelY+aY)*0.01/2;
	vZ2=(accelZ+aZ)*0.01/2;

	sokudox[q]=vX2;
	sokudoy[q]=vY2;
	sokudoz[q]=vZ2;

	pX1=(vX1+vX2)*0.01/2;             //速度→位置
	pY1=(vY1+vY2)*0.01/2;
	pZ1=(vZ1+vZ2)*0.01/2;

	idoux[q]=pX1;
	idouy[q]=pY1;
	idouz[q]=pZ1;


	accelX=aX;  
	accelY=aY;
	accelZ=aZ; 

	vX1=vX2;
	vY1=vY2;
	vZ1=vZ2;

	pX2=pX2+pX1;
	pY2=pY2+pY1;
	pZ2=pZ2+pZ1;
	
	pX3=pX2;
	pY3=pY2;
	pZ3=pZ2;


//	printf("加速度x=%d 加速度y=%d 加速度z=%d\n",aX,aY,aZ);
//	printf("ax=%d ay=%d az=%d\n",accelX,accelY,accelZ);
//	printf("速度x=%d 速度y=%d 速度z=%d\n",vX2,vY2,vZ2);
	
//	printf("px=%f py=%f pz=%f\n",pX1,pY1,pZ1);

//	printf("変位x=%f 変位y=%f 変位z=%f\n",pX3,pY3,pZ3);
//	printf("\n");
	
	ut=ut+kt;
//	printf("kt2=%06f\n",kt);
//	printf("ut=%f\n",ut);
	
	kt=0.0;
	
	accelX=0.0;
      	accelY=0.0;
       	accelZ=0.0;

	vX1=0.0;
	vY1=0.0;
	vZ1=0.0;
	
//	printf("ido=%d\n",ido);
//	printf("keido=%d\n",keido);

//	ido2[q]=ido1;
//	keido2[q]=keido1;

/*	char gps[255];		//緯度,経度データをtxt化
	sprintf(gps,"gps.txt");
	char dgps[255];
	ofstream GPS(gps);

	for(i=0;i<=r;i++){
		sprintf(dgps,"%d %d",ido2[i],keido2[i]);
		GPS<<dgps<<endl;
	}
	GPS.close();
*/
	ido2[q]=ido1;
	keido2[q]=keido1;

	q=q+1;

	

	if(ut>3.0){			//3秒経過

		

	//	printf("緯度=%d 経度=%d\n",ido2[r],keido2[r]);
		r=r+1;


		scale1=sqrt(pX3*pX3+pY3*pY3+pZ3*pZ3);	

		char gps[255];		//緯度,経度データをtxt化
		sprintf(gps,"gps.txt");
		char dgps[255];
		ofstream GPS(gps);

		for(i=0;i<=10000;i++){
			sprintf(dgps,"%d %d",ido2[i],keido2[i]);
			GPS<<dgps<<endl;
		}
		GPS.close();


		char data[255];		//移動データをtxt化
		sprintf(data,"test.txt");
		char ddata[255];
		ofstream fs(data);

		for(i=0;i<=1000;i++){
			sprintf(ddata,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %f ",hoge1[i],hoge2[i],hoge3[i],sokudox[i],sokudoy[i],sokudoz[i],idoux[i],idouy[i],idouz[i],time1[i]);
			fs<<ddata<<endl;
		}
		fs.close();

	
	//	printf("scale=%f\n",scale1);
	//	printf("変位x=%f 変位y=%f 変位z=%f\n",pX2,pY2,pZ2);
	
		pX3=0.0;
		pY3=0.0;
		pZ3=0.0;

		ut=0.0;
	}

   }
  }
}
