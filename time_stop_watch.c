#include<LPC17xx.h>
#include "lcd.h"
#define PCLK 25000000
#include<stdio.h>
/*************************************/
void timerInit(void);
void delaySec(unsigned int ms);
void uartInit(void);
int TX(int ch);
int RX(void);
/************************************/

static unsigned int sec=0,min=0,hour=0;

/*************************************/
static unsigned int flag=0;

int main(){
 init_lcd();
//	uartInit();
	//LPC_UART3->IER |=(1<<1);
	

	
	

	send_cmd(0x80);
		user_string("PRESS SWITCH");
	send_cmd(0xC0);
	user_string("TO START");
	
	/************************************/
	LPC_PINCON->PINSEL4|=(1<<24);
	LPC_SC->EXTMODE|=(1<<2);
	//LPC_SC->EXTPOLAR|=(1<<2);

	NVIC_EnableIRQ(EINT2_IRQn);
	/**********************************/
	
	
  char arr[10];
timerInit();
	while(1){
		if(flag){

	delaySec(1);
		sec++;
		if(sec==60){
		min++;
			sec=0;
			
			send_cmd(0x01);
		}
		if(min==60){
		hour++;
			min=0;
			send_cmd(0x01);
		}
		
		sprintf(arr,"TIME->%d:%d:%d",hour,min,sec);
		send_cmd(0x80);
		user_string(arr);
	}
	 }
	
}
/**********************EINT2*************************/
void EINT2_IRQHandler(void){
	if(!flag){
	LPC_TIM0->TCR=0x01;
		flag=1;
		send_cmd(0x01); //clear screen
		send_cmd(0xC0); // second line in lcd
		user_string("START");
	
	}
	else{
		LPC_TIM0->TCR=0x00;
		flag=0;
		//send_cmd(0x01);
		send_cmd(0xC0);
		user_string("PAUSE");
	}
		LPC_SC->EXTINT|=(1<<2);
	
}


/******************TIMER************************/


void timerInit(void){
	
  LPC_SC->PCONP|=(1<<1);
  LPC_TIM0->CTCR=0x00;
	LPC_TIM0->PR=PCLK-1;
	LPC_TIM0->TCR =(1<<1);
	LPC_TIM0->MCR|=(1<<2);
	LPC_TIM0->TCR=0x02;
	}

void delaySec(unsigned int ms){
	LPC_TIM0->MR0=ms;
	LPC_TIM0->TCR=0x01;
	while(LPC_TIM0->TC<LPC_TIM0->MR0);
	LPC_TIM0->TCR=0x00;
}

/*************************UARTs**************************/

void uartInit(void){
LPC_SC->PCONP |=(1<<25);
LPC_PINCON->PINSEL0|=(1<<1)|(1<<3);
LPC_UART3->LCR|=(1<<0)|(1<<1)|(1<<7); // 8 bit mode and Dlab set for baud rate config
LPC_UART3->	FDR=0x000000F0;
LPC_UART3->DLM=0x00;
LPC_UART3->DLL|=0xA2;
LPC_UART3->TER |=(1<<7);
LPC_UART3->LCR &=~(1<<7);

}

int RX(){
while(!(LPC_UART3->LSR&0x01));
	return(LPC_UART3->RBR);
}

int TX(int ch){
     
	while(!(LPC_UART3->LSR&(1<<5)));
	return(LPC_UART3->THR=ch);
}
