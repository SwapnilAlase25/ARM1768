#include "lpc17xx.h"
#include "system_LPC17xx.h"
#include "lcd.h"
#include <string.h>

char str[20];
unsigned int datax,final_data;
char str_ext[10];
float volt;
int arr[4];


/////////////UART//////////////////
void uart_init(void){

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

char rx_char(){
	
	while(!(LPC_UART0->LSR&(1<<0)));
	char rx = LPC_UART0->RBR;
	return rx;
	
}

void rx_str(char *ptr)
{
	int i=0;
	while(ptr[i-1]!='\r'){
			ptr[i]=rx_char();
			i++;
		}
	ptr[i-1]='\0';
}

void tx_char(char tx){

		while(!(LPC_UART0->LSR&(1<<5)));
		LPC_UART0->THR = tx;

}

void tx_str(char *ptr){

	for(int i=0;ptr[i]!='\0';i++){
	
		tx_char(ptr[i]);
		delay();
		
	}
}
//////////////LED//////////////////
void led_toggle()
{
	static int i=0;
	if(i==0)
	{
		i=1;
		LPC_GPIO2->FIOCLR1|=(1<<1)|(1<<3);		//LED
	}
	else
	{
		i=0;
		LPC_GPIO2->FIOSET1|=(1<<1)|(1<<3);
	}
}

////////////////SPI//////////////////
void spi_init(){

	LPC_PINCON->PINSEL0 |= (1<<31)|(1<<30);
	LPC_PINCON->PINSEL1 |= (1<<5)|(1<<4);
	LPC_GPIO0->FIODIR2 |= (1<<0);		//enable pin output

	//LPC_SPI->SPCR = 0x00;
	LPC_SPI->SPCR |= (1<<5);		//master
	LPC_SPI->SPCCR	 = 0x10;		//spi clock div
	
}

void spi_send()
{
	static int i=0;
	int j;
	if(i==0)
	{
		i=1;
		LPC_GPIO0->FIOCLR2|=(1<<0);		//low
		while(!(LPC_SPI->SPSR)&(1<<7));	//spif polling
		LPC_SPI->SPDR = 0x55;			//data lock
		LPC_GPIO0->FIOSET2|=(1<<0);		//high

	}
	else{
		i=0;
		LPC_GPIO0->FIOCLR2|=(1<<0);		//low
		while(!(LPC_SPI->SPSR)&(1<<7));	//spif polling
		LPC_SPI->SPDR = 0xAA;			//data lock
		LPC_GPIO0->FIOSET2|=(1<<0);		//high
	
		
	}
		j=LPC_SPI->SPSR;
		j=LPC_SPI->SPDR;
}

//////////////LCD////////////////////

void uart_to_lcd()
{
	int i=0;
	char lcd[]="\n\rEnter string to be displayed on LCD\n\r";
	send_data('A');
	tx_str(lcd);
	rx_str(str);
	send_cmd(0x01);
	send_cmd(0x80);
	while(str[i]!='\0')
	{	send_data(str[i++]);
		//user_delay(2000);
	}
}

///////////Ext Int on LCD///////////
void ext_int(){
	
	
	LPC_PINCON->PINSEL4 |= (1<<24);//ext int 2
	LPC_SC->EXTMODE |=(1<<2);		//edge sensitive
	LPC_SC->EXTPOLAR |=(1<<2);	//rising edge
	LPC_SC->EXTINT|=(1<<2);		//clear intrpt flag
	NVIC_EnableIRQ(EINT2_IRQn);
	
}
void EINT2_IRQHandler(void)
{
	int j=0;
	send_cmd(0x01);
	send_cmd(0x80);
	while(str_ext[j]!='\0')
	{	send_data(str_ext[j++]);
		//user_delay(2000);
	}
	LPC_SC->EXTINT |=(1<<2);
}
void extint_to_lcd()
{
	char lcd[]="\n\rEnter string to be displayed on LCD\n\r";
	send_data('A');
	tx_str(lcd);
	rx_str(str_ext);
	
}

////////////////ADC//////////////////////

void adc_init()
{
	LPC_SC->PCLKSEL0 |= (1 << 24);		//ADC clock=CLCK;
	LPC_SC->PCONP |=(1<<12);
	LPC_PINCON->PINSEL1|=(1<<18); 							//p0.30 as AD0.2
	LPC_ADC->ADCR =0x00;
	LPC_ADC->ADCR |=(1<<2)|(1<<16)|(1<<21);//channel 2 select,burst conv,no power down 
	LPC_ADC->ADCR|=(0x0000FF00);									//bit 8:15 clock div val

}



void adc_print()
{
		int i,j;
		while(!(LPC_ADC->ADDR2&(1<<31)));
		datax=LPC_ADC->ADDR2;
		datax=(datax>>6)&(0x3FF);
		volt=(datax*3.3)/1023;				//convertion to volts formula
		final_data=volt*1000;							//mV in integer
	
			for(i=0;i<4;i++)
		{
			arr[i]=(final_data%10)+48;
			final_data/=10;
		}
	
		for(i=3;i>=0;i--)
		{
			tx_char(arr[i]);
			if(i==3) tx_char('.');
		}
		for(i=0;i<300;i++)
			for(j=0;j<100;j++);
		tx_char('V');
		tx_char(' ');
		tx_char('\n');
		tx_char('\r');
}
/*************************************/

int main()
{
	SystemInit();
	LPC_GPIO0->FIODIR1|=(1<<1)|(1<<3);
	LPC_GPIO2->FIOCLR1|=(1<<1);
	LPC_GPIO2->FIOSET1|=(1<<3);
	adc_init();
	spi_init();
	init_lcd();
	ext_int();
//	timer_init();
	char w[]="Home Automation\r\n";
	char str1[]="\n\r1.Led Toggle\n\r2.ADC value on Uart\n\r3.Send Data on SPI\n\r";
	char str2[]="4.Enter string to be displayed on LCD\n\r5.Display on LCD with Ext Interrupt\n\r";
	char error[]="\n\rInvalid. Enter correct choice\n\r";
	uart_init();
	tx_str(w);
	tx_char('\n');

	//tx(str1);
	while(1)
	{
		tx_str(str1);
		tx_str(str2);
		rx_str(str);
		
		switch(str[0]){
			case '1':
							led_toggle();
							break;
				
			case '2':
							adc_print();
							break;
				
			case '3':
							
							spi_send();
							break;
			case '4':
							uart_to_lcd();
							break;
			case '5':
				
							extint_to_lcd();
							break;
							
				
			default:
							
							tx_str(error);
							break;
		}
	}
}


