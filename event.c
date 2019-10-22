#include "lpc17xx.h"
#include "system_LPC17xx.h"
#include "lcd.h"

int flag =0;


int main(void)
{
	SystemInit();
	init_lcd();
	LPC_GPIO2->FIODIR1 |= (1<<3);
	LPC_GPIO2->FIODIR1 |= (1<<1);
	
	LPC_SC->PCONP |= (1<<1);
	LPC_SC->PCLKSEL0 |= (1<<2);

	LPC_TIM0->PR = 0x186A0;             //this will lead to milliseconds
	LPC_TIM0->MR0 =1000;	     		 //event at 1000 mS
	LPC_TIM0->MR1 =2000;			    //event at 2000 mS
	LPC_TIM0->MR2 =3000;				//event at 3000 mS
	LPC_TIM0->MR3 =10000;			    //event at 4000 mS
	
	LPC_TIM0->MCR |= (1 << 0)|(1 << 3)|(1 << 6)|(1 << 9);              // interrupt on for different events
	LPC_TIM0->MCR |= (1 << 2)|(1<<5)|(1<<8)|(1<<11);                 // stop on event
	LPC_TIM0->TCR = 0x02;																// reset timer
                                                                                                               	//LPC_TIM0->TCR &= 0 << 1;

	NVIC_EnableIRQ(TIMER0_IRQn);
	LPC_TIM0->TCR = 0x01;                                                                 /* enable timer */
	LPC_GPIO2->FIOSET1 |= (1<<3);														//FIOSET = ( 1<<11) buzzer
	LPC_GPIO2->FIOSET1 |= (1<<1);														//FIOSET = ( 1<<9)   LED
	while(1){}

}

void TIMER0_IRQHandler(void)
{
    if((LPC_TIM0->IR & 0x01) == 0x01)
    {
      LPC_TIM0->IR |= 1 << 0;   			// clear interrupt flag
			user_string("1");
			LPC_TIM0->TCR = 0x01;/* enable timer */
		}
		if((LPC_TIM0->IR & 0x02) == 0x02)
		{
			LPC_TIM0->IR |= 1 << 1;   			// clear interrupt flag
			user_string("2");
			LPC_TIM0->TCR = 0x01;/* enable timer */
		}
		
		if((LPC_TIM0->IR & 0x04) == 0x04)
		{
			LPC_TIM0->IR |= 1 << 2;   			// clear interrupt flag
			user_string("3");
			LPC_TIM0->TCR = 0x01;/* enable timer */
    }
		
		if((LPC_TIM0->IR & 0x08) == 0x08)
		{
			LPC_TIM0->IR |= 1 << 3;   			// clear interrupt flag
			user_string("4");
			LPC_TIM0->TCR = 0x01;/* enable timer */
		}
}
