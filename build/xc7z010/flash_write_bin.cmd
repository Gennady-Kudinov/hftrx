REM Utility files can be downloaded from https://sourceforge.net/projects/dfu-util/files/latest/download
@echo Press any key for write APPLICATION section of FLASH
@pause
dfu-util.exe --device ffff:0738 --alt 0 --dfuse-address 0x70080000 --download "tc1_xc7z010_app_xyl32.bin"
@pause

