#include <msp430fg4618.h>

#define MODE_OFF   0
#define MODE_SLOW  1
#define MODE_FAST  2

volatile unsigned char mode = MODE_OFF;
volatile unsigned int  led_ms_counter = 0;
volatile unsigned char debounce_active = 0;
volatile unsigned int  debounce_counter = 0;

void init_clock(void)
{
    FLL_CTL0 |= XCAP18PF;
}

void init_ports(void)
{
    P5DIR |= BIT1;
    P5OUT &= ~BIT1;

    P1DIR &= ~BIT0;
    P1IES |= BIT0;
    P1IFG &= ~BIT0;
    P1IE  |= BIT0;
}

void init_basic_timer(void)
{
    BTCTL = 0x07;
    IE2  |= BTIE;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    init_clock();
    init_ports();
    init_basic_timer();

    __bis_SR_register(LPM3_bits + GIE);

    while (1)
    {
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
    if (P1IFG & BIT0)
    {
        if (!debounce_active)
        {
            if (mode == MODE_OFF)
            {
                mode = MODE_SLOW;
            }
            else if (mode == MODE_SLOW)
            {
                mode = MODE_FAST;
            }
            else
            {
                mode = MODE_SLOW;
            }
            led_ms_counter = 0;
            debounce_active  = 1;
            debounce_counter = 20;
            P1IE &= ~BIT0;
        }
        P1IFG &= ~BIT0;
    }
}

#pragma vector=BASICTIMER_VECTOR
__interrupt void BasicTimer_ISR(void)
{
    if (debounce_active)
    {
        if (debounce_counter > 0)
        {
            debounce_counter--;
            if (debounce_counter == 0)
            {
                debounce_active = 0;
                P1IFG &= ~BIT0;
                P1IE  |= BIT0;
            }
        }
    }

    if (mode == MODE_SLOW)
    {
        led_ms_counter++;
        if (led_ms_counter >= 128)
        {
            P5OUT ^= BIT1;
            led_ms_counter = 0;
        }
    }
    else if (mode == MODE_FAST)
    {
        led_ms_counter++;
        if (led_ms_counter >= 64)
        {
            P5OUT ^= BIT1;
            led_ms_counter = 0;
        }
    }
    else
    {
        P5OUT &= ~BIT1;
        led_ms_counter = 0;
    }
}