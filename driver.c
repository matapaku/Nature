#include "stm32f446xx.h"

void delay_ms(uint32_t ms){
    for(volatile uint32_t i = 0; i < ms; i++);
}

void I2C_Init(void){
    //クロック
    RCC_GPIOBEN |= ( 1 << 1); // GPIOBのクロックを有効化
    RCC_I2C1EN |= ( 1 << 21); // I2C1のクロックを有効化

    //GPIOB8,B9をI2C
    GPIOB->MODER |= (2 << (2 * 8)); // PB8を代替機能モードに設定
    GPIOB->MODER |= (2 << (2 * 9)); //　PB9を代替機能モードに設定
    GPIOB->AFR[1] |= (4 << (4 * 0)); // PB8をI2C機能(AF4)に設定
    GPIOB->AFR[1] |= (4 << (4 * 1)); // PB9をI2C機能(AF4)に設定

    //I2C1設定
    I2C1->CR1 &= ~(1 << 0); //リセット
    I2C1->CR2 = 16; // APB1クロックを16MHzに設定
    I2C1->CCR = 80; // 通信速度を100kHzに設定
    I2C1->TRISE = 17; // 立ち上がり時間を1000nsに設定
    I2C1->CR1 |= (1 << 0); // I2Cを有効化
}

void TIM2_Init(void){
    RCC_TIM2EN |= (1 << 0); // Timer2のクロックを有効化
    TIM2->PSC = 16000 - 1; // プリスケーラを設定(1msごと)
    TIM2->ARR = 4200 - 1; // オートリロード値を設定(4.2秒周期)
    TIM2->CCMR1 |= (6 << 4); // PWMモード1を設定
    TIM2->CCER |= (1 << 0);  // 出力を有効化
    TIM2->CR1 |= (1 << 0);   // カウンタを有効化
}

uint16_t I2C_Read(uint8_t addr){
    //6バイトのバッファを用意
    uint8_t data[6];

    //I2C通信を開始
    I2C1->CR1 |= (1 << 8); // START
    while(!(I2C1->SR1 & (1 << 0))); // SBがセットされるまで待つ

    // センサからデータを受信
    I2C1->DR = addr | 0x01; // スレーブアドレスと読み込みビットを送信
    while(!(I2C1->SR1 & (1 << 1))); // ADDRがセットされるまで待つ
    (void)I2C1->SR2;

    // 6バイト分のデータを受信
    for(uint32_t i = 0; i < 6; i++){
        if(i == 5) I2C1->CR1 &= ~(1 << 10); // NACKを送信して停止
        while(!(I2C1->SR1 & (1 << 6))); // RXNEがセットされるまで待つ
        data[i] = I2C1->DR;
    }

    I2C1->CR1 |= (1 << 9); // STOP

    uint16_t raw_temp = (data[0] << 8) | data[1]; // 16bitデータ
    uint16_t raw_hum = (data[3] << 8) | data[4]; // 16bitデータ

    float temp = -45.0 + 175.0 * (raw_temp / 65535.0); // 温度の変換公式
    float hum = 100.0 * (raw_hum / 65535.0); // 湿度の変換公式

    return (uint16_t)(temp * 100) << 8 | (uint16_t)(hum * 100); // 8bit温度 + 8bit湿度
}

void I2C1_Write(uint8_t addr, uint16_t command) {
    I2C1->CR1 |= (1 << 8); // START
    while (!(I2C1->SR1 & (1 << 0))); // SBがセットされるまで待つ

    // デバイスを認識
    I2C1->DR = addr; // 7bitアドレス
    while (!(I2C1->SR1 & (1 << 1))); // ADDRがセットされるまで待つ
    (void)I2C1->SR2; // SR2を読み捨て

    I2C1->DR = (command >> 8); // 上位8bitコマンドを送信
    while (!(I2C1->SR1 & (1 << 7))); // TXEがセットされるまで待つ

    I2C1->DR = (command & 0xFF); // 下位8bitコマンドを送信
    while (!(I2C1->SR1 & (1 << 7))); // TXEがセットされるまで待つ

    I2C1->CR1 |= (1 << 9);  // STOP
}

float calc_discomfort_index(float temp, float hum) {
    return 0.81 * temp + 0.01 * hum * (0.99 * temp - 14.3) + 46.3; // 不快指数の計算
}

void IR_SendSignal(void) {
    //赤外線を送信する
}