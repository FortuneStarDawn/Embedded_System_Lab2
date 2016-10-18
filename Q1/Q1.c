/*
 * This code will flash red LED with 1Hz first, and 0.3s on, 0.7s off.
 * If the button is pressed and released, it will change to flash green LED with the same mode, and so on.
 */

#include <msp430.h>
#define LED1 BIT0
#define LED2 BIT6
#define B1 BIT3

volatile unsigned int down=0, color=0;
void main (void)
{
	WDTCTL = WDTPW|WDTHOLD; // Stop watchdog timer
	BCSCTL3 |= LFXT1S_2; // VLO to ACLK
	IFG1 &= ~OFIFG;// Clear OSCFault flag
	P1OUT = B1 + LED1;
	P1DIR = LED1 + LED2;
	P1REN = B1;
	TA0CCR0 = 449; //0.3s
	TA0CTL = MC_1|ID_3|TASSEL_1|TACLR; //up mode, divide clk by 8, use ACLK(VLO), clr timer
	for (;;)
	{
		while((P1IN & B1) != 0) //up
		{
			if(down==1)
			{
				down = 0;
				TA0CTL &= ~TAIFG;
				TA0CTL |= TACLR;
				TA0CCR0 = 449;
				if(color==0)
				{
					P1OUT &= ~LED1;
					P1OUT |= LED2;
					color = 1;
				}
				else
				{
					P1OUT &= ~LED2;
					P1OUT |= LED1;
					color = 0;
				}
			}
			if(TA0CTL & TAIFG)
			{
				TA0CTL &= ~TAIFG;
				if(color==0) P1OUT ^= LED1;
				else P1OUT ^= LED2;
				if(TA0CCR0==449) TA0CCR0 = 1049; //0.7s
				else TA0CCR0 = 449;
			}
		}
		while((P1IN & B1) == 0) //down
		{
			down = 1;
			if(TA0CTL & TAIFG)
			{
				TA0CTL &= ~TAIFG;
				if(color==0) P1OUT ^= LED1;
				else P1OUT ^= LED2;
				if(TA0CCR0==449) TA0CCR0 = 1049;
				else TA0CCR0 = 449;
			}
		}
	}
}
