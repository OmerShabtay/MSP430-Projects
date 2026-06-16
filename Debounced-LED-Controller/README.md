# Debounced LED Mode Controller

This project demonstrates a robust, interrupt-driven state machine for the MSP430 microcontroller. It features a non-blocking software debounce mechanism to handle physical switch noise and seamlessly toggles between specific LED blinking rates.

## Features
* **Multi-State Operation:** The system starts in an OFF state. Pressing **SW1** transitions between the following modes:
  * **1st Press (Slow Blink):** LED is ON for 1 second and OFF for 1 second (2-second period).
  * **2nd Press (Fast Blink):** LED is ON for 0.5 seconds and OFF for 0.5 seconds (1-second period).
  * **3rd Press:** Returns to the Slow Blink state.
* **Software Debouncing:** Utilizes the Basic Timer to enforce a ~150ms (20-tick) debounce window. This easily satisfies the standard >20ms debounce requirement and prevents false triggers without freezing the processor with delay loops.
* **Maximum Power Efficiency:** The main loop remains completely empty. The microcontroller sleeps in Low Power Mode 3 (`LPM3`) to minimize energy consumption, waking up only briefly via hardware interrupts to handle timing and state transitions.

## Hardware Setup
* **LED Output:** Connected to `P5.1` (LED4).
* **SW1 (Mode Select):** Connected to `P1.0` (Interrupt-driven, falling edge).
* **Microcontroller:** MSP430FG4618.

## How It Works
When **SW1** is pressed, the Port 1 interrupt triggers. If the debounce window is inactive, the system registers the button press, updates the state machine to the next mode, temporarily disables further SW1 interrupts, and starts a countdown timer. 

The **Basic Timer** interrupt handles both the blinking rate of the active mode (using counters) and the debounce countdown. Once the debounce counter reaches zero, the switch interrupt is re-enabled, ready for a clean, noise-free button press.
