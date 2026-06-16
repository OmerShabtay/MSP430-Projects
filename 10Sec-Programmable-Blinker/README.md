# 10-Second Programmable LED Blinker

This project implements a state-capture and timed blinking system on the MSP430 microcontroller. It uses hardware interrupts to cycle through LED states and a Basic Timer to handle precise blinking intervals and durations.

## Features
* **State Selection:** Pressing **SW1** cycles through three different LED configurations:
  1. LED1 ON, LED2 OFF
  2. LED1 OFF, LED2 ON
  3. LED1 ON, LED2 ON
* **Timed Blinking (State Capture):** Pressing **SW2** captures the currently active LEDs and blinks them for exactly 10 seconds. After 10 seconds, the blinking stops and the active LEDs turn off.
* **Lockout Mechanism:** Switch inputs are ignored while the 10-second blinking sequence is active to prevent unwanted state changes.

## Hardware Setup
* **LED1:** Connected to `P2.2`
* **LED2:** Connected to `P2.1`
* **SW1 (State Select):** Connected to `P1.0` (Interrupt-driven, falling edge)
* **SW2 (Blink Trigger):** Connected to `P1.1` (Interrupt-driven, falling edge)
* **Microcontroller:** MSP430 series (configured for the Experimenter's Board).

## How It Works
* **Port 1 Interrupts (`P1_ISR`):** Handles the switch presses. SW1 advances a simple state machine to change the LED outputs. SW2 triggers the blinking mode by recording which LEDs are currently on (`blinkMask`) and resetting the timers.
* **Basic Timer (`BT_ISR`):** Generates periodic interrupts. When the blinking flag is active, it toggles the captured LEDs every ~0.5 seconds. Once the timer reaches the 10-second threshold, it automatically halts the blinking sequence and turns the LEDs off.
