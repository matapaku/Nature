#include "stm32f446xx.h"
#include <driver.c>


int main(void){
    I2C1_Init(); // I2C1初期化
    TIM2_Init(); // TIM2初期化

    while(1){
        I2C1_Write(I2C_ADDR, 0x2C06); // 温湿度センサにコマンドを送信
        delay_ms(15); // 15ms待つ

        uint16_t raw_data = I2C1_Read(I2C_ADDR); // 温湿度センサからデータを読み取る
        float temp = ((raw_data >> 8) & 0xFF) / 100.0; // 8bit temperature
        float hum = (raw_data & 0xFF) / 100.0; // 8bit humidity

        float discomfort_index = calc_discomfort_index(temp, hum); //  discomfort indexの計算

        if (discomfort_index > 75.0) {
            IR_SendSignal(); // 赤外線を送信
        }

        delay_ms(5000); // 5sくらい待つ
    }
}