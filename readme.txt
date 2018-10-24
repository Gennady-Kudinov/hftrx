���������� �� ���������� ����� ���������� ��� ARM ����������� ��������� ����.

ATMEGA/ATXMEGA:

������� ������� �� ������ ��������� � ���� ������� (��� ������ ������������) ����. ������� ���������.

��� ���������� ������������

AVR Studio 4.19 (build 730):
http://www.atmel.com/Images/AvrStudio4Setup.exe

� ����������� ���� ������� ��� AVR Studio 4.19 �������� ��� ������ tc1.aps.prototype, ��� ���� ������������� � tc1.aps � ������� � ����� ��� ������.
��� �� ��������� �������������� ����� product.h.prototype � product.h. � ��� ����������������� ��������� ������������ � ���������������� �� ������.

������ ���� ������������ 
Atmel AVR 8-bit and 32-bit Toolchain 3.4.2 - Windows 
http://www.atmel.com/images/avr-toolchain-installer-3.4.2.1573-win32.win32.x86.exe

��� ����������� ������ ��� x64 �������� Windows 8.x, Windows 10 �������
http://www.madwizard.org/download/electronics/msys-1.0-vista64.zip
� ��������������� msys-1.0.dll � ��� �� �������, ��� ��������� make.exe
��� ����� ������ ���� (��� ������� �����) � download page ������� MinGW - Minimalist GNU for Windows 
http://sourceforge.net/projects/mingw

�������������� ������������� avr-gcc ������ avr-gcc-6.1.1 - ��������� ����� ���� ����������� � ��������� �� ������������� avr-gcc �� ����. 
������� make ������������ �� ��������� toolchain �� ATMEL. 
������ ��� ����� ������� https://sourceforge.net/projects/mobilechessboar/?source=directory
������ ��� ����������:
http://kent.dl.sourceforge.net/project/mobilechessboard/avr-gcc%20snapshots%20%28Win32%29/avr-gcc-6.1.1_2016-06-26_mingw32.zip

ARM:

��� ���������� ������������ �������� ������������ � ����� www.yagarto.de
����� ����, ��� ���������� ��� ������������ CORTEX-M0/M3/M4/M7 ������������ ���������� CMSIS �� ARM LIMITED. ������� ��������� �� ��� �� ������, ��� � ������� �������. 
������� CMSIS ����� ���: 
https://silver.arm.com/download/ARM_and_AMBA_Architecture/CMSIS-SP-00300-r4p5-00rel0/CMSIS-SP-00300-r4p5-00rel0.zip
���������� � ������� �������:
https://developer.arm.com/embedded/cmsis
https://github.com/ARM-software/CMSIS_5/releases/tag/5.2.0

���������� ��� ����������� � 
yagarto-tools-20121018-setup.exe

���������� ����������� - ������ ���: https://launchpad.net/gcc-arm-embedded
��������� ������ - 
https://developer.arm.com/-/media/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-win32.exe
���������� � ������� �������:
https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
Old product page:
https://launchpad.net/gcc-arm-embedded

FPGA:

������������ Quartis II 13.1 (� ��������) Paid version

http://download.altera.com/akdlm/software/acdsinst/13.1/162/ib_tar/Quartus-13.1.0.162-windows-complete.tar

�  ������ �����

http://download.altera.com/akdlm/software/acdsinst/13.1.4/182/update/QuartusSetup-13.1.4.182.exe

ATMEL ARM:

��������� Atmel Software Framework 3.27 

http://www.atmel.com/images/asf-standalone-archive-3.27.0.28.zip

�����:

����� ������� ������������ ������� ������������ � ����� product.h
�� ��������� �������� ������������ � �������� ���������� (arm/atmega) ���������� ���� ���������������� ������
.\board\*ctlstyle*.h � .\board\*cpustyle*.h.
ctlstyle ��������� ������� (�� ��������� � ����������) ����������� ���������� - ������ ��������� �� ���� spi, 
���� ���������� ��������� � ��� ���������� (� ��� �����). 
cpustyle ��������� ���������� ������� ���������� (������������� �� ������ �����/������).

