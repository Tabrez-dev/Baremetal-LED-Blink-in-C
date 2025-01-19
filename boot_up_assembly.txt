.extern _etext
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss


.section .bss
  array: .space 10

.section .data
  data_val: .word 1234,2345,678

/*.section .rodata
   RCC_AHBENR: .word 0x40021014   // RCC AHB Enable Register
   GPIOC_MODER: .word 0x48000800 // GPIOC Mode Register
  GPIOC_OTYPER: .word 0x48000804  // GPIOC Output Type Register
  GPIOC_OSPEEDR: .word 0x48000808  // GPIOC Output Speed Register
  GPIOC_PUPDR: .word 0x4800080C    // GPIOC Pull-Up/Pull-Down Register
   GPIOC_ODR: .word 0x48000814 // GPIOC Output Data Register*/

.equ RCC_AHBENR, 0x40021014       // RCC AHB Enable Register
.equ GPIOC_MODER, 0x48000800      // GPIOC Mode Register
.equ GPIOC_OTYPER, 0x48000804     // GPIOC Output Type Register
.equ GPIOC_OSPEEDR, 0x48000808    // GPIOC Output Speed Register
.equ GPIOC_PUPDR, 0x4800080C      // GPIOC Pull-Up/Pull-Down Register
.equ GPIOC_ODR, 0x48000814        // GPIOC Output Data Register

.section .vector
    vector_table:
          .word 0x20004000   //16kb top of sram
          .word reset_handler
          .zero 156 //acts as a placeholder for IVT 7+32=39x4=156

.section .text
   .align 1
   .type reset_handler, %function
   reset_handler:
          ldr r1, =_etext
          ldr r2, =_sdata
          ldr r3, =_edata
       up:mov r0,#0
          ldrb r0,[r1] 
          strb r0,[r2]
          add r1,r1,#1
          add r2,r2,#1 
          cmp r2, r3
          bne up
          ldr r1,=_sbss
          ldr r2,=_ebss
          mov r3,#0
     next:strb r3,[r1]
          add r1,r1,#1
          cmp r1,r2
          bne next
          b main
     main: //ENABLE GPIOC clock
          mov r0,#19
          ldr r1, =RCC_AHBENR          
          ldr r2,[r1]
	  mov r3,#1
	  lsl r3,r3,r0
          orr r2,r2,r3
          str r2,[r1]
         //config pc6 as output (moder)
          ldr r1, =GPIOC_MODER
          ldr r2,[r1]
          ldr r0, =0x00ff0ff
          and r2,r2,r0
          ldr r0, =0x00001000
          orr r2,r2,r0
          str r2,[r1]
      // Configure PC6 as push-pull (OTYPER)
          ldr r1, =GPIOC_OTYPER          
          ldr r2, [r1]
          ldr r0, =0xffffffbf
          and r2,r2,r0
          str r2, [r1]
            // Configure PC6 with low-speed (OSPEEDR)        
          ldr r1, =GPIOC_OSPEEDR
          ldr r2,[r1]
          ldr r0, =0x00ffffff
          and r2,r2,r0
          str r2,[r1]
          // Configure PC6 as no pull-up, no pull-down (PUPDR)          
          ldr r1, =GPIOC_PUPDR          
          ldr r2,[r1]
          ldr r0, =0x00ffffff
          and r2,r2,r0
          str r2,[r1]
          ldr r1, =GPIOC_ODR    // Set ODR address
   repeat:ldr r2,[r1]
          ldr r0, =0x000000f0
          orr r2,r2,r0 
          str r2,[r1]
          bl delay
          ldr r2,[r1]
          ldr r0, =0xffffff0f
          and r2,r2,r0 
          str r2,[r1]
          bl delay
          b repeat
  delay:ldr r6, =0x000ff000
     del:sub r6,r6,#1
         cmp r6,#0
          bne del
         mov pc,lr

          



