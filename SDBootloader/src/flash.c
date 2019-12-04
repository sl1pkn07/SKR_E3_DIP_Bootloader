#include "flash.h"
#include "rtc.h"

uint32_t BlockNbr = 0;
uint32_t UserMemoryMask = 0;

FLASH_RESULT FlashFirmware() {
    /* Get the number of block (4 or 2 pages) from where the user program will be loaded */
    BlockNbr = (ApplicationAddress - 0x08000000) >> 12;
    UserMemoryMask = ((uint32_t) ~((1 << BlockNbr) - 1));

    if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask) {
        FLASH_DisableWriteProtectionPages();
    }

    FATFS fatfs;
    FIL file;
    u8 buffer[1024];
    UINT btr = 1024;
    UINT br = 0;
    UINT index = 0;

    FRESULT result = f_mount(0, &fatfs);

    if (result == FR_OK) {
        TCHAR filename[14] = "firmware.bin";
        TCHAR curfilename[14] = "FIRMWARE.CUR";

        result = f_open(&file, (TCHAR*) filename, FA_READ);

        if (result == FR_OK) {
            FLASH_Unlock();
            FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_BANK1_PGERR | FLASH_FLAG_BANK1_BSY | FLASH_FLAG_BANK1_EOP | FLASH_FLAG_BANK1_WRPRTERR);

            FLASH_Status FLASHStatus = FLASH_COMPLETE;
            uint32_t eraseCounter = 0;
            uint32_t writeAddress = ApplicationAddress;
            uint32_t flashValue = 0;

            // Calculate how many flash pages to erase
            uint32_t pages = FLASH_PagesMask(file.fsize) + 1;

            // Erase the flash pages
            for (eraseCounter = 0; (eraseCounter < pages) && (FLASHStatus == FLASH_COMPLETE); eraseCounter++) {
                FLASHStatus = FLASH_ErasePage(ApplicationAddress + (PAGE_SIZE * eraseCounter));
            }

            do {
                //Read the data from the file
                result = f_read(&file, &buffer, btr, &br);

                if (result == FR_OK) {
                    Led1On();

                    for (index = 0; index < br; index = index + 4) {
                        //Flash Data
                        flashValue = buffer[index + 3] << 24;
                        flashValue += buffer[index + 2] << 16;
                        flashValue += buffer[index + 1] << 8;
                        flashValue += buffer[index];

                        FLASHStatus = FLASH_ProgramWord(writeAddress, flashValue);

                        if (*(uint32_t*) writeAddress != flashValue) {
                            return FLASH_MEMORY_MISMATCH;
                        }

                        writeAddress += 4;
                    }
                } else {
                    return FLASH_DISK_ERR;
                }
            } while (br == btr);

            // Close the file
            result = f_close(&file);

            // Rename the file
            result = f_rename((TCHAR*) filename, (TCHAR*) curfilename);

            Led1Flash(3, 2000);
        } else {
            return FLASH_NO_FILE;
        }
    } else {
        return FLASH_NO_SD_CARD;
    }

    return FLASH_OK;
}

uint32_t CheckFirmware(FIL *file) {

    return 1;
}

uint32_t FLASH_PagesMask(__IO uint32_t Size) {
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0) {
        pagenumber = (size / PAGE_SIZE) + 1;
    } else {
        pagenumber = size / PAGE_SIZE;
    }

    return pagenumber;
}

void FLASH_DisableWriteProtectionPages(void) {
    uint32_t useroptionbyte = 0, WRPR = 0;
    uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
    FLASH_Status status = FLASH_BUSY;

    WRPR = FLASH_GetWriteProtectionOptionByte();

    /* Test if user memory is write protected */
    if ((WRPR & UserMemoryMask) != UserMemoryMask) {
        useroptionbyte = FLASH_GetUserOptionByte();

        UserMemoryMask |= WRPR;

        status = FLASH_EraseOptionBytes();

        if (UserMemoryMask != 0xFFFFFFFF) {
            status = FLASH_EnableWriteProtection((uint32_t) ~UserMemoryMask);
        }

        /* Test if user Option Bytes are programmed */
        if ((useroptionbyte & 0x07) != 0x07) {
            /* Restore user Option Bytes */
            if ((useroptionbyte & 0x01) == 0x0) {
                var1 = OB_IWDG_HW;
            }
            if ((useroptionbyte & 0x02) == 0x0) {
                var2 = OB_STOP_RST;
            }
            if ((useroptionbyte & 0x04) == 0x0) {
                var3 = OB_STDBY_RST;
            }

            FLASH_UserOptionByteConfig(var1, var2, var3);
        }

        if (status == FLASH_COMPLETE) {
            /* Generate System Reset to load the new option byte values */
            NVIC_SystemReset();
        }
    }
}
