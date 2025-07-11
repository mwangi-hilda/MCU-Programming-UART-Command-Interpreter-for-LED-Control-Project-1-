# MCU-Programming-UART-Command-Interpreter-for-LED-Control-Project-1-
This project demonstrates how to control a microcontroller's output (an LED) using serial commands sent via UART. It features **interrupt-based UART communication**, a **command buffer**, and simple **string parsing** to toggle an LED through commands like `"Light on"` and `"Light off"`.

---

## Project Overview

- **Microcontroller**: AVR (tested on ATmega2560)
- **Clock Frequency**: 8 MHz
- **Communication**: UART @ 9600 baud
- **Input**: Text commands via serial terminal
- **Output**: LED (PA0) turns on or off based on command

---

## How It Works

1. The microcontroller initializes UART0 at 9600 baud using double-speed mode.
2. Incoming characters are received using an **interrupt (`USART0_RX_vect`)** and stored in a buffer.
3. When a newline (`\n` or `\r`) is detected:
   - The buffer is parsed.
   - Supported commands:
     - `"Light on"` → Turns ON LED at PA0
     - `"Light off"` → Turns OFF LED at PA0
   - Other commands get a response: `"Unknown command: <your input>"`

4. The MCU sends feedback over UART confirming the action taken.

---

## Circuit Setup in Proteus
<img width="646" height="589" alt="image" src="https://github.com/user-attachments/assets/722ddeeb-c4e2-446b-aab0-9cf212956f62" />
