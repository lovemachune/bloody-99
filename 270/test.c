#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"
#include <pthread.h>

#include "sockop.h"
#define _7SEG_MODE_PATTERN 0
#define _7SEG_MODE_HEX_VALUE 1
#define _7SEG_D5_INDEX 8
#define _7SEG_D6_INDEX 4
#define _7SEG_D7_INDEX 2
#define _7SEG_D8_INDEX 1
#define _7SEG_ALL (_7SEG_D5_INDEX|_7SEG_D6_INDEX|_7SEG_D7_INDEX|_7SEG_D8_INDEX)
int fd,connfd;
int retval;
unsigned short data,key;
char s[1024]="";
int curpos=0,total=0,ninenine=0,choose=0;
int selected[5]={0,0,0,0,0};
char mycard[5]={'1','2','3','Q','K'};
int card=5,player;

_7seg_info_t data7;
lcd_write_info_t display;
lcd_full_image_info_t pic;
#define VK_S2 1
#define VK_S3 2
#define VK_S4 3
#define VK_S5 10
#define VK_S6 4
#define VK_S7 5
#define VK_S8 6
#define VK_S9 11
#define VK_S10 7
#define VK_S11 8
#define VK_S12 9
#define VK_S13 12
#define VK_S14 14
#define VK_S15 0
#define VK_S16 15
#define VK_S17 13
#define BUFSIZE 1024

unsigned long SEG7(char c){
	if(c=='1')
		return 0x6;
	else if(c=='2')
		return 0x5b;
	else if(c=='3')
		return 0x4f;
	else if(c=='4')
		return 0x66;
	else if(c=='5')
		return 0x6d;
	else if(c=='6')
		return 0x7d;
	else if(c=='7')
		return 0x7;
	else if(c=='8')
		return 0x7f;
	else if(c=='9')
		return 0x6f;
	else if(c=='0')
		return 0x3f;
	else if(c=='A')
		return 0x77;
	else if(c=='B')
		return 0x7c;
	else if(c=='C')
		return 0x58;
	else if(c=='D')
		return 0x5e;
	else
		return 0x80;


}
void chooseP(){
	
	while(1){
		retval=ioctl(fd,KEY_IOCTL_CHECK_EMTPY,&key);
		if(retval<0){
			continue;		
		}
		retval=ioctl(fd,KEY_IOCTL_GET_CHAR,&key);
		char c;
		c=char(key);

		if(c=='2'){
			choose=2;
			break;
		}
		if(c=='4'){
			choose=4;
			break;
		}
		if(c=='6'){
			choose=6;
			break;
		}
		
	}
	return;
}
void chooseAS(){
	
	while(1){
		retval=ioctl(fd,KEY_IOCTL_CHECK_EMTPY,&key);
		if(retval<0){
			continue;		
		}
		retval=ioctl(fd,KEY_IOCTL_GET_CHAR,&key);
		char c;
		c=char(key);

		if(c=='2'){
			choose=2;
			break;
		}
		if(c=='8'){
			choose=8;
			break;
		}
	}
	return;
}
int value(int index){
	if(mycard[index] < 65 && mycard[index] >47)
		return mycard[index]-48;
	else if(mycard[index]=='+')
		return 10;
	else if(mycard[index]=='J')
		return 11;
	else if(mycard[index]=='Q')
		return 12;
	else if(mycard[index]=='K')
		return 13;
}
void setcardpic(int index,int select){
	int i,j,def=0x080;
	for(j=0;j<0x800-select*def;j+=0x010){
		if(select==0)
			for(i=0+index*3+j;i<0+index*3+3+j;i++){
				if(i<0x5d0)
					pic.data[i]=0x0;
				else if(i<0x600)
						if(i%16==15)
							pic.data[i]=0x8f;
						else
							pic.data[i]=0xffff;
				else if(i>0x7e0)
					pic.data[i]=0xff;
				else if(i%16%3==0)
					pic.data[i]=0x8f;
				else 
					pic.data[i]=0x0;
			}
		else
			for(i=0+index*3+j;i<0+index*3+3+j;i++){
				if(i<(0x5d0-def))
					pic.data[i]=0x0;
				else if(i<(0x600-def))
						if(i%16==15)
							pic.data[i]=0x8f;
						else
							pic.data[i]=0xffff;
				else if(i>(0x7e0-def)&&i<(0x800-def))
					pic.data[i]=0xff;
				else if(i%16%3==0)
					pic.data[i]=0x8f;
				else 
					pic.data[i]=0x0;
			}
	}


}
void drawcardpic(){
	int i;
	for(i=0;i<card;i++)
		setcardpic(i,selected[i]);
	ioctl(fd,LCD_IOCTL_DRAW_FULL_IMAGE,&pic);
}
void cleancardpic(){
	int i;
	for(i=0;i<0x800;i++)
		pic.data[i]=0x0;
}
void setnumpic(int index){
	int select=selected[index];
	if(select==0){
		display.CursorX=1+index*3;
		display.CursorY=13;	
	}
	else{
		display.CursorX=1+index*3;
		display.CursorY=12;
	}
	char c[2];
	c[0]=mycard[index];
	c[1]='\0';
	display.Count=sprintf((char *)display.Msg,c);
	ioctl(fd,LCD_IOCTL_CUR_SET,&display);
	ioctl(fd,LCD_IOCTL_WRITE,&display);
	display.CursorX=1+curpos*3;
	display.CursorY=10-selected[curpos];
	ioctl(fd,LCD_IOCTL_CUR_SET,&display);
}
void drawnumpic(){
	char c[5];
	sprintf(c, "%d", total);
	display.CursorX=0;
	display.CursorY=0;
	display.Count=sprintf((char *)display.Msg,c);
	ioctl(fd,LCD_IOCTL_CUR_SET,&display);
	ioctl(fd,LCD_IOCTL_WRITE,&display);
	int i;
	for(i=0;i<card;i++)
		setnumpic(i);
}
void updatelcd(){
	ioctl(fd,LCD_IOCTL_CLEAR,NULL);
	cleancardpic();
	drawcardpic();
	drawnumpic();
}
void update7seg(){
	data7.Mode=_7SEG_MODE_PATTERN;
	data7.Which=_7SEG_D7_INDEX;
	data7.Value=SEG7(ninenine/10+48);
	ioctl(fd,_7SEG_IOCTL_SET,&data7);
	data7.Mode=_7SEG_MODE_PATTERN;
	data7.Which=_7SEG_D8_INDEX;
	data7.Value=SEG7(ninenine%10+48);
	ioctl(fd,_7SEG_IOCTL_SET,&data7);

}
void *handlescan(void *q){
	char snd[BUFSIZE]="1",rcv[BUFSIZE]="",*pch;
	char c;
	//read(connfd,rcv,BUFSIZE);
	memset(rcv,0,BUFSIZE);
	read(connfd,rcv,BUFSIZE);
	printf("%s %c\n",rcv,rcv[strlen(rcv)-1]);
	player=rcv[strlen(rcv)-1]-48;
	memset(rcv,0,BUFSIZE);
	write(connfd,snd,strlen(snd));
	read(connfd,rcv,BUFSIZE);
	printf("%s\n",rcv);
	int i=0;
	pch=strtok(rcv," ");
	if(strlen(pch)==2){
		if(pch[1]=='0')
			mycard[0]='+';
		if(pch[1]=='1')
			mycard[0]='J';
		if(pch[1]=='2')
			mycard[0]='Q';
		if(pch[1]=='3')
			mycard[0]='K';
		}
	else
		mycard[0]=pch[0];
	printf("%s\n",pch);
	pch=strtok(NULL," ");
	if(strlen(pch)==2){
		if(pch[1]=='0')
			mycard[1]='+';
		if(pch[1]=='1')
			mycard[1]='J';
		if(pch[1]=='2')
			mycard[1]='Q';
		if(pch[1]=='3')
			mycard[1]='K';
		}
	else
		mycard[1]=pch[0];
	printf("%s\n",pch);
	pch=strtok(NULL," ");
	if(strlen(pch)==2){
		if(pch[1]=='0')
			mycard[2]='+';
		if(pch[1]=='1')
			mycard[2]='J';
		if(pch[1]=='2')
			mycard[2]='Q';
		if(pch[1]=='3')
			mycard[2]='K';
		}
	else
		mycard[2]=pch[0];
	printf("%s\n",pch);
	pch=strtok(NULL," ");
	if(strlen(pch)==2){
		if(pch[1]=='0')
			mycard[3]='+';
		if(pch[1]=='1')
			mycard[3]='J';
		if(pch[1]=='2')
			mycard[3]='Q';
		if(pch[1]=='3')
			mycard[3]='K';
		}
	else
		mycard[3]=pch[0];
	printf("%s\n",pch);
	pch=strtok(NULL," ");
	if(strlen(pch)==2){
		if(pch[1]=='0')
			mycard[4]='+';
		if(pch[1]=='1')
			mycard[4]='J';
		if(pch[1]=='2')
			mycard[4]='Q';
		if(pch[1]=='3')
			mycard[4]='K';
		}
	else
		mycard[4]=pch[0];
	
	int j;	
	
	for(i=0;i<5;i++)
		printf("[%c] \n",mycard[i]);
	memset(rcv,0,BUFSIZE);
	
	updatelcd();
	update7seg();
	while(read(connfd,rcv,BUFSIZE)){
		printf("%s\n",rcv);
		pch=strtok(rcv," ");
		ninenine=atoi(pch);
		//printf("%s\n",pch);
		pch=strtok(NULL," ");
		i=0;
		while(pch!=NULL){
			if(strlen(pch)==2){
				if(pch[1]=='0')
					mycard[i]='+';
				if(pch[1]=='1')
					mycard[i]='J';
				if(pch[1]=='2')
					mycard[i]='Q';
				if(pch[1]=='3')
					mycard[i]='K';

			}
			else
				mycard[i]=pch[0];
			pch=strtok(NULL," ");
			i++;
		}
		
		card=i;
		for(i;i<5;i++){
			mycard[i]='0';
		}
		if(ninenine>99)
			break;
		updatelcd();
		update7seg();
		for(j=0;j<5;j++)
			selected[j]=0;
		read(connfd,rcv,BUFSIZE);
		memset(rcv,0,BUFSIZE);
		display.CursorX=1;
		display.CursorY=10;
		ioctl(fd,LCD_IOCTL_CUR_SET,&display);	
		curpos=0;
	}
	printf("Byebye");
	return 0;
}
int main(int argc,char *argv[]){
	if((fd=open("/dev/lcd",O_RDWR))<0){
		printf("Open_lcd_faild\n");
		exit(-1);
	}
	
	connfd = connectsock(argv[1], argv[2], "tcp");

	printf("qweqwe\n");
	pthread_t *threads = new pthread_t [1];
	pthread_create(&threads[0],NULL,handlescan,(void *) 0);
	ioctl(fd,KEY_IOCTL_CLEAR,key);
	ioctl(fd,LCD_IOCTL_CLEAR,NULL);
	int count=0;
	ioctl(fd,_7SEG_IOCTL_ON,NULL);
	data7.Mode=_7SEG_MODE_PATTERN;
	data7.Which=_7SEG_ALL;		
	data7.Value=0;
	ioctl(fd,_7SEG_IOCTL_SET,&data7);
	char cmd[100]="";

	ioctl(fd,LCD_IOCTL_CLEAR,NULL);
	data=LED_ALL_OFF;
	ioctl(fd,LED_IOCTL_SET,&data);
	//drawcardpic();
	//drawnumpic();
	int i=0;
	
	update7seg();
	sleep(1);
	while(1){
		update7seg();
		//updatelcd();
		retval=ioctl(fd,KEY_IOCTL_CHECK_EMTPY,&key);
		if(retval<0){
			continue;		
		}
		retval=ioctl(fd,KEY_IOCTL_GET_CHAR,&key);
		char c;
		c=char(key);
		if(c=='4'){
			if(curpos>0){
				updatelcd();
				curpos--;
				display.CursorX=1+curpos*3;
				display.CursorY=10-selected[curpos];
				ioctl(fd,LCD_IOCTL_CUR_SET,&display);	
			}
		}
		if(c=='6'){
			if(curpos<card-1){
				updatelcd();
				curpos++;
				display.CursorX=1+curpos*3;
				display.CursorY=10-selected[curpos];
				ioctl(fd,LCD_IOCTL_CUR_SET,&display);	
			}
		}
		if(c=='#'){
			if(selected[curpos]==0){
				selected[curpos]=1;
				total+=value(curpos);
			}
			else{
				selected[curpos]=0;
				total-=value(curpos);
			}
			updatelcd();
		}
		if(c=='D'){
			if(total<=13&&total>0){
				if(total==5 || total==7 || total==9){
					display.CursorX=7;
					display.CursorY=2;
					ioctl(fd,LCD_IOCTL_CUR_SET,&display);
					display.Count=sprintf((char *)display.Msg,"2");
					ioctl(fd,LCD_IOCTL_WRITE,&display);
					display.CursorX=2;
					display.CursorY=5;
					ioctl(fd,LCD_IOCTL_CUR_SET,&display);
					display.Count=sprintf((char *)display.Msg,"4");
					ioctl(fd,LCD_IOCTL_WRITE,&display);
					display.CursorX=12;
					display.CursorY=5;
					ioctl(fd,LCD_IOCTL_CUR_SET,&display);
					display.Count=sprintf((char *)display.Msg,"6");
					ioctl(fd,LCD_IOCTL_WRITE,&display);
					display.CursorX=2;
					display.CursorY=6;
					ioctl(fd,LCD_IOCTL_CUR_SET,&display);
					display.Count=sprintf((char *)display.Msg,"Choose one:");
					ioctl(fd,LCD_IOCTL_WRITE,&display);
					display.CursorX=1+curpos*3;
					display.CursorY=10-selected[curpos];
					ioctl(fd,LCD_IOCTL_CUR_SET,&display);	
					chooseP();
					char snd[1024]="",pch[1024]="";
					for(i=0;i<5;i++){
						if(selected[i]==1){
							sprintf(pch,"%d ",i);	
							strcat(snd,pch);
						}
					}
					if(choose==2)
						strcat(snd,"+2");
					if(choose==4)
						strcat(snd,"-1");
					if(choose==6)
						strcat(snd,"+1");
					printf("%s\n",snd);
					write(connfd,snd,strlen(snd));
				}
				else if(total==10||total==12){
					display.CursorX=7;
					display.CursorY=4;
					ioctl(fd,LCD_IOCTL_CUR_SET,&display);
					display.Count=sprintf((char *)display.Msg,"+");
					ioctl(fd,LCD_IOCTL_WRITE,&display);
					display.CursorX=7;
					display.CursorY=8;
					ioctl(fd,LCD_IOCTL_CUR_SET,&display);
					display.Count=sprintf((char *)display.Msg,"-");
					ioctl(fd,LCD_IOCTL_WRITE,&display);
					display.CursorX=2;
					display.CursorY=6;
					ioctl(fd,LCD_IOCTL_CUR_SET,&display);
					display.Count=sprintf((char *)display.Msg,"Add or Sub:");
					ioctl(fd,LCD_IOCTL_WRITE,&display);
					display.CursorX=1+curpos*3;
					display.CursorY=10-selected[curpos];
					ioctl(fd,LCD_IOCTL_CUR_SET,&display);
					chooseAS();
					
					char snd[1024]="",pch[1024]="";
					for(i=0;i<5;i++){
						if(selected[i]==1){
							sprintf(pch,"%d ",i);	
							strcat(snd,pch);
						}
					}
					if(choose==2)
						strcat(snd,"1");
					if(choose==8)
						strcat(snd,"0");
					printf("%s\n",snd);
					write(connfd,snd,strlen(snd));
				}
				else{
					char snd[1024]="",pch[1024]="";
					for(i=0;i<5;i++){
						if(selected[i]==1){
							sprintf(pch,"%d ",i);	
							strcat(snd,pch);
						}
					}	
					strcat(snd,"0");
					printf("%s\n",snd);
					write(connfd,snd,strlen(snd));
				}
				total=0;

				//break;
			}
		}
		updatelcd();
	}
	return 0;

}
