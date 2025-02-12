#ifndef STM32F446XX_H
#define STM32F446XX_H

#include <stdint.h>

#define GPIOA_BASEADDR 0x40020000U
#define GPIOB_BASEADDR 0x40020400U
#define TIM2_BASEADDR 0x40000000U
#define I2C1_BASEADDR 0x40005400U

#define RCC_GPIOBEN (*(volatile uint32_t *) 0x40023830U)
#define RCC_I2C1EN (*(volatile uint32_t *) 0x40023840U)
#define RCC_TIM2EN (*(volatile uint32_t *) 0x40023844U)

#define GPIOA ((GPIO_TypeDef *)GPIOA_BASEADDR)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASEADDR)
#define TIM2  ((TIM_TypeDef *)TIM2_BASEADDR)
#define I2C1   ((I2C_TypeDef *)I2C1_BASEADDR)

#define I2C_ADDR  0x44 << 1


typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
    volatile uint32_t FLTR;
}I2C_TypeDef;

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
    volatile uint32_t OR;
} TIM_TypeDef;


#endif // STM32F446XX_H