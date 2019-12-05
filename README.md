# SKR_E3_DIP_Bootloader
Try to create a SKR E3 DIP Bootloader for units with RET6 mcu

Original code I use: https://community.st.com/s/question/0D50X00009XkaeUSAR/sd-card-bootloader-fat-fs-filesystem-usb-mass-storage</br>
Target Board: https://github.com/bigtreetech/BIGTREETECH-SKR-E3-DIP-V1.0

# WHY?
Because stock bootloader can't upload firmware >256KB because the manufacturer (BTT), in some batch, change the RCT6 mcu to RET6 mcu (256KB -> 512KB) and refuse to update their bootloader for this batch.

RCT6 -> https://www.st.com/en/microcontrollers-microprocessors/stm32f103rc.html</br>
RET6 -> https://www.st.com/en/microcontrollers-microprocessors/stm32f103re.html


# STATUS
Build: OK</br>
Works: NOT AT ALL: only works the first time flash the firmware. new version of firmware is not flashed. IDK why this happen. any help?
