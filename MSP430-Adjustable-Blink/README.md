# Adjustable LED Blink Rate

This project demonstrates real-time hardware interrupt handling on the MSP430 to adjust the blink rate of an LED dynamically.

## Features
* **Dynamic Rate Adjustment:** The blink period starts at a default of 1 second.
* **Increase Interval (SW1):** Pressing SW1 increases the blink period by 1 second, up to a maximum limit of 5 seconds.
* **Decrease Interval (SW2):** Pressing SW2 decreases the blink period by 1 second, down to a minimum limit of 1 second.
* **Hardware Timers:** Utilizes the Basic Timer to generate precise 100ms ticks, which are counted to create accurate 1-to-5 second delays without freezing the processor.

## Hardware Setup
* **LED Output:** Connected to `P2.2` (LED1).
* **SW1 (Increase Rate):** Connected to `P1.0` (Interrupt-driven, falling edge).
* **SW2 (Decrease Rate):** Connected to `P1.1` (Interrupt-driven, falling edge).
