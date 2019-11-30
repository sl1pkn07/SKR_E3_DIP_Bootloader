#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hw_config.h"
#include "platform_config.h"

void SD_LowLevel_Init(void);
void SD_LowLevel_DeInit(void);

void Led1Flash(uint16_t times, uint16_t duration);
void Led1On(void);
void Led1Off(void);

#define ApplicationAddress          (0x8007000)
#define PAGE_SIZE                   (0x800)    /* 2 Kbyte */
#define FLASH_SIZE                  (0x80000)  /* 512 KBytes */

/* Led 1*/
#define LED_1_PIN                   GPIO_Pin_13
#define LED_1_GPIO_PORT             GPIOA

/* SD Card */
#define SD_SPI                      SPI1
#define SD_SPI_CLK                  RCC_APB2Periph_SPI1
#define SD_SPI_SCK_PIN              GPIO_Pin_5
#define SD_SPI_SCK_GPIO_PORT        GPIOA
#define SD_SPI_SCK_GPIO_CLK         RCC_APB2Periph_GPIOA
#define SD_SPI_MISO_PIN             GPIO_Pin_6
#define SD_SPI_MISO_GPIO_PORT       GPIOA
#define SD_SPI_MISO_GPIO_CLK        RCC_APB2Periph_GPIOA
#define SD_SPI_MOSI_PIN             GPIO_Pin_7
#define SD_SPI_MOSI_GPIO_PORT       GPIOA
#define SD_SPI_MOSI_GPIO_CLK        RCC_APB2Periph_GPIOA
#define SD_CS_PIN                   GPIO_Pin_4
#define SD_CS_GPIO_PORT             GPIOA
#define SD_CS_GPIO_CLK              RCC_APB2Periph_GPIOA
#define SD_DETECT_PIN               GPIO_Pin_4
#define SD_DETECT_GPIO_PORT         GPIOC
#define SD_DETECT_GPIO_CLK          RCC_APB2Periph_GPIOC

#endif /* MAIN_H_ */
