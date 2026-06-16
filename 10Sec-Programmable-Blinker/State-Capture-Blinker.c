#include <msp430.h>

#define LED1_BIT   BIT2
#define LED2_BIT   BIT1
#define LED_MASK   (LED1_BIT | LED2_BIT)

#define TICKS_PER_SEC        100u
#define BLINK_DURATION_TICKS (10u * TICKS_PER_SEC)
#define BLINK_TOGGLE_TICKS   50u

volatile unsigned char seq = 0;
volatile unsigned char blinking = 0;
volatile unsigned int  blinkElapsed = 0;
volatile unsigned int  blinkDiv = 0;
volatile unsigned char blinkMask = 0;

static void set_leds_by_seq(unsigned char s)
{
    switch (s)
    {
        case 0:
            P2OUT = (P2OUT & ~LED_MASK) | LED1_BIT;
            break;

        case 1:
            P2OUT = (P2OUT & ~LED_MASK) | LED2_BIT;
            break;

        default:
            P2OUT = (P2OUT & ~LED_MASK) | (LED1_BIT | LED2_BIT);
            break;
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    if (P1IFG & BIT0)
    {
        if (!blinking)
        {
            set_leds_by_seq(seq);
            seq = (seq + 1) % 3;
        }
        P1IFG &= ~BIT0;
    }

    if (P1IFG & BIT1)
    {
        if (!blinking)
        {
            blinkMask = (unsigned char)(P2OUT & LED_MASK);

            if (blinkMask != 0)
            {
                blinking = 1;
                blinkElapsed = 0;
                blinkDiv = 0;
            }
        }
        P1IFG &= ~BIT1;
    }
}

#pragma vector=BASICTIMER_VECTOR
__interrupt void BT_ISR(void)
{
    if (blinking)
    {
        blinkElapsed++;
        blinkDiv++;

        if (blinkDiv >= BLINK_TOGGLE_TICKS)
        {
            P2OUT ^= blinkMask;
            blinkDiv = 0;
        }

        if (blinkElapsed >= BLINK_DURATION_TICKS)
        {
            blinking = 0;
            P2OUT &= ~blinkMask;
            blinkMask = 0;
        }
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    P2DIR |= LED_MASK;
    P2OUT &= ~LED_MASK;

    P1DIR &= ~(BIT0 | BIT1);
    P1IES |=  (BIT0 | BIT1);
    P1IFG &= ~(BIT0 | BIT1);
    P1IE  |=  (BIT0 | BIT1);

    BTCTL = BTIP2 | BTIP1 | BTIP0;
    IE2  |= BTIE;

    __bis_SR_register(GIE);

    while(1) { }
}