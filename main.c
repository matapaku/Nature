#include "stm32f446xx.h"

void delay_ms(uint32_t ms){
    for(volatile uint32_t i = 0; i < ms; i++);
}

void I2C_Init(void){
    //クロック
    RCC_GPIOBEN |= ( 1 << 1); //GPIOB_ENR
    RCC_I2C1EN |= ( 1 << 21); //I2C1_ENR

    //GPIOB8,B9をI2C
    GPIOB->MODER |= (2 << (2 * 8)); //GPIOB8
    GPIOB->MODER |= (2 << (2 * 9)); //GPIOB9
    GPIOB->AFR[1] |= (4 << (4 * 0)); //GPIOB8
    GPIOB->AFR[1] |= (4 << (4 * 1)); //GPIOB9

    //I2C1設定
    I2C1->CR1 &= ~(1 << 0); //リセット
    I2C1->CR2 = 16; //APB1CLK = 16MHz
    I2C1->CCR = 80; //100kHz
    I2C1->TRISE = 17; //1000ns
    I2C1->CR1 |= (1 << 0); //I2C有効化
}

void TIM2_Init(void){
    RCC_TIM2EN |= (1 << 0); // TIM2クロック有効化
    TIM2->PSC = 16000 - 1; // 1ms
    TIM2->ARR = 4200 - 1; // 5s
    TIM2->CCMR1 |= (6 << 4); // PWM mode 1
    TIM2->CCER |= (1 << 0);  // Enable output
    TIM2->CR1 |= (1 << 0);   // Enable counter
}

uint16_t I2C_Read(uint8_t addr){
    uint8_t data[6];
    I2C1->CR1 |= (1 << 8); // START
    while(!(I2C1->SR1 & (1 << 0))); // SBがセットされるまで待つ
    I2C1->DR = addr | 0x01; // 7bitアドレス + 1bit read
    while(!(I2C1->SR1 & (1 << 1))); // ADDRがセットされるまで待つ
    (void)I2C1->SR2;
    for(uint32_t i = 0; i < 6; i++){
        if(i == 5) I2C1->CR1 &= ~(1 << 10); // NACK
        while(!(I2C1->SR1 & (1 << 6))); // RXNEがセットされるまで待つ
        data[i] = I2C1->DR;
    }
    I2C1->CR1 |= (1 << 9); // STOP
    uint16_t raw_temp = (data[0] << 8) | data[1]; // 16bitデータ
    uint16_t raw_hum = (data[3] << 8) | data[4]; // 16bitデータ
    float temp = -45.0 + 175.0 * (raw_temp / 65535.0); // 温度
    float hum = 100.0 * (raw_hum / 65535.0); // 湿度
    return (uint16_t)(temp * 100) << 8 | (uint16_t)(hum * 100); // 8bit温度 + 8bit湿度
}

void I2C1_Write(uint8_t addr, uint16_t command) {
    I2C1->CR1 |= (1 << 8); // START
    while (!(I2C1->SR1 & (1 << 0))); // SBがセットされるまで待つ

    I2C1->DR = addr; // 7bitアドレス
    while (!(I2C1->SR1 & (1 << 1))); // ADDRがセットされるまで待つ
    (void)I2C1->SR2; // SR2を読み捨て

    I2C1->DR = (command >> 8); // 8bitコマンド
    while (!(I2C1->SR1 & (1 << 7))); // TXEがセットされるまで待つ

    I2C1->DR = (command & 0xFF); // 8bitコマンド
    while (!(I2C1->SR1 & (1 << 7))); // TXEがセットされるまで待つ

    I2C1->CR1 |= (1 << 9);  // STOP
}

float calc_discomfort_index(float temp, float hum) {
    return 0.81 * temp + 0.01 * hum * (0.99 * temp - 14.3) + 46.3; // 不快指数の計算
}

void IR_SendSignal(void) {
    //赤外線を送信する
}

int main(void){
    I2C1_Init(); // I2C1初期化
    TIM2_Init(); // TIM2初期化

    while(1){
        I2C1_Write(I2C_ADDR, 0x2C06); // 14bit resolution
        delay_ms(15); // wait for measurement

        uint16_t raw_data = I2C1_Read(I2C_ADDR); // read data
        float temp = ((raw_data >> 8) & 0xFF) / 100.0; // 8bit temperature
        float hum = (raw_data & 0xFF) / 100.0; // 8bit humidity

        float discomfort_index = calc_discomfort_index(temp, hum); //  discomfort indexの計算

        if (discomfort_index > 75.0) {
            IR_SendSignal(); // 赤外線を送信
        }

        delay_ms(5000); // 5s
    }
}