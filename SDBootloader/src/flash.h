#ifndef FLASH_H_
#define FLASH_H_

#include "main.h"
#include "stm32f10x_flash.h"
#include "ff.h"

/* Private typedef */
typedef enum {
	FLASH_OK = 0,				/* (0) Succeeded */
	FLASH_NO_SD_CARD,			/* (1) No SD Card Inserted */
	FLASH_NO_FILE,				/* (2) No New firmware.bin file */
	FLASH_DISK_ERR,				/* (3) Error reading firmware from sd card */
	FLASH_MEMORY_MISMATCH		/* (4) Flashed Memory does not match value */
} FLASH_RESULT;

uint32_t CheckFirmware(FIL *file);
FLASH_RESULT FlashFirmware(void);
uint32_t FLASH_PagesMask(__IO uint32_t Size);
void FLASH_DisableWriteProtectionPages(void);

#endif /* FLASH_H_ */
