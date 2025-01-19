#include <stdint.h>

void reset_handler(void);
int main(void);
void SystemClock_Config(void);

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

#define RCC_CR        (*((volatile uint32_t*)0x40021000))
#define RCC_CFGR      (*((volatile uint32_t*)0x40021004))
#define RCC_AHBENR    (*((volatile uint32_t*)0x40021014))
#define GPIOC_MODER   (*((volatile uint32_t*)0x48000800))
#define GPIOC_OTYPER  (*((volatile uint32_t*)0x48000804))
#define GPIOC_OSPEEDR (*((volatile uint32_t*)0x48000808))
#define GPIOC_PUPDR   (*((volatile uint32_t*)0x4800080C))
#define GPIOC_ODR     (*((volatile uint32_t*)0x48000814))

__attribute__((section(".vector")))
const uint32_t vector_table[]={
    0x20004000,
    (uint32_t)&reset_handler,
    [2 ... 37] = 0
};

static void delay(void) {
    for(volatile uint32_t i=0x000ff000;i>0;i--) {
        __asm("nop");
    }
}

void reset_handler(void) {
    uint8_t *src=(uint8_t*)&_etext;
    uint8_t *dest=(uint8_t*)&_sdata;
    while (dest<(uint8_t*)&_edata) {
        *dest++=*src++;
    }

    dest=(uint8_t*)&_sbss;
    while (dest<(uint8_t*)&_ebss) {
        *dest++=0;
    }
    // Configure system clock to HSI
    SystemClock_Config();

    main();
    // Prevent execution from continuing beyond main if it ever returns unexpectedly
    // The infinite loop ensures the processor does not proceed to unintended code
    // after main(), safeguarding against any return from main().    
    while(1);
}

void SystemClock_Config(void) {
    RCC_CR|= (1<<0);
    // Select HSI as the system clock (SW[1:0] bits in RCC_CFGR)
    RCC_CFGR &=~(3<<0);
    RCC_CFGR|=(1<<0);// Set SW to 01 (HSI(High Speed Internal) (8MHz) selected as system clock)
}

int main(void) {
    RCC_AHBENR|=(1<<19);//enable gpioc clock

    GPIOC_MODER&=~(3<<12);
    GPIOC_MODER|=(1<<12);//01 is general purpose output mode

    GPIOC_OTYPER&=~(1<<6);//push pull configuration

    GPIOC_OSPEEDR|=(3<<12);//high speed

    GPIOC_PUPDR&=~(3<<12);//no pull up/pull down

    volatile uint32_t temp;//read-modify-write principle

    while (1) {
        temp=GPIOC_ODR;
        temp|=(1<<6);
        GPIOC_ODR=temp;
        delay();

        temp=GPIOC_ODR;
        temp&=~(1<<6);
        GPIOC_ODR=temp;
        delay();
    }

    return 0;
}


