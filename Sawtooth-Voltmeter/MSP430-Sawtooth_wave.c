#include <msp430xG46x.h>

volatile unsigned int dac_val = 0;
#define STEP      0x10u
#define DAC_MASK  0x0FFFu

#define LED1_P2   BIT2
#define LED2_P2   BIT1
#define LED3_P5   BIT1

volatile unsigned char do_adc_led = 0;

#define HEARTBEAT BIT0

static void LED_Init(void)
{
    P2SEL &= ~(LED1_P2 | LED2_P2);
    P5SEL &= ~LED3_P5;

    P2DIR |= (LED1_P2 | LED2_P2);
    P5DIR |= LED3_P5;

    P2OUT &= ~(LED1_P2 | LED2_P2);
    P5OUT &= ~LED3_P5;
}

static void REF_Init(void)
{
    ADC12CTL0 = REFON | REF2_5V;
    __delay_cycles(50000);
}

static void DAC12_Init(void)
{
    P6SEL |= BIT6;

    DAC12_0CTL = DAC12IR | DAC12AMP_5 | DAC12ENC;
    DAC12_0DAT = 0;
}

static void ADC12_Init_A0(void)
{
    P6SEL |= BIT0;

    ADC12CTL0 &= ~ENC;

    ADC12CTL0 |= ADC12ON | SHT0_2;
    ADC12CTL1  = SHP;
    ADC12MCTL0 = INCH_0;

    ADC12CTL0 |= ENC;
}

static void TimerA_Init_1ms_ACLK(void)
{
    TACCR0  = 33 - 1;
    TACCTL0 = CCIE;
    TACTL   = TASSEL_1 | MC_1 | TACLR;
}

static void LEDs_Update_From_ADC(unsigned int adc)
{
    P2OUT &= ~(LED1_P2 | LED2_P2);
    P5OUT &= ~LED3_P5;

    if (adc < 1024)
    {
    }
    else if (adc < 2048)
    {
        P2OUT |= LED1_P2;
    }
    else if (adc < 3072)
    {
        P2OUT |= LED2_P2;
    }
    else
    {
        P5OUT |= LED3_P5;
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    P1DIR |= HEARTBEAT;
    P1OUT &= ~HEARTBEAT;

    LED_Init();
    REF_Init();
    DAC12_Init();
    ADC12_Init_A0();
    TimerA_Init_1ms_ACLK();

    __enable_interrupt();

    while (1)
    {
        __bis_SR_register(LPM0_bits | GIE);

        if (do_adc_led)
        {
            unsigned int adc_val;

            do_adc_led = 0;

            ADC12CTL0 |= ADC12SC;
            while (ADC12CTL1 & ADC12BUSY) ;
            adc_val = ADC12MEM0;

            LEDs_Update_From_ADC(adc_val);
        }
    }
}

#pragma vector = TIMERA0_VECTOR
__interrupt void TimerA_ISR(void)
{
    P1OUT ^= HEARTBEAT;

    dac_val = (dac_val + STEP) & DAC_MASK;
    DAC12_0DAT = dac_val;

    do_adc_led = 1;
    __bic_SR_register_on_exit(LPM0_bits);
}
