/*
 * This code will flash green LED if the button is down, and if the press is longer than 3 seconds, it will change to flash red LED.
 * The flash of green is 0.3s on and 0.7s off. And the red is 0.2s on and 0.8s off.
 */

#include <msp430.h>
#define LED1 BIT6
#define LED2 BIT0
#define B1 BIT3

volatile unsigned int down=0, count=0;
void main (void)
{
 	WDTCTL = WDTPW|WDTHOLD; // Stop watchdog timer
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ; // DCO = 1Mz
	BCSCTL3 |= LFXT1S_2; // VLO to ACLK
	IFG1 &= ~OFIFG;// Clear OSCFault flag
	BCSCTL2 |= DIVS_3; // SMCLK = DCO/8 (Default SMCLK is DCO)
	P1OUT = B1;
	P1DIR = LED1 + LED2;
	P1REN = B1;
	TA0CCR0 = 449;
	TA1CCR0 = 3124;
	TA0CTL = MC_1|ID_3|TASSEL_1|TACLR; //up mode, divide clk by 8, use ACLK, clr timer
	TA1CTL = MC_1|ID_3|TASSEL_2|TACLR; //up mode, divide clk by 8, use SMCLK, clr timer
	for (;;)
	{
		while((P1IN & B1) != 0) //up
		{
			if(down==1)
			{
				P1OUT &= ~LED1;
				P1OUT &= ~LED2;
				down = 0;
				count = 0;
			}
		}
		while((P1IN & B1) == 0) //down
		{
			if(down==0)
			{
				down = 1;
				TA0CTL &= ~TAIFG;
				TA0CTL |= TACLR;
				TA0CCR0 = 449;
				P1OUT |= LED1;
			}
			if(count<6)
			{
				if(TA0CTL & TAIFG)
				{
					TA0CTL &= ~TAIFG;
					P1OUT ^= LED1;
					if(TA0CCR0==449) TA0CCR0 = 1049;
					else TA0CCR0 = 449;
					count++;
					if(count==6)
					{
						TA1CTL &= ~TAIFG;
						TA1CTL |= TACLR;
						TA1CCR0 = 3124;
						P1OUT &= ~LED1;
						P1OUT |= LED2;
					}
				}
			}
			else
			{
				if(TA1CTL & TAIFG)
				{
					TA1CTL &= ~TAIFG;
					P1OUT ^= LED2;
					if(TA1CCR0==3124) TA1CCR0 = 12499;
					else TA1CCR0 = 3124;
				}

			}
		}
	}
}
