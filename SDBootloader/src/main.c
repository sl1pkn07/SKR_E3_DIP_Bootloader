/* Includes */
#include <stddef.h>
#include "stm32f10x.h"
#include "main.h"
#include "usb_lib.h"
#include "flash.h"

/* Private define  */
/* Private macro */
/* Private variables */
typedef  void (*pFunction)(void);
uint32_t JumpAddress;
pFunction Jump_To_Application;

/* Private function prototypes */
/* Private functions */
void RCC_Configuration();
void NVIC_Configuration(void);
void Timer2_Init(void);
void Timer2_DeInit(void);
void Timer3_Init(void);
void Timer3_DeInit(void);
void Led1_Init(void);
void USBMassStorage_Init(void);

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  RCC_Configuration();
  NVIC_Configuration();
  Timer2_Init();
  Timer3_Init();
  Led1_Init();

  Led1Flash(2, 2000);

  //If new firmware present on microSD card -> Flash to MCU
  FLASH_RESULT flashResult = FlashFirmware();

  /* Check if the result of the Flash Firmware function */
  if(flashResult == FLASH_OK
    || flashResult == FLASH_NO_FILE
    || flashResult == FLASH_NO_SD_CARD)
  {
      /* Test if user code is programmed starting from address "ApplicationAddress" */
      if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
      {
          Led1Flash(2, 900);

          // Disable Timers
          Timer2_DeInit();
          Timer3_DeInit();

          /* Jump to user application */
          JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
          Jump_To_Application = (pFunction) JumpAddress;

          /* Initialise user application's Stack Pointer */
          __set_MSP(*(__IO uint32_t*) ApplicationAddress);
          Jump_To_Application();
      }
  }

  USBMassStorage_Init();

  while(1)
  {
      Led1Flash(1, 5000);
  }
}

void Led1Flash(uint16_t times, uint16_t duration)
{
    uint16_t count = 0;

    for(count = 0; count < times; count++)
    {
        Led1On();

        TIM_SetCounter(TIM2, 0);
        while(TIM_GetCounter(TIM2) < duration);

        Led1Off();

        TIM_SetCounter(TIM2, 0);
        while(TIM_GetCounter(TIM2) < duration);
    }
}

void Led1On(void)
{
    GPIO_WriteBit(LED_1_GPIO_PORT, LED_1_PIN, SET);
}

void Led1Off(void)
{
    GPIO_WriteBit(LED_1_GPIO_PORT, LED_1_PIN, RESET);
}

void RCC_Configuration()
{
    /* GPIOA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* GPIOB */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* GPIOC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* GPIOD */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    /* GPIOE */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    /* AFIO */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* TIM2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* TIM3 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

    NVIC_InitTypeDef NVIC_InitStructure;

    // Enable the TIM3 Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USBMassStorage_Init()
{
    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
}

void Timer2_Init(void)
{
    // Time base configuration Timer3 clock
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void Timer3_Init(void)
{
    // Time base configuration Timer3 clock
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 10;
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void Timer2_DeInit()
{
    TIM_Cmd(TIM2, DISABLE);
    TIM_DeInit(TIM2);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
}

void Timer3_DeInit(void)
{
    TIM_Cmd(TIM3, DISABLE);
    TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
    TIM_ARRPreloadConfig(TIM3, DISABLE);
    NVIC_DisableIRQ(TIM3_IRQn);
    TIM_DeInit(TIM3);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
}

void Led1_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = LED_1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_1_GPIO_PORT, &GPIO_InitStructure);
}

void SD_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  SPI_Cmd(SD_SPI, DISABLE); /*!< SD_SPI disable */
  SPI_I2S_DeInit(SD_SPI);   /*!< DeInitializes the SD_SPI */

  /*!< SD_SPI Periph clock disable */
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, DISABLE);

  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
}


void SD_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;

  /*!< SD_SPI_CS_GPIO, SD_SPI_MOSI_GPIO, SD_SPI_MISO_GPIO, SD_SPI_DETECT_GPIO
       and SD_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(SD_CS_GPIO_CLK | SD_SPI_MOSI_GPIO_CLK | SD_SPI_MISO_GPIO_CLK |
                         SD_SPI_SCK_GPIO_CLK | SD_DETECT_GPIO_CLK, ENABLE);

  /*!< SD_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, ENABLE);

  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);

  /*!< SD_SPI Config */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SD_SPI, &SPI_InitStructure);

  SPI_Cmd(SD_SPI, ENABLE); /*!< SD_SPI enable */
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
  while(1)
  {}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr)
{
   __assert_func (file, line, NULL, failedexpr);
}

#ifdef USE_SEE
#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval sEE_FAIL.
  */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* Return with error code */
  return sEE_FAIL;
}
#endif
#endif /* USE_SEE */

