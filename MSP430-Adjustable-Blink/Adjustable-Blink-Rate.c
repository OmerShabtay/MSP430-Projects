#include <msp430.h>

volatile unsigned int rate = 100;
volatile unsigned int counter = 0;

#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    if (P1IFG & BIT0)
    {
        if (rate < 500)
            rate += 100;
        P1IFG &= ~BIT0;
    }

    if (P1IFG & BIT1)
    {
        if (rate > 100)
            rate -= 100;
        P1IFG &= ~BIT1;
    }
}

#pragma vector=BASICTIMER_VECTOR
__interrupt void BT_ISR(void)
{
    counter++;
    unsigned int tempCounter = counter;
    unsigned int tempRate    = rate;

    if (tempCounter >= tempRate)
    {
        P2OUT ^= BIT2;
        counter = 0;
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    P2DIR |= BIT2;
    P2OUT &= ~BIT2;

    P1DIR &= ~(BIT0 | BIT1);
    P1IES |=  (BIT0 | BIT1);
    P1IFG &= ~(BIT0 | BIT1);
    P1IE  |=  (BIT0 | BIT1);

    BTCTL = BTIP2 | BTIP1 | BTIP0;
    IE2  |= BTIE;

    __bis_SR_register(GIE);

    while (1);
}