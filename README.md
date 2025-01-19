# Baremetal LED Blink in C  

This project implements a bare-metal LED blinking application on the STM32F072B-DISCO board. The implementation involves direct manipulation of hardware registers, avoiding libraries like HAL or CMSIS, providing insight into how embedded systems work at a low level.  

---

## Table of Contents
1. [Overview](#overview)
2. [Technical Details](#technical-details)
   - [Memory Initialization](#memory-initialization)
   - [GPIO Configuration](#gpio-configuration)
   - [Delay Function](#delay-function)
   - [LED Control](#led-control)
3. [Build and Flash Instructions](#build-and-flash-instructions)
   - [Prerequisites](#prerequisites)
   - [Build](#build)
   - [Flash](#flash)
4. [Challenge and Solution to Resolve LED Blink Rate Discrepancy](#challenge-and-solution-to-resolve-led-blink-rate-discrepancy)

---

## Overview

The application utilizes the **GPIOC pin 6** (PC6) to control onboard LED3. The LED alternates between ON and OFF states with a delay in between. Key aspects of the code include:
- **Reset Handler:** Sets up the `.data` and `.bss` memory sections and configures the system clock.
- **GPIO Configuration:** Configures GPIOC pin 6 for output using the `MODER`, `OTYPER`, `OSPEEDR`, and `PUPDR` registers.
- **Delay Function:** Provides a software delay loop, allowing the LED to remain ON or OFF for a fixed duration.

---

## Technical Details  

![image](https://github.com/user-attachments/assets/2f534e48-a53f-4470-8ea6-cbd4dd0af749)

From board schematic we can see there are 4 user LEDs

![image](https://github.com/user-attachments/assets/16fcd696-5c92-4c70-8676-541014eb4d7c)

From Board user manual we find that LED3 is controlled by I/O PC6

![image](https://github.com/user-attachments/assets/e75df093-4a31-4320-82d2-c01840f3c078)

OpenOCD (Open On-Chip Debugger) is used to interface with the STM32F072B-DISCO board for debugging and flashing the firmware. It communicates with the board through a debugger (such as ST-Link) and allows for low-level control over the microcontroller. Telnet is used to remotely connect to OpenOCD, enabling the user to send commands like resetting the board, flashing firmware, and running or halting execution

### Memory Initialization  

During startup, the **Reset Handler** performs the following tasks:
1. **Initialize the `.data` section:** Copies initialized variables from flash memory to RAM and configures the system clock.
2. **Zero-initialize the `.bss` section:** Clears uninitialized variables in RAM.


This ensures that all variables are correctly initialized before the `main()` function executes.  

### GPIO Configuration  

Each GPIO pin is controlled by four 32-bit configuration registers:  
- **MODER (Mode Register):** Configures the pin mode (input, output, alternate function, or analog).  
- **OTYPER (Output Type):** Sets output type to push-pull or open-drain.  
- **OSPEEDR (Output Speed):** Configures output speed (low, medium, or high).  
- **PUPDR (Pull-Up/Pull-Down):** Sets pull-up or pull-down resistors.  

For PC6, the following configurations are applied:  
1. Enable the GPIOC clock in the `RCC_AHBENR` register.  
2. Set PC6 as an output pin in the `GPIOC_MODER` register.  
3. Configure PC6 as push-pull in the `GPIOC_OTYPER` register.  
4. Set PC6 to high-speed output in the `GPIOC_OSPEEDR` register.  
5. Disable pull-up and pull-down resistors for PC6 in the `GPIOC_PUPDR` register.  

### Delay Function  

The delay is implemented as a software loop, where the `__asm("nop")` instruction ensures precise timing by introducing no-operation instructions.  

### LED Control  

The LED is toggled using a **Read-Modify-Write** approach on the `GPIOC_ODR` (Output Data Register):  
- **Turn ON:** Set bit 6 of `GPIOC_ODR`.  
- **Turn OFF:** Clear bit 6 of `GPIOC_ODR`.  

# Build and Flash Instructions

## Prerequisites

Before building and flashing the project, make sure you have the following tools installed:

- GNU ARM Toolchain (for compiling and linking the project)
- OpenOCD (for flashing the STM32F072B-DISCO board)
- Telnet client (for communicating with OpenOCD)

## Build

Use the Makefile to compile the project:

```
make clean
make
```

## Flash

1. Start OpenOCD:
```
openocd -f stm32f0discovery.cfg
```

2. Flash the binary via Telnet:

```
telnet localhost 4444
reset halt
flash write_image erase boot_up.elf
reset run
```
or 

```
# Reset the target and initialize
reset init

# Write the firmware image
flash write_image erase boot_up.elf

# Resume the target (start executing the code)
resume
```

## Challenge and Solution to Resolve LED Blink Rate Discrepancy

### Problem Observed
When flashing the firmware onto the STM32F072B-DISCO board using OpenOCD and Telnet commands, I noticed that the LED blinked at different rates depending on the commands used. Specifically:  
1. Using the following commands resulted in one blink rate:
   ```bash
   reset halt
   flash write_image erase boot_up.elf
   reset run
   ```
The reset halt command halts the processor before flashing the firmware, meaning the OpenOCD is not configured immediately after the reset. As a result, the processor starts executing with the default clock settings provided by system.(in my case the LED was blinking slower).

2. Using these commands caused a different blink rate:
   ```bash
   reset init
   flash write_image erase boot_up.elf
   resume
   ```
The reset init command not only resets the processor but also initializes the system clock(it uses the debug environment clock) before the firmware is flashed. This ensures that the system starts with the clock configuration set by OpenOCD(in my case LED was blinking faster).

Additionally, pressing the reset button on the board caused the LED to blink at a different rate compared to resuming the code execution through Telnet commands.

### Root Cause
After investigating the issue, I determined that the difference in blink rates was due to the system clock configuration. The STM32F072B-DISCO board’s `.cfg` file used by OpenOCD specifies a default clock rate, which may differ from the one expected by the application.  

Since the delay loop in the code relies on the system clock frequency, variations in the clock configuration directly affect the timing of the delay, causing the LED to blink faster or slower.

- **Faster Clock:** Results in a shorter delay and a faster blink rate.
- **Slower Clock:** Leads to a longer delay and a slower blink rate.

### Solution
To address this, I explicitly configured the system clock in the **Reset Handler** by adding a `SystemClock_Config()` function. This function sets the clock source to the HSI (High Speed Internal) oscillator and ensures the clock rate is consistent from the very beginning of the firmware’s execution.  

![image](https://github.com/user-attachments/assets/21cd6b17-0ea4-419f-855f-e3c5afedc9ed)

From Data sheet we get to know about clocks available to us.

![image](https://github.com/user-attachments/assets/a450ab77-d38a-4fe3-8c46-b4b2a832b5f6)

The SW[1:0] bits in the RCC_CFGR register, which are used to select the system clock source. The SW bits control which clock source is used for the system clock

The HSI oscillator is controlled via the RCC_CR register, and the 0th bit (HSION) is responsible for enabling or disabling the HSI oscillator. By setting this bit to 1, we enable the HSI oscillator.

![image](https://github.com/user-attachments/assets/032db28e-9af8-42c7-b163-61b87e6ffb09)

From block diagram HSI is 8MHz

With this change:
1. The clock configuration is set immediately after the processor resets, before the `main()` function runs.
2. The application no longer depends on the default clock settings from the OpenOCD environment.

### Outcome
After implementing the `SystemClock_Config()` function in the Reset Handler:
- The LED blink rate became consistent regardless of which Telnet commands were used.
- Pressing the reset button on the board no longer caused a change in the blink rate.
- The system now behaves predictably in all scenarios, ensuring that the application operates at the intended clock frequency.

This modification highlights the importance of explicitly configuring the system clock in bare-metal programming to avoid dependencies on external debugging environments.
References:
1. stm32 rm--91 data sheet
2. stm32f072rb-4 data sheet
3. UM1690 User manual
