#include "lpc17xx.h"
#include "system_LPC17xx.h"
#include "lcd.h"
#include <string.h>

void uartInit(void);
void uartTx(char);
char uartRx(void );
void uartStr(char *);
void delay(void);
void send_spi(int);
void spi_init();
void my_delay();
void ext_int();
	
char arr[15];
unsigned int i=0;


int main(void)
{
	SystemInit();
	uartInit();
	init_lcd();
	spi_init();
	ext_int();
//	int x;
	
	
	uartStr("Welcome to Combinition in 1768 :)\n\r");
	uartStr("Enter Password With 8 charcters only to Login\n\r");
	user_string("welcome");
	my_delay();
	send_cmd(0x01);
	
	
	
		while(arr[i-1]!='\r'){
			arr[i]=uartRx();
			i++;
		}

		
	if(!((strncmp(arr,"password",8)))){
	
		uartStr("\r\nLogin Successful\r\n");
		user_string("Login Successful");
		my_delay();
		
		send_spi(0x55);
		delay();
		
	}
	else{
	
		LPC_GPIO2->FIODIR1|=(1<<3)|(1<<1);
		LPC_GPIO2->FIOCLR1|=(1<<3);
		uartStr("\r\nWrong Password Try Again\r\n");
		user_string("Wrong Password");
		send_spi(0x0f);
		 
		
	}

	
	while(1);
}


void uartInit(void){

		LPC_PINCON->PINSEL0 |= (1<<4)|(1<<6);
	//LPC_SC->PCLKSEL0 |= (1 << 7);
	LPC_UART0->LCR |= (1<<0)|(1<<1)|(1<<2);
	LPC_UART0->LCR |= (1<<7); //DLAB bit set

	/*
		Baud Rate = _________________PCLK_________________
					16*[256*DLM + DLL]*[1+(DivVal/MulVal)]
	*/

	LPC_UART0->DLL |= 162;	
	LPC_UART0->DLM |=0x00;
	LPC_UART0->LCR &= ~(1<<7); //DLAB bit reset
	LPC_UART0->FDR |= (1<<4);
	LPC_UART0->TER |= (1<<7);
}
char uartRx(){
	
	while(!(LPC_UART0->LSR&(1<<0)));
	char data = LPC_UART0->RBR;
	return data;
	
}

void uartTx(char data){

		while(!(LPC_UART0->LSR&(1<<5)));
		LPC_UART0->THR = data;

}

void uartStr(char *ptr){

	for(int i=0;ptr[i]!='\0';i++){
	
		uartTx(ptr[i]);
		delay();
		
	}
}

////////////////////////////////////////////////////SPI/////////////////////////////////
void spi_init(){

	LPC_PINCON->PINSEL0 |= (1<<31)|(1<<30);
	LPC_PINCON->PINSEL1 |= (1<<5)|(1<<4);
	LPC_GPIO0->FIODIR2 |= (1<<0);

	//LPC_SPI->SPCR = 0x00;
	LPC_SPI->SPCR |= (1<<5);
	LPC_SPI->SPCCR	 = 0x10;
	
}

void send_spi(int data){

		LPC_GPIO0->FIOSET2 |= (1<<0);
		LPC_SPI->SPDR = data;
		while(!(LPC_SPI->SPSR)&(1<<7));
		LPC_GPIO0->FIOCLR2 |= (1<<0);
		
		
}
//////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////// Ext Interrupt///////////////////////////
void ext_int(){
	
	
	LPC_PINCON->PINSEL4 |= (1<<24);//ext int 2
	LPC_SC->EXTMODE &=~(1<<2); 
	NVIC_EnableIRQ(EINT2_IRQn);
	
}


//// ISR
void EINT2_IRQHandler(void)
{
	LPC_SC->EXTINT |=(1<<2);
	LPC_GPIO2->FIODIR1|=(1<<3)|(1<<1); 		// LED and BUZZER
	LPC_GPIO2->FIOCLR1|=(1<<3);					// buzzer off
	LPC_GPIO2->FIOSET1|=(1<<3);
	LPC_GPIO2->FIOSET1|=(1<<1);
}

void my_delay(){

	for(int i=0;i<10000;i++){
	
		for(int j=0;j<10000;j++);
	}
}
