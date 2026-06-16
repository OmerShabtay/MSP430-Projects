#include <msp430xG46x.h>

/* ---------------- Sawtooth (KEEP EXACT) ---------------- */
volatile unsigned int dac_val = 0;              // Current DAC code (0..4095)
#define STEP      0x10u                         // *** keep same step ***
#define DAC_MASK  0x0FFFu                       // 12-bit wrap mask

/* ---------------- LED mapping (from your working LED code style) ---------------- */
#define LED1_P2   BIT2                          // P2.2
#define LED2_P2   BIT1                          // P2.1
#define LED3_P5   BIT1                          // P5.1

/* ---------------- ADC/LED helper globals ---------------- */
volatile unsigned char do_adc_led = 0;          // Set by ISR, handled in main

/* Optional debug pin (if you want) */
#define HEARTBEAT BIT0                          // P1.0 toggles each tick (may not be an LED)

static void LED_Init(void)
{
    /* Force GPIO on LED pins */
    P2SEL &= ~(LED1_P2 | LED2_P2);              // P2.1/P2.2 as GPIO
    P5SEL &= ~LED3_P5;                          // P5.1 as GPIO

    /* Directions */
    P2DIR |= (LED1_P2 | LED2_P2);               // outputs
    P5DIR |= LED3_P5;                           // output

    /* Start OFF */
    P2OUT &= ~(LED1_P2 | LED2_P2);
    P5OUT &= ~LED3_P5;
}

static void REF_Init(void)
{
    /* Enable internal 2.5V reference (used by DAC and ADC) */
    ADC12CTL0 = REFON | REF2_5V;                // 2.5V reference ON
    __delay_cycles(50000);                      // Wait for reference to settle
}

static void DAC12_Init(void)
{
    P6SEL |= BIT6;                              // P6.6 = DAC12_0 output function

    /* Keep DAC config simple (as in sawtooth code) */
    DAC12_0CTL = DAC12IR | DAC12AMP_5 | DAC12ENC; // Int ref range, medium drive, enable
    DAC12_0DAT = 0;                             // Start at 0
}

static void ADC12_Init_A0(void)
{
    P6SEL |= BIT0;                              // P6.0 = ADC input A0 (analog)

    ADC12CTL0 &= ~ENC;                          // Disable conversions while configuring

    /* IMPORTANT: do NOT overwrite REFON/REF2_5V -> only OR-in ADC bits */
    ADC12CTL0 |= ADC12ON | SHT0_2;              // ADC ON + sample/hold time
    ADC12CTL1  = SHP;                           // Use sampling timer
    ADC12MCTL0 = INCH_0;                        // Channel A0 -> MEM0

    ADC12CTL0 |= ENC;                           // Enable conversions
}

static void TimerA_Init_1ms_ACLK(void)
{
    /* KEEP EXACT timing from sawtooth code: ACLK ~ 32768Hz -> ~1ms ~ 33 counts */
    TACCR0  = 33 - 1;                           // ~1ms period
    TACCTL0 = CCIE;                             // Enable CCR0 interrupt
    TACTL   = TASSEL_1 | MC_1 | TACLR;          // ACLK, Up mode, clear TAR
}

static void LEDs_Update_From_ADC(unsigned int adc)
{
    /* Quartiles over 12-bit range (0..4095) */
    P2OUT &= ~(LED1_P2 | LED2_P2);
    P5OUT &= ~LED3_P5;

    if (adc < 1024)
    {
        /* All OFF */
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
    WDTCTL = WDTPW | WDTHOLD;                   // Stop watchdog

    /* Optional debug heartbeat pin */
    P1DIR |= HEARTBEAT;
    P1OUT &= ~HEARTBEAT;

    LED_Init();                                 // LEDs work like in your “LED working” code
    REF_Init();                                 // 2.5V reference (shared for DAC+ADC)
    DAC12_Init();                               // Sawtooth output on P6.6
    ADC12_Init_A0();                            // Read A0 on P6.0 (jumper from P6.6)
    TimerA_Init_1ms_ACLK();                     // *** keep sawtooth timing ***

    __enable_interrupt();

    while (1)
    {
        __bis_SR_register(LPM0_bits | GIE);     // Sleep until ISR wakes us

        if (do_adc_led)                         // Handle ADC+LED in main (NOT in ISR)
        {
            unsigned int adc_val;

            do_adc_led = 0;

            ADC12CTL0 |= ADC12SC;               // Start conversion
            while (ADC12CTL1 & ADC12BUSY) ;     // Wait until done
            adc_val = ADC12MEM0;                // Read result

            LEDs_Update_From_ADC(adc_val);      // Update LEDs based on level
        }
    }
}

/* ---------------- TimerA ISR (KEEP SAWTOOTH EXACT) ---------------- */
#pragma vector = TIMERA0_VECTOR
__interrupt void TimerA_ISR(void)
{
    P1OUT ^= HEARTBEAT;                         // Optional debug toggle

    /* EXACT sawtooth: same step, same wrap, same update frequency */
    dac_val = (dac_val + STEP) & DAC_MASK;
    DAC12_0DAT = dac_val;

    do_adc_led = 1;                             // Request ADC+LED update in main
    __bic_SR_register_on_exit(LPM0_bits);       // Wake main without delaying ISR
}
