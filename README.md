# SKR_E3_DIP_Bootloader
Fake-try to create(modified) SKR E3 DIP Bootloader (RET6 mcu)

Original: https://community.st.com/s/question/0D50X00009XkaeUSAR/sd-card-bootloader-fat-fs-filesystem-usb-mass-storage</br>
Board: https://github.com/bigtreetech/BIGTREETECH-SKR-E3-DIP-V1.0

# WHY?
Because stock bootloader can't upload firmware >256KB because the manufacturer (BTT), in some batch, change the RTC6 mcu to RET6 mcu (256KB -> 512KB) and refuse to update their bootloader for this batch.

RTC6 -> https://www.st.com/en/microcontrollers-microprocessors/stm32f103rc.html</br>
RET6 -> https://www.st.com/en/microcontrollers-microprocessors/stm32f103re.html


# STATUS
Build: OK</br>
Works: MAYBE?
