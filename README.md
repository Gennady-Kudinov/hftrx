# ������ HF Dream Receiver (�� ������� �����)
## ����� ���� ����������� mgs2001@mail.ru UA1ARN

���������� �� ���������� ����� � ����������:

## ���������������:

1. ����������� ��������� (���������� � ������� ��� ������ �������) <br>
1.1 **ARM** GNU ARM Toolchain https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads (��������� ����������� ������ 7-2018-q2-update) <br>
1.2 **ATMEGA/ATXMEGA** AVR 8-bit Toolchain https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers (�������������, ����������� ���� � ����� BIN � ���������� ��������� PATH)<br>
1.3 ������� ��� ������ (����. make) ������: http://www.yagarto.org/ ��� ������: http://www.cygwin.com/ (��������� ��� � ��������� ������ �������� ������� make, ����� ��������� ���������� PATH � �������) <br>
1.4 ������� ��� ������ � GIT ������������� https://git-scm.com/downloads

2. ��������� �������������� ����������, ������������� �� �� ������� ����, ��� ����� �������. <br>
2.1 **ARM** CMSIS, ���������� ���������������� ����, ��� ����� � ����� ���� ������� ������ ������� "git clone https://github.com/XGudron/CMSIS_5" <br>
2.2 **ATMEL ARM** � ������ ������������� Atmel ARM (SAM) �����������, ������� � ����������� � ����� "xdk-asf" ����� Advanced Software Framework (ASF) https://www.microchip.com/mplab/avr-support/advanced-software-framework

3. ������������� IDE ��� ���������� <br>
3.1 ��������� � ������������� Eclipse https://www.eclipse.org/downloads/ <br>
3.2 � ������� ���� Help -> Check for updates, ������������� ���������� <br>
3.3 � ������� ���� Help -> Eclipse Marketplace, ������������� ���������� <br>
3.4 � ������� ���� Help -> Eclipse Marketplace, ��������� �����, ������������� ���������� GNU MCU Eclipse

4. �������� ������ <br>
4.1 ��������� ��������� ������ ������� �������� "git clone https://github.com/ua1arn/hftrx" <br>
4.2 ��������� ������ ����� File -> Open projects from File System
4.3 �������� ���� product.h.prototype � product.h <br>
4.4 ��������������� ������ ��� ��������� � ��������� ������������� <br>
4.5 � ���� Eclipse, Project -> Build configurations -> Set active �������� ����������� ��������� (��� ����� ���������� ���� � �������� � ToolBox). Build target �������� default.

5. ��������� <br>
5.1 ��������������� �������� ��������� � ����� /build/<���������>/

## FPGA:

������������ Quartis II 13.1 (� ��������) Paid version http://download.altera.com/akdlm/software/acdsinst/13.1/162/ib_tar/Quartus-13.1.0.162-windows-complete.tar <br>
�  ������ ����� http://download.altera.com/akdlm/software/acdsinst/13.1.4/182/update/QuartusSetup-13.1.4.182.exe

## �����:

����� ������� ������������ ������� ������������ � ����� product.h <br>
�� ��������� �������� ������������ � �������� ���������� (arm/atmega) ���������� ���� ���������������� ������ <br>
.\board\*ctlstyle*.h � .\board\*cpustyle*.h. <br>
ctlstyle ��������� ������� (�� ��������� � ����������) ����������� ���������� - ������ ��������� �� ���� spi, ���� ���������� ��������� � ��� ���������� (� ��� �����).  <br>
cpustyle ��������� ���������� ������� ���������� (������������� �� ������ �����/������).

