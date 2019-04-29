# ������ HF Dream Receiver (�� ������� �����)
## ����� ���� ����������� mgs2001@mail.ru UA1ARN

���������� �� ���������� ����� ���������� ��� ARM ����������� ��������� ����.

## ATMEGA/ATXMEGA:

������� ������� �� ������ ��������� � ���� ������� (��� ������ ������������) ����. ������� ���������.

��� ���������� ������������

AVR Studio 4.19 (build 730) http://www.atmel.com/Images/AvrStudio4Setup.exe

� ����������� ���� ������� ��� AVR Studio 4.19 �������� ��� ������ tc1.aps.prototype, ��� ���� ������������� � tc1.aps � ������� � ����� ��� ������.<br>
��� �� ��������� �������������� ����� product.h.prototype � product.h. � ��� ����������������� ��������� ������������ � ���������������� �� ������.

������ ���� ������������<br>
Atmel AVR 8-bit and 32-bit Toolchain 3.4.2 - Windows<br> 
http://www.atmel.com/images/avr-toolchain-installer-3.4.2.1573-win32.win32.x86.exe

��� ����������� ������ ��� x64 �������� Windows 8.x, Windows 10 ������� http://www.madwizard.org/download/electronics/msys-1.0-vista64.zip <br>
� ��������������� msys-1.0.dll � ��� �� �������, ��� ��������� make.exe<br>
��� ����� ������ ���� (��� ������� �����) � download page ������� MinGW - Minimalist GNU for Windows http://sourceforge.net/projects/mingw

�������������� ������������� avr-gcc ������ avr-gcc-6.1.1 - ��������� ����� ���� ����������� � ��������� �� ������������� avr-gcc �� ����. <br>
������� make ������������ �� ��������� toolchain �� ATMEL. <br>
������ ��� ����� ������� https://sourceforge.net/projects/mobilechessboar/?source=directory <br>
������ ��� ����������: http://kent.dl.sourceforge.net/project/mobilechessboard/avr-gcc%20snapshots%20%28Win32%29/avr-gcc-6.1.1_2016-06-26_mingw32.zip

## ARM:

1. ����������� ��������� (���������� � ������� ��� ������ �������) <br>
1.1 GNU ARM Toolchain https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads (��������� ����������� ������ 7-2018-q2-update) <br>
1.2 ������� ��� ������ (����. make) ������: http://www.yagarto.org/ ��� ������: http://www.cygwin.com/ (��������� ��� � ��������� ������ �������� ������� make, ����� ��������� ���������� PATH � �������) <br>
1.3 ������� ��� ������ � GIT ������������� https://git-scm.com/downloads

2. ��������� �������������� ����������, ������������� �� �� ������� ����, ��� ����� �������. <br>
2.1 CMSIS, ���������� ���������������� ����, ��� ����� � ����� ���� ������� ������ ������� "git clone https://github.com/XGudron/CMSIS_5"

3. ������������� IDE ��� ���������� <br>
3.1 ��������� � ������������� Eclipse https://www.eclipse.org/downloads/ <br>
3.2 � ������� ���� Help -> Check for updates, ������������� ���������� <br>
3.3 � ������� ���� Help -> Eclipse Marketplace, ������������� ���������� <br>
3.4 � ������� ���� Help -> Eclipse Marketplace, ��������� �����, ������������� ���������� GNU MCU Eclipse

4. �������� ������ <br>
4.1 ��������� ��������� ������ ������� �������� "git clone https://github.com/ua1arn/hftrx" <br>
4.2 �������� ���� product.h.prototype � product.h <br>
4.3 ��������������� ������ ��� ��������� � ��������� ������������� <br>
4.4 � ���� Eclipse, Project -> Build configurations -> Set active �������� ����������� ��������� (��� ����� ���������� ���� � �������� � ToolBox)

5. ��������� <br>
5.1 ��������������� �������� ��������� � ����� /build/<���������>/

## FPGA:

������������ Quartis II 13.1 (� ��������) Paid version http://download.altera.com/akdlm/software/acdsinst/13.1/162/ib_tar/Quartus-13.1.0.162-windows-complete.tar <br>
�  ������ ����� http://download.altera.com/akdlm/software/acdsinst/13.1.4/182/update/QuartusSetup-13.1.4.182.exe

## ATMEL ARM:

��������� Atmel Software Framework 3.27  <br>
http://www.atmel.com/images/asf-standalone-archive-3.27.0.28.zip

## �����:

����� ������� ������������ ������� ������������ � ����� product.h <br>
�� ��������� �������� ������������ � �������� ���������� (arm/atmega) ���������� ���� ���������������� ������ <br>
.\board\*ctlstyle*.h � .\board\*cpustyle*.h. <br>
ctlstyle ��������� ������� (�� ��������� � ����������) ����������� ���������� - ������ ��������� �� ���� spi, ���� ���������� ��������� � ��� ���������� (� ��� �����).  <br>
cpustyle ��������� ���������� ������� ���������� (������������� �� ������ �����/������).

