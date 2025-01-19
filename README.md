# Baremetal LED Blink in C  

This project implements a bare-metal LED blinking application on the STM32F072B-DISCO board. The implementation involves direct manipulation of hardware registers, avoiding libraries like HAL or CMSIS, providing insight into how embedded systems work at a low level.  

---

## Overview  

The application utilizes the **GPIOC pin 6** (PC6) to control on board LED3. The LED alternates between ON and OFF states with a delay in between. Key aspects of the code include:  
- **Reset Handler:** Sets up the `.data` and `.bss` memory sections.  
- **GPIO Configuration:** Configures GPIOC pin 6 for output using the `MODER`, `OTYPER`, `OSPEEDR`, and `PUPDR` registers.  
- **Delay Function:** Provides a software delay loop, allowing the LED to remain ON or OFF for a fixed duration.  

---

## Technical Details  
From board schematic we can see there are 4 user LEDs

![image](https://github.com/user-attachments/assets/4ce86cd1-8bcd-4520-9047-306dde55bca0)

From Board user manual we find that LED3 is controoled by I/O PC6

![image](https://github.com/user-attachments/assets/67fea08e-1a3a-4ff7-ad36-14dc9cc8b87b)



### Memory Initialization  

During startup, the **Reset Handler** performs the following tasks:  
1. **Initialize the `.data` section:** Copies initialized variables from flash memory to RAM.  
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

---

## Code Snippets  

### GPIO Configuration in `main()`  

```c  
// Enable GPIOC clock
RCC_AHBENR |= (1 << 19);

// Configure PC6 as output 
GPIOC_MODER &= ~(3 << 12);   // Clear the MODER bits for PC6
GPIOC_MODER |= (1 << 12);    // Set the MODER bits for PC6 to output

// Configure PC6 as push-pull
GPIOC_OTYPER &= ~(1 << 6);   // Clear the OTYPE bit for PC6 (push-pull)

// Configure PC6 with high-speed
GPIOC_OSPEEDR |= (3 << 12);  // Set the OSPEEDR bits for PC6 to high-speed

// Configure PC6 as no pull-up, no pull-down
GPIOC_PUPDR &= ~(3 << 12);   // Clear the PUPDR bits for PC6 (no pull-up, no pull-down)

```
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
flash write_image erase boot_up.bin 0x08000000
reset run
```

## Highlights

- This project illustrates how to program the STM32F072B-DISCO board at the bare-metal level.
- By directly manipulating registers, the implementation provides a deeper understanding of hardware operations.
- It serves as an excellent starting point for learning about embedded systems programming.

